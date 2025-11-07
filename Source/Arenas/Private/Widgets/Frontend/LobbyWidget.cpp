// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "LobbyWidget.h"

#include "Character/PA_CharacterDefinition.h"
#include "Components/TileView.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/ArenasAssetManager.h"
#include "Framework/ArenasGameState.h"
#include "Network/ArenasNetFunctionLibrary.h"
#include "player/LobbyPlayerController.h"
#include "Types/PlayerInfoTypes.h"
#include "Widgets/Component/ArenasButton.h"
#include "Widgets/Frontend/TeamSelectionWidget.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
	
	ClearAndPopulateTeamSelectionSlots();

	ConfigureGameState();

	if (LobbyPlayerController)
	{
		LobbyPlayerController->OnSwitchToHeroSelectionPhase.BindUObject(this, &ULobbyWidget::SwitchToHeroSelectionPhase);
	}

	StartHeroSelectionButton->ButtonArea->SetIsEnabled(false);
	StartHeroSelectionButton->ButtonArea->OnClicked.AddDynamic(this, &ULobbyWidget::OnStartHeroSelectionClicked);

	// 加载角色PA
	UArenasAssetManager::Get().LoadCharacterDefinitions(FStreamableDelegate::CreateUObject(this, &ULobbyWidget::OnLoadCharacterDefinitions));
	
}

void ULobbyWidget::ClearAndPopulateTeamSelectionSlots()
{
	TeamSelectionSlotGridPanel->ClearChildren();

	uint8 PlayerCounterPerTeam = UArenasNetFunctionLibrary::GetPlayerCounterPerTeam();
	for (uint8 i = 0; i < 2 * PlayerCounterPerTeam; ++i)
	{
		UTeamSelectionWidget* NewSlotWidget = CreateWidget<UTeamSelectionWidget>(GetWorld(), TeamSelectionWidgetClass);
		if (NewSlotWidget)
		{
			NewSlotWidget->SetSlotID(i);
			NewSlotWidget->OnSlotClicked.AddUObject(this, &ULobbyWidget::SlotSelectedTeamSelection);
			if (UUniformGridSlot* NewGridSlot = TeamSelectionSlotGridPanel->AddChildToUniformGrid(NewSlotWidget))
			{
				// eg：0~4号槽在第一列，5~9号槽在第二列
				int32 Row = i % PlayerCounterPerTeam;
				int32 Col = i < PlayerCounterPerTeam ? 0 : 1;
				NewGridSlot->SetRow(Row);
				NewGridSlot->SetColumn(Col);
				TeamSelectionSlots.Add(NewSlotWidget);
			}
		}
	}
	
	
}

void ULobbyWidget::SlotSelectedTeamSelection(uint8 InTeamSelectionSlotId)
{
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_RequestChangeSlotSelection(InTeamSelectionSlotId);
	}
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& InPlayerSelections)
{
	for (UTeamSelectionWidget* SlotWidget : TeamSelectionSlots)
	{
		SlotWidget->UpdateSlotInfoText("...");
	}

	for (const FPlayerSelection& PlayerSelection : InPlayerSelections)
	{
		if (PlayerSelection.IsValid())
		{
			TeamSelectionSlots[PlayerSelection.GetSlot()]->UpdateSlotInfoText(PlayerSelection.GetPlayerNickName());
		}
	}

	if (ArenasGameState)
	{
		StartHeroSelectionButton->ButtonArea->SetIsEnabled(ArenasGameState->CanStartHeroSelection());
	}
	
}

void ULobbyWidget::ConfigureGameState()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ArenasGameState = World->GetGameState<AArenasGameState>();
	if (!ArenasGameState)
	{
		World->GetTimerManager().SetTimer(RequestArenasGameStateTimerHandle, this, &ULobbyWidget::ConfigureGameState, 1.f, false);
	}
	else
	{
		ArenasGameState->OnPlayerSelectionChangedSignature.AddUObject(this, &ULobbyWidget::UpdatePlayerSelectionDisplay);
		UpdatePlayerSelectionDisplay(ArenasGameState->GetPlayerSelectionArray());		// 立即更新一次显示
	}
	
}

void ULobbyWidget::OnStartHeroSelectionClicked()
{
	// 切换到英雄选择界面
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_StartHeroSelection();
	}
}

void ULobbyWidget::SwitchToHeroSelectionPhase()
{
	MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}

void ULobbyWidget::OnLoadCharacterDefinitions()
{
	TArray<UPA_CharacterDefinition*> LoadedCharacterDefinitions;
	if (UArenasAssetManager::Get().GetLoadedCharacterDefinitions(LoadedCharacterDefinitions))
	{
		CharacterSelectionTileView->SetListItems(LoadedCharacterDefinitions);
	}
}
