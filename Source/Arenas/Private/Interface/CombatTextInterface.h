// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Types/ArenaStructTypes.h"
#include "UObject/Interface.h"
#include "CombatTextInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatTextInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARENAS_API ICombatTextInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 显示战斗文字
	virtual void ShowCombatText(float ValueNumber, EArenasComboTextType TextType, const FVector& HitLocation) = 0;
	
};
