// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Types/PlayerInfoTypes.h"
#include "ArenasPlayerState.generated.h"

class AArenasGameState;
class UPA_CharacterDefinition;
/**
 * 
 */
UCLASS()
class ARENAS_API AArenasPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AArenasPlayerState();
	
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 客户端UI请求服务端更新英雄角色选择
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedCharacter(const UPA_CharacterDefinition* InSelectedCharacter);
	TSubclassOf<APawn> GetSelectedPawnClass() const;

private:
	UPROPERTY(Replicated)
	FPlayerSelection PlayerSelectionData;

	UPROPERTY()
	AArenasGameState* CachedArenasGameState;

	void OnPlayerSelectionChanged(const TArray<FPlayerSelection>& InPlayerSelections);

	
};
