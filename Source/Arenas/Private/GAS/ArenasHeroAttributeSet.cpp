// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasHeroAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UArenasHeroAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, PrevLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, NextLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, UpgradePoints, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, MaxLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, MaxLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, Gold, COND_None, REPNOTIFY_Always);

	// 实际上这两个属性在游戏中不会频繁变化，我们可以考虑使用 REPNOTIFY_OnChanged 来优化网络流量
	// 甚至这个增量数据只在服务端存在就行，但是因为方便客户端展示，所以这里还是选择同步到客户端
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, StrengthGrowthRate, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasHeroAttributeSet, IntelligenceGrowthRate, COND_None, REPNOTIFY_OnChanged);
	
}

void UArenasHeroAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, Intelligence, OldValue);
}

void UArenasHeroAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, Strength, OldValue);
}

void UArenasHeroAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, Experience, OldValue);
}

void UArenasHeroAttributeSet::OnRep_PrevLevelExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, PrevLevelExperience, OldValue);
}

void UArenasHeroAttributeSet::OnRep_NextLevelExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, NextLevelExperience, OldValue);
}

void UArenasHeroAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, Level, OldValue);
}

void UArenasHeroAttributeSet::OnRep_UpgradePoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, UpgradePoints, OldValue);
}

void UArenasHeroAttributeSet::OnRep_MaxLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, MaxLevel, OldValue);
}

void UArenasHeroAttributeSet::OnRep_MaxLevelExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, MaxLevelExperience, OldValue);
}

void UArenasHeroAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, Gold, OldValue);
}

void UArenasHeroAttributeSet::OnRep_StrengthGrowthRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, StrengthGrowthRate, OldValue);
}

void UArenasHeroAttributeSet::OnRep_IntelligenceGrowthRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasHeroAttributeSet, IntelligenceGrowthRate, OldValue);
}
