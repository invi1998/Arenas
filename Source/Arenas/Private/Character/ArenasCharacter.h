// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interface/PawnUIInterface.h"
#include "ArenasCharacter.generated.h"

class UArenasUserWidget;
class UWidgetComponent;
class UArenasAttributeSet;
class UArenasAbilitySystemComponent;

UCLASS()
class ARENAS_API AArenasCharacter : public ACharacter, public IAbilitySystemInterface, public IPawnUIInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArenasCharacter();
	void ServerSideInit();
	void ClientSideInit();
	bool IsLocallyControlledByPlayer() const;

	// 当角色被控制器控制时调用(只会在服务端被调用)
	virtual void PossessedBy(AController* NewController) override;

	// Interface IPawnUIInterface Begin
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	// Interface IPawnUIInterface End

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/********************************************************************************************/
	/*								 	Gameplay Ability										*/
	/********************************************************************************************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	UArenasAbilitySystemComponent* ArenasAbilitySystemComponent;

	UPROPERTY()
	UArenasAttributeSet* ArenasAttributeSet;	// 属性集


	/********************************************************************************************/
	/*								 		UI													*/
	/********************************************************************************************/
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	UWidgetComponent* OverheadWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UArenasUserWidget> OverheadWidgetClass;

	UPROPERTY()
	UArenasUserWidget* OverheadWidget;

	void SpawnOverheadWidgetComponent();
	
};
