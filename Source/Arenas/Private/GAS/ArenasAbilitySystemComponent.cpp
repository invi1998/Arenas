// Ace of Arenas. (invi1998 All Rights Reserved)


#include "GAS/ArenasAbilitySystemComponent.h"

#include "ArenasAttributeSet.h"
#include "ArenasGameplayTags.h"
#include "ArenasHeroAttributeSet.h"
#include "GAS/Abilities/ArenasGameplayAbility.h"
#include "player/ArenasPlayerCharacter.h"

UArenasAbilitySystemComponent::UArenasAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetHealthAttribute()).AddUObject(this, &UArenasAbilitySystemComponent::HandleHealthChanged);
	GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).AddUObject(this, &UArenasAbilitySystemComponent::HandleManaChanged);
	
	GenericConfirmInputID = static_cast<int32>(EArenasAbilityInputID::Confirm);	// 确认输入ID
	GenericCancelInputID = static_cast<int32>(EArenasAbilityInputID::Cancel);	// 取消输入ID
}

void UArenasAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!BaseStatsDataTable || !GetOwner()) return;

	const FHeroBaseStats* BaseStats = nullptr;

	for (const TPair<FName, uint8*>& DataPair : BaseStatsDataTable->GetRowMap())
	{
		// BaseStats = reinterpret_cast<FHeroBaseStats*>(DataPair.Value);
		BaseStats = BaseStatsDataTable->FindRow<FHeroBaseStats>(DataPair.Key, "");
		if (BaseStats && BaseStats->HeroClass == GetOwner()->GetClass())
		{
			break;
		}
	}

	// 此处我们只筛选出英雄类的基础数据，小兵、建筑等其他类型的单位不在此处初始化属性
	if (!BaseStats) return;

	SetNumericAttributeBase(UArenasAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);
	SetNumericAttributeBase(UArenasAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);
	SetNumericAttributeBase(UArenasAttributeSet::GetAttackDamageAttribute(), BaseStats->BaseAttackDamage);
	SetNumericAttributeBase(UArenasAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);
	SetNumericAttributeBase(UArenasAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);

	SetNumericAttributeBase(UArenasHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetExperienceAttribute(), 0.f);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetPrevLevelExperienceAttribute(), 0.f);
	// SetNumericAttributeBase(UArenasHeroAttributeSet::GetNextLevelExperienceAttribute(), BaseStats->ExperienceToNextLevel);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetLevelAttribute(), 1.f);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetUpgradePointsAttribute(), 0.f);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetMaxLevelAttribute(), BaseStats->MaxLevel);
	// SetNumericAttributeBase(UArenasHeroAttributeSet::GetMaxLevelExperienceAttribute(), BaseStats->MaxLevelExperience);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetGoldAttribute(), BaseStats->StartingGold);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowthRate);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate);
	
}

void UArenasAbilitySystemComponent::ServerSideInit()
{
	// 此处顺序不可颠倒，因为初始化属性时，可能会有些属性是由技能计算得来的
	InitializeBaseAttributes();
	ApplyInitialEffects();
	GiveInitialAbilities();
}

void UArenasAbilitySystemComponent::ApplyFullStateEffect()
{
	AuthApplyGameplayEffectToSelf(FullStateEffectClass);
}

void UArenasAbilitySystemComponent::AddGameplayTagToActorIfNotHas(FGameplayTag InTag)
{
	if (!HasMatchingGameplayTag(InTag))
	{
		// 这是一个只会在本地机器上被添加的标签，不会被网络同步（因为在调用该函数之前，我们已经确保了只在服务端调用，所以需要注意，该函数添加标签是不会进行同步的）
		AddLooseGameplayTag(InTag);
	}
}

void UArenasAbilitySystemComponent::RemoveGameplayTagFromActorIfHas(FGameplayTag InTag)
{
	if (HasMatchingGameplayTag(InTag))
	{
		RemoveLooseGameplayTag(InTag);
	}
}

void UArenasAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialGameplayEffects)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UArenasAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TPair<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		if (AbilityPair.Value)
		{
			FGameplayAbilitySpec AbilitySpec(AbilityPair.Value, 0, static_cast<int32>(AbilityPair.Key));
			GiveAbility(AbilitySpec);
		}
	}

	for (const TPair<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		if (AbilityPair.Value)
		{
			FGameplayAbilitySpec AbilitySpec(AbilityPair.Value, 1, static_cast<int32>(AbilityPair.Key));
			GiveAbility(AbilitySpec);
		}
	}
	
}

void UArenasAbilitySystemComponent::AuthApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 EffectLevel)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffectClass, EffectLevel, MakeEffectContext());
	if (EffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UArenasAbilitySystemComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	if (Data.NewValue <= 0.f)
	{
		AddGameplayTagToActorIfNotHas(ArenasGameplayTags::Status_Health_Empty);
		RemoveGameplayTagFromActorIfHas(ArenasGameplayTags::Status_Health_Full);
		if (DeathEffectClass)
		{
			AuthApplyGameplayEffectToSelf(DeathEffectClass);
		}
	}
	else
	{
		RemoveGameplayTagFromActorIfHas(ArenasGameplayTags::Status_Health_Empty);
		bool bFound = false;
		float MaxHealth = GetGameplayAttributeValue(UArenasAttributeSet::GetMaxHealthAttribute(), bFound);
		if (bFound)
		{
			if (Data.NewValue >= MaxHealth)
			{
				AddGameplayTagToActorIfNotHas(ArenasGameplayTags::Status_Health_Full);
			}
			else
			{
				RemoveGameplayTagFromActorIfHas(ArenasGameplayTags::Status_Health_Full);
			}
			
		}
	}
}

void UArenasAbilitySystemComponent::HandleManaChanged(const FOnAttributeChangeData& Data)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (Data.NewValue <= 0.f)
	{
		AddGameplayTagToActorIfNotHas(ArenasGameplayTags::Status_Mana_Empty);
		RemoveGameplayTagFromActorIfHas(ArenasGameplayTags::Status_Mana_Full);
	}
	else
	{
		RemoveGameplayTagFromActorIfHas(ArenasGameplayTags::Status_Mana_Empty);
		bool bFound = false;
		float MaxMana = GetGameplayAttributeValue(UArenasAttributeSet::GetMaxManaAttribute(), bFound);
		if (bFound)
		{
			if (Data.NewValue >= MaxMana)
			{
				AddGameplayTagToActorIfNotHas(ArenasGameplayTags::Status_Mana_Full);
			}
			else
			{
				RemoveGameplayTagFromActorIfHas(ArenasGameplayTags::Status_Mana_Full);
			}
			
		}
	}
	
}

