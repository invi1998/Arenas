// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArenasGameState.generated.h"

class UPA_CharacterDefinition;
struct FPlayerSelection;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionChangedSignature, const TArray<FPlayerSelection>& /*NewPlayerSelectionArray*/);

/**
 * 
 */
UCLASS()
class ARENAS_API AArenasGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddTeamOnePlayerKillCount();
	void AddTeamTwoPlayerKillCount();

	void SetCharacterSelected(const APlayerState* InSelectingPlayer, const UPA_CharacterDefinition* InSelectedCharacter);		// 设置玩家已选择角色
	void DeSelectedCharacter(const UPA_CharacterDefinition* InSelectedCharacter);	// 取消选择该角色
	bool CanStartMatch() const;		// 是否可以开始比赛

	FOnPlayerSelectionChangedSignature OnPlayerSelectionChangedSignature;

	void RequestPlayerSelection(const APlayerState* InPlayerState, uint8 InTeamSelectionSlotId);
	bool IsSlotOccupied(uint8 InTeamSelectionSlotId) const;		// 是否有玩家占用了该槽位
	bool IsDefinitionSelected(const UPA_CharacterDefinition* InCharacterDef) const;		// 该角色定义是否已被选择
	const TArray<FPlayerSelection>& GetPlayerSelectionArray() const { return PlayerSelectionArray; }

	bool CanStartHeroSelection() const;		// 是否可以开始英雄选择
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamOnePlayerKillCount)
	int32 TeamOnePlayerKillCount;

	UPROPERTY(ReplicatedUsing = OnRep_TeamTwoPlayerKillCount)
	int32 TeamTwoPlayerKillCount;

	UFUNCTION()
	void OnRep_TeamOnePlayerKillCount();

	UFUNCTION()
	void OnRep_TeamTwoPlayerKillCount();

	UPROPERTY(ReplicatedUsing = OnRep_PlayerSelectionArray)
	TArray<FPlayerSelection> PlayerSelectionArray;

	UFUNCTION()
	void OnRep_PlayerSelectionArray();
	
	
};
