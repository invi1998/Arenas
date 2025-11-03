// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "Component/UI/PawnUIComponent.h"
#include "ArenasAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UPawnUIComponent;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	void BroadcastAttributeInitialValue(const UPawnUIComponent* InPawnUIComponent) const;

	void RescaleHealth();
	void RescaleMana();
	
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, Health);

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, MaxHealth);

	UPROPERTY(ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, Mana);

	UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, MaxMana);

	UPROPERTY(ReplicatedUsing = OnRep_DamageTaken)
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, DamageTaken);

	UPROPERTY(ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, AttackDamage);

	UPROPERTY(ReplicatedUsing = OnRep_AttackDamageEx)
	FGameplayAttributeData AttackDamageEx;	// 额外攻击力（来自物品等）
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, AttackDamageEx);

	UPROPERTY(ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, Armor);

	UPROPERTY(ReplicatedUsing = OnRep_ArmorEx)
	FGameplayAttributeData ArmorEx;	// 额外护甲（来自物品等）
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, ArmorEx);
	
	UPROPERTY(ReplicatedUsing = OnRep_MagicResist)
	FGameplayAttributeData MagicResist;		// 魔法抗性，百分比表示法，25表示25%魔抗
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, MagicResist);
	
	UPROPERTY(ReplicatedUsing = OnRep_MagicResistEx)
	FGameplayAttributeData MagicResistEx;	// 额外魔法抗性，百分比表示法，25表示25%魔抗
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, MagicResistEx);

	UPROPERTY(ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;		// 攻击速度，百分比表示法，100表示1倍攻速，150表示1.5倍攻速
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, AttackSpeed);

	UPROPERTY(ReplicatedUsing = OnRep_AttackSpeedEx)
	FGameplayAttributeData AttackSpeedEx;	// 额外攻击速度，百分比表示法，100表示1倍攻速，150表示1.5倍攻速
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, AttackSpeedEx);

	UPROPERTY(ReplicatedUsing = OnRep_AttackRange)
	FGameplayAttributeData AttackRange;		// 攻击范围，单位：厘米
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, AttackRange);

	UPROPERTY(ReplicatedUsing = OnRep_AttackRangEx)
	FGameplayAttributeData AttackRangeEx;	// 额外攻击范围，单位：厘米
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, AttackRangeEx);
	
	UPROPERTY(ReplicatedUsing = OnRep_DogeChance)
	FGameplayAttributeData DogeChance;	// 闪避，百分比表示法，10表示10%几率闪避
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, DogeChance);

	UPROPERTY(ReplicatedUsing = OnRep_DogeChanceEx)
	FGameplayAttributeData DogeChanceEx;	// 额外闪避，百分比表示法，10表示10%几率闪避
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, DogeChanceEx);

	UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, MoveSpeed);

	UPROPERTY(ReplicatedUsing = OnRep_MoveSpeedEx)
	FGameplayAttributeData MoveSpeedEx;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, MoveSpeedEx);

	UPROPERTY(ReplicatedUsing = OnRep_MoveAcceleration)
	FGameplayAttributeData MoveAcceleration;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, MoveAcceleration);

	// 魔法恢复
	UPROPERTY(ReplicatedUsing = OnRep_ManaRegen)
	FGameplayAttributeData ManaRegen;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, ManaRegen);

	// 生命恢复
	UPROPERTY(ReplicatedUsing = OnRep_HealthRegen)
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, HealthRegen);

	// 缓存的血量和蓝量百分比，在最大血量和蓝量变化时，我们希望同步缩放血量和蓝量，该比例只存在服务端，不需要同步
	UPROPERTY()
	FGameplayAttributeData CachedHealthPercent;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, CachedHealthPercent);

	UPROPERTY()
	FGameplayAttributeData CachedManaPercent;
	ATTRIBUTE_ACCESSORS(UArenasAttributeSet, CachedManaPercent);


private:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_DamageTaken(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_AttackDamageEx(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_ArmorEx(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MagicResist(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MagicResistEx(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_AttackSpeedEx(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_AttackRange(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_AttackRangEx(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_DogeChance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_DogeChanceEx(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_ManaRegen(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HealthRegen(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeedEx(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MoveAcceleration(const FGameplayAttributeData& OldValue) const;
};
