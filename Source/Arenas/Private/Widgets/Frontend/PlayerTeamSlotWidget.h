// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTeamSlotWidget.generated.h"

class UBorder;
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

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateTeamSide(bool bIsRightSide);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* NameTextBlock;

private:
	UPROPERTY(meta=(BindWidget))
	UImage* PlayerCharacterImage;		// 玩家角色图像
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* HoveredAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterIconMaterialParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterEmptyIconMaterialParamName = "Empty";

	FString CachedPlayerNameString;
	FString CachedCharacterNameString;

	void UpdateNameText();
	
};
