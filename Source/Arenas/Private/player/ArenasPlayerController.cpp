// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenasPlayerController.h"

#include "ArenasPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/ArenasUserWidget.h"

void AArenasPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ArenasPlayerCharacter = Cast<AArenasPlayerCharacter>(InPawn);
	if (ArenasPlayerCharacter)
	{
		ArenasPlayerCharacter->ServerSideInit();
		ArenasPlayerCharacter->SetGenericTeamId(TeamID);
	}
}

void AArenasPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenasPlayerController, TeamID);
}

void AArenasPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);
	ArenasPlayerCharacter = Cast<AArenasPlayerCharacter>(P);
	if (ArenasPlayerCharacter)
	{
		ArenasPlayerCharacter->ClientSideInit();
		SpawnPlayerUIWidget();
	}
	
}

void AArenasPlayerController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	TeamID = InTeamID;
}

FGenericTeamId AArenasPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void AArenasPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		InputSubsystem->RemoveMappingContext(UIInputMappingContext);
		InputSubsystem->AddMappingContext(UIInputMappingContext, 1);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ShopToggleInputAction, ETriggerEvent::Triggered, this, &AArenasPlayerController::OnShopToggleActionTriggered);
	}
}

void AArenasPlayerController::SpawnPlayerUIWidget()
{
	if (!IsLocalPlayerController()) return;

	PlayerUIWidget = CreateWidget<UArenasUserWidget>(this, PlayerUIWidgetClass.Get());
	if (PlayerUIWidget)
	{
		PlayerUIWidget->AddToViewport();
		PlayerUIWidget->ConfigureAbilities(ArenasPlayerCharacter->GetAbilities());
	}
	
}

void AArenasPlayerController::OnShopToggleActionTriggered()
{
	if (PlayerUIWidget)
	{
		PlayerUIWidget->ToggleShopPopup();
	}
}
