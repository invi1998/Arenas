// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "LobbyWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Network/ArenasNetFunctionLibrary.h"
#include "Widgets/Frontend/TeamSelectionWidget.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ClearAndPopulateTeamSelectionSlots();
	
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
	UE_LOG(LogTemp, Log, TEXT("SlotSelectedTeamSelection: %d"), InTeamSelectionSlotId);
}
