// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ItemTreeWidget.h"

#include "SplineWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"


void UItemTreeWidget::DrawSteam(bool bUpperSteam, const ITreeNodeInterface* StartNode, UUserWidget* StartWidget,
	UCanvasPanelSlot* StartNodeSlot, int StartNodeDepth, float& NextLeafXPos, TArray<UCanvasPanelSlot*>& OutSteamSlots)
{
	// 获取下一级节点，根据是上游还是下游来决定（上游取输入节点，输入节点存储的是当前装备可以合成哪些装备列表，下游取输出节点，输出节点存储的是当前装备的合成材料列表）
	TArray<const ITreeNodeInterface*> NextTreeNodes = bUpperSteam ? StartNode->GetInputs() : StartNode->GetOutputs();

	float StartNodeYPos = (NodeSize.Y + NodeGap.Y) * StartNodeDepth * (bUpperSteam ? -1.f : 1.f);	// 计算当前节点的Y位置
	if (NextTreeNodes.Num() == 0)
	{
		// 没有下一级节点，说明已经到达叶节点
		StartNodeSlot->SetPosition(FVector2D(NextLeafXPos, StartNodeYPos));
		NextLeafXPos += NodeSize.X + NodeGap.X;	// 更新下一叶节点的X位置
		return;
	}

	float NextNodeXPosSum = 0.f;	// 下一级节点的总宽度

	// 如果不是叶子节点，则递归绘制下一级节点
	for (const ITreeNodeInterface* NextNode : NextTreeNodes)
	{
		UCanvasPanelSlot* NextCanvasPanelSlot = nullptr;
		UUserWidget* NextNodeWidget = CreateWidgetForNode(NextNode, NextCanvasPanelSlot);
		OutSteamSlots.Add(NextCanvasPanelSlot);
		if (bUpperSteam)
		{
			// 绘制上游节点连线（起点是下一节点（或者说是当前节点的输入节点），终点是当前节点）
			CreateConnection(NextNodeWidget, StartWidget);
		}
		else
		{
			// 绘制下游节点连线（起点是当前节点，终点是下一节点（或者说是当前节点的输出节点））
			CreateConnection(StartWidget, NextNodeWidget);
		}

		DrawSteam(bUpperSteam, NextNode, NextNodeWidget, NextCanvasPanelSlot, StartNodeDepth + 1, NextLeafXPos, OutSteamSlots);

		NextNodeXPosSum += NextCanvasPanelSlot->GetPosition().X;	// 累加下一级节点的X位置
	}

	float StartingNodeXPos = NextNodeXPosSum / NextTreeNodes.Num();	// 计算当前节点的X位置，使其居中对齐
	StartNodeSlot->SetPosition(FVector2D(StartingNodeXPos, StartNodeYPos));
	
}

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
