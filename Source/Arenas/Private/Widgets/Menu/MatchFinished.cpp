// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MatchFinished.h"

#include "Components/TextBlock.h"
#include "Framework/ArenasGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Component/ArenasButton.h"

void UMatchFinished::NativeConstruct()
{
	Super::NativeConstruct();

	MainMenuButton->ButtonArea->OnClicked.AddDynamic(this, &UMatchFinished::OnMainMenuButtonClicked);
	QuitGameButton->ButtonArea->OnClicked.AddDynamic(this, &UMatchFinished::OnQuitGameButtonClicked);
	MainMenuButton->ButtonArea->SetIsEnabled(true);
	
}

void UMatchFinished::SetMenuTitleText(const FText& NewTitleText)
{
	MenuTitle->SetText(NewTitleText);
}

void UMatchFinished::OnMainMenuButtonClicked()
{
	if (UArenasGameInstance* GameInstance = Cast<UArenasGameInstance>(GetGameInstance()))
	{
		MainMenuButton->ButtonArea->SetIsEnabled(false);
		GameInstance->LeaveCurrentSessionAndReturnToMainMenu();
	}
}

void UMatchFinished::OnQuitGameButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
