// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MinionBarrack.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "Character/Minion/MinionCharacter.h"
#include "DefenceTower/DefenseTowerCharacter.h"
#include "DefenceTower/StormCore.h"
#include "Framework/ArenasGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMinionBarrack::AMinionBarrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

AActor* AMinionBarrack::GetCurrentMinionGoal() const
{
	if (AMinionBarrack* EnemyBarrack = GetEnemyBarrack())
	{
		return EnemyBarrack->GetCurrentCanBeAttackGoalActor();
	}
	return nullptr;
}

AMinionBarrack* AMinionBarrack::GetEnemyBarrack() const
{
	// 获取场景内敌方兵营对象
	if (!HasAuthority()) return nullptr;

	// 从GameMode中获取
	if (AArenasGameMode* ArenasGameMode = Cast<AArenasGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (AMinionBarrack* EnemyBarrack = ArenasGameMode->GetBarrackByTeamID(EnemyBarrackTeamID))
		{
			return EnemyBarrack;
		}
		
	}
	return nullptr;
}

void AMinionBarrack::OnDefenseTowerDeath(AActor* TowerActor)
{
	if (HasAuthority())
	{
		if (ADefenseTowerCharacter* TowerCharacter = Cast<ADefenseTowerCharacter>(TowerActor))
		{
			if (UArenasAbilitySystemComponent* TowerASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(TowerCharacter))
			{
				TowerASC->OnActorDeath.RemoveAll(this); // 取消监听防御塔的死亡事件
			}
			
			if (SpawnedTowers.Contains(TowerCharacter))
			{
				SpawnedTowers.Remove(TowerCharacter);

				// 防御塔死亡后，修改敌方小兵的默认目标，这样就不会有小兵一直攻击死去的防御塔或者跳过一塔攻击二塔或者水晶
				if (AMinionBarrack* EnemyBarrack = GetEnemyBarrack())
				{
					AActor* NewGoalActor = GetCurrentCanBeAttackGoalActor();
					for (AMinionCharacter* Minion : EnemyBarrack->SpawnedMinionPool)
					{
						Minion->SetGoal(NewGoalActor);
					}
				}
			}
		}
	}
}

void AMinionBarrack::OnStormCoreDestroyedInGame(AActor* Actor)
{
	if (AStormCore* Core = Cast<AStormCore>(Actor))
	{
		if (AMinionBarrack* Barrack = Core->GetOwnedBarrack())
		{
			OnStormCoreDestroyed.Broadcast(Barrack);
		}
	}
}

void AMinionBarrack::SpawnDefenseTowers()
{
	if (HasAuthority())
	{
		FTransform SpawnPointTransform = GetActorTransform();
		// 生成防御塔核心
		if (StormCoreClass && BaseCoreSpawnPoint)
		{
			SpawnPointTransform = BaseCoreSpawnPoint->GetActorTransform();
			
			if (AStormCore* Core = GetWorld()->SpawnActorDeferred<AStormCore>(StormCoreClass, SpawnPointTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
			{
				Core->SetGenericTeamId(BarrackTeamID);
				Core->FinishSpawning(SpawnPointTransform);
				Core->SetOwnerBarrack(this);
				SpawnedStormCore = Core;

				if (UArenasAbilitySystemComponent* CoreASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(Core))
				{
					CoreASC->OnActorDeath.AddUObject(this, &AMinionBarrack::OnStormCoreDestroyedInGame);
				}
				
			}
		}
		
		for (const APlayerStart* SpawnPoint : TowerSpawnPoints)
		{
			SpawnPointTransform = SpawnPoint->GetActorTransform();
			// 在每个生成点生成一个防御塔
			if (DefenseTowerClass)
			{
				if (ADefenseTowerCharacter* Tower = GetWorld()->SpawnActorDeferred<ADefenseTowerCharacter>(DefenseTowerClass, SpawnPointTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
				{
					Tower->SetGenericTeamId(BarrackTeamID);
					Tower->FinishSpawning(SpawnPointTransform);
					Tower->SetDefenseTowerFaceGoal(DefaultFaceGoalActor);
					SpawnedTowers.Add(Tower);

					// 监听防御塔的死亡事件
					if (UArenasAbilitySystemComponent* TowerASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(Tower))
					{
						TowerASC->OnActorDeath.AddUObject(this, &AMinionBarrack::OnDefenseTowerDeath);
					}
					
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnDefenseTowers();	// 生成防御塔
		
		GetWorldTimerManager().SetTimer(
			SpawnGroupMinionTimerHandle,
			this,
			&AMinionBarrack::SpawnMinionGroup,
			SpawnGroupInterval,
			true,
			SpawnFirstGroupDelay); // 10秒后开始生成第一波小兵

		if (AArenasGameMode* ArenasGameMode = Cast<AArenasGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			ArenasGameMode->RegisterMinionBarrack(BarrackTeamID, this);
		}
	}
	
}

// Called every frame
void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const APlayerStart* AMinionBarrack::GetNextSpawnPoint()
{
	if (MinionSpawnPoints.Num() > 0)
	{
		++NextSpawnSpotIndex;

		if (NextSpawnSpotIndex >= MinionSpawnPoints.Num())
		{
			NextSpawnSpotIndex = 0;
		}

		return MinionSpawnPoints[NextSpawnSpotIndex];
	}

	return nullptr;
}

void AMinionBarrack::SpawnOrFindOneMinion()
{
	FTransform SpawnPointTransform = GetActorTransform();
	if (const APlayerStart* TempSpawnPoint = GetNextSpawnPoint())
	{
		SpawnPointTransform = TempSpawnPoint->GetActorTransform();
	}

	if (AMinionCharacter* MinionToSpawn = GetNextAvailableMinionFromPool())
	{
		MinionToSpawn->SetActorTransform(SpawnPointTransform);
		MinionToSpawn->RespawnImmediately();
	}
	else
	{
		SpawnMinionsToPool();
	}
}

void AMinionBarrack::SpawnMinionGroup()
{
	if (HasAuthority())
	{

		// 清除之前的定时器，防止重复触发
		GetWorldTimerManager().ClearTimer(SpawnMinionTimerHandle);
		// 设置新的定时器，每隔SpawnMinionInterval秒生成一个小兵，直到生成MinionPerGroup个小兵
		GetWorldTimerManager().SetTimer(
			SpawnMinionTimerHandle,
			[this]()
			{
				if (NumSpawnedThisRound < MinionPerGroup)
				{
					SpawnOrFindOneMinion();
					++NumSpawnedThisRound;
				}
				else
				{
					// 达到每组小兵的数量后，清除定时器并重置计数
					GetWorldTimerManager().ClearTimer(SpawnMinionTimerHandle);
					NumSpawnedThisRound = 0;
				}
			},
			SpawnMinionInterval,
			true);
		
		
	}
}

void AMinionBarrack::SpawnMinionsToPool()
{
	FTransform SpawnPointTransform = GetActorTransform();
	if (const APlayerStart* TempSpawnPoint = GetNextSpawnPoint())
	{
		SpawnPointTransform = TempSpawnPoint->GetActorTransform();
	}

	AMinionCharacter* NewMinion = GetWorld()->SpawnActorDeferred<AMinionCharacter>(
		MinionClass,
		SpawnPointTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (NewMinion)
	{
		NewMinion->SetGenericTeamId(BarrackTeamID);
		NewMinion->FinishSpawning(SpawnPointTransform);
		NewMinion->SetGoal(GetCurrentMinionGoal());
		SpawnedMinionPool.Add(NewMinion);
	}
}

AMinionCharacter* AMinionBarrack::GetNextAvailableMinionFromPool() const
{
	for (AMinionCharacter* Minion : SpawnedMinionPool)
	{
		if (Minion && !Minion->IsAlive())
		{
			return Minion;
		}
	}
	return nullptr;
}

AActor* AMinionBarrack::GetCurrentCanBeAttackGoalActor() const
{
	// 返回当前可以作为防御塔攻击目标的Actor
	if (SpawnedTowers.Num() > 0)
	{
		return SpawnedTowers[0];
	}
	return SpawnedStormCore;
}

