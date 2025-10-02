// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItem;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ARENAS_API UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	bool IsEmpty() const;
	void EmptySlot();
	void SetInventoryItem(const UInventoryItem* NewInventoryItem);
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
	void SetSlotNumber(int InSlotNumber) { SlotNumber = InSlotNumber; }
	void UpdateStackCountText();
	UTexture2D* GetIconTexture() const;

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


	
};
