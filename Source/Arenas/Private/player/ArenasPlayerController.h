// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArenasPlayerController.generated.h"

class UArenasUserWidget;
class AArenasPlayerCharacter;
/**
 * 
 */
UCLASS()
class ARENAS_API AArenasPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// 只会在服务端被调用
	virtual void OnPossess(APawn* InPawn) override;

	// 只会在客户端被调用，同时也会在监听服务器上调用（即在没有专用服务器的情况下的P2P里的客户机上调用）
	virtual void AcknowledgePossession(class APawn* P) override;

private:
	UPROPERTY()
	AArenasPlayerCharacter* ArenasPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UArenasUserWidget> PlayerUIWidgetClass;

	UPROPERTY()
	UArenasUserWidget* PlayerUIWidget;

	void SpawnPlayerUIWidget();
	
};
