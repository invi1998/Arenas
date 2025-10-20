// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArenasGameState.generated.h"

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
private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamOnePlayerKillCount)
	int32 TeamOnePlayerKillCount;

	UPROPERTY(ReplicatedUsing = OnRep_TeamTwoPlayerKillCount)
	int32 TeamTwoPlayerKillCount;

	UFUNCTION()
	void OnRep_TeamOnePlayerKillCount();

	UFUNCTION()
	void OnRep_TeamTwoPlayerKillCount();
	
};
