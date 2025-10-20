// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MatchFinished.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Component/ArenasButton.h"

void UMatchFinished::NativeConstruct()
{
	Super::NativeConstruct();

	MainMenuButton->ButtonArea->OnClicked.AddDynamic(this, &UMatchFinished::OnMainMenuButtonClicked);
	QuitGameButton->ButtonArea->OnClicked.AddDynamic(this, &UMatchFinished::OnQuitGameButtonClicked);
	
}

void UMatchFinished::SetMenuTitleText(const FText& NewTitleText)
{
	MenuTitle->SetText(NewTitleText);
}

void UMatchFinished::OnMainMenuButtonClicked()
{
	
}

void UMatchFinished::OnQuitGameButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
