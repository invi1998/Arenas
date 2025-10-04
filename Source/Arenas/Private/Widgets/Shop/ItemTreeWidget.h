// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/TreeNodeInterface.h"
#include "ItemTreeWidget.generated.h"

class UCanvasPanelSlot;
class UCanvasPanel;
/**
 * 
 */
UCLASS()
class ARENAS_API UItemTreeWidget : public UUserWidget
{
	GENERATED_BODY()

public:


private:
	void DrawSteam(bool bUpperSteam, const ITreeNodeInterface* StartNode, UUserWidget* StartWidget, UCanvasPanelSlot* StartNodeSlot, int StartNodeDepth, float& NextLeafXPos, TArray<UCanvasPanelSlot*>& OutSteamSlots);
	void ClearTree();
	UUserWidget* CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot*& OutCanvasPanelSlot);
	void CreateConnection(const UUserWidget* From, const UUserWidget* To);
	
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* RootPanel;

	UPROPERTY(EditDefaultsOnly, Category = "ItemTree")
	FVector2D NodeSize = FVector2D(50.f, 50.f);

	UPROPERTY(EditDefaultsOnly, Category = "ItemTree")
	FVector2D NodeGap = FVector2D(16.f, 30.f);

	// 连接线颜色 (默认橘黄色）
	UPROPERTY(EditDefaultsOnly, Category = "ItemTree")
	FLinearColor ConnectionColor = FLinearColor{1.f, 0.647f, 0.f, 1.f};

	UPROPERTY(EditDefaultsOnly, Category = "ItemTree")
	float ConnectionThickness = 4.f;

	UPROPERTY(EditDefaultsOnly, Category = "ItemTree")
	FVector2D SourcePortLocationPos = FVector2D(0.5f, 0.1f);
	
	UPROPERTY(EditDefaultsOnly, Category = "ItemTree")
	FVector2D SourcePortDirection = FVector2D(0.f, 90.f);
	
	UPROPERTY(EditDefaultsOnly, Category = "ItemTree")
	FVector2D DestinationPortDirection = FVector2D(0.f, 90.f);
	
};
