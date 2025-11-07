// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArenasGameState.generated.h"

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

	FOnPlayerSelectionChangedSignature OnPlayerSelectionChangedSignature;

	void RequestPlayerSelection(const APlayerState* InPlayerState, uint8 InTeamSelectionSlotId);
	bool IsSlotOccupied(uint8 InTeamSelectionSlotId) const;		// 是否有玩家占用了该槽位
	const TArray<FPlayerSelection>& GetPlayerSelectionArray() const { return PlayerSelectionArray; }
	
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
