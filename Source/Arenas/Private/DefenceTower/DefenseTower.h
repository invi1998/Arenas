// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "DefenseTower.generated.h"

UCLASS()
class ARENAS_API ADefenseTower : public ACharacter
{
	GENERATED_BODY()

public:
	ADefenseTower();

protected:
	virtual void BeginPlay() override;
	

};
