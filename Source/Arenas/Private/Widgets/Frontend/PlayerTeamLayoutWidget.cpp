// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "PlayerTeamLayoutWidget.h"

#include "PlayerTeamSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Network/ArenasNetFunctionLibrary.h"

void UPlayerTeamLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerTeamLeftLayoutBox->ClearChildren();
	PlayerTeamRightLayoutBox->ClearChildren();

	if (!PlayerTeamSlotWidgetClass) return;

	for (int i = 0; i < UArenasNetFunctionLibrary::GetPlayerCounterPerTeam() * 2; ++i)
	{
		if (UPlayerTeamSlotWidget* NewSlotWidget = CreateWidget<UPlayerTeamSlotWidget>(GetOwningPlayer(), PlayerTeamSlotWidgetClass))
		{
			TeamSlotWidgets.Add(NewSlotWidget);
			UHorizontalBoxSlot* NewSlotBoxSlot = nullptr;
			if (i < UArenasNetFunctionLibrary::GetPlayerCounterPerTeam())
			{
				NewSlotWidget->UpdateTeamSide(false);
				NewSlotBoxSlot = PlayerTeamLeftLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
			}
			else
			{
				NewSlotWidget->UpdateTeamSide(true);
				NewSlotBoxSlot = PlayerTeamRightLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
			}
			if (NewSlotBoxSlot)
			{
				NewSlotBoxSlot->SetPadding(FMargin(PlayerTeamSlotWidgetMargin, 0.f));
			}
		}
	}
	
}

void UPlayerTeamLayoutWidget::UpdatePlayerelection(const TArray<FPlayerSelection>& InPlayerSelections)
{
	for (UPlayerTeamSlotWidget* TeamSlotWidget : TeamSlotWidgets)
	{
		TeamSlotWidget->UpdateSlot(TEXT("..."), nullptr);
	}

	for (const FPlayerSelection& PlayerSelection : InPlayerSelections)
	{
		if (!PlayerSelection.IsValid()) continue;

		TeamSlotWidgets[PlayerSelection.GetSlot()]->UpdateSlot(PlayerSelection.GetPlayerNickName(), PlayerSelection.GetSelectedCharacter());
		
	}
	
}
