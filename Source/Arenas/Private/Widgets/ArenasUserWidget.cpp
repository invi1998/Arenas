// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasUserWidget.h"

#include "Interface/PawnUIInterface.h"
#include "Component/UI/PlayerUIComponent.h"

void UArenasUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (UPlayerUIComponent* PlayerUIComponent = PawnUIInterface->GetPlayerUIComponent())
		{
			BP_OnOwningPlayerUIComponentInitialized(PlayerUIComponent);
		}
	}
}
