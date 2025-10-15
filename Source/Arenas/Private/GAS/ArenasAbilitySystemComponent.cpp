// Ace of Arenas. (invi1998 All Rights Reserved)


#include "GAS/ArenasAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasAttributeSet.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "ArenasHeroAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "PA_AbilitySystemGenerics.h"
#include "GAS/Abilities/ArenasGameplayAbility.h"
#include "player/ArenasPlayerCharacter.h"

UArenasAbilitySystemComponent::UArenasAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetHealthAttribute()).AddUObject(this, &UArenasAbilitySystemComponent::HandleHealthChanged);
	GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).AddUObject(this, &UArenasAbilitySystemComponent::HandleManaChanged);
	GetGameplayAttributeValueChangeDelegate(UArenasHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &UArenasAbilitySystemComponent::HandleExperienceChanged);
	
	GenericConfirmInputID = static_cast<int32>(EArenasAbilityInputID::Confirm);	// 确认输入ID
	GenericCancelInputID = static_cast<int32>(EArenasAbilityInputID::Cancel);	// 取消输入ID
}

void UArenasAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!AbilitySystemGenerics->GetBaseStatsDataTable() || !GetOwner()) return;

	const FHeroBaseStats* BaseStats = nullptr;

	for (const TPair<FName, uint8*>& DataPair : AbilitySystemGenerics->GetBaseStatsDataTable()->GetRowMap())
	{
		// BaseStats = reinterpret_cast<FHeroBaseStats*>(DataPair.Value);
		BaseStats = AbilitySystemGenerics->GetBaseStatsDataTable()->FindRow<FHeroBaseStats>(DataPair.Key, "");
		if (BaseStats && BaseStats->HeroClass == GetOwner()->GetClass())
		{
			break;
		}
	}

	// 此处我们只筛选出英雄类的基础数据，小兵、建筑等其他类型的单位不在此处初始化属性
	if (!BaseStats) return;

	SetNumericAttributeBase(UArenasHeroAttributeSet::GetHeroTypeAttribute(), BaseStats->HeroType);
	
	SetNumericAttributeBase(UArenasAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);
	SetNumericAttributeBase(UArenasAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);
	SetNumericAttributeBase(UArenasAttributeSet::GetAttackDamageAttribute(), BaseStats->BaseAttackDamage);
	SetNumericAttributeBase(UArenasAttributeSet::GetAttackSpeedAttribute(), BaseStats->BaseAttackSpeed);
	SetNumericAttributeBase(UArenasAttributeSet::GetAttackRangeAttribute(), BaseStats->BaseAttackRange);
	SetNumericAttributeBase(UArenasAttributeSet::GetDogeChanceAttribute(), BaseStats->BaseDogeChance);
	SetNumericAttributeBase(UArenasAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);
	SetNumericAttributeBase(UArenasAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);

	SetNumericAttributeBase(UArenasHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetAgilityAttribute(), BaseStats->Agility);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetExperienceAttribute(), 0.f);
	
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetGoldAttribute(), BaseStats->StartingGold);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowthRate);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetAgilityGrowthRateAttribute(), BaseStats->AgilityGrowthRate);
	
	if (const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetEXPCurve())
	{
		int32 MaxLevel = ExperienceCurve->GetNumKeys();
		SetNumericAttributeBase(UArenasHeroAttributeSet::GetMaxLevelAttribute(), MaxLevel);
		SetNumericAttributeBase(UArenasHeroAttributeSet::GetMaxLevelExperienceAttribute(), ExperienceCurve->GetKeyValue(ExperienceCurve->GetLastKeyHandle()));
	}

	// 初始化当前经验值和等级，以及升级点信息
	HandleExperienceChanged(FOnAttributeChangeData{});
	
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
	AuthApplyGameplayEffectToSelf(AbilitySystemGenerics->GetFullStateEffectClass());
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

bool UArenasAbilitySystemComponent::IsAtMaxLevel() const
{
	bool bFound = false;
	float CurrentLevel = GetGameplayAttributeValue(UArenasHeroAttributeSet::GetLevelAttribute(), bFound);
	if (bFound)
	{
		float MaxLevel = GetGameplayAttributeValue(UArenasHeroAttributeSet::GetMaxLevelAttribute(), bFound);
		if (bFound)
		{
			return FMath::IsNearlyEqual(CurrentLevel, MaxLevel);
		}
	}
	return false;
}

void UArenasAbilitySystemComponent::Server_UpgradeAbilityWithInputID_Implementation(EArenasAbilityInputID AbilityID)
{
	bool bFound = false;
	float UpgradePoints = GetGameplayAttributeValue(UArenasHeroAttributeSet::GetUpgradePointsAttribute(), bFound);
	
	if (bFound && UpgradePoints >= 1.f)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID(static_cast<int32>(AbilityID)))
		{
			// 如果技能已经达到最大等级，则不能再升级
			if (UArenasBlueprintFunctionLibrary::IsAbilityAtMaxLevel(*AbilitySpec)) return;
			
			SetNumericAttributeBase(UArenasHeroAttributeSet::GetUpgradePointsAttribute(), UpgradePoints - 1.f);
			AbilitySpec->Level += 1;
			// 标记AbilitySpec为已更改，以便在网络上同步
			MarkAbilitySpecDirty(*AbilitySpec);

			// 通知客户端升级成功
			Client_AbilityUpgradeSuccess(AbilitySpec->Handle, AbilitySpec->Level);
		}
	}
}

bool UArenasAbilitySystemComponent::Server_UpgradeAbilityWithInputID_Validate(EArenasAbilityInputID AbilityID)
{
	return true;
}

void UArenasAbilitySystemComponent::Client_AbilityUpgradeSuccess_Implementation(FGameplayAbilitySpecHandle AbilitySpecHandle, int32 NewAbilityLevel)
{
	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle))
	{
		AbilitySpec->Level = NewAbilityLevel;
		AbilitySpecDirtiedCallbacks.Broadcast(*AbilitySpec);
	}
}

void UArenasAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : AbilitySystemGenerics->GetInitialGameplayEffects())
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

	for (const TSubclassOf<UGameplayAbility>& Ability : AbilitySystemGenerics->GetPassiveAbilities())
	{
		if (Ability)
		{
			FGameplayAbilitySpec AbilitySpec(Ability, 1, -1, nullptr);
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

	// 是否是受到伤害
	if (Data.NewValue < Data.OldValue)
	{
		// 广播触发受伤事件
		if (Data.GEModData)
		{
			AActor* DamageSource = Data.GEModData->EffectSpec.GetContext().GetOriginalInstigator();
			float DamageAmount = Data.OldValue - Data.NewValue;
			OnAnyDamageTaken.Broadcast(DamageSource, DamageAmount);
		}
	}
	
	if (Data.NewValue <= 0.f)
	{
		OnActorDeath.Broadcast(GetOwner());
		AddGameplayTagToActorIfNotHas(ArenasGameplayTags::Status_Health_Empty);
		RemoveGameplayTagFromActorIfHas(ArenasGameplayTags::Status_Health_Full);
		if (AbilitySystemGenerics->GetDeathEffectClass())
		{
			AuthApplyGameplayEffectToSelf(AbilitySystemGenerics->GetDeathEffectClass());
		}

		/*
		 * 下面的代码逻辑已经被放到 ArenasAttributeSet::PostGameplayEffectExecute 中处理了(这里我两个地方都保留）
		 * 因为我们从接收伤害到血量变更之间是通过DamageToken这个中间量来设置的，所以此处监听血量变化并不会附带Data.GEModData信息，因为我们是在属性集里直接SetHealth来修改的血量
		 * 当然我们也可以舍弃这个中间量去在GE里应用并修改血量，这样就能在此处监听血量变化时有GE信息了
		 */
		if (Data.GEModData)
		{
			FGameplayEventData DeathEventData;
			DeathEventData.ContextHandle = Data.GEModData->EffectSpec.GetContext();	// 传递效果上下文，以便获取击杀者等信息

			// 触发死亡事件
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), ArenasGameplayTags::Status_Dead, DeathEventData);
			
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

void UArenasAbilitySystemComponent::HandleExperienceChanged(const FOnAttributeChangeData& Data)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (IsAtMaxLevel()) return;

	if (!AbilitySystemGenerics) return;

	float CurrentExp = Data.NewValue;
	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetEXPCurve();
	if (!ExperienceCurve) return;

	float PrevLevelExp = 0.f;
	float NextLevelExp = 0.f;
	float NewLevel = 1;
	for (auto It = ExperienceCurve->GetKeyHandleIterator(); It; ++It)
	{
		float ExpToReachLevel = ExperienceCurve->GetKeyValue(*It);
		if (CurrentExp < ExpToReachLevel)
		{
			NextLevelExp = ExpToReachLevel;
			break;
		}
		PrevLevelExp = ExpToReachLevel;
		NewLevel = It.GetIndex() + 1;
	}

	float CurrentLevel = GetNumericAttributeBase(UArenasHeroAttributeSet::GetLevelAttribute());
	float CurrentUpgradePoints = GetNumericAttributeBase(UArenasHeroAttributeSet::GetUpgradePointsAttribute());
	float LevelUpgraded = NewLevel - CurrentLevel;
	float NewUpgradePoints = CurrentUpgradePoints + LevelUpgraded;

	SetNumericAttributeBase(UArenasHeroAttributeSet::GetUpgradePointsAttribute(), NewUpgradePoints);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetLevelAttribute(), NewLevel);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetPrevLevelExperienceAttribute(), PrevLevelExp);
	SetNumericAttributeBase(UArenasHeroAttributeSet::GetNextLevelExperienceAttribute(), NextLevelExp);
	
}

