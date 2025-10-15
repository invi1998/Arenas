// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "ArenasPlayerController.generated.h"

class AAttackRangeDecal;
class UInputAction;
class UInputMappingContext;
class UArenasUserWidget;
class AArenasPlayerCharacter;
class UMaterialInterface;
/**
 * 
 */
UCLASS()
class ARENAS_API AArenasPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// 只会在服务端被调用
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 只会在客户端被调用，同时也会在监听服务器上调用（即在没有专用服务器的情况下的P2P里的客户机上调用）
	virtual void AcknowledgePossession(class APawn* P) override;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void SetupInputComponent() override;

	void DrawDefenseTowerRangeDecal(const FName& DefenseTowerName, const FVector& Location, float Range, bool bUnderAttack);
	void ClearDefenseTowerRangeDecal(const FName& DefenseTowerName);

private:
	UPROPERTY()
	AArenasPlayerCharacter* ArenasPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UArenasUserWidget> PlayerUIWidgetClass;

	UPROPERTY()
	UArenasUserWidget* PlayerUIWidget;

	void SpawnPlayerUIWidget();

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* UIInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ShopToggleInputAction;

	UFUNCTION()
	void OnShopToggleActionTriggered();

	// Decal 
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	TSubclassOf<AAttackRangeDecal> AttackRangeDecalClass;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	FName DecalDynamicMaterialColorParamName = "Color";
	
	UPROPERTY()
	TMap<FName, TObjectPtr<AAttackRangeDecal>> ActiveDefenseTowerAttackRangeDecals;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	FLinearColor AttackColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	FLinearColor InRangeColor = FLinearColor::Yellow;

	UFUNCTION(Client, Reliable)
	void Client_ShowTowerAttackRangeDecal(const FName& DefenseTowerName, const FVector& GroundLocation, float Range, bool bUnderAttack);

	UFUNCTION(Client, Reliable)
	void Client_HideTowerAttackRangeDecal(const FName& DefenseTowerName);

	
};
