// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "PlayerInfoTypes.h"

#include "GameFramework/PlayerState.h"
#include "Network/ArenasNetFunctionLibrary.h"

FPlayerSelection::FPlayerSelection() : Slot(GetInvalidSlot()), PlayerUniqueId{FUniqueNetIdRepl::Invalid()},
                                       PlayerNickName(TEXT("")), SelectedCharacter(nullptr)
{
}

FPlayerSelection::FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState)
	: Slot(InSlot), SelectedCharacter(nullptr)
{
	if (InPlayerState)
	{
		PlayerUniqueId = InPlayerState->GetUniqueId();
		PlayerNickName = InPlayerState->GetPlayerName();
	}
}

bool FPlayerSelection::IsForPlayer(const APlayerState* InPlayerState) const
{
	if (!InPlayerState) return false;

#if WITH_EDITOR
	// 如果是在编辑器中，则使用昵称进行比较，以便测试时更方便识别玩家
	return PlayerNickName == InPlayerState->GetPlayerName();
#else
	return PlayerUniqueId == InPlayerState->GetUniqueId();
#endif

	
}

bool FPlayerSelection::IsValid() const
{
#if WITH_EDITOR
	return true;
#else
	return Slot != GetInvalidSlot()
		&& PlayerUniqueId.IsValid()
		&& Slot < UArenasNetFunctionLibrary::GetPlayerCounterPerTeam() * 2;
#endif
}
