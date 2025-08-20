// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasBlueprintFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/ArenasAbilitySystemComponent.h"

UArenasAbilitySystemComponent* UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(AActor* InActor)
{
	return Cast<UArenasAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

UArenasAbilitySystemComponent* UArenasBlueprintFunctionLibrary::BP_GetArenasASCFromActor(AActor* InActor)
{
	return NativeGetArenasASCFromActor(InActor);
}
