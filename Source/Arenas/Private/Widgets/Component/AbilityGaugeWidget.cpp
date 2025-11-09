// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "AbilityGaugeWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "GAS/ArenasHeroAttributeSet.h"

void UAbilityGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WholeNumberFormatOptions.MaximumFractionalDigits = 0;
	OneDigitNumberFormatOptions.MaximumFractionalDigits = 1;

	CooldownCountdownText->SetVisibility(ESlateVisibility::Hidden);

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownMaterialParamName, 1.f);

	UArenasAbilitySystemComponent* OwnerASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		// 订阅技能冷却时间变化的委托
		
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &UAbilityGaugeWidget::OnAbilityCommited);
		OwnerASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &UAbilityGaugeWidget::OnAbilitySpecDirtied);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UArenasHeroAttributeSet::GetUpgradePointsAttribute()).AddUObject(this, &UAbilityGaugeWidget::UpgradePointUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).AddUObject(this, &UAbilityGaugeWidget::ManaUpdated);
		
		bool bFound = false;
		float UpgradePoints = OwnerASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetUpgradePointsAttribute(), bFound);
		if (bFound)
		{
			// 初始更新技能升级点状态
			FOnAttributeChangeData DummyData;
			DummyData.NewValue = UpgradePoints;
			UpgradePointUpdated(DummyData);
		}

	}

	OwnerAbilitySystemComponent = OwnerASC;
}

void UAbilityGaugeWidget::ConfigureWidgetData(const FAbilityWidgetData* AbilityWidgetData)
{
	if (Icon && InputKeyText && AbilityWidgetData)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, AbilityWidgetData->AbilityIcon.LoadSynchronous());

		TArray<FString> InputKeys = UArenasBlueprintFunctionLibrary::GetKeyNamesForInputAction(GetOwningPlayer(), AbilityWidgetData->InputAction);
		FString InputKey = "";
		for (const FString& Key : InputKeys)
		{
			InputKey += Key + " / ";
		}
		if (InputKey.Len() > 3)
		{
			InputKey = InputKey.LeftChop(3); // 去掉最后的 " / "
		}
		InputKeyText->SetText(FText::FromString(InputKey));
	}
}

void UAbilityGaugeWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);
	
	if (AbilityCDO && CostText && CooldownDurationText && LevelImage)
	{

		const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
		float Cooldown = UArenasBlueprintFunctionLibrary::GetAbilityCooldownDuration(AbilityCDO, OwnerAbilitySystemComponent, AbilitySpec->Level);
		float ManaCost = UArenasBlueprintFunctionLibrary::GetAbilityManaCost(AbilityCDO, OwnerAbilitySystemComponent, AbilitySpec->Level);

		CooldownDurationText->SetText(FText::AsNumber(Cooldown));
		CostText->SetText(FText::AsNumber(ManaCost, &WholeNumberFormatOptions));
		
		LevelImage->GetDynamicMaterial()->SetScalarParameterValue(LevelMaterialParamName, AbilitySpec->Level);
		LevelImage->GetDynamicMaterial()->SetScalarParameterValue(MaxLevelMaterialParamName, 4); // 假设最大等级为4
		
	}
	
}

void UAbilityGaugeWidget::OnAbilityCommited(UGameplayAbility* InAbility)
{
	if (InAbility->GetClass()->GetDefaultObject() == AbilityCDO)
	{
		float CooldownTimeRemaining = 0.f;
		float CooldownDuration = 0.f;
		InAbility->GetCooldownTimeRemainingAndDuration(InAbility->GetCurrentAbilitySpecHandle(), InAbility->GetCurrentActorInfo(),CooldownTimeRemaining, CooldownDuration);

		StartCooldown(CooldownTimeRemaining, CooldownDuration);
	}
}

void UAbilityGaugeWidget::StartCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	CachedCooldownDuration = CooldownDuration;
	CachedCooldownTimeRemaining = CooldownTimeRemaining;

	CooldownCountdownText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), CooldownTimeRemaining)));
	CooldownCountdownText->SetVisibility(ESlateVisibility::Visible);
	
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UAbilityGaugeWidget::CooldownFinished, CooldownTimeRemaining, false);
	GetWorld()->GetTimerManager().SetTimer(CooldownUpdateTimerHandle, this, &UAbilityGaugeWidget::UpdateCooldown, CooldownUpdateInterval, true);
}

void UAbilityGaugeWidget::CooldownFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);
	CooldownCountdownText->SetVisibility(ESlateVisibility::Hidden);
	CachedCooldownDuration = 0.f;
	CachedCooldownTimeRemaining = 0.f;
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownMaterialParamName, 1.f);
}

void UAbilityGaugeWidget::UpdateCooldown()
{
	CachedCooldownTimeRemaining -= CooldownUpdateInterval;
	FNumberFormattingOptions FormattingOptions = CachedCooldownTimeRemaining > 1.f ? WholeNumberFormatOptions : OneDigitNumberFormatOptions;
	CooldownCountdownText->SetText(FText::AsNumber(CachedCooldownTimeRemaining, &FormattingOptions));

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownMaterialParamName, FMath::Clamp(1 - (CachedCooldownTimeRemaining / CachedCooldownDuration), 0.f, 1.f));
	
}

const FGameplayAbilitySpec* UAbilityGaugeWidget::GetAbilitySpec()
{
	if (!OwnerAbilitySystemComponent) return nullptr;
	if (!AbilityCDO) return nullptr;
	if (!CachedAbilitySpecHandle.IsValid())
	{
		if (FGameplayAbilitySpec* FoundAbilitySpec = OwnerAbilitySystemComponent->FindAbilitySpecFromClass(AbilityCDO->GetClass()))
		{
			CachedAbilitySpecHandle = FoundAbilitySpec->Handle;
			return FoundAbilitySpec;
		}
	}

	return OwnerAbilitySystemComponent->FindAbilitySpecFromHandle(CachedAbilitySpecHandle);
}

void UAbilityGaugeWidget::OnAbilitySpecDirtied(const FGameplayAbilitySpec& GameplayAbilitySpec)
{
	// 如果是当前技能
	if (GameplayAbilitySpec.Ability == AbilityCDO)
	{
		bIsAbilityLearned = GameplayAbilitySpec.Level > 0;
		LevelImage->GetDynamicMaterial()->SetScalarParameterValue(LevelMaterialParamName, GameplayAbilitySpec.Level);
		LevelImage->GetDynamicMaterial()->SetScalarParameterValue(MaxLevelMaterialParamName, 4); // 假设最大等级为4

		float NewCooldown = UArenasBlueprintFunctionLibrary::GetAbilityCooldownDuration(AbilityCDO, OwnerAbilitySystemComponent, GameplayAbilitySpec.Level);
		float NewManaCost = UArenasBlueprintFunctionLibrary::GetAbilityManaCost(AbilityCDO, OwnerAbilitySystemComponent, GameplayAbilitySpec.Level);
		CooldownDurationText->SetText(FText::AsNumber(NewCooldown));
		CostText->SetText(FText::AsNumber(NewManaCost, &WholeNumberFormatOptions));

		UpdateCanCastAbilityVisual();
	}
}

void UAbilityGaugeWidget::UpgradePointUpdated(const FOnAttributeChangeData& OnAttributeChangeData)
{
	bIsCanUpgrade = OnAttributeChangeData.NewValue > 0;
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec())
	{
		if (UArenasBlueprintFunctionLibrary::IsAbilityAtMaxLevel(*AbilitySpec))
		{
			bIsCanUpgrade = false;
		}
	}

	UpdateCanCastAbilityVisual();
	
}

void UAbilityGaugeWidget::ManaUpdated(const FOnAttributeChangeData& OnAttributeChangeData)
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	if (!AbilitySpec || !OwnerAbilitySystemComponent) return;
	bCanAffordAbilityCost = UArenasBlueprintFunctionLibrary::CheckAbilityCanCost(*AbilitySpec, OwnerAbilitySystemComponent);
	
	Icon->GetDynamicMaterial()->SetScalarParameterValue(HasEnoughManaMaterialParamName, bCanAffordAbilityCost ? 1.f : 0.f);

}

void UAbilityGaugeWidget::UpdateCanCastAbilityVisual()
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	if (!AbilitySpec || !OwnerAbilitySystemComponent) return;
	
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CanCastAbilityMaterialParamName, bIsAbilityLearned ? 1.f : 0.f);
	Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableMaterialParamName, bIsCanUpgrade ? 1.f : 0.f);
	
}
