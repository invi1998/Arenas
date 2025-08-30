// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasBlueprintFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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
