// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Blueprint/UserWidget.h"
#include "Types/ArenaStructTypes.h"
#include "ArenasUserWidget.generated.h"

class UCrosshairWidget;
class UMatchFinished;
class UCanvasPanel;
class UWidgetSwitcher;
class UGameplayMenu;
class UMatchStatWidget;
class UInventoryWidget;
class UShopWidget;
class UHeroUserWidget;
class UAbilityListView;
class UGameplayAbility;
class UPawnUIComponent;
class UArenasAbilitySystemComponent;
struct FGameplayAttribute;
class UPlayerUIComponent;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitOverheadWidget(AActor* InActor);
	void ConfigureAbilities(const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& InAbilities);

	void ToggleShopPopup();

	UFUNCTION()
	void ToggleGameplayMenu();
	
	void ShowGameplayMenu();
	void SetGameplayMenuTitle(const FText& InTitleText);
	void ShowMatchResult(bool bIsWin);

protected:
	// NativeOnInitialized 先执行，NativeConstruct 后执行
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta=(DisplayName = "On Owning Player UI Component Initialized"))
	void BP_OnOwningPlayerUIComponentInitialized(UPlayerUIComponent* InPlayerUIComponent);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta=(DisplayName = "On OverHead Created Widget Initialized"))
	void BP_OnOverheadWidgetInitialized(UPawnUIComponent* InOverHeadUIComponent, ETeamAttitude::Type InTeamAttitude);

private:
	UPROPERTY()
	UArenasAbilitySystemComponent* CachedArenasASC;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	bool bIsPlayerWidget = false;

	UPROPERTY(meta = (BindWidgetOptional))
	UAbilityListView* AbilityListView;

	UPROPERTY(meta = (BindWidgetOptional))
	UHeroUserWidget* HeroUserWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	UShopWidget* ShopWidget;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	UWidgetAnimation* ShopPopupAnim;

	UPROPERTY(meta=(BindWidgetOptional))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	UMatchStatWidget* MatchStatWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	UGameplayMenu* GameplayMenuWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta=(BindWidgetOptional))
	UCanvasPanel* GameplayWidgetsRootPanel;

	UPROPERTY(meta=(BindWidgetOptional))
	UCanvasPanel* GameplayMenuRootPanel;

	UPROPERTY(meta=(BindWidgetOptional))
	UCanvasPanel* MatchResultRootPanel;

	UPROPERTY(meta=(BindWidgetOptional))
	UMatchFinished* MatchFinishedWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	UCrosshairWidget* CrosshairWidget;
	
	void PlayShopPopupAnim(bool bPlayForward);
	void SetOwningPawnInputEnabled(bool bEnabled);
	void SetShowMouseCursor(bool bShow);
	void SetFocusToGameAndUI();
	void SetFocusToGameOnly();
	
};