// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "DefenseTowerCharacter.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "AI/ArenasTowerAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "player/ArenasPlayerCharacter.h"
#include "player/ArenasPlayerController.h"


ADefenseTowerCharacter::ADefenseTowerCharacter()
{

	GroundDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("GroundDecalComponent"));
	GroundDecalComponent->SetupAttachment(GetRootComponent());

	AttackRangeSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphereComponent"));
	AttackRangeSphereComponent->SetupAttachment(GetRootComponent());
	AttackRangeSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADefenseTowerCharacter::ActorEnterTowerAttackRange);
	AttackRangeSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ADefenseTowerCharacter::ActorExitTowerAttackRange);

}

void ADefenseTowerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 仅在服务器上执行
	if (!HasAuthority()) return;
	
	TowerAIController = Cast<AArenasTowerAIController>(NewController);
	if (TowerAIController)
	{
		if (TowerAIController->GetBlackboardComponent())
		{
			TowerAIController->GetBlackboardComponent()->SetValueAsFloat(DefenseTowerAttackRange, DefaultAttackRange);
			
		}
	}

	
}

void ADefenseTowerCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);
	if (TowerAIController)
	{
		TowerAIController->SetGenericTeamId(InTeamID);
	}
}

void ADefenseTowerCharacter::SetDefenseTowerFaceGoal(AActor* NewFaceGoal)
{
	if (NewFaceGoal && TowerAIController)
	{
		TowerAIController->GetBlackboardComponent()->SetValueAsObject(TowerDefaultFaceGoalName, NewFaceGoal);
	}
}

void ADefenseTowerCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ADefenseTowerCharacter, DefaultAttackRange))
	{
		FVector DecalSize = GroundDecalComponent->DecalSize;
		GroundDecalComponent->DecalSize = FVector(DecalSize.X, DefaultAttackRange, DefaultAttackRange);
	}
}

void ADefenseTowerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 隐藏贴花
	GroundDecalComponent->SetVisibility(false);
}

void ADefenseTowerCharacter::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}

void ADefenseTowerCharacter::OnFinishDeathAnimation()
{
	Destroy();
}

void ADefenseTowerCharacter::OnDeath()
{
	// 防御塔消亡
	for (AActor* EnemyHero : EnemyTeamHeroesInRange)
	{
		TowerStopAttack(EnemyHero);
		if (UArenasAbilitySystemComponent* HeroASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(EnemyHero))
		{
			HeroASC->OnActorDeath.RemoveAll(this);
		}
	}
	EnemyTeamHeroesInRange.Empty();

	for (AActor* EnemyMinion : EnemyTeamMinionsInRange)
	{
		// 清空绑定
		if (UArenasAbilitySystemComponent* MinionASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(EnemyMinion))
		{
			MinionASC->OnActorDeath.RemoveAll(this);
		}
	}
	EnemyTeamMinionsInRange.Empty();
	
	for (AActor* SameTeamHero : SameTeamHeroInRange)
	{
		// 清空绑定
		if (UArenasAbilitySystemComponent* HeroASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(SameTeamHero))
		{
			HeroASC->OnAnyDamageTaken.RemoveAll(this);
		}
	}
}

void ADefenseTowerCharacter::PickSkinBasedOnTeamID()
{
	if (TowerMaterialMap.Contains(GetGenericTeamId()))
	{
		if (UMaterialInstance* MaterialInstance = TowerMaterialMap[GetGenericTeamId()])
		{
			if (GetMesh())
			{
				GetMesh()->SetMaterial(0, MaterialInstance);
			}
		}
	}
}

void ADefenseTowerCharacter::TowerBeginAttack(AActor* Actor, bool bIsAttackTarget)
{
	if (!HasAuthority()) return;
	
	// 如果感知到的目标是玩家角色，并且该角色是当前玩家客户端控制的角色，则进行处理
	if (!UArenasBlueprintFunctionLibrary::IsHeroActor(Actor)) return;
	if (!UArenasBlueprintFunctionLibrary::IsAlive(Actor)) return;
	// 通知玩家角色绘制防御塔攻击范围贴花
	if (AArenasPlayerCharacter* PlayerCharacter = Cast<AArenasPlayerCharacter>(Actor))
	{
		if (AArenasPlayerController* PlayerController = Cast<AArenasPlayerController>(PlayerCharacter->GetController()))
		{
			PlayerController->DrawDefenseTowerRangeDecal(GetFName(), GetActorLocation(), DefaultAttackRange, bIsAttackTarget);
		}
	}
	
}

void ADefenseTowerCharacter::TowerStopAttack(AActor* Actor)
{
	if (!HasAuthority()) return;
	if (!UArenasBlueprintFunctionLibrary::IsHeroActor(Actor)) return;
	// 通知玩家角色隐藏防御塔攻击范围贴花
	if (AArenasPlayerCharacter* PlayerCharacter = Cast<AArenasPlayerCharacter>(Actor))
	{
		if (AArenasPlayerController* PlayerController = Cast<AArenasPlayerController>(PlayerCharacter->GetController()))
		{
			// 获取防御塔的唯一标识符
			FName TowerName = GetFName();
			PlayerController->ClearDefenseTowerRangeDecal(TowerName);
		}
		
	}
}

void ADefenseTowerCharacter::WhileSameTeamHeroInRangeTakeDamage(AActor* DamageSourceActor, float DamageValue)
{
	if (!HasAuthority()) return;
	if (!DamageSourceActor) return;
	// 如果当前防御塔已经有攻击目标，并且该目标是英雄角色，则不进行切换
	if (UArenasBlueprintFunctionLibrary::IsHeroActor(CurrentTargetActor)) return;
	// 如果伤害来源角色不是英雄角色或者不是敌方英雄角色或者该角色已经不再防御塔范围内，则直接返回
	if (!EnemyTeamHeroesInRange.Contains(DamageSourceActor)) return;
	if (UArenasBlueprintFunctionLibrary::IsAlive(DamageSourceActor))
	{
		// 如果伤害来源角色仍然存活，则通知防御塔的AI控制器将该角色设为当前攻击目标
		if (TowerAIController)
		{
			TowerAIController->SetCurrentTargetActor(DamageSourceActor);
			TowerBeginAttack(DamageSourceActor, true);
		}
	}
}

void ADefenseTowerCharacter::UpdateTowerAttackTargetOnEnemyDeath(AActor* DeadActor)
{
	if (DeadActor)
	{
		if (UArenasAbilitySystemComponent* DeadActorASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(DeadActor))
		{
			DeadActorASC->OnActorDeath.RemoveAll(this);
		}
		// 从防御塔的敌方目标列表中移除该死亡角色
		if (EnemyTeamHeroesInRange.Contains(DeadActor))
		{
			EnemyTeamHeroesInRange.Remove(DeadActor);
		}
		if (EnemyTeamMinionsInRange.Contains(DeadActor))
		{
			EnemyTeamMinionsInRange.Remove(DeadActor);
		}
		TowerStopAttack(DeadActor);
	}
	if (TowerAIController)
	{
		if (AActor* CurrentAITarget = TowerAIController->GetCurrentTargetActor())
		{
			// 如果当前AI攻击目标就是传入的死亡角色，则需要切换攻击目标
			if (DeadActor == CurrentAITarget)
			{
				CurrentTargetActor = nullptr;
				SelectAttackTarget();
			}
		}
	}
}

void ADefenseTowerCharacter::SelectAttackTarget()
{
	if (!HasAuthority()) return;
	// 如果当前已经有攻击目标，则不进行选择
	if (CurrentTargetActor)
	{
		for (AActor* EnemyHero : EnemyTeamHeroesInRange)
		{
			// 通知范围内的敌方英雄角色绘制防御塔攻击范围贴花
			if (EnemyHero == CurrentTargetActor)
			{
				TowerBeginAttack(EnemyHero, true);
			}
			else
			{
				TowerBeginAttack(EnemyHero, false);
			}
		}
		return;
	}
	
	// 优先选择敌方小兵作为攻击目标
	if (EnemyTeamMinionsInRange.Num() > 0)
	{
		if (TowerAIController)
		{
			TowerAIController->SetCurrentTargetActor(EnemyTeamMinionsInRange[0]);
			CurrentTargetActor = EnemyTeamMinionsInRange[0];
		}
	}
	else
	{
		if (EnemyTeamHeroesInRange.Num() > 0)
		{
			if (TowerAIController)
			{
				TowerAIController->SetCurrentTargetActor(EnemyTeamHeroesInRange[0]);
				CurrentTargetActor = EnemyTeamHeroesInRange[0];
			}
		}
		else
		{
			// 如果没有敌方小兵和英雄角色，则清空当前攻击目标
			if (TowerAIController)
			{
				CurrentTargetActor = nullptr;
				TowerAIController->SetCurrentTargetActor(nullptr);
			}
		}
	}

	for (AActor* EnemyHero : EnemyTeamHeroesInRange)
	{
		// 通知范围内的敌方英雄角色绘制防御塔攻击范围贴花
		if (EnemyHero == CurrentTargetActor)
		{
			TowerBeginAttack(EnemyHero, true);
		}
		else
		{
			TowerBeginAttack(EnemyHero, false);
		}
	}
	
}

void ADefenseTowerCharacter::ActorEnterTowerAttackRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	// 判断是否是同队伍角色
	bool bIsHero = UArenasBlueprintFunctionLibrary::IsHeroActor(OtherActor);
	if (IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherActor))
	{
		if (OtherTeamAgent->GetGenericTeamId() == GetGenericTeamId())
		{
			if (bIsHero)
			{
				SameTeamHeroInRange.AddUnique(OtherActor);
				// 同时己方防御塔内的英雄角色需要监听伤害事件，以便在英雄角色受到伤害时通知防御塔
				if (UArenasAbilitySystemComponent* HeroASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(OtherActor))
				{
					HeroASC->OnAnyDamageTaken.AddUObject(this, &ADefenseTowerCharacter::WhileSameTeamHeroInRangeTakeDamage);
				}
			}
		}
		else
		{
			if (UArenasAbilitySystemComponent* EnemyASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(OtherActor))
			{
				// 对防御塔内的敌方角色进行角色死亡状态监听，以便在敌方角色死亡时将其从防御塔的敌方目标列表中移除以及切换防御塔攻击目标
				EnemyASC->OnActorDeath.AddUObject(this, &ADefenseTowerCharacter::UpdateTowerAttackTargetOnEnemyDeath);
			}
			
			// 敌对队伍角色
			if (bIsHero)
			{
				EnemyTeamHeroesInRange.AddUnique(OtherActor);
			}
			else
			{
				EnemyTeamMinionsInRange.AddUnique(OtherActor);
			}
		}
	}

	SelectAttackTarget();
}

void ADefenseTowerCharacter::ActorExitTowerAttackRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
	// 判断是否是同队伍角色
	bool bIsHero = UArenasBlueprintFunctionLibrary::IsHeroActor(OtherActor);
	if (IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherActor))
	{
		if (OtherTeamAgent->GetGenericTeamId() == GetGenericTeamId())
		{
			if (bIsHero)
			{
				if (SameTeamHeroInRange.Contains(OtherActor))
				{
					SameTeamHeroInRange.Remove(OtherActor);
				}
				
				// 同时移除对该角色伤害事件的监听
				if (UArenasAbilitySystemComponent* HeroASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(OtherActor))
				{
					HeroASC->OnAnyDamageTaken.RemoveAll(this);
				}
			}
		}
		else
		{
			if (UArenasAbilitySystemComponent* EnemyASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(OtherActor))
			{
				EnemyASC->OnActorDeath.RemoveAll(this);
			}
			
			if (bIsHero)
			{
				if (EnemyTeamHeroesInRange.Contains(OtherActor))
				{
					EnemyTeamHeroesInRange.Remove(OtherActor);
				}
				
			}
			else
			{
				if (EnemyTeamMinionsInRange.Contains(OtherActor))
				{
					EnemyTeamMinionsInRange.Remove(OtherActor);
				}
			}
		}
	}

	TowerStopAttack(OtherActor);

	// 如果当前攻击目标是离开范围的角色，则需要切换攻击目标
	if (TowerAIController)
	{
		if (AActor* CurrentAITarget = TowerAIController->GetCurrentTargetActor())
		{
			if (OtherActor == CurrentAITarget)
			{
				CurrentTargetActor = nullptr;
				SelectAttackTarget();
			}
		}
	}
}




