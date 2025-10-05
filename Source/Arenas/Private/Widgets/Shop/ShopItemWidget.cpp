// Ace of Arenas. (invi_1998 All Rights Reserved)

#include "ShopItemWidget.h"

#include "Components/ListView.h"
#include "Framework/ArenasAssetManager.h"
#include "Inventory/PA_ShopItem.h"

UUserWidget* UShopItemWidget::GetWidget() const
{
	UShopItemWidget* CopyWidget = CreateWidget<UShopItemWidget>(GetOwningPlayer(), GetClass());
	if (!CopyWidget) return nullptr;
	CopyWidget->CopyFromOther(this);		// 复制当前对象的属性到新创建的对象中
	
	return CopyWidget;
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetInputs() const
{
	TArray<const ITreeNodeInterface*> Result;
	// 通过物品管理器获取该物品的合成材料条目
	// 如果存在，则将材料物品转换为ITreeNodeInterface接口并返回
	if (const FItemCollectionEntry* Collection = UArenasAssetManager::Get().GetCombinationEntry(GetShopItem()))
	{
		return ItemToInterface(Collection->GetItems());
	}

	return TArray<const ITreeNodeInterface*>();
	
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetOutputs() const
{
	// 通过物品管理器获取该物品的合成材料条目
	// 如果存在，则将材料物品转换为ITreeNodeInterface接口并返回
	if (const FItemCollectionEntry* Collection = UArenasAssetManager::Get().GetIngredientEntry(GetShopItem()))
	{
		return ItemToInterface(Collection->GetItems());
	}
	return TArray<const ITreeNodeInterface*>();
}

const UObject* UShopItemWidget::GetItemObject() const
{
	return ShopItem;
}

void UShopItemWidget::OnItemSelected_Implementation(bool bSelected)
{
	bIsSelected = bSelected;
}

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InitWithShopItem(Cast<UPA_ShopItem>(ListItemObject));
	// 缓存父ListView指针，方便后续操作
	ParentListView = Cast<UListView>(IUserListEntry::GetOwningListView());
}

void UShopItemWidget::CopyFromOther(const UShopItemWidget* Other)
{
	OnItemPurchaseIssued = Other->OnItemPurchaseIssued;
	OnShopItemSelected = Other->OnShopItemSelected;
	InitWithShopItem(Other->GetShopItem());
	ParentListView = Other->ParentListView;
	bIsSelected = Other->IsItemSelected();
}

void UShopItemWidget::InitWithShopItem(const UPA_ShopItem* NewShopItem)
{
	ShopItem = NewShopItem;
	if (!ShopItem) return;
	SetIcon(ShopItem->GetIcon());
	SetTooltipWidget(ShopItem);
}

TArray<const ITreeNodeInterface*> UShopItemWidget::ItemToInterface(const TArray<const UPA_ShopItem*>& Items) const
{
	TArray<const ITreeNodeInterface*> Result;
	for (const UPA_ShopItem* Item : Items)
	{
		// 通过父ListView获取对应的Item Widget
		if (const UShopItemWidget* ItemWidget = ParentListView->GetEntryWidgetFromItem<UShopItemWidget>(Item))
		{
			Result.Add(ItemWidget);
		}
	}
	return Result;
}

void UShopItemWidget::RightButtonClicked()
{
	OnItemPurchaseIssued.Broadcast(GetShopItem());
}

void UShopItemWidget::LeftButtonClicked()
{
	OnItemSelected(true);
	OnShopItemSelected.Broadcast(this);
}
