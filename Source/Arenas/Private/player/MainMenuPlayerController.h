// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "MenuPlayerController.h"
#include "MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API AMainMenuPlayerController : public AMenuPlayerController
{
	GENERATED_BODY()
	
public:
	AMainMenuPlayerController();
	
	virtual void OnPossess(APawn* InPawn) override;		// 当控制器拥有一个Pawn时调用
	
	
};
