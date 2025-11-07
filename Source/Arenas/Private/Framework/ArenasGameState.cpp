// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGameState.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "player/ArenasPlayerController.h"
#include "Types/PlayerInfoTypes.h"

void AArenasGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenasGameState, TeamOnePlayerKillCount);
	DOREPLIFETIME(AArenasGameState, TeamTwoPlayerKillCount);
	DOREPLIFETIME_CONDITION_NOTIFY(AArenasGameState, PlayerSelectionArray, COND_None, REPNOTIFY_Always);
	
}

void AArenasGameState::AddTeamOnePlayerKillCount()
{
	if (!HasAuthority()) return;
	TeamOnePlayerKillCount++;
}

void AArenasGameState::AddTeamTwoPlayerKillCount()
{
	if (!HasAuthority()) return;
	TeamTwoPlayerKillCount++;
}

void AArenasGameState::RequestPlayerSelection(const APlayerState* InPlayerState, uint8 InTeamSelectionSlotId)
{
	if (!HasAuthority()) return;
	if (IsSlotOccupied(InTeamSelectionSlotId)) return;

	FPlayerSelection* SelectionPtr = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& SelectionData)
		{
			return SelectionData.IsForPlayer(InPlayerState);
		});

	if (SelectionPtr)
	{
		// 更新已有玩家的选择数据
		SelectionPtr->SetSlot(InTeamSelectionSlotId);
	}
	else
	{
		// 添加新的玩家选择数据
		FPlayerSelection NewSelectionData(InTeamSelectionSlotId, InPlayerState);
		PlayerSelectionArray.Add(NewSelectionData);
	}

	OnPlayerSelectionChangedSignature.Broadcast(PlayerSelectionArray);
	
}

bool AArenasGameState::IsSlotOccupied(uint8 InTeamSelectionSlotId) const
{
	for (const FPlayerSelection& SelectionData : PlayerSelectionArray)
	{
		if (SelectionData.GetSlot() == InTeamSelectionSlotId)
		{
			// 该槽位已被占用
			return true;
		}
	}

	return false;
}

void AArenasGameState::OnRep_TeamOnePlayerKillCount()
{
	if (AArenasPlayerController* PC = Cast<AArenasPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->UpdateTeamOnePlayerKillCount(TeamOnePlayerKillCount);
	}
}

void AArenasGameState::OnRep_TeamTwoPlayerKillCount()
{
	if (AArenasPlayerController* PC = Cast<AArenasPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->UpdateTeamTwoPlayerKillCount(TeamTwoPlayerKillCount);
	}
}

void AArenasGameState::OnRep_PlayerSelectionArray()
{
	OnPlayerSelectionChangedSignature.Broadcast(PlayerSelectionArray);
}

