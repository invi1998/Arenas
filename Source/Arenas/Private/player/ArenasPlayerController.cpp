// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenasPlayerController.h"

#include "ArenasPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/ArenasUserWidget.h"

void AArenasPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ArenasPlayerCharacter = Cast<AArenasPlayerCharacter>(InPawn);
	if (ArenasPlayerCharacter)
	{
		ArenasPlayerCharacter->ServerSideInit();
	}
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

void AArenasPlayerController::SpawnPlayerUIWidget()
{
	if (!IsLocalPlayerController()) return;

	PlayerUIWidget = CreateWidget<UArenasUserWidget>(this, PlayerUIWidgetClass.Get());
	if (PlayerUIWidget)
	{
		PlayerUIWidget->AddToViewport();
	}
	
}
