// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "SessionEntryWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SessionButton->OnClicked.AddDynamic(this, &USessionEntryWidget::OnSessionButtonClicked);
}

void USessionEntryWidget::InitializeEntry(const FString& InSessionName, const FString& InSessionIDString)
{
	SessionNameText->SetText(FText::FromString(InSessionName));
	CachedSessionIDString = InSessionIDString;
}

void USessionEntryWidget::OnSessionButtonClicked()
{
	OnSessionEntrySelectedDelegate.Broadcast(CachedSessionIDString);
}
