// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Types/ArenaStructTypes.h"
#include "ArenasAbilitySystemComponent.generated.h"

class UPA_AbilitySystemGenerics;
class IPawnUIInterface;

// 伤害变化委托（伤害来源，伤害数值）
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAnyDamageTakenDelegate, AActor*, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorDeathDelegate, AActor*);

/**
 * 
 */
UCLASS()
class UArenasAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UArenasAbilitySystemComponent();

	FOnAnyDamageTakenDelegate OnAnyDamageTaken;
	FOnActorDeathDelegate OnActorDeath;

	void InitializeBaseAttributes();
	void ServerSideInit();
	void ApplyFullStateEffect();
	
	bool IsHeroCharacter() const;
	bool IsMinionCharacter() const;

	void AddKillsMatchStatNumber(float InNum);
	void AddDeathMatchStatNumber(float InNum);
	void AddDamageMatchStatNumber(float InDamage);
	void AddAssistsMatchStatNumber(float InNum);
	void AddGoldEarnedMatchStatNumber(float InNum);
	void AddKillMinionMatchStatNumber(float InNum);

	const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }

	void AddGameplayTagToActorIfNotHas(FGameplayTag InTag);
	void RemoveGameplayTagFromActorIfHas(FGameplayTag InTag);

	bool IsAtMaxLevel() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpgradeAbilityWithInputID(EArenasAbilityInputID AbilityID);

	UFUNCTION(Client, Reliable)
	void Client_AbilityUpgradeSuccess(FGameplayAbilitySpecHandle AbilitySpecHandle, int32 NewAbilityLevel);

private:
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	
	void AuthApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 EffectLevel = 1);
	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	void HandleManaChanged(const FOnAttributeChangeData& Data);
	void HandleExperienceChanged(const FOnAttributeChangeData& Data);
	void HandleGoldChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	UPROPERTY(EditDefaultsOnly, Category = "PA_Generics")
	UPA_AbilitySystemGenerics* AbilitySystemGenerics;		// 赋予初始属性和效果
	
	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
	float AssistTimeThreshold = 30.f; // 助攻时间阈值，单位秒，在这个时间内对敌人造成伤害的角色会被计为助攻

	// TMap， Key为伤害来源Actor, Value为受到伤害的时间戳，用来统计助攻
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, float> RecentDamageSourcesMap;
	
};
