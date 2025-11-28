// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "GameplayMenu.h"

#include "Components/TextBlock.h"
#include "Framework/ArenasGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Component/ArenasButton.h"

void UGameplayMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MainMenuButton->ButtonArea->OnClicked.AddDynamic(this, &UGameplayMenu::OnMainMenuButtonClicked);
	QuitGameButton->ButtonArea->OnClicked.AddDynamic(this, &UGameplayMenu::OnQuitGameButtonClicked);
	MainMenuButton->ButtonArea->SetIsEnabled(true);
}

FOnButtonClickedEvent& UGameplayMenu::GetResumeButtonClickedEventDelegate() const
{
	return ResumeButton->ButtonArea->OnClicked;
}

void UGameplayMenu::SetMenuTitleText(const FText& NewTitleText)
{
	MenuTitle->SetText(NewTitleText);
}

void UGameplayMenu::OnMainMenuButtonClicked()
{
	if (UArenasGameInstance* GameInstance = Cast<UArenasGameInstance>(GetGameInstance()))
	{
		MainMenuButton->ButtonArea->SetIsEnabled(false);
		GameInstance->LeaveCurrentSessionAndReturnToMainMenu();
	}
}

void UGameplayMenu::OnQuitGameButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
