// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoTypes.generated.h"

/**
 * 
 */

class UPA_CharacterDefinition;
class APlayerState;

USTRUCT()
struct FPlayerSelection
{
	GENERATED_BODY()

public:
	FPlayerSelection();
	FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState);

	static uint8 GetInvalidSlot() { return 255; }

	FORCEINLINE void SetSlot(uint8 InSlot) { Slot = InSlot; }
	FORCEINLINE uint8 GetSlot() const { return Slot; }
	FORCEINLINE FUniqueNetIdRepl GetPlayerUniqueId() const { return PlayerUniqueId; }
	FORCEINLINE FString GetPlayerNickName() const { return PlayerNickName; }
	FORCEINLINE const UPA_CharacterDefinition* GetSelectedCharacter() const { return SelectedCharacter; }
	FORCEINLINE void SetSelectedCharacter(const UPA_CharacterDefinition* InSelectedCharacter) { SelectedCharacter = InSelectedCharacter; }

	bool IsForPlayer(const APlayerState* InPlayerState) const;
	bool IsValid() const;

private:
	UPROPERTY()
	uint8 Slot;

	UPROPERTY()
	FUniqueNetIdRepl PlayerUniqueId;		// 玩家唯一ID（支持网络复制）

	UPROPERTY()
	FString PlayerNickName;				// 玩家昵称

	UPROPERTY()
	const UPA_CharacterDefinition* SelectedCharacter;	// 玩家选择的角色
	
};


