// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "GameplayMenu.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Component/ArenasButton.h"

void UGameplayMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MainMenuButton->ButtonArea->OnClicked.AddDynamic(this, &UGameplayMenu::OnMainMenuButtonClicked);
	QuitGameButton->ButtonArea->OnClicked.AddDynamic(this, &UGameplayMenu::OnQuitGameButtonClicked);
	
}

FOnButtonClickedEvent& UGameplayMenu::GetResumeButtonClickedEventDelegate() const
{
	return ResumeButton->ButtonArea->OnClicked;
}

void UGameplayMenu::OnMainMenuButtonClicked()
{
	
}

void UGameplayMenu::OnQuitGameButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
