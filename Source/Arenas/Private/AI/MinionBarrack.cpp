// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MinionBarrack.h"

#include "Character/Minion/MinionCharacter.h"
#include "GameFramework/PlayerStart.h"


// Sets default values
AMinionBarrack::AMinionBarrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();

	SpawnMinionsToPool(InitialMinionPoolSize);
	
}

// Called every frame
void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const APlayerStart* AMinionBarrack::GetNextSpawnPoint()
{
	if (MinionSpawnPoints.Num() > 0)
	{
		++NextSpawnSpotIndex;

		if (NextSpawnSpotIndex >= MinionSpawnPoints.Num())
		{
			NextSpawnSpotIndex = 0;
		}

		return MinionSpawnPoints[NextSpawnSpotIndex];
	}

	return nullptr;
}

void AMinionBarrack::SpawnMinionsToPool(int32 NumMinions)
{
	for (int32 i = 0; i < NumMinions; ++i)
	{
		FTransform SpawnPointTransform = GetActorTransform();
		if (const APlayerStart* SpawnPoint = GetNextSpawnPoint())
		{
			SpawnPointTransform = SpawnPoint->GetActorTransform();
		}

		AMinionCharacter* NewMinion = GetWorld()->SpawnActorDeferred<AMinionCharacter>(
			MinionClass,
			SpawnPointTransform,
			this,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

		if (NewMinion)
		{
			NewMinion->SetGenericTeamId(BarrackTeamID);
			NewMinion->FinishSpawning(SpawnPointTransform);
			SpawnedMinionPool.Add(NewMinion);
		}
		
	}
}

