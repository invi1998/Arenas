// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RenderActorWidget.generated.h"

class ARenderActor;
class USizeBox;
class UImage;
class UTextureRenderTarget2D;

/**
 * 
 */
UCLASS(Abstract)
class ARENAS_API URenderActorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;
	
private:
	void ConfigureRenderActor();
	// 如果想在UUserWidget中定义一个纯虚函数，该类必须是一个Abstract（抽象类）
	virtual void SpawnRenderActor() PURE_VIRTUAL(URenderActorWidget::SpawnRenderActor, );
	virtual ARenderActor* GetRenderActor() const PURE_VIRTUAL(URenderActorWidget::GetRenderActor, return nullptr;);
	
	void BeginRenderCapture();
	void UpdateRender();
	void StopRenderCapture();
	
	UPROPERTY(meta=(BindWidget))
	UImage* DisplayImage;

	UPROPERTY(meta=(BindWidget))
	USizeBox* RenderSizeBox;

	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	FName DisplayImageRenderTargetParamName = "RenderTarget";

	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	FVector2D RenderSize;

	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	int FrameRate = 24;		// 渲染帧率

	float RenderTickInterval;
	FTimerHandle RenderTickTimerHandle;

	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget;
	
};
