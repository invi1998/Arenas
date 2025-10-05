// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

class UPA_ShopItem;
class UItemToolTip;
class UImage;
/**
 * 
 */
UCLASS()
class ARENAS_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void SetIcon(UTexture2D* IconTexture);

protected:
	UItemToolTip* SetTooltipWidget(const UPA_ShopItem* InShopItem);

	UImage* GetImageIcon() const { return ImageIcon; }

private:
	UPROPERTY(meta = (BindWidget))
	UImage* ImageIcon;

	virtual void RightButtonClicked();
	virtual void LeftButtonClicked();

	UPROPERTY(EditDefaultsOnly, Category = "Tooltip")
	TSubclassOf<UItemToolTip> ItemTooltipWidgetClass;
	
};
