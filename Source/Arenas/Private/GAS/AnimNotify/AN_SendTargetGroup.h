// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SendTargetGroup.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UAN_SendTargetGroup : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	TArray<FName> TargetSocketNames;

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta=(Categories="Event", GameplayTagFilter = "Event"))
	FGameplayTag EventTag;
	
};
