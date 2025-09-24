// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "PawnUIComponent.h"
#include "PlayerUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRegenStateChangedDelegate, bool, bInRegenState, FText, InRegenText);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARENAS_API UPlayerUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnExperiencePercentChanged;

	UPROPERTY(BlueprintAssignable)
	FOnRegenStateChangedDelegate OnHealthRegenStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnRegenStateChangedDelegate OnManaRegenStateChanged;


	void OnExpChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxExpChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	void OnStrengthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnIntelligenceChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	
	virtual void SetAndBoundAttributeDelegate(UArenasAbilitySystemComponent* InArenasASC) override;

	virtual void BoardCastHealthAndManaRegenState() override;

private:
	float Exp = 0.f;
	float MaxExp = 0.f;

	float GetCurrentHealthRegenNumber() const;
	float GetCurrentManaRegenNumber() const;

	void OnStatusTagChanged(FGameplayTag StatTag, int32 Count);
	
};
