// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ItemWidget.h"

#include "ItemToolTip.h"
#include "Components/Image.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
}

FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		// 如果是右键按下，此时要确保我们获取焦点，传入TakeWidget()，表示将焦点设置到当前Widget上
		// 这样做的目的是为了让右键点击时，当前Widget能够接收键盘输入事件，同时这样做也能让其他被聚焦的Widget失去焦点
		// 同时，还需要注意：这样做之后，会触发其他被聚焦的Widget的 ButtonUp 事件
		return FReply::Handled().SetUserFocus(TakeWidget());
	}

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// 如果是左键按下，此处我们希望控件能支持拖拽功能，所以在设置控件聚焦后，调用 DetectDrag 方法
		// 该方法会检测用户是否在按下鼠标左键后移动了鼠标，如果移动了鼠标，则会触发拖拽操作
		// 这里同样传入 TakeWidget()，表示将焦点设置到当前Widget上
		return FReply::Handled().SetUserFocus(TakeWidget()).DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return SuperReply;
	
}

FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	if (HasAnyUserFocus())
	{
		// 如果当前Widget拥有焦点，且鼠标左键抬起，则表示用户完成了点击操作
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			RightButtonClicked();
			return FReply::Handled();
		}
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			LeftButtonClicked();
			return FReply::Handled();
		}
	}

	return SuperReply;
}

void UItemWidget::SetIcon(UTexture2D* IconTexture)
{
	ImageIcon->SetBrushFromTexture(IconTexture);
}

UItemToolTip* UItemWidget::SetTooltipWidget(const UPA_ShopItem* InShopItem)
{
	if (!InShopItem)
	{
		return nullptr;
	}

	if (GetOwningPlayer() && ItemTooltipWidgetClass)
	{
		UItemToolTip* ToolTip = CreateWidget<UItemToolTip>(GetOwningPlayer(), ItemTooltipWidgetClass);
		if (ToolTip)
		{
			ToolTip->SetItem(InShopItem);
			SetToolTip(ToolTip);
		}
		return ToolTip;
	}

	return nullptr;
}

void UItemWidget::RightButtonClicked()
{
	// 在屏幕上打印日志
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Right Button Clicked"));
}

void UItemWidget::LeftButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Left Button Clicked"));
}
