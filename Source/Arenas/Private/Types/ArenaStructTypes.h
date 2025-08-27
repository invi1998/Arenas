// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ArenaStructTypes.generated.h"

class UArenasGameplayAbility;

USTRUCT(BlueprintType)
struct FArenasAbilitySet
{
	GENERATED_BODY()

	// 技能输入标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Category = "InputTag", GameplayTagFilter = "InputTag"))
	FGameplayTag InputTag;

	// 赋予的技能
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UArenasGameplayAbility> AbilityToGrantClass;

	bool IsValid() const;
	
};

