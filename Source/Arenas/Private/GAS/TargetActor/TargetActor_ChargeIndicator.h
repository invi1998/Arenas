// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_ChargeIndicator.generated.h"

/**
 * 目标Actor——蓄力指示器
 */

UCLASS()
class ARENAS_API ATargetActor_ChargeIndicator : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetActor_ChargeIndicator();

	virtual void Tick(float DeltaTime) override;

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;

	void SetTargetDistance(float Distance);		// 设置目标距离
	void StopAttaching();		// 停止附着在角色上

private:
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	UDecalComponent* ChargeDecalComponent;		// 蓄力指示器Decal组件

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float MaxChargeDistance = 1000.f;		// 最大蓄力距离

	UPROPERTY()
	const AActor* AvatarActor;		// 持有该TargetActor的角色
	
	bool bStopAttaching = false;
};
