// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasBlueprintFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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

float UArenasBlueprintFunctionLibrary::GetStaticCostFromAbility(const UGameplayAbility* InAbility)
{
	if (!InAbility) return 0.f;
	if (const UGameplayEffect* CostGE = InAbility->GetCostGameplayEffect())
	{
		if (CostGE->Modifiers.Num() == 0) return 0.f; // 如果没有任何修饰符，说明没有消耗
		
		float Cost = 0.f;
		CostGE->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
		return FMath::Abs(Cost);	// 消耗值应该是正数
	}
	return 0.f;
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
