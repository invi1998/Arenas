// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Character/ArenasCharacter.h"
#include "StormCore.generated.h"

class AArenasTowerAIController;
class UCameraComponent;

UCLASS()
class ARENAS_API AStormCore : public AArenasCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStormCore();

	virtual void PossessedBy(AController* NewController) override;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	UCameraComponent* ViewCam;

	UPROPERTY()
	AArenasTowerAIController* CoreAIController;
};
