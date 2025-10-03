// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "ArenasHeroAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasHeroAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// 智力
	UPROPERTY(ReplicatedUsing = OnRep_Intelligence)
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, Intelligence);

	// 力量
	UPROPERTY(ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, Strength);

	// 敏捷
	UPROPERTY(ReplicatedUsing = OnRep_Agility)
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, Agility);

	UPROPERTY(ReplicatedUsing = OnRep_Experience)
	FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, Experience);

	UPROPERTY(ReplicatedUsing = OnRep_PrevLevelExperience)
	FGameplayAttributeData PrevLevelExperience;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, PrevLevelExperience);

	UPROPERTY(ReplicatedUsing = OnRep_NextLevelExperience)
	FGameplayAttributeData NextLevelExperience;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, NextLevelExperience);

	UPROPERTY(ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, Level);

	UPROPERTY(ReplicatedUsing = OnRep_UpgradePoints)
	FGameplayAttributeData UpgradePoints;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, UpgradePoints);

	UPROPERTY(ReplicatedUsing = OnRep_MaxLevel)
	FGameplayAttributeData MaxLevel;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, MaxLevel);

	UPROPERTY(ReplicatedUsing = OnRep_MaxLevelExperience)
	FGameplayAttributeData MaxLevelExperience;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, MaxLevelExperience);

	UPROPERTY(ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, Gold);

	UPROPERTY(ReplicatedUsing = OnRep_StrengthGrowthRate)
	FGameplayAttributeData StrengthGrowthRate;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, StrengthGrowthRate);

	UPROPERTY(ReplicatedUsing = OnRep_IntelligenceGrowthRate)
	FGameplayAttributeData IntelligenceGrowthRate;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, IntelligenceGrowthRate);

	UPROPERTY(ReplicatedUsing = OnRep_AgilityGrowthRate)
	FGameplayAttributeData AgilityGrowthRate;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, AgilityGrowthRate);

	// 魔法恢复
	UPROPERTY(ReplicatedUsing = OnRep_ManaRegen)
	FGameplayAttributeData ManaRegen;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, ManaRegen);

	// 生命恢复
	UPROPERTY(ReplicatedUsing = OnRep_HealthRegen)
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UArenasHeroAttributeSet, HealthRegen);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Agility(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PrevLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_NextLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_UpgradePoints(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxLevel(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_StrengthGrowthRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_IntelligenceGrowthRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AgilityGrowthRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ManaRegen(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HealthRegen(const FGameplayAttributeData& OldValue);
	
	
};
