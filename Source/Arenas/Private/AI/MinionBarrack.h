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

	// 预生成的兵营小兵池
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<AMinionCharacter*> SpawnedMinionPool;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AMinionCharacter> MinionClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<APlayerStart *> MinionSpawnPoints;

	int32 NextSpawnSpotIndex = -1;

	const APlayerStart* GetNextSpawnPoint();
	void SpawnOrFindOneMinion();

	void SpawnMinionGroup();
	void SpawnMinionsToPool();
	AMinionCharacter* GetNextAvailableMinionFromPool() const;

	// 生成一组小兵的定时器
	FTimerHandle SpawnGroupMinionTimerHandle;

	// 生成小兵的定时器（比如一组里面的小兵不是同时生成的，而是间隔一段时间生成）
	FTimerHandle SpawnMinionTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 MinionPerGroup = 5; // 每组生成的小兵数量

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnGroupInterval = 30.f; // 小兵生成间隔，单位秒

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnMinionInterval = 1.f; // 小兵组内生成间隔，单位秒
	
};
