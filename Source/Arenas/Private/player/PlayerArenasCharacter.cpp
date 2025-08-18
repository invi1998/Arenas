// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerArenasCharacter.h"


// Sets default values
APlayerArenasCharacter::APlayerArenasCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerArenasCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerArenasCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerArenasCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

