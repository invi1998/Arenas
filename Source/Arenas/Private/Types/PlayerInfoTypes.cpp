// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "PlayerInfoTypes.h"

#include "GameFramework/PlayerState.h"
#include "Network/ArenasNetFunctionLibrary.h"

FPlayerInfoData::FPlayerInfoData() : Slot(GetInvalidSlot()), PlayerUniqueId{FUniqueNetIdRepl::Invalid()}, PlayerNickName(TEXT(""))
{
}

FPlayerInfoData::FPlayerInfoData(uint8 InSlot, const APlayerState* InPlayerState)
	: Slot(InSlot)
{
	if (InPlayerState)
	{
		PlayerUniqueId = InPlayerState->GetUniqueId();
		PlayerNickName = InPlayerState->GetPlayerName();
	}
}

bool FPlayerInfoData::IsForPlayer(const APlayerState* InPlayerState) const
{
	if (!InPlayerState) return false;

#if WITH_EDITOR
	// 如果是在编辑器中，则使用昵称进行比较，以便测试时更方便识别玩家
	return PlayerNickName == InPlayerState->GetPlayerName();
#else
	return PlayerUniqueId == InPlayerState->GetUniqueId();
#endif

	
}

bool FPlayerInfoData::IsValid() const
{
#if WITH_EDITOR
	return true;
#else
	return Slot != GetInvalidSlot()
		&& PlayerUniqueId.IsValid()
		&& Slot < UArenasNetFunctionLibrary::GetPlayerCounterPerTeam() * 2;
#endif
}
