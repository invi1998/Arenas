// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_FallingStar.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class ARENAS_API ATargetActor_FallingStar : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetActor_FallingStar();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ConfigureFallingStarTargetActor(float InAOERadius, float InLoopDuration, float InSpawnRate, float InSpawnHeight, float InFallingSpeed, const FGenericTeamId& OwningTeamId);
	
	virtual void StartTargeting(UGameplayAbility* Ability) override;	// 开始选择目标
	virtual void BeginDestroy() override;
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* DetectionSphereComponent;

	UPROPERTY(ReplicatedUsing = OnRep_IsActiveVFX)
	bool bIsActiveVFX = false;	// 特效是否处于激活状态

	UFUNCTION()
	void OnRep_IsActiveVFX();
	
	UPROPERTY(Replicated)
	float AOERadius = 1200.f;		// 范围伤害半径

	UPROPERTY(Replicated)
	float LoopDuration;		// 循环持续时间

	UPROPERTY(Replicated)
	float SpawnRate;			// 生成速率

	UPROPERTY(Replicated)
	float SpawnHeight = 1500.f;		// 生成高度

	UPROPERTY(Replicated)
	float FallingSpeed = 3000.f;	// 下落速度

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* FallingStarVFXComp;

	FTimerHandle FallingStarSpawnTimerHandle;	// 星落生成定时器
	FTimerHandle FallingStarImpactTimerHandle;		// 星落冲击定时器（每隔一段时间检查一次星落伤害）

	UPROPERTY(EditDefaultsOnly, Category = "VFX Param Name")
	FName VFXParamName_AOERadius = "AOE Radius";

	UPROPERTY(EditDefaultsOnly, Category = "VFX Param Name")
	FName VFXParamName_LoopDuration = "Loop Duration";

	UPROPERTY(EditDefaultsOnly, Category = "VFX Param Name")
	FName VFXParamName_Offset = "Offset";

	UPROPERTY(EditDefaultsOnly, Category = "VFX Param Name")
	FName VFXParamName_SpawnRate = "Spawn Rate";

	UPROPERTY(EditDefaultsOnly, Category = "VFX Param Name")
	FName VFXParamName_Velocity = "Velocity";

	UPROPERTY(Replicated)
	const AActor* AvatarActor;		// 持有该TargetActor的角色
	
	void StopFallingStar();
	void DoTargetCheckAndReport();

	bool ShouldReportActorAsTarget(AActor* CheckedActor) const;

};
