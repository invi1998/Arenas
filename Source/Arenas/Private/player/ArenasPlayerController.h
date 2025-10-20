// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Interface/CombatTextInterface.h"
#include "ArenasPlayerController.generated.h"

class AAttackRangeDecal;
class UInputAction;
class UInputMappingContext;
class UArenasUserWidget;
class AArenasPlayerCharacter;
class UMaterialInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamPlayerKillChangedSignature, int32, NewKillCount);

/**
 * 
 */
UCLASS()
class ARENAS_API AArenasPlayerController : public APlayerController, public IGenericTeamAgentInterface, public ICombatTextInterface
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

	virtual void ShowCombatText(float ValueNumber, EArenasComboTextType TextType, const FVector& HitLocation) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ShowCombatText(float ValueNumber, EArenasComboTextType TextType, const FVector& HitLocation);

	void MatchFinished(AActor* ViewTarget, FGenericTeamId LooseTeamID);

	UPROPERTY(BlueprintAssignable, Category="Team")
	FOnTeamPlayerKillChangedSignature OnTeamOnePlayerKillChanged;

	UPROPERTY(BlueprintAssignable, Category="Team")
	FOnTeamPlayerKillChangedSignature OnTeamTwoPlayerKillChanged;

	void UpdateTeamOnePlayerKillCount(int32 NewKillCount);
	void UpdateTeamTwoPlayerKillCount(int32 NewKillCount);
	
private:
	UPROPERTY(EditDefaultsOnly, Category="View")
	float FinishedMatchViewBlendTimeDuration = 3.5f;

	UPROPERTY(EditDefaultsOnly, Category="View")
	float FinishedViewBlendTimeDuration = 3.5f;
	
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

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ToggleGameplayMenuInputAction;

	UFUNCTION()
	void OnShopToggleActionTriggered();

	UFUNCTION()
	void OnToggleGameplayMenuActionTriggered();

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

	UFUNCTION(Client, Reliable)
	void Client_ShowCombatText(float ValueNumber, EArenasComboTextType TextType, const FVector& HitLocation);

	UFUNCTION(Client, Reliable)
	void Client_MatchFinished(AActor* ViewTarget, FGenericTeamId LooseTeamID);

	bool bIsWin;
	void ShowMatchFinishedState();
	
};
