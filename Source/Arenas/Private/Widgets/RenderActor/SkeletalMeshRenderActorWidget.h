// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "RenderActorWidget.h"
#include "SkeletalMeshRenderActorWidget.generated.h"

class ASkeletalMeshRenderActor;
class UAnimInstance;
/**
 * 
 */
UCLASS()
class ARENAS_API USkeletalMeshRenderActorWidget : public URenderActorWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	void SetMeshAnimInstance();
	
	virtual void SpawnRenderActor() override;
	virtual ARenderActor* GetRenderActor() const override;
	
	UPROPERTY(EditDefaultsOnly, Category = "SkeletalMeshRenderActorWidget")
	TSubclassOf<ASkeletalMeshRenderActor> SkeletalMeshRenderActorClass;

	UPROPERTY()
	ASkeletalMeshRenderActor* SkeletalMeshRenderActor;
	
};
