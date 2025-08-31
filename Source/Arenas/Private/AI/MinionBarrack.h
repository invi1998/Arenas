// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "MinionBarrack.generated.h"

// 兵营

class AMinionCharacter;

UCLASS()
class ARENAS_API AMinionBarrack : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMinionBarrack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 兵营所属队伍ID
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FGenericTeamId BarrackTeamID;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 InitialMinionPoolSize = 5; // 初始预生成的小兵数量

	// 预生成的兵营小兵池
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<AMinionCharacter*> SpawnedMinionPool;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AMinionCharacter> MinionClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<APlayerStart *> MinionSpawnPoints;

	int32 NextSpawnSpotIndex = -1;

	const APlayerStart* GetNextSpawnPoint();

	void SpawnMinionsToPool(int32 NumMinions);
	
};
