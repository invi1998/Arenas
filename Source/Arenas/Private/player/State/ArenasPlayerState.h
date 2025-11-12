// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerState.h"
#include "Network/ArenasNetFunctionLibrary.h"
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

	virtual void CopyProperties(APlayerState* PlayerState) override;		// 复制玩家状态属性

	// 客户端UI请求服务端更新英雄角色选择
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedCharacter(const UPA_CharacterDefinition* InSelectedCharacter);

	// 获取玩家英雄选择数据
	TSubclassOf<APawn> GetSelectedPawnClass() const;

	FGenericTeamId GetTeamIdBasedOnSlot() const;
	

private:
	UPROPERTY(Replicated)
	FPlayerSelection PlayerSelectionData;

	UPROPERTY()
	AArenasGameState* CachedArenasGameState;

	void OnPlayerSelectionChanged(const TArray<FPlayerSelection>& InPlayerSelections);

	
};
