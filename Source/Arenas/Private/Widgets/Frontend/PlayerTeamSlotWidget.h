// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTeamSlotWidget.generated.h"

class UPA_CharacterDefinition;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class ARENAS_API UPlayerTeamSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void UpdateSlot(const FString& InPlayerName, const UPA_CharacterDefinition* InCharacterDef);
	void UpdateTeamSide(bool bIsRightSide);

private:
	UPROPERTY(meta=(BindWidget))
	UImage* PlayerCharacterImage;		// 玩家角色图像

	UPROPERTY(meta=(BindWidget))
	UTextBlock* NameTextBlock;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* HoveredAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterIconMaterialParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterEmptyIconMaterialParamName = "Empty";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName TeamLeftOrRightMaterialParamName = "Right";

	FString CachedPlayerNameString;
	FString CachedCharacterNameString;

	void UpdateNameText();
	
};
