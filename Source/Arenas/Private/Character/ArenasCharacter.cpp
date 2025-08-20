// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenasCharacter.h"

#include "Components/WidgetComponent.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "Widgets/ArenasUserWidget.h"


// Sets default values
AArenasCharacter::AArenasCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ArenasAbilitySystemComponent = CreateDefaultSubobject<UArenasAbilitySystemComponent>(TEXT("ArenasAbilitySystemComponent"));
	// ArenasAbilitySystemComponent->SetIsReplicated(true);
	// ArenasAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ArenasAttributeSet = CreateDefaultSubobject<UArenasAttributeSet>(TEXT("ArenasAttributeSet"));

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComponent"));
	OverheadWidgetComponent->SetupAttachment(GetRootComponent());
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

	SpawnOverheadWidgetComponent();
	
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

void AArenasCharacter::SpawnOverheadWidgetComponent()
{
	if (OverheadWidgetComponent && OverheadWidgetClass)
	{
		OverheadWidgetComponent->SetWidgetClass(OverheadWidgetClass);
		OverheadWidgetComponent->SetDrawSize(FVector2D(200.0f, 50.0f));
		OverheadWidgetComponent->SetVisibility(true);
		OverheadWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		OverheadWidget = Cast<UArenasUserWidget>(OverheadWidgetComponent->GetUserWidgetObject());
		if (OverheadWidget)
		{
			OverheadWidget->InitOverheadWidget(this);
		}
	}
}

