// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CharacterEntryWidget.generated.h"

class UPA_CharacterDefinition;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class ARENAS_API UCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void SetSelectedState(bool bIsSelected);
	
	FORCEINLINE const UPA_CharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }

private:
	UPROPERTY(meta=(BindWidget))
	UImage* CharacterIcon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CharacterNameText;

	UPROPERTY(EditDefaultsOnly, Category="CharacterEntry")
	FName IconTextureMatParameterName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category="CharacterEntry")
	FName SaturationMatParameterName = "Saturation";		// 饱和度参数名称

	UPROPERTY()
	UPA_CharacterDefinition* CharacterDefinition;

	
};
