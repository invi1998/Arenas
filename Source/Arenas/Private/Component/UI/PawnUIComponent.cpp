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
	BoardCastHealthAndManaRegenState();
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
	BoardCastHealthAndManaRegenState();
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
	BoardCastHealthAndManaRegenState();
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
	BoardCastHealthAndManaRegenState();
}

void UPawnUIComponent::SetAndBoundAttributeDelegate(UArenasAbilitySystemComponent* InArenasASC)
{
	if (InArenasASC)
	{
		CachedArenasASC = InArenasASC;
		bool bFound = false;
		float HealthC = CachedArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetHealthAttribute(), bFound);
		float MaxHealthC = CachedArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetMaxHealthAttribute(), bFound);
		float ManaC = CachedArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetManaAttribute(), bFound);
		float MaxManaC = CachedArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetMaxManaAttribute(), bFound);
		// 初始化当前属性值
		if (bFound)
		{
			Health = HealthC;
			MaxHealth = MaxHealthC;
			Mana = ManaC;
			MaxMana = MaxManaC;

			bIsHealthFull = FMath::IsNearlyEqual(Health, MaxHealth) && MaxHealth > 0.f;
			bIsManaFull = FMath::IsNearlyEqual(Mana, MaxMana) && MaxMana > 0.f;
			
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
		CachedArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		CachedArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);
		CachedArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaChanged);
		CachedArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ThisClass::OnMaxManaChanged);

		
	}
}

void UPawnUIComponent::BoardCastHealthAndManaRegenState()
{
	bIsHealthFull = FMath::IsNearlyEqual(Health, MaxHealth) && MaxHealth > 0.f;
	bIsManaFull = FMath::IsNearlyEqual(Mana, MaxMana) && MaxMana > 0.f;
}

