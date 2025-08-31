// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MinionBarrack.h"

#include "Character/Minion/MinionCharacter.h"
#include "GameFramework/PlayerStart.h"


// Sets default values
AMinionBarrack::AMinionBarrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			SpawnGroupMinionTimerHandle,
			this,
			&AMinionBarrack::SpawnMinionGroup,
			SpawnGroupInterval,
			true,
			SpawnGroupInterval);
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
	if (const APlayerStart* SpawnPoint = GetNextSpawnPoint())
	{
		SpawnPointTransform = SpawnPoint->GetActorTransform();
	}

	if (AMinionCharacter* MinionToSpawn = GetNextAvailableMinionFromPool())
	{
		MinionToSpawn->SetActorTransform(SpawnPointTransform);
		MinionToSpawn->Activate();
	}
	else
	{
		SpawnMinionsToPool();
	}
}

void AMinionBarrack::SpawnMinionGroup()
{
	int32 NumSpawnedThisRound = 0;

	// 清除之前的定时器，防止重复触发
	GetWorldTimerManager().ClearTimer(SpawnMinionTimerHandle);
	// 设置新的定时器，每隔SpawnMinionInterval秒生成一个小兵，直到生成MinionPerGroup个小兵
	GetWorldTimerManager().SetTimer(
		SpawnMinionTimerHandle,
		FTimerDelegate::CreateLambda([this, &NumSpawnedThisRound]()
		{
			SpawnOrFindOneMinion();
			++NumSpawnedThisRound;
			if (NumSpawnedThisRound >= MinionPerGroup)
			{
				GetWorldTimerManager().ClearTimer(SpawnMinionTimerHandle);
			}
		}),
		SpawnMinionInterval,
		true);
	
}

void AMinionBarrack::SpawnMinionsToPool()
{
	FTransform SpawnPointTransform = GetActorTransform();
	if (const APlayerStart* SpawnPoint = GetNextSpawnPoint())
	{
		SpawnPointTransform = SpawnPoint->GetActorTransform();
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

