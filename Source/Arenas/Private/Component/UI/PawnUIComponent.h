// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Component/PawnExtensionComponentBase.h"
#include "PawnUIComponent.generated.h"

class UArenasAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPercentChangedDelegate, float, NewPercent, float, NewValue, float, MaxValue);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARENAS_API UPawnUIComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnHealthPercentChanged;	// 百分比变化委托

	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnManaPercentChanged;		// 百分比变化委托

	void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnManaChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxManaChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	virtual void SetAndBoundAttributeDelegate(UArenasAbilitySystemComponent* InArenasASC);
	virtual void BoardCastHealthAndManaRegenState();

protected:
	UPROPERTY()
	UArenasAbilitySystemComponent* CachedArenasASC;

	bool bIsDead = false;
	bool bIsStunned = false;
	bool bIsHealthFull = true;
	bool bIsManaFull = true;

private:
	float Health = 0.f;
	float MaxHealth = 0.f;
	float Mana = 0.f;
	float MaxMana = 0.f;
	
};
