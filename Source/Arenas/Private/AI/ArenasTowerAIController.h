// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "ArenasTowerAIController.generated.h"

class UBehaviorTree;

UCLASS()
class ARENAS_API AArenasTowerAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArenasTowerAIController();

	virtual void OnPossess(APawn* InPawn) override;

	void SetCurrentTargetActor(AActor* NewTargetActor);
	AActor* GetCurrentTargetActor() const;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void OnOwnerDeadTagChanged(FGameplayTag GameplayTag, int Count);
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName BlackboardKeyName_TargetActor = "TargetActor";
};
