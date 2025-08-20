// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UArenasAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 使用 DOREPLIFETIME_CONDITION_NOTIFY 宏来注册属性的复制和通知
	// 这里我们使用 COND_None 来表示这些属性在所有情况下都需要复制
	// REPNOTIFY_Always 表示每次属性发生变化时都会调用 OnRep
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UArenasAttributeSet::BroadcastAttributeInitialValue(const UPawnUIComponent* InPawnUIComponent) const
{
	InPawnUIComponent->OnHealthPercentChanged.Broadcast(UKismetMathLibrary::SafeDivide(GetHealth(), GetMaxHealth()), GetHealth(), GetMaxHealth());
	InPawnUIComponent->OnManaPercentChanged.Broadcast(UKismetMathLibrary::SafeDivide(GetMana(), GetMaxMana()), GetMana(), GetMaxMana());
}

void UArenasAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasAttributeSet, Health, OldValue);
}

void UArenasAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasAttributeSet, MaxHealth, OldValue);
}

void UArenasAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasAttributeSet, Mana, OldValue);
}

void UArenasAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasAttributeSet, MaxMana, OldValue);
}
