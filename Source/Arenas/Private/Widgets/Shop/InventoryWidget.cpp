// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryWidget.h"

#include "InventoryContextMenuWidget.h"
#include "InventoryItemWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/WrapBox.h"
#include "Inventory/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
		if (OwnerInventoryComponent)
		{
			// 订阅库存组件的物品添加事件
			OwnerInventoryComponent->OnItemAdded.AddUObject(this, &UInventoryWidget::ItemAdded);
			// 订阅库存组件的物品移除事件
			OwnerInventoryComponent->OnItemRemoved.AddUObject(this, &UInventoryWidget::ItemRemoved);
			// 订阅库存物品堆叠数量变更事件
			OwnerInventoryComponent->OnItemStackCountChanged.AddUObject(this, &UInventoryWidget::ItemStackChanged);
			// 订阅物品主动技能释放事件
			OwnerInventoryComponent->OnItemActiveAbilityCommitted.AddUObject(this, &UInventoryWidget::ItemActiveAbilityCommitted);
			
			int Capacity = OwnerInventoryComponent->GetCapacity();
			ItemListWrapBox->ClearChildren();
			InventoryItemsWidgets.Empty();
			for (int i = 0; i < Capacity; ++i)
			{
				if (UInventoryItemWidget* NewItemWidget = CreateWidget<UInventoryItemWidget>(GetOwningPlayer(), InventoryItemWidgetClass))
				{
					NewItemWidget->SetSlotNumber(i);
					UWrapBoxSlot* NewSlot = ItemListWrapBox->AddChildToWrapBox(NewItemWidget);
					InventoryItemsWidgets.Add(NewItemWidget);

					// 订阅物品拖放事件
					NewItemWidget->OnInventoryItemDropped.AddUObject(this, &UInventoryWidget::HandleItemDragDrop);
					// 订阅物品点击事件
					NewItemWidget->OnRightButtonClicked.AddUObject(this, &UInventoryWidget::OnItemRightButtonClicked);
					NewItemWidget->OnLeftButtonClicked.AddUObject(this, &UInventoryWidget::OnItemLeftButtonClicked);
				}
			}
			SpawnInventoryContextMenuWidget();
		}
	}
	
}

void UInventoryWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
	const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	if (!NewWidgetPath.ContainsWidget(InventoryContextMenuWidget->GetCachedWidget().Get()))
	{
		// 如果新的焦点路径不包含右键菜单，则隐藏右键菜单
		ClearContextMenu();
	}
}

void UInventoryWidget::SellButtonClicked()
{
	OwnerInventoryComponent->SellItem(CurrentFocusedItemHandle);
	SetInventoryContextMenuVisible(false);
}

void UInventoryWidget::UseButtonClicked()
{
	OwnerInventoryComponent->TryActivateItemAbility(CurrentFocusedItemHandle);
	SetInventoryContextMenuVisible(false);
}

void UInventoryWidget::GetShowInShopButtonClickedEvent()
{
	// 显示物品在商店中的位置，以及物品的合成配方
	if (OwnerInventoryComponent)
	{
		// 广播物品需要在商店中显示的事件，然后商店界面会订阅该事件并处理
		OwnerInventoryComponent->OnItemNeedShowInShop.Broadcast(CurrentFocusedItemHandle);
	}
	
	SetInventoryContextMenuVisible(false);
}

void UInventoryWidget::SetInventoryContextMenuVisible(bool bVisible)
{
	if (InventoryContextMenuWidget)
	{
		InventoryContextMenuWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UInventoryWidget::SpawnInventoryContextMenuWidget()
{
	if (!InventoryContextMenuWidgetClass) return;

	InventoryContextMenuWidget = CreateWidget<UInventoryContextMenuWidget>(this, InventoryContextMenuWidgetClass);
	if (InventoryContextMenuWidget)
	{
		// 绑定右键菜单的事件
		InventoryContextMenuWidget->GetSellButtonClickedEvent().AddDynamic(this, &UInventoryWidget::SellButtonClicked);
		InventoryContextMenuWidget->GetUseButtonClickedEvent().AddDynamic(this, &UInventoryWidget::UseButtonClicked);
		InventoryContextMenuWidget->GetShowInShopButtonClickedEvent().AddDynamic(this, &UInventoryWidget::GetShowInShopButtonClickedEvent);
		
		InventoryContextMenuWidget->AddToViewport(1);	// 添加到视口，层级为1
		SetInventoryContextMenuVisible(false);	// 初始时隐藏
	}
}

void UInventoryWidget::ItemAdded(const UInventoryItem* NewInventoryItem)
{
	if (!NewInventoryItem) return;

	if (UInventoryItemWidget* NextAvailableSlot = GetNextAvailableSlot())
	{
		NextAvailableSlot->SetInventoryItem(NewInventoryItem);
		PopulatedItemEntryWidgetsMap.Add(NewInventoryItem->GetHandle(), NextAvailableSlot);
		if (OwnerInventoryComponent)
		{
			OwnerInventoryComponent->ItemSlotChanged(NewInventoryItem->GetHandle(), NextAvailableSlot->GetSlotNumber());
		}
	}
}

void UInventoryWidget::ItemRemoved(const FInventoryItemHandle& InventoryItemHandle)
{
	if (UInventoryItemWidget* InInventoryItemWidget = PopulatedItemEntryWidgetsMap.FindRef(InventoryItemHandle))
	{
		InInventoryItemWidget->EmptySlot();
		PopulatedItemEntryWidgetsMap.Remove(InventoryItemHandle);
	}
}

void UInventoryWidget::ItemStackChanged(const FInventoryItemHandle& InInventoryItemHandle, int NewStackCount)
{
	if (UInventoryItemWidget* InInventoryItemWidget = PopulatedItemEntryWidgetsMap.FindRef(InInventoryItemHandle))
	{
		InInventoryItemWidget->UpdateStackCountText();
	}
}

UInventoryItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
	for (UInventoryItemWidget* Widget : InventoryItemsWidgets)
	{
		if (Widget && Widget->IsEmpty())
		{
			return Widget;
		}
	}

	return nullptr;
}

void UInventoryWidget::HandleItemDragDrop(UInventoryItemWidget* TargetItemWidget, UInventoryItemWidget* SourceItemWidget)
{
	const UInventoryItem* SrcInventoryItem = SourceItemWidget->GetInventoryItem();
	const UInventoryItem* DstInventoryItem = TargetItemWidget->GetInventoryItem();

	TargetItemWidget->SetInventoryItem(SrcInventoryItem);
	SourceItemWidget->SetInventoryItem(DstInventoryItem);

	PopulatedItemEntryWidgetsMap[TargetItemWidget->GetInventoryItem()->GetHandle()] = TargetItemWidget;
	if (OwnerInventoryComponent)
	{
		OwnerInventoryComponent->ItemSlotChanged(TargetItemWidget->GetInventoryItem()->GetHandle(), TargetItemWidget->GetSlotNumber());
	}

	if (!SourceItemWidget->IsEmpty())
	{
		PopulatedItemEntryWidgetsMap[SourceItemWidget->GetInventoryItem()->GetHandle()] = SourceItemWidget;
		if (OwnerInventoryComponent)
		{
			OwnerInventoryComponent->ItemSlotChanged(SourceItemWidget->GetInventoryItem()->GetHandle(), SourceItemWidget->GetSlotNumber());
		}
	}
	
}

void UInventoryWidget::OnItemRightButtonClicked(const FInventoryItemHandle& InventoryItemHandle)
{
	// 右键点击物品时，显示物品右键菜单
	if (CurrentFocusedItemHandle == InventoryItemHandle && InventoryContextMenuWidget && InventoryContextMenuWidget->IsVisible())
	{
		// 如果当前已经是聚焦该物品，并且右键菜单已经可见，则隐藏右键菜单
		ClearContextMenu();
		return;
	}
	CurrentFocusedItemHandle = InventoryItemHandle;
	UInventoryItemWidget* FocusedItemWidget = PopulatedItemEntryWidgetsMap.FindRef(InventoryItemHandle);
	if (!FocusedItemWidget || !InventoryContextMenuWidget) return;
	
	// 显示右键菜单
	SetInventoryContextMenuVisible(true);
	// 将右键菜单位置设置为物品图标位置的右下角
	// 注意这里使用 GetAbsolutePositionAtCoordinates 来获取物品图标的绝对位置
	// 传入 FVector2D{1.f, 0.5f} 表示获取图标右侧中点的位置
	// 这样可以确保右键菜单不会遮挡物品图标
	// 然后再加上图标的宽度和高度，来将菜单位置设置在图标的右下角
	// 这里假设菜单不会超出屏幕边界，如果有需要可以添加边界检查逻辑
	FVector2D WidgetPosition = FocusedItemWidget->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D{1.f, 0.5f});
	FVector2D ItemWidgetPixelPos, ItemWidgetViewportPos;
	USlateBlueprintLibrary::AbsoluteToViewport(this, WidgetPosition, ItemWidgetPixelPos, ItemWidgetViewportPos);
	
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		int ViewportX, ViewportY;
		PlayerController->GetViewportSize(ViewportX, ViewportY);
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		int Overshoot = ItemWidgetPixelPos.Y + InventoryContextMenuWidget->GetDesiredSize().Y * Scale - ViewportY;
		if (Overshoot > 0)
		{
			ItemWidgetPixelPos.Y -= Overshoot;
		}
		int OvershootX = ItemWidgetPixelPos.X + InventoryContextMenuWidget->GetDesiredSize().X * Scale - ViewportX;
		if (OvershootX > 0)
		{
			ItemWidgetPixelPos.X -= OvershootX;
		}
	}
	InventoryContextMenuWidget->SetPositionInViewport(ItemWidgetPixelPos);
}

void UInventoryWidget::OnItemLeftButtonClicked(const FInventoryItemHandle& InventoryItemHandle)
{
	// 左键点击物品时，尝试使用该物品
	if (OwnerInventoryComponent)
	{
		OwnerInventoryComponent->TryActivateItemAbility(InventoryItemHandle);
	}
}

void UInventoryWidget::ItemActiveAbilityCommitted(const FInventoryItemHandle& InventoryItemHandle, float CooldownDuration, float CooldownTimeRemaining)
{
	if (UInventoryItemWidget* InInventoryItemWidget = PopulatedItemEntryWidgetsMap.FindRef(InventoryItemHandle))
	{
		InInventoryItemWidget->StartCooldown(CooldownDuration, CooldownTimeRemaining);
	}
}

void UInventoryWidget::ClearContextMenu()
{
	SetInventoryContextMenuVisible(false);
	CurrentFocusedItemHandle = FInventoryItemHandle::InvalidHandle();
}
