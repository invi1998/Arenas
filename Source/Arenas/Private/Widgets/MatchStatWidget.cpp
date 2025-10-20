// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MatchStatWidget.h"

void UMatchStatWidget::SetOwningArenasPlayerController(AArenasPlayerController* InArenasPC)
{
	if (InArenasPC)
	{
		OwningArenasPlayerController = InArenasPC;
		BP_OnOwningArenasPlayerControllerSet(InArenasPC);
	}
}
