// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Types/ArenaStructTypes.h"
#include "BTTask_SendInputToAbilitySystem.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UBTTask_SendInputToAbilitySystem : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Ability")
	EArenasAbilityInputID AbilityInputID = EArenasAbilityInputID::None;
	
};
