// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasAbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
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
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasAttributeSet, DamageTaken, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UArenasAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
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

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 处理生命值变化
		float NewHealth = GetHealth();
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		SetCachedHealthPercent(UKismetMathLibrary::SafeDivide(GetHealth(), GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// 处理最大生命值变化
		float NewMaxHealth = GetMaxHealth();
		SetMaxHealth(FMath::Max(NewMaxHealth, 1.f)); // 最大生命值至少为1

		// 调整当前生命值以适应新的最大生命值
		float CurrentHealth = GetHealth();
		if (CurrentHealth > NewMaxHealth)
		{
			SetHealth(NewMaxHealth);
		}
		
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// 处理法力值变化
		float NewMana = GetMana();
		SetMana(FMath::Clamp(NewMana, 0.f, GetMaxMana()));
		SetCachedManaPercent(UKismetMathLibrary::SafeDivide(GetMana(), GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
	{
		// 处理最大法力值变化
		float NewMaxMana = GetMaxMana();
		SetMaxMana(FMath::Max(NewMaxMana, 0.f)); // 最大法力值至少为0

		// 调整当前法力值以适应新的最大法力值
		float CurrentMana = GetMana();
		if (CurrentMana > NewMaxMana)
		{
			SetMana(NewMaxMana);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		// 处理伤害
		float DamageAmount = GetDamageTaken();
		if (DamageAmount > 0.f)
		{
			float NewHealth = GetHealth() - DamageAmount;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			SetDamageTaken(0.f); // 重置伤害值

			// 这里如果直接通过SetHealth来设置角色血量，会导致丢失FGameplayEffectModCallbackData中的数据
			// 所以需要手动调用OnHealthChanged等函数来处理血量变化
			AActor* OwnerActor = GetOwningActor();
			if ( OwnerActor && OwnerActor->HasAuthority() && NewHealth <= 0.f)
			{
				if (UArenasAbilitySystemComponent* ArenasASC = Cast<UArenasAbilitySystemComponent>(OwnerActor->GetComponentByClass(UArenasAbilitySystemComponent::StaticClass())))
				{
					FGameplayEventData DeathEventData;
					DeathEventData.ContextHandle = Data.EffectSpec.GetContext();	// 传递效果上下文，以便获取击杀者等信息

					// 触发死亡事件
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, ArenasGameplayTags::Status_Dead, DeathEventData);
			
				}
			}
			
		}
	}
}

void UArenasAttributeSet::BroadcastAttributeInitialValue(const UPawnUIComponent* InPawnUIComponent) const
{
	InPawnUIComponent->OnHealthPercentChanged.Broadcast(UKismetMathLibrary::SafeDivide(GetHealth(), GetMaxHealth()), GetHealth(), GetMaxHealth());
	InPawnUIComponent->OnManaPercentChanged.Broadcast(UKismetMathLibrary::SafeDivide(GetMana(), GetMaxMana()), GetMana(), GetMaxMana());
}

void UArenasAttributeSet::RescaleHealth()
{
	if (!GetOwningActor() || !GetOwningActor()->HasAuthority()) return;

	if (GetCachedHealthPercent() != 0.f)
	{
		float NewHealth = GetMaxHealth() * GetCachedHealthPercent();
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
	}
}

void UArenasAttributeSet::RescaleMana()
{
	if (!GetOwningActor() || !GetOwningActor()->HasAuthority()) return;

	if (GetCachedManaPercent() != 0.f)
	{
		float NewMana = GetMaxMana() * GetCachedManaPercent();
		SetMana(FMath::Clamp(NewMana, 0.f, GetMaxMana()));
	}
	
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

void UArenasAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasAttributeSet, AttackDamage, OldValue);
}

void UArenasAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasAttributeSet, Armor, OldValue);
}

void UArenasAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UArenasAttributeSet, MoveSpeed, OldValue);
}
