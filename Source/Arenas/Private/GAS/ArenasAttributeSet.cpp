// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasAttributeSet.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

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

void UArenasAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);	// 最大生命值至少为1
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);	// 最大法力值至少为0
	}
}

void UArenasAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		// 处理伤害
		float DamageAmount = GetDamageTaken();
		if (DamageAmount > 0.f)
		{
			float NewHealth = GetHealth() - DamageAmount;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			SetDamageTaken(0.f); // 重置伤害值
		}
	}
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

void UArenasAttributeSet::OnRep_DamageTaken(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasAttributeSet, DamageTaken, OldValue);
}
