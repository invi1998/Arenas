// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "PlayerTeamSlotWidget.h"

#include "Character/PA_CharacterDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerTeamSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedCharacterNameString = TEXT("...");
	PlayerCharacterImage->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyIconMaterialParamName, 1.f);

	
}

void UPlayerTeamSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	NameTextBlock->SetText(FText::FromString(CachedCharacterNameString));
	PlayAnimationForward(HoveredAnimation);
}

void UPlayerTeamSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	NameTextBlock->SetText(FText::FromString(CachedPlayerNameString));
	PlayAnimationReverse(HoveredAnimation);
}

void UPlayerTeamSlotWidget::UpdateSlot(const FString& InPlayerName, const UPA_CharacterDefinition* InCharacterDef)
{
	CachedPlayerNameString = InPlayerName;
	if (InCharacterDef)
	{
		CachedCharacterNameString = InCharacterDef->GetCharacterName();
		PlayerCharacterImage->GetDynamicMaterial()->SetTextureParameterValue(CharacterIconMaterialParamName, InCharacterDef->LoadCharacterIcon());
		PlayerCharacterImage->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyIconMaterialParamName, 0.f);
	}
	else
	{
		CachedCharacterNameString = TEXT("...");
		PlayerCharacterImage->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyIconMaterialParamName, 1.f);
	}
}

void UPlayerTeamSlotWidget::UpdateTeamSide(bool bIsRightSide)
{
	PlayerCharacterImage->GetDynamicMaterial()->SetScalarParameterValue(TeamLeftOrRightMaterialParamName, bIsRightSide ? 1.f : 0.f);
}

void UPlayerTeamSlotWidget::UpdateNameText()
{
	if (IsHovered())
	{
		NameTextBlock->SetText(FText::FromString(CachedCharacterNameString));
	}
	else
	{
		NameTextBlock->SetText(FText::FromString(CachedPlayerNameString));
	}
}
