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

void ALobbyPlayerController::Server_StartHeroSelection_Implementation()
{
	if (!GetWorld()) return;
	if (!HasAuthority()) return;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		// 通知所有玩家控制器开始英雄选择阶段
		if (ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(Iterator->Get()))
		{
			LobbyPC->Client_StartHeroSelection();
		}
	}
	
}

bool ALobbyPlayerController::Server_StartHeroSelection_Validate()
{
	return true;
}

void ALobbyPlayerController::Client_StartHeroSelection_Implementation()
{
	OnSwitchToHeroSelectionPhase.ExecuteIfBound();
}
