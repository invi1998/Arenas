// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "DefenseTowerBarrack.h"

#include "DefenseTowerCharacter.h"
#include "GameFramework/PlayerStart.h"


// Sets default values
ADefenseTowerBarrack::ADefenseTowerBarrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADefenseTowerBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// 仅在服务器上执行，生成防御塔
		SpawnDefenseTowers();
	}
	
}

// Called every frame
void ADefenseTowerBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADefenseTowerBarrack::SpawnDefenseTowers()
{
	if (HasAuthority())
	{
		FTransform SpawnPointTransform = GetActorTransform();
		for (const APlayerStart* SpawnPoint : TowerSpawnPoints)
		{
			SpawnPointTransform = SpawnPoint->GetActorTransform();
			// 在每个生成点生成一个防御塔
			if (DefenseTowerClass)
			{
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				if (ADefenseTowerCharacter* Tower = GetWorld()->SpawnActor<ADefenseTowerCharacter>(DefenseTowerClass, SpawnPointTransform, SpawnParameters))
				{
					Tower->SetGenericTeamId(TowerTeamID);
					Tower->FinishSpawning(SpawnPointTransform);
					Tower->SetDefenseTowerFaceGoal(DefaultFaceGoalActor);
					SpawnedTowers.Add(Tower);
				}
			}
		}
	}
}

