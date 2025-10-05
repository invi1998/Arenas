// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItemWidget;
class UInventoryItemDrapDropOP;
class UInventoryItem;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemDropped, UInventoryItemWidget* /* TargetWidget */, UInventoryItemWidget* /* DraggedWidget */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnButtonClicked, const FInventoryItemHandle& /* ItemInventoryHandle */);

/**
 * 
 */
UCLASS()
class ARENAS_API UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FOnInventoryItemDropped OnInventoryItemDropped;	// 当有物品被拖放到该物品上时触发的委托
	FOnButtonClicked OnRightButtonClicked;			// 当右键点击该物品时触发的委托（通常用于出售物品）
	FOnButtonClicked OnLeftButtonClicked;			// 当左键点击该物品时触发的委托（通常用于使用物品）
	
	bool IsEmpty() const;
	void EmptySlot();
	void SetInventoryItem(const UInventoryItem* NewInventoryItem);
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
	void SetSlotNumber(int InSlotNumber) { SlotNumber = InSlotNumber; }
	void UpdateStackCountText();
	UTexture2D* GetIconTexture() const;

	FORCEINLINE const UInventoryItem* GetInventoryItem() const { return InventoryItem; }
	FInventoryItemHandle GetInventoryItemHandle() const;

	void StartCooldown(float CooldownDuration, float CooldownTimeRemaining);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UTexture2D* EmptyIcon;			// 空图标
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* StackCountText;		// 堆叠数量文本

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownText;		// 冷却时间文本

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownDurabilityText;		// 冷却持续时间文本

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ManaCostText;		// 法力消耗文本

	UPROPERTY()
	const UInventoryItem* InventoryItem;	// 库存物品

	// 该物品在库存中的槽位编号
	// 该编号从0开始，到Capacity-1结束
	int SlotNumber;

	/* 物品拖拽相关  */
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
	TSubclassOf<UInventoryItemDrapDropOP> DragDropOperationClass;	// 拖拽操作类

	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;	// 冷却时间更新间隔

	void CooldownFinished();
	void UpdateCooldown();
	void ClearCooldown();

	FTimerHandle CooldownDurationTimerHandle;
	FTimerHandle CooldownUpdateTimerHandle;
	float CurrentCooldownTimeRemaining = 0.f;	// 当前冷却剩余时间
	float CurrentCooldownDuration = 0.f;		// 当前冷却持续时间

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CooldownAmtDynamicMaterialParamName = "Percent";	// 冷却进度动态材质参数名称

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName IconTextureDynamicMaterialParamName = "AbilityIcon";		// 图标纹理动态材质参数名称

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CanCastDynamicMaterialParamName = "HasEnoughMana";		// 能否施放动态材质参数名称

	virtual void SetIcon(UTexture2D* IconTexture) override;

	FNumberFormattingOptions CooldownFormattingOptions;

	void UpdateCanCastVisual(bool bCanCast);

	void BindCanCastUpdateDelegate();
	void UnbindCanCastUpdateDelegate();
	
};
