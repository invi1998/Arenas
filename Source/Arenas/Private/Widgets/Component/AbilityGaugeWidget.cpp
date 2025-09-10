// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "AbilityGaugeWidget.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/ArenasAbilitySystemComponent.h"

void UAbilityGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WholeNumberFormatOptions.MaximumFractionalDigits = 0;
	OneDigitNumberFormatOptions.MaximumFractionalDigits = 1;

	CooldownCountdownText->SetVisibility(ESlateVisibility::Hidden);

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownMaterialParamName, 1.f);

	if (UArenasAbilitySystemComponent* OwnerASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(GetOwningPlayerPawn()))
	{
		// 订阅技能冷却时间变化的委托
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &UAbilityGaugeWidget::OnAbilityCommited);
	}
	
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
	
	if (AbilityCDO && CostText && CooldownDurationText)
	{
		float StaticCooldown = UArenasBlueprintFunctionLibrary::GetStaticCooldownDurationFromAbility(AbilityCDO);
		float StaticCost = UArenasBlueprintFunctionLibrary::GetStaticCostFromAbility(AbilityCDO);

		CooldownDurationText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), StaticCooldown)));
		CostText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), StaticCost)));
		
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
