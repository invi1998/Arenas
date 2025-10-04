// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "SplineWidget.h"

int32 USplineWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	LayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FVector2D StartPosition = TestStartPortPos;
	FVector2D EndPosition = TestEndPortPos;

	if (StartWidget && EndWidget)
	{
		StartPosition = StartWidget->GetCachedGeometry().GetLocalPositionAtCoordinates(StartPortLocalCoord);
		EndPosition = EndWidget->GetCachedGeometry().GetLocalPositionAtCoordinates(EndPortLocalCoord);
		
	}

	// 绘制样条曲线
	FSlateDrawElement::MakeSpline(
		OutDrawElements,
		++LayerId,
		AllottedGeometry.ToPaintGeometry(),
		StartPosition,
		StartPortDir,
		EndPosition,
		EndPortDir,
		SplineThickness,
		ESlateDrawEffect::None,
		SplineColor
	);

	return LayerId;

}

void USplineWidget::SetupSpline(const UUserWidget* InStartWidget, const UUserWidget* InEndWidget,
                                const FVector2D& InStartPortLocalCoord, const FVector2D& InEndPortLocalCoord, const FVector2D& InStartPortDir,
                                const FVector2D& InEndPortDir)
{
	StartWidget = InStartWidget;
	EndWidget = InEndWidget;
	StartPortLocalCoord = InStartPortLocalCoord;
	EndPortLocalCoord = InEndPortLocalCoord;
	StartPortDir = InStartPortDir;
	EndPortDir = InEndPortDir;
}

void USplineWidget::SetSplineStyle(const FLinearColor& InColor, float InThickness)
{
}
