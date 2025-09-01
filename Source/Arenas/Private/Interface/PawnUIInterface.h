// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "UObject/Interface.h"
#include "PawnUIInterface.generated.h"

class UPlayerUIComponent;
class UPawnUIComponent;
// This class does not need to be modified.
UINTERFACE()
class UPawnUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARENAS_API IPawnUIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UPawnUIComponent* GetPawnUIComponent() const = 0;
	virtual UPlayerUIComponent* GetPlayerUIComponent() const;
	virtual FGenericTeamId GetOwningGenericTeamId() const { return FGenericTeamId::NoTeam; }
	
};
