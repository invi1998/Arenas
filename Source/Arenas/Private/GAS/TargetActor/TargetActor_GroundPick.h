// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundPick.generated.h"

/**
 * 
 */
UCLASS()
class ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ATargetActor_GroundPick();

	virtual void ConfirmTargetingAndContinue() override;

	void SetTargetAreaRadius(float InRadius);
	void SetTargetOptions(bool bInShouldTargetEnemies, bool bInShouldTargetAllies);

protected:
	virtual void Tick(float DeltaSeconds) override;

	FVector GetTargetPoint() const;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceDistance = 2000.f;	// 目标追踪距离

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;		// 目标区域半径

	bool bShouldTargetEnemies = true;	// 是否应该选择敌人
	bool bShouldTargetAllies = false;	// 是否应该选择友军
	
};
