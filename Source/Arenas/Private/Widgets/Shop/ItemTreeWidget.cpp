// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ItemTreeWidget.h"

#include "SplineWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UItemTreeWidget::ClearTree()
{
	RootPanel->ClearChildren();
}

UUserWidget* UItemTreeWidget::CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot*& OutCanvasPanelSlot)
{
	if (!Node) return nullptr;

	UUserWidget* NodeWidget = Node->GetWidget();
	OutCanvasPanelSlot = RootPanel->AddChildToCanvas(NodeWidget);
	if (OutCanvasPanelSlot)
	{
		OutCanvasPanelSlot->SetSize(NodeSize);
		OutCanvasPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f));		// 设置锚点为中心
		OutCanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));	// 设置对齐方式为中心
		OutCanvasPanelSlot->SetZOrder(1);
	}

	return NodeWidget;
	
}

void UItemTreeWidget::CreateConnection(const UUserWidget* From, const UUserWidget* To)
{
	if (!From || !To) return;

	USplineWidget* ConnectionSpline = CreateWidget<USplineWidget>(GetOwningPlayer());
	if (UCanvasPanelSlot* ConnectionPanelSlot = RootPanel->AddChildToCanvas(ConnectionSpline))
	{
		ConnectionPanelSlot->SetAnchors(FAnchors(0.f));		// 设置锚点为左上角
		ConnectionPanelSlot->SetAlignment(FVector2D(0.f, 0.f));
		ConnectionPanelSlot->SetPosition(FVector2D::ZeroVector);
		ConnectionPanelSlot->SetZOrder(0);
	}

	ConnectionSpline->SetupSpline(From, To, SourcePortLocationPos, DestinationPortDirection, SourcePortDirection, DestinationPortDirection);
	ConnectionSpline->SetSplineStyle(ConnectionColor, ConnectionThickness);
	
}
