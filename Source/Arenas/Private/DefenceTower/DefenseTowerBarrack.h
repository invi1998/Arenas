// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "DefenseTowerBarrack.generated.h"

class ADefenseTowerCharacter;

UCLASS()
class ARENAS_API ADefenseTowerBarrack : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADefenseTowerBarrack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 防御塔所属的队伍ID
	UPROPERTY(EditAnywhere, Category = "Team")
	FGenericTeamId TowerTeamID;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<ADefenseTowerCharacter> DefenseTowerClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<APlayerStart *> TowerSpawnPoints;

	UPROPERTY()
	TArray<ADefenseTowerCharacter *> SpawnedTowers;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	AActor* DefaultFaceGoalActor; // 默认面向的目标

	void SpawnDefenseTowers();
	
};
