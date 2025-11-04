// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundPick.generated.h"

class UDecalComponent;

/**
 * 
 */
UCLASS()
class ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ATargetActor_GroundPick();

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;

	void SetTargetAreaRadius(float InRadius);
	void SetTargetOptions(bool bInShouldTargetEnemies, bool bInShouldTargetAllies);
	FORCEINLINE void SetShouldDrawDebugSphere(bool bInShouldDrawDebugSphere) { bShouldDrawDebugSphere = bInShouldDrawDebugSphere; }
	void SetTargetTraceDistance(float InDistance);

protected:
	virtual void Tick(float DeltaSeconds) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	UDecalComponent* DecalComponent;		// 技能指示器

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	UDecalComponent* CastRangeDecalComponent;	// 施法范围指示器
	
	FVector GetTargetPoint() const;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceDistance = 2000.f;	// 目标追踪距离

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;		// 目标区域半径

	bool bShouldTargetEnemies = true;	// 是否应该选择敌人
	bool bShouldTargetAllies = false;	// 是否应该选择友军

	bool bShouldDrawDebugSphere = false; // 是否绘制调试用的球体

	UPROPERTY()
	const AActor* AvatarActor;		// 持有该TargetActor的角色
	
};
