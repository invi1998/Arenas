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

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPerceptionUpdate, AActor*);	// 感知更新事件委托

UCLASS()
class ARENAS_API AArenasAIController : public AAIController
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	AArenasAIController();
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;

	void SetSight(float SightRadius, float LoseSightRadius, float PeripheralVisionAngleDegrees);

	FOnPerceptionUpdate OnPerceptionUpdated;		// 感知更新事件
	FOnPerceptionUpdate OnPerceptionForgotten;		// 感知遗忘事件

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

};
