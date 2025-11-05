// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Around.generated.h"

/**
 * 目标Actor——环绕目标
 */

class USphereComponent;

UCLASS()
class ARENAS_API ATargetActor_Around : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetActor_Around();

	void ConfigureDetection(float DetectionRadius, const FGenericTeamId& InTeamId, const FGameplayTag& InLocalGameplayCueTag);

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY(VisibleDefaultsOnly, Category = "Comp")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Targeting")
	USphereComponent* DetectionSphere;

	UPROPERTY(ReplicatedUsing = OnRep_TargetDetectionRadiusReplicated)
	float TargetDetectionRadius;

	UFUNCTION()
	void OnRep_TargetDetectionRadiusReplicated();

	UPROPERTY(Replicated)
	FGameplayTag LocalGameplayCueTag;

	UFUNCTION()
	void ActorInDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	

};
