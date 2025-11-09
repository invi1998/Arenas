// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasPlayerState.h"

#include "Framework/ArenasGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AArenasPlayerState::AArenasPlayerState()
{
	bReplicates = true;
	SetNetUpdateFrequency(100.f);
}

void AArenasPlayerState::BeginPlay()
{
	Super::BeginPlay();

	CachedArenasGameState = Cast<AArenasGameState>(UGameplayStatics::GetGameState(this));

	if (CachedArenasGameState)
	{
		CachedArenasGameState->OnPlayerSelectionChangedSignature.AddUObject(this, &AArenasPlayerState::OnPlayerSelectionChanged);
	}
	
}

void AArenasPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenasPlayerState, PlayerSelectionData);

	
}

void AArenasPlayerState::Server_SetSelectedCharacter_Implementation(const UPA_CharacterDefinition* InSelectedCharacter)
{
	if (!CachedArenasGameState) return;

	if (!InSelectedCharacter) return;

	if (CachedArenasGameState->IsDefinitionSelected(InSelectedCharacter)) return;

	if (PlayerSelectionData.GetSelectedCharacter())
	{
		// 取消之前选择的角色
		CachedArenasGameState->DeSelectedCharacter(PlayerSelectionData.GetSelectedCharacter());
	}

	PlayerSelectionData.SetSelectedCharacter(InSelectedCharacter);
	CachedArenasGameState->SetCharacterSelected(this, InSelectedCharacter);
}

bool AArenasPlayerState::Server_SetSelectedCharacter_Validate(const UPA_CharacterDefinition* InSelectedCharacter)
{
	return true;
}

void AArenasPlayerState::OnPlayerSelectionChanged(const TArray<FPlayerSelection>& InPlayerSelections)
{
	for (const FPlayerSelection& PlayerSelection : InPlayerSelections)
	{
		if (PlayerSelection.IsForPlayer(this))
		{
			PlayerSelectionData = PlayerSelection;
			break;
		}
	}
}
