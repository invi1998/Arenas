// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArenasBlueprintFunctionLibrary.generated.h"

class UInputAction;
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

	static void AddGameplayTagToActorIfNotHas(AActor* InActor, FGameplayTag InTag);
	static void RemoveGameplayTagFromActorIfHas(AActor* InActor, FGameplayTag InTag);

	// 获取Actor是否存在某个FGameplayTag
	static bool NativeActorHasGameplayTag(AActor* InActor, const FGameplayTag& InTag);

	static FName NativeGetGameplayTagLastName(const FGameplayTag& InTag);

	// 从Ability中获取静态的冷却时间和消耗（不考虑任何加成，这些是技能的原始数据，没有上下文）
	static float GetStaticCooldownDurationFromAbility(const UGameplayAbility* InAbility);
	static float GetStaticCostFromAbility(const UGameplayAbility* InAbility);

	// 传入PlayerController，InputAction，获取InputAction对应的按键名称
	static TArray<FString> GetKeyNamesForInputAction(const APlayerController* PlayerController, const UInputAction* InputAction);

	static FGameplayTag GetCameraShakeGameplayCueTag();

	static bool IsHeroActor(AActor* InActor);
	static bool IsAlive(AActor* InActor);
};
