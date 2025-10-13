// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "DefenseTowerCharacter.h"

#include "AI/ArenasAIController.h"
#include "BehaviorTree/BlackboardComponent.h"


void ADefenseTowerCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);
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


