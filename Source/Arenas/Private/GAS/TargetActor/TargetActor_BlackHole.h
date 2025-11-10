// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "NiagaraSystem.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_BlackHole.generated.h"

class UNiagaraComponent;
class USphereComponent;

UCLASS()
class ARENAS_API ATargetActor_BlackHole : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetActor_BlackHole();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ConfigureBlackHoleTargetActor(float InBlackHoleRadius, float InPullSpeed, float InBlackHoleDuration, const FGenericTeamId& OwningTeamId);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
	virtual void StartTargeting(UGameplayAbility* Ability) override;	// 开始选择目标
	virtual void ConfirmTargetingAndContinue() override;	// 确认目标选择并继续
	virtual void CancelTargeting() override;				// 取消目标选择
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* DetectionSphereComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UNiagaraComponent* VFXComp;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UNiagaraSystem* BlackHoleLinkVFX;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FName BlackHoleVFXOriginParamName = TEXT("Origin");	// 黑洞特效中心位置参数名称

	

	UPROPERTY(ReplicatedUsing = OnRep_BlackHoleRadius)
	float BlackHoleRadius;		// 黑洞半径

	float PullSpeed;					// 吸引速度
	float BlackHoleDuration;			// 黑洞持续时间

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
	
	FTimerHandle BlackHoleDurationTimerHandle;

	UFUNCTION()
	void OnRep_BlackHoleRadius();

	UFUNCTION()
	void ActorInBlackHoleArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ActorOutBlackHoleArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void TryAddTarget(AActor* OtherTarget);
	void TryRemoveTarget(AActor* OtherTarget);

	void StopBlackHole();

	UPROPERTY()
	TMap<AActor*, UNiagaraComponent*> ActorInRangeMap;
};
