// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Character/ArenasCharacter.h"
#include "ArenasPlayerCharacter.generated.h"

UCLASS()
class ARENAS_API AArenasPlayerCharacter : public AArenasCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArenasPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
