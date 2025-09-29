// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GenericTeamAgentInterface.h"
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

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	FGameplayTagContainer TriggerGameplayCueTags;

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	TEnumAsByte<ETeamAttitude::Type> TargetTeamType = ETeamAttitude::Hostile;

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	float SphereSweepRadius = 60.f;

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	bool bIgnoreSelf = true;

	void SendLocalGameplayCue(const FHitResult& HitResult) const;
	
};
