// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasPlayerCharacter.h"


// Sets default values
AArenasPlayerCharacter::AArenasPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AArenasPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArenasPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AArenasPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

