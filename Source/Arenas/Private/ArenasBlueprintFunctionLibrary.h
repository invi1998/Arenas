// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArenasBlueprintFunctionLibrary.generated.h"

struct FGameplayTag;
class UArenasAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UArenasAbilitySystemComponent* NativeGetArenasASCFromActor(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "Arenas | FunctionLibrary", meta=(DisplayName="Get Arenas ASC From Actor"))
	static UArenasAbilitySystemComponent* BP_GetArenasASCFromActor(AActor* InActor);

	static FName NativeGetGameplayTagLastName(const FGameplayTag& InTag);
	
};
