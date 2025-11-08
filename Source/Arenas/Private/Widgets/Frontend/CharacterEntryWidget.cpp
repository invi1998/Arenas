// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "CharacterEntryWidget.h"

#include "Character/PA_CharacterDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CharacterDefinition = Cast<UPA_CharacterDefinition>(ListItemObject);
	if (CharacterDefinition)
	{
		CharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(IconTextureMatParameterName, CharacterDefinition->LoadCharacterIcon());
		CharacterNameText->SetText(FText::FromString(CharacterDefinition->GetCharacterName()));
	}
	
}

void UCharacterEntryWidget::SetSelectedState(bool bIsSelected)
{
	CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParameterName, bIsSelected ? 0.0f : 1.0f);
}
