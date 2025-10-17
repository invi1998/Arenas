// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "ArenasAIController.generated.h"

struct FActorPerceptionUpdateInfo;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class UBehaviorTree;


UCLASS()
class ARENAS_API AArenasAIController : public AAIController
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	AArenasAIController(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;

	void SetSight(float SightRadius, float LoseSightRadius, float PeripheralVisionAngleDegrees);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName BlackboardKeyName_TargetActor = "TargetActor";
	
	// 感知组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	UAIPerceptionComponent* AIPerceptionComponent;

	// 视觉感知配置
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void OnTargetPerceptionUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);

	UFUNCTION()
	void OnPerceptionForgottenTarget(AActor* Actor);

	const AActor* GetCurrentTargetActor() const;
	void SetCurrentTargetActor(AActor* NewTargetActor);

	AActor* GetNextPerceivedActor() const;	// 获取下一个被感知的Actor

	void ForgetActorIfDead(AActor* Actor); // 如果Actor死亡则忘记它

	void OnOwnerDeadTagChanged(FGameplayTag GameplayTag, int Count);
	void OnOwnerStunTagChanged(FGameplayTag GameplayTag, int Count);

	void ClearAndDisablePerception(); // 清除并禁用感知系统
	void EnableAllPerception(); // 启用感知系统

	bool bIsPawnDead = false;

	UPROPERTY(EditDefaultsOnly, Category="Detor Crowd Avoidance Config")
	bool bEnableCrowdAvoidance = true;	// 是否启用人群避让

	// meta = (EditCondition = "bEnableCrowdAvoidance")) 意味着只有当 bEnableCrowdAvoidance 为 true 时，才会显示这个属性（即只有在启用人群避让时，才会显示这个属性，才能进行该项设置）
	// , UIMin = "1", UIMax = "4" 则是设置了该属性的最小值和最大值，让我能在编辑器中可以像滑动条一样调整这个整数值，进而控制人群避让的质量
	UPROPERTY(EditDefaultsOnly, Category="Detor Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance", UIMin = "1", UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality = 4;	// 人群避让质量

	UPROPERTY(EditDefaultsOnly, Category="Detor Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance"))
	float CollisionQueryRange = 600.0f;		// 人群避让范围

};
