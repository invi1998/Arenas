// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Beam.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class ARENAS_API ATargetActor_Beam : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetActor_Beam();

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UNiagaraComponent* BeamVFXComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* TargetEndDetectionSphere;		// 目标终点检测球体

	UPROPERTY()
	const AActor* AvatarActor;		// 持有该TargetActor的角色
	
};
