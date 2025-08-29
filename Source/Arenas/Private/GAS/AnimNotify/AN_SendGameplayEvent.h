// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SendGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UAN_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;


private:
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta=(Categories="Event", GameplayTagFilter = "Event"))
	FGameplayTag EventTag;

	// 重写该函数以在编辑器中显示自定义名称（如动画通知的名称），这里我们显示的是EventTag里的最后一个层级的名称
	virtual FString GetNotifyName_Implementation() const override;

};
