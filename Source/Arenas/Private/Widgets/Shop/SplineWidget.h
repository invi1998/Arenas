// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SplineWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API USplineWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 绘制贝塞尔曲线
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	
	void SetupSpline(
		const UUserWidget* InStartWidget,		// 起点小部件
		const UUserWidget* InEndWidget,			// 终点小部件
		const FVector2D& InStartPortLocalCoord,	// 起点端口在起点小部件中的本地坐标
		const FVector2D& InEndPortLocalCoord,	// 终点端口在终点小部件中的本地坐标
		const FVector2D& InStartPortDir,		// 起点端口方向（单位向量）
		const FVector2D& InEndPortDir);		// 终点端口方向（单位向量）

	void SetSplineStyle(const FLinearColor& InColor, float InThickness);

private:
	UPROPERTY(EditAnywhere, Category = "Test")
	FVector2D TestStartPortPos;

	UPROPERTY(EditAnywhere, Category = "Test")
	FVector2D TestEndPortPos;
	
	UPROPERTY()
	const UUserWidget* StartWidget;

	UPROPERTY()
	const UUserWidget* EndWidget;

	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D StartPortDir;

	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D EndPortDir;

	UPROPERTY(EditAnywhere, Category = "Spline")
	FLinearColor SplineColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Spline")
	float SplineThickness = 3.f;

	FVector2D StartPortLocalCoord;
	FVector2D EndPortLocalCoord;
	
};
