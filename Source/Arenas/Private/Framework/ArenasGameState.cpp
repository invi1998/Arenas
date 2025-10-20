// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGameState.h"

#include "Net/UnrealNetwork.h"
#include "player/ArenasPlayerController.h"

void AArenasGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenasGameState, TeamOnePlayerKillCount);
	DOREPLIFETIME(AArenasGameState, TeamTwoPlayerKillCount);
	
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

