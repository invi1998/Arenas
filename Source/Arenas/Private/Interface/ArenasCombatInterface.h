// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ArenasCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UArenasCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARENAS_API IArenasCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 获取AI当前目标Actor
	virtual AActor* GetCurrentTargetActor() const = 0;
	
};
