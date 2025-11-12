// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UPlayerTeamLayoutWidget;
class UAbilityListView;
class ACharacterDisplay;
class AArenasPlayerState;
class UTileView;
struct FPlayerSelection;
class AArenasGameState;
class ALobbyPlayerController;
class UTeamSelectionWidget;
class UUniformGridPanel;
class UArenasButton;
class UWidgetSwitcher;
/**
 * 大厅界面Widget
 */
UCLASS()
class ARENAS_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta=(BindWidget))
	UWidget* TeamSelectionRoot;

	UPROPERTY(meta=(BindWidget))
	UArenasButton* StartHeroSelectionButton;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* TeamSelectionSlotGridPanel;		// 队伍选择槽的网格面板
	
	UPROPERTY(meta=(BindWidget))
	UWidget* HeroSelectionRoot;

	UPROPERTY(meta=(BindWidget))
	UTileView* CharacterSelectionTileView;		// 角色选择的TileView

	UPROPERTY(meta=(BindWidget))
	UAbilityListView* AbilityListView;		// 技能列表视图

	UPROPERTY(meta=(BindWidget))
	UPlayerTeamLayoutWidget* TeamLayoutWidget;		// 玩家队伍布局Widget

	UPROPERTY(meta=(BindWidget))
	UArenasButton* StartMatchButton;

	UPROPERTY(EditDefaultsOnly, Category="TeamSelection")
	TSubclassOf<UTeamSelectionWidget> TeamSelectionWidgetClass;

	UPROPERTY()
	TArray<UTeamSelectionWidget*> TeamSelectionSlots;

	void ClearAndPopulateTeamSelectionSlots();		// 清空并填充队伍选择槽
	void SlotSelectedTeamSelection(uint8 InTeamSelectionSlotId);

	UPROPERTY()
	ALobbyPlayerController* LobbyPlayerController;

	void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& InPlayerSelections);
	void ConfigureGameState();

	UPROPERTY()
	AArenasGameState* ArenasGameState;

	FTimerHandle RequestArenasGameStateTimerHandle;

	UFUNCTION()
	void OnStartHeroSelectionClicked();

	void SwitchToHeroSelectionPhase();

	void OnLoadCharacterDefinitions();

	UPROPERTY()
	AArenasPlayerState* OwningArenasPlayerState;
	
	void OnCharacterSelected(UObject* InSelectedObject);

	UPROPERTY(EditDefaultsOnly, Category="Character Display")
	TSubclassOf<ACharacterDisplay> CharacterDisplayClass;

	UPROPERTY()
	ACharacterDisplay* CurrentCharacterDisplay;

	void SpawnCharacterDisplay();
	void UpdateCharacterDisplay(const FPlayerSelection& InPlayerSelection);

	UFUNCTION()
	void OnStartMatchClicked();
	
};
