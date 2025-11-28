// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "MenuPlayerController.h"
#include "LobbyPlayerController.generated.h"

DECLARE_DELEGATE(FOnSwitchToHeroSelectionPhaseSignature);	// 切换到英雄选择阶段委托

/**
 * 玩家在大厅中的控制器
 */
UCLASS()
class ARENAS_API ALobbyPlayerController : public AMenuPlayerController
{
	GENERATED_BODY()

public:
	ALobbyPlayerController();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestChangeSlotSelection(uint8 InRequestedSlotID);		// 向服务端请求改变队列插槽
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestReturnMainMenu();		// 向服务端请求返回主菜单

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartHeroSelection();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestStartMatch();

	UFUNCTION(Client, Reliable)
	void Client_StartHeroSelection();

	FOnSwitchToHeroSelectionPhaseSignature OnSwitchToHeroSelectionPhase;		// 切换到英雄选择阶段委托

private:
	
	
	
};
