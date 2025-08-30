// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "BTTask_SendInputToAbilitySystem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_SendInputToAbilitySystem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIController->GetPawn()))
		{
			OwnerASC->PressInputID(static_cast<int32>(AbilityInputID));
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;

	
}
