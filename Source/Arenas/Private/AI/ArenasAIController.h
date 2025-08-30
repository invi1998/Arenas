// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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
	AArenasAIController();
	virtual void OnPossess(APawn* InPawn);

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

};
