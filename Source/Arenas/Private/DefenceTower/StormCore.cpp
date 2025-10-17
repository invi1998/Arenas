// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "StormCore.h"

#include "AI/ArenasTowerAIController.h"
#include "Camera/CameraComponent.h"


// Sets default values
AStormCore::AStormCore()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ViewCam = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCam"));
	ViewCam->SetupAttachment(GetRootComponent());
}

void AStormCore::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (HasAuthority())
	{
		CoreAIController = Cast<AArenasTowerAIController>(NewController);
	}
	
}

void AStormCore::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);

	if (CoreAIController)
	{
		CoreAIController->SetGenericTeamId(InTeamID);
	}
}

// Called when the game starts or when spawned
void AStormCore::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStormCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AStormCore::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

