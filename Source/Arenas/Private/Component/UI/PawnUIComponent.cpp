// Ace of Arenas. (invi1998 All Rights Reserved)


#include "PawnUIComponent.h"

#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

void UPawnUIComponent::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	Health = OnAttributeChangeData.NewValue;
	float HealthPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	OnHealthPercentChanged.Broadcast(HealthPercent, Health, MaxHealth);
}

void UPawnUIComponent::OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MaxHealth = OnAttributeChangeData.NewValue;
	float HealthPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	OnHealthPercentChanged.Broadcast(HealthPercent, Health, MaxHealth);
}

void UPawnUIComponent::OnManaChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	Mana = OnAttributeChangeData.NewValue;
	float ManaPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
	OnManaPercentChanged.Broadcast(ManaPercent, Mana, MaxMana);
}

void UPawnUIComponent::OnMaxManaChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MaxMana = OnAttributeChangeData.NewValue;
	float ManaPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
	OnManaPercentChanged.Broadcast(ManaPercent, Mana, MaxMana);
}

void UPawnUIComponent::SetAndBoundAttributeDelegate(UArenasAbilitySystemComponent* InArenasASC)
{
	if (InArenasASC)
	{
		if (ArenasASC)
		{
			// 广播当前属性值
			if (const UArenasAttributeSet* AttributeSet = ArenasASC->GetSet<UArenasAttributeSet>())
			{
				AttributeSet->BroadcastAttributeInitialValue(this);
			}

			return;
		}

		ArenasASC = InArenasASC;

		// 广播当前属性值
		if (const UArenasAttributeSet* AttributeSet = ArenasASC->GetSet<UArenasAttributeSet>())
		{
			AttributeSet->BroadcastAttributeInitialValue(this);
		}

		// 绑定属性变化的委托
		ArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		ArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);
		ArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaChanged);
		ArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ThisClass::OnMaxManaChanged);
	}
}
