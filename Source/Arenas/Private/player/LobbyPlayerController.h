// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "MenuPlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 玩家在大厅中的控制器
 */
UCLASS()
class ARENAS_API ALobbyPlayerController : public AMenuPlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestChangeSlotSelection(uint8 InRequestedSlotID);		// 向服务端请求改变队列插槽

	
private:
	
	
	
};
