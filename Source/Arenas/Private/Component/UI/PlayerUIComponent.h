// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "PawnUIComponent.h"
#include "PlayerUIComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARENAS_API UPlayerUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnExperiencePercentChanged;
	
	void OnExpChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxExpChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	virtual void SetAndBoundAttributeDelegate(UArenasAbilitySystemComponent* InArenasASC);

private:
	float Exp = 0.f;
	float MaxExp = 0.f;
};
