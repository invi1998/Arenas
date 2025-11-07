// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "LobbyPlayerController.h"

#include "GameFramework/PlayerState.h"
#include "Framework/ArenasGameState.h"

void ALobbyPlayerController::Server_RequestChangeSlotSelection_Implementation(uint8 InRequestedSlotID)
{
	if (!GetWorld()) return;

	AArenasGameState* ArenasGameState = GetWorld()->GetGameState<AArenasGameState>();
	if (!ArenasGameState) return;

	ArenasGameState->RequestPlayerSelection(GetPlayerState<APlayerState>(), InRequestedSlotID);
	
}


bool ALobbyPlayerController::Server_RequestChangeSlotSelection_Validate(uint8 InRequestedSlotID)
{
	return true;
}
