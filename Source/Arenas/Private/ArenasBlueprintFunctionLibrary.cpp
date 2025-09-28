// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasBlueprintFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagsManager.h"
#include "GAS/ArenasAbilitySystemComponent.h"

UArenasAbilitySystemComponent* UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(AActor* InActor)
{
	return Cast<UArenasAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

UArenasAbilitySystemComponent* UArenasBlueprintFunctionLibrary::BP_GetArenasASCFromActor(AActor* InActor)
{
	return NativeGetArenasASCFromActor(InActor);
}

void UArenasBlueprintFunctionLibrary::AddGameplayTagToActorIfNotHas(AActor* InActor, FGameplayTag InTag)
{
	if (UArenasAbilitySystemComponent* ArenasASC = NativeGetArenasASCFromActor(InActor))
	{
		if (!ArenasASC->HasMatchingGameplayTag(InTag))
		{
			ArenasASC->AddLooseGameplayTag(InTag);
		}
	}
}

void UArenasBlueprintFunctionLibrary::RemoveGameplayTagFromActorIfHas(AActor* InActor, FGameplayTag InTag)
{
	if (UArenasAbilitySystemComponent* ArenasASC = NativeGetArenasASCFromActor(InActor))
	{
		if (ArenasASC->HasMatchingGameplayTag(InTag))
		{
			ArenasASC->RemoveLooseGameplayTag(InTag);
		}
	}
}

bool UArenasBlueprintFunctionLibrary::NativeActorHasGameplayTag(AActor* InActor, const FGameplayTag& InTag)
{
	if (UArenasAbilitySystemComponent* ArenasASC = NativeGetArenasASCFromActor(InActor))
	{
		return ArenasASC->HasMatchingGameplayTag(InTag);
	}
	return false;
}

FName UArenasBlueprintFunctionLibrary::NativeGetGameplayTagLastName(const FGameplayTag& InTag)
{
	if (InTag.IsValid())
	{
		TArray<FName> EventNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(InTag, EventNames);
		if (EventNames.Num() > 0)
		{
			return EventNames.Last();
		}
	}
	return NAME_None;
}

float UArenasBlueprintFunctionLibrary::GetStaticCooldownDurationFromAbility(const UGameplayAbility* InAbility)
{
	if (!InAbility) return 0.f;
	if (const UGameplayEffect* CooldownGE = InAbility->GetCooldownGameplayEffect())
	{
		if (CooldownGE->DurationPolicy == EGameplayEffectDurationType::HasDuration)
		{
			float Cooldown = 0.f;
			CooldownGE->DurationMagnitude.GetStaticMagnitudeIfPossible(1, Cooldown);
			return Cooldown;
		}
	}
	return 0.f;
}

float UArenasBlueprintFunctionLibrary::GetAbilityManaCost(const UGameplayAbility* InAbility, const UArenasAbilitySystemComponent* InArenasASC, float AbilityLevel)
{
	float ManaCost = 0.f;
	if (!InAbility || !InArenasASC) return ManaCost;
	if (const UGameplayEffect* CostGE = InAbility->GetCostGameplayEffect())
	{
		FGameplayEffectSpecHandle EffectSpecHandle = InArenasASC->MakeOutgoingSpec(CostGE->GetClass(), AbilityLevel, InArenasASC->MakeEffectContext());
		
		CostGE->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(*EffectSpecHandle.Data.Get(), ManaCost);

	}

	return FMath::Abs(ManaCost);
	
}

float UArenasBlueprintFunctionLibrary::GetAbilityCooldownDuration(const UGameplayAbility* InAbility, const UArenasAbilitySystemComponent* InArenasASC, float AbilityLevel)
{
	float Cooldown = 0.f;
	if (!InAbility || !InArenasASC) return Cooldown;
	if (const UGameplayEffect* CooldownGE = InAbility->GetCooldownGameplayEffect())
	{
		FGameplayEffectSpecHandle EffectSpecHandle = InArenasASC->MakeOutgoingSpec(CooldownGE->GetClass(), AbilityLevel, InArenasASC->MakeEffectContext());
		CooldownGE->DurationMagnitude.AttemptCalculateMagnitude(*EffectSpecHandle.Data.Get(), Cooldown);
	}
	return FMath::Abs(Cooldown);
}

float UArenasBlueprintFunctionLibrary::GetAbilityCooldownRemainingTime(const UGameplayAbility* InAbility, const UArenasAbilitySystemComponent* InArenasASC)
{
	if (!InAbility || !InArenasASC) return 0.f;

	UGameplayEffect* CooldownGE = InAbility->GetCooldownGameplayEffect();
	if (!CooldownGE) return 0.f;

	FGameplayEffectQuery CooldownQuery;
	CooldownQuery.EffectDefinition = CooldownGE->GetClass();
	FJsonSerializableArrayFloat ActiveCooldowns = InArenasASC->GetActiveEffectsTimeRemaining(CooldownQuery);
	float CooldownRemaining = 0.f;
	for (float RemainingTime : ActiveCooldowns)
	{
		if (RemainingTime > CooldownRemaining)
		{
			CooldownRemaining = RemainingTime;
		}
	}
	return CooldownRemaining;
}

TArray<FString> UArenasBlueprintFunctionLibrary::GetKeyNamesForInputAction(const APlayerController* PlayerController, const UInputAction* InputAction)
{
	TArray<FString> KeyNames;
	if (!PlayerController || !InputAction)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetKeyNamesForInputAction: Invalid PlayerController or InputAction"));
		return KeyNames;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		TArray<FKey> Keys = Subsystem->QueryKeysMappedToAction(InputAction);

		for (const FKey& Key : Keys)
		{
			KeyNames.Add(Key.GetDisplayName().ToString()); // 或者使用 Key.GetDisplayName().ToString()
		}
	}
	return KeyNames;
}

FGameplayTag UArenasBlueprintFunctionLibrary::GetCameraShakeGameplayCueTag()
{
	return FGameplayTag::RequestGameplayTag("GameplayCue.CameraShake");
}

bool UArenasBlueprintFunctionLibrary::IsHeroActor(AActor* InActor)
{
	if (UArenasAbilitySystemComponent* ArenasASC = NativeGetArenasASCFromActor(InActor))
	{
		return ArenasASC->HasMatchingGameplayTag(ArenasGameplayTags::RoleType_Hero);
	}

	return false;
	
}

bool UArenasBlueprintFunctionLibrary::IsAlive(AActor* InActor)
{
	if (UArenasAbilitySystemComponent* ArenasASC = NativeGetArenasASCFromActor(InActor))
	{
		return !ArenasASC->HasMatchingGameplayTag(ArenasGameplayTags::Status_Dead);
	}
	return false;
}

bool UArenasBlueprintFunctionLibrary::IsAbilityAtMaxLevel(const FGameplayAbilitySpec& InAbilitySpec)
{
	return InAbilitySpec.Level >= 4;	// 假设最大等级为4
}

bool UArenasBlueprintFunctionLibrary::CheckAbilityCanCost(const FGameplayAbilitySpec& InAbilitySpec, const UArenasAbilitySystemComponent* InAbilitySystemComponent)
{
	const UGameplayAbility* AbilityCDO = InAbilitySpec.Ability;
	if (!AbilityCDO || !InAbilitySystemComponent) return false;

	return AbilityCDO->CheckCost(InAbilitySpec.Handle, InAbilitySystemComponent->AbilityActorInfo.Get());
}
