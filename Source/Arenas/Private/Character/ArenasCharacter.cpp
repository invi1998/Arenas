// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenasCharacter.h"

#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"


// Sets default values
AArenasCharacter::AArenasCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ArenasAbilitySystemComponent = CreateDefaultSubobject<UArenasAbilitySystemComponent>(TEXT("ArenasAbilitySystemComponent"));
	// ArenasAbilitySystemComponent->SetIsReplicated(true);
	// ArenasAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ArenasAttributeSet = CreateDefaultSubobject<UArenasAttributeSet>(TEXT("ArenasAttributeSet"));
}

void AArenasCharacter::ServerSideInit()
{
	ArenasAbilitySystemComponent->InitAbilityActorInfo(this, this);
	ArenasAbilitySystemComponent->ApplyInitialEffects();
}

void AArenasCharacter::ClientSideInit()
{
	ArenasAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UPawnUIComponent* AArenasCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

void AArenasCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArenasCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArenasCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* AArenasCharacter::GetAbilitySystemComponent() const
{
	return ArenasAbilitySystemComponent;
}

