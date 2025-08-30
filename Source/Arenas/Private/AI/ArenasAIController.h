// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ArenasAIController.generated.h"

class UAISenseConfig_Sight;
class UAIPerceptionComponent;

UCLASS()
class ARENAS_API AArenasAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArenasAIController();
	virtual void OnPossess(APawn* InPawn);

protected:

private:
	// 感知组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	UAIPerceptionComponent* PerceptionComp;

	// 视觉感知配置
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	UAISenseConfig_Sight* SightConfig;

};
