// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasPlayerState.h"

#include "Character/PA_CharacterDefinition.h"
#include "player/ArenasPlayerCharacter.h"
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

void AArenasPlayerState::CopyProperties(APlayerState* PlayerState)
{
	// 复制玩家状态属性到新的PlayerState实例，只会发生在服务端
	// 例如：切换关卡时，PlayerState会被复制到新的实例中
	Super::CopyProperties(PlayerState);

	if (AArenasPlayerState* NewArenasPlayerState = Cast<AArenasPlayerState>(PlayerState))
	{
		NewArenasPlayerState->PlayerSelectionData = PlayerSelectionData;
	}
	
}

TSubclassOf<APawn> AArenasPlayerState::GetSelectedPawnClass() const
{
	if (PlayerSelectionData.GetSelectedCharacter())
	{
		return PlayerSelectionData.GetSelectedCharacter()->LoadCharacterClass();
	}
	return nullptr;
}

FGenericTeamId AArenasPlayerState::GetTeamIdBasedOnSlot() const
{
	return PlayerSelectionData.GetSlot() < UArenasNetFunctionLibrary::GetPlayerCounterPerTeam() ? FGenericTeamId(0) : FGenericTeamId(1);
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
