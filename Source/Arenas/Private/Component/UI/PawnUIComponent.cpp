// Ace of Arenas. (invi1998 All Rights Reserved)


#include "PawnUIComponent.h"

#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

void UPawnUIComponent::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	Health = OnAttributeChangeData.NewValue;
	if (MaxHealth <= 0.f)
	{
		return;
	}
	float HealthPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	
	OnHealthPercentChanged.Broadcast(HealthPercent, Health, MaxHealth);
}

void UPawnUIComponent::OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MaxHealth = OnAttributeChangeData.NewValue;
	if (MaxHealth <= 0.f)
	{
		return;
	}
	float HealthPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	OnHealthPercentChanged.Broadcast(HealthPercent, Health, MaxHealth);
}

void UPawnUIComponent::OnManaChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	Mana = OnAttributeChangeData.NewValue;
	if (MaxMana <= 0.f)
	{
		return;
	}
	float ManaPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
	OnManaPercentChanged.Broadcast(ManaPercent, Mana, MaxMana);
}

void UPawnUIComponent::OnMaxManaChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MaxMana = OnAttributeChangeData.NewValue;
	if (MaxMana <= 0.f)
	{
		return;
	}
	float ManaPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
	OnManaPercentChanged.Broadcast(ManaPercent, Mana, MaxMana);
}

void UPawnUIComponent::SetAndBoundAttributeDelegate(UArenasAbilitySystemComponent* InArenasASC)
{
	if (InArenasASC)
	{
		bool bFound = false;
		float HealthC = InArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetHealthAttribute(), bFound);
		float MaxHealthC = InArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetMaxHealthAttribute(), bFound);
		float ManaC = InArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetManaAttribute(), bFound);
		float MaxManaC = InArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetMaxManaAttribute(), bFound);
		// 初始化当前属性值
		if (bFound)
		{
			Health = HealthC;
			MaxHealth = MaxHealthC;
			Mana = ManaC;
			MaxMana = MaxManaC;
			
			// 广播当前属性值
			OnHealthPercentChanged.Broadcast(UKismetMathLibrary::SafeDivide(Health, MaxHealth), Health, MaxHealth);
			OnManaPercentChanged.Broadcast(UKismetMathLibrary::SafeDivide(Mana, MaxMana), Mana, MaxMana);
		}
		else
		{
			// 如果没有找到属性值，则将其设置为0
			Health = 0.f;
			MaxHealth = 0.f;
			Mana = 0.f;
			MaxMana = 0.f;
		}

		/*
		// 广播当前属性值
		if (const UArenasAttributeSet* AttributeSet = Cast<UArenasAttributeSet>(InArenasASC->GetAttributeSet(UArenasAttributeSet::StaticClass())))
		{
			AttributeSet->BroadcastAttributeInitialValue(this);
		}
		 */
		
		// 绑定属性变化的委托
		InArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		InArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);
		InArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaChanged);
		InArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ThisClass::OnMaxManaChanged);

		
	}
}
