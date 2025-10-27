// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "TargetActor_Beam.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class ARENAS_API ATargetActor_Beam : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetActor_Beam();
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ConfigureTargetSetting(float InTargetRange, float InDetectionCylinderRadius, float InTargetingInterval, FGenericTeamId OwningTeamId, bool bShouldDrawDebug = false);

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void StartTargeting(UGameplayAbility* Ability) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UNiagaraComponent* BeamVFXComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* TargetEndDetectionSphere;		// 目标终点检测球体

	UPROPERTY(VisibleDefaultsOnly, Category = "VFX")
	FName BeamFXLengthParamName = TEXT("Length");		// 激光束特效长度参数名称

	UPROPERTY(Replicated)
	float TargetRange;		// 激光束目标范围

	UPROPERTY(Replicated)
	float DetectionCylinderRadius;	// 检测球体半径

	UPROPERTY()
	float TargetingInterval;		// 目标选择间隔时间

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY()
	bool bDrawDebug;		// 是否绘制调试信息

	UPROPERTY(Replicated)
	const AActor* AvatarActor;		// 持有该TargetActor的角色

	FTimerHandle PeriodicalTargetingTimerHandle;		// 定时目标选择定时器

	void DoTargetCheckAndReport();
	void UpdateTargetTrace();
};
