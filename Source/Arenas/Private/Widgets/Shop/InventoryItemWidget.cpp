// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryItemWidget.h"

#include "InventoryItemDrapDropOP.h"
#include "ItemToolTip.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/PA_ShopItem.h"

void UInventoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	EmptySlot();
}

bool UInventoryItemWidget::IsEmpty() const
{
	return !InventoryItem || !(InventoryItem->IsValid());
}

void UInventoryItemWidget::EmptySlot()
{
	ClearCooldown();
	UnbindCanCastUpdateDelegate();
	InventoryItem = nullptr;
	SetIcon(EmptyIcon);
	SetToolTip(nullptr);

	StackCountText->SetVisibility(ESlateVisibility::Hidden);
	CooldownText->SetVisibility(ESlateVisibility::Hidden);
	CooldownDurabilityText->SetVisibility(ESlateVisibility::Hidden);
	ManaCostText->SetVisibility(ESlateVisibility::Hidden);
	InventoryItemUseInputKey->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryItemWidget::SetInventoryItem(const UInventoryItem* NewInventoryItem)
{
	UnbindCanCastUpdateDelegate();
	
	InventoryItem = NewInventoryItem;
	if (!InventoryItem || !(InventoryItem->IsValid()) || InventoryItem->GetStackCount() <= 0)
	{
		EmptySlot();
		return;
	}

	SetIcon(InventoryItem->GetShopItem()->GetIcon());
	if (UItemToolTip* ItemToolTipWidget = SetTooltipWidget(InventoryItem->GetShopItem()))
	{
		ItemToolTipWidget->SetPrice(InventoryItem->GetShopItem()->GetSellPrice());
	}

	if (InventoryItem->GetShopItem()->IsStackable())
	{
		// 如果物品是可堆叠的，则显示堆叠数量
		StackCountText->SetVisibility(ESlateVisibility::Visible);
		UpdateStackCountText();
	}
	else
	{
		StackCountText->SetVisibility(ESlateVisibility::Hidden);
	}

	ClearCooldown();
	if (InventoryItem->IsGrantedAnyActiveAbility())
	{
		// 如果有主动技能，则显示使用按键
		InventoryItemUseInputKey->SetVisibility(ESlateVisibility::Visible);
		// 物品栏使用的按键组合按键为 Alt+(SlotNumber+1)
		FString UseInputKeyString = FString::Printf(TEXT("Alt+%d"), SlotNumber + 1);
		InventoryItemUseInputKey->SetText(FText::FromString(UseInputKeyString));
		
		UpdateCanCastVisual(InventoryItem->CanCastItemAbility());
		float AbilityCooldownTimeRemaining = InventoryItem->GetAbilityCooldownTimeRemaining();
		float AbilityCooldownDuration = InventoryItem->GetAbilityCooldownDuration();
		if (AbilityCooldownTimeRemaining > 0.f && AbilityCooldownDuration > 0.f)
		{
			StartCooldown(AbilityCooldownDuration, AbilityCooldownTimeRemaining);
		}

		// 显示冷却时间和法力消耗
		float ManaCost = InventoryItem->GetAbilityManaCost();
		ManaCostText->SetVisibility(ManaCost > 0.f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		ManaCostText->SetText(FText::AsNumber(ManaCost));

		CooldownDurabilityText->SetVisibility(AbilityCooldownDuration > 0.f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		CooldownDurabilityText->SetText(FText::AsNumber(AbilityCooldownDuration));
		BindCanCastUpdateDelegate();
	}
	else
	{
		UpdateCanCastVisual(true);
		ManaCostText->SetVisibility(ESlateVisibility::Hidden);
		CooldownDurabilityText->SetVisibility(ESlateVisibility::Hidden);
		CooldownText->SetVisibility(ESlateVisibility::Hidden);
		InventoryItemUseInputKey->SetVisibility(ESlateVisibility::Hidden);
	}
	
}

void UInventoryItemWidget::UpdateStackCountText()
{
	if (!InventoryItem || !(InventoryItem->IsValid())) return;

	if (InventoryItem->GetStackCount() <= 1)
	{
		StackCountText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	StackCountText->SetVisibility(ESlateVisibility::Visible);
	StackCountText->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
}

UTexture2D* UInventoryItemWidget::GetIconTexture() const
{
	if (InventoryItem && InventoryItem->IsValid() && InventoryItem->GetShopItem())
	{
		return InventoryItem->GetShopItem()->GetIcon();
	}
	return nullptr;
}

FInventoryItemHandle UInventoryItemWidget::GetInventoryItemHandle() const
{
	return !IsEmpty() ? InventoryItem->GetHandle() : FInventoryItemHandle::InvalidHandle();
}

void UInventoryItemWidget::StartCooldown(float CooldownDuration, float CooldownTimeRemaining)
{
	CurrentCooldownDuration = CooldownDuration;
	CurrentCooldownTimeRemaining = CooldownTimeRemaining;
	GetWorld()->GetTimerManager().SetTimer(CooldownDurationTimerHandle, this, &UInventoryItemWidget::CooldownFinished, CooldownTimeRemaining, false);
	GetWorld()->GetTimerManager().SetTimer(CooldownUpdateTimerHandle, this, &UInventoryItemWidget::UpdateCooldown, CooldownUpdateInterval, true);

	CooldownText->SetVisibility(ESlateVisibility::Visible);

}

void UInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!IsEmpty() && DragDropOperationClass)
	{
		if (UInventoryItemDrapDropOP* DragDropOp = NewObject<UInventoryItemDrapDropOP>(this, DragDropOperationClass))
		{
			DragDropOp->SetDraggedItem(this);
			OutOperation = DragDropOp;
		}
	}
}

bool UInventoryItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

	if (UInventoryItemWidget* OtherItemWidget = InOperation ? Cast<UInventoryItemWidget>(InOperation->Payload) : nullptr)
	{
		// 确保拖放的物品不是自己，并且拖放的物品不是空的
		if (OtherItemWidget && OtherItemWidget != this && !OtherItemWidget->IsEmpty())
		{
			// 广播物品被放下的事件, 由监听该事件的对象（如物品栏组件）来处理物品交换或移动的逻辑
			OnInventoryItemDropped.Broadcast(this, OtherItemWidget);
			return true;
		}
	}
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryItemWidget::RightButtonClicked()
{
	if (!IsEmpty())
	{
		OnRightButtonClicked.Broadcast(GetInventoryItemHandle());
	}
}

void UInventoryItemWidget::LeftButtonClicked()
{
	if (!IsEmpty())
	{
		OnLeftButtonClicked.Broadcast(GetInventoryItemHandle());
	}
}

void UInventoryItemWidget::CooldownFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);
	CooldownText->SetVisibility(ESlateVisibility::Hidden);
	if (GetImageIcon() && GetImageIcon()->GetDynamicMaterial())
	{
		GetImageIcon()->GetDynamicMaterial()->SetScalarParameterValue(CooldownAmtDynamicMaterialParamName, 1.f);
	}
}

void UInventoryItemWidget::UpdateCooldown()
{
	CurrentCooldownTimeRemaining -= CooldownUpdateInterval;
	float CooldownAmt = 1.f - (CurrentCooldownTimeRemaining / CurrentCooldownDuration);
	CooldownFormattingOptions.MaximumFractionalDigits = CurrentCooldownTimeRemaining > 1.f ? 0 : 1;
	CooldownText->SetText(FText::AsNumber(CurrentCooldownTimeRemaining, &CooldownFormattingOptions));
	if (GetImageIcon() && GetImageIcon()->GetDynamicMaterial())
	{
		GetImageIcon()->GetDynamicMaterial()->SetScalarParameterValue(CooldownAmtDynamicMaterialParamName, CooldownAmt);
	}
}

void UInventoryItemWidget::ClearCooldown()
{
	CooldownFinished();
}

void UInventoryItemWidget::SetIcon(UTexture2D* IconTexture)
{
	if (GetImageIcon() && GetImageIcon()->GetDynamicMaterial())
	{
		GetImageIcon()->GetDynamicMaterial()->SetTextureParameterValue(IconTextureDynamicMaterialParamName, IconTexture);
		return;
	}

	Super::SetIcon(IconTexture);
}

void UInventoryItemWidget::UpdateCanCastVisual(bool bCanCast)
{
	if (GetImageIcon() && GetImageIcon()->GetDynamicMaterial())
	{
		GetImageIcon()->GetDynamicMaterial()->SetScalarParameterValue(CanCastDynamicMaterialParamName, bCanCast ? 1.f : 0.f);
	}
}

void UInventoryItemWidget::BindCanCastUpdateDelegate()
{
	if (InventoryItem)
	{
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdated.AddUObject(this, &UInventoryItemWidget::UpdateCanCastVisual);
	}
}

void UInventoryItemWidget::UnbindCanCastUpdateDelegate()
{
	if (InventoryItem)
	{
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdated.RemoveAll(this);
	}
}
