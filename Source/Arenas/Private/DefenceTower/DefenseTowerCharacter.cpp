// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "DefenseTowerCharacter.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "AI/ArenasAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/DecalComponent.h"
#include "player/ArenasPlayerCharacter.h"
#include "player/ArenasPlayerController.h"


ADefenseTowerCharacter::ADefenseTowerCharacter()
{

	GroundDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("GroundDecalComponent"));
	GroundDecalComponent->SetupAttachment(RootComponent);

}

void ADefenseTowerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 仅在服务器上执行
	if (!HasAuthority()) return;
	
	OwnerAIC = Cast<AArenasAIController>(NewController);
	if (OwnerAIC)
	{
		// 设置AIC的感知范围为防御塔的攻击范围
		OwnerAIC->SetSight(DefaultAttackRange, DefaultAttackRange, 180.f);
		
		// 监听AI控制器的感知更新事件
		OwnerAIC->OnPerceptionUpdated.AddUObject(this, &ADefenseTowerCharacter::TowerBeginAttack);
		OwnerAIC->OnPerceptionForgotten.AddUObject(this, &ADefenseTowerCharacter::TowerStopAttack);
	}
}

void ADefenseTowerCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);
	if (AArenasAIController* AIController = Cast<AArenasAIController>(GetController()))
	{
		AIController->SetGenericTeamId(InTeamID);
	}
}

void ADefenseTowerCharacter::SetTowerAttackRange(float NewRange)
{
	if (AArenasAIController* AIController = Cast<AArenasAIController>(GetController()))
	{
		if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsFloat(DefenseTowerAttackRange, NewRange);
		}
	}
}

void ADefenseTowerCharacter::SetDefenseTowerFaceGoal(AActor* NewFaceGoal)
{
	if (AArenasAIController* AIController = Cast<AArenasAIController>(GetController()))
	{
		if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsObject(TowerDefaultFaceGoalName, NewFaceGoal);
		}
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




