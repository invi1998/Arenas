// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ArenaStructTypes.generated.h"

class AArenasPlayerCharacter;
class UInputAction;
class UGameplayAbility;
class UGameplayEffect;
class UArenasGameplayAbility;

UENUM(BlueprintType)
enum class EArenasAbilityInputID : uint8
{
	None				UMETA(DisplayName = "None"),
	BasicAttack			UMETA(DisplayName = "基础攻击"),
	Ability1			UMETA(DisplayName = "技能1"),
	Ability2			UMETA(DisplayName = "技能2"),
	Ability3			UMETA(DisplayName = "技能3"),
	Ability4			UMETA(DisplayName = "技能4"),
	Ability5			UMETA(DisplayName = "技能5"),
	Ability6			UMETA(DisplayName = "技能6"),
	Confirm				UMETA(DisplayName = "确认"),
	Cancel				UMETA(DisplayName = "取消"),
};


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

USTRUCT(BlueprintType)
struct FGenericDamageEffectData
{
	GENERATED_BODY()

	FGenericDamageEffectData();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	// 推力方向
	UPROPERTY(EditAnywhere)
	FVector PushVelocity;
	
};

USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()

	FAbilityWidgetData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> AbilityIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* InputAction;
	
};

USTRUCT(BlueprintType)
struct FHeroBaseStats : public FTableRowBase
{
	GENERATED_BODY()

	FHeroBaseStats();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AArenasPlayerCharacter> HeroClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseMaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseMaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseArmor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseAttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDogeChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Intelligence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Agility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StrengthGrowthRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IntelligenceGrowthRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AgilityGrowthRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExpGrowthRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StartingGold;
	
};


