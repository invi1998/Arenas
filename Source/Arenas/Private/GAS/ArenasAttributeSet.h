// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "ArenasAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, MaxMana);

private:
	UPROPERTY()
	FGameplayAttributeData Health;

	UPROPERTY()
	FGameplayAttributeData MaxHealth;

	UPROPERTY()
	FGameplayAttributeData Mana;

	UPROPERTY()
	FGameplayAttributeData MaxMana;
	
};
