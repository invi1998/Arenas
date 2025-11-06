// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AI/MinionBarrack.h"
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

	UCameraComponent* GetStormCoreViewCam() const { return ViewCam; }

	void SetOwnerBarrack(AMinionBarrack* InBarrack) { OwnedBarrack = InBarrack; }
	AMinionBarrack* GetOwnedBarrack() const { return OwnedBarrack; }

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	UCameraComponent* ViewCam;

	UPROPERTY()
	AArenasTowerAIController* CoreAIController;

	UPROPERTY()
	AMinionBarrack* OwnedBarrack;
};
