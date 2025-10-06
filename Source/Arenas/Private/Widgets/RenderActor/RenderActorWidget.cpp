// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "RenderActorWidget.h"

#include "RenderActor.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/TextureRenderTarget2D.h"

void URenderActorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	RenderSizeBox->SetWidthOverride(RenderSize.X);
	RenderSizeBox->SetHeightOverride(RenderSize.Y);
	
}

void URenderActorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SpawnRenderActor();
	ConfigureRenderActor();
	BeginRenderCapture();
	
}

void URenderActorWidget::BeginDestroy()
{
	StopRenderCapture();
	
	Super::BeginDestroy();
}

void URenderActorWidget::ConfigureRenderActor()
{
	if (!GetRenderActor())
	{
		UE_LOG(LogTemp, Error, TEXT("RenderActorWidget is NULL"));
		return;
	}

	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(static_cast<uint32>(RenderSize.X), static_cast<uint32>(RenderSize.Y));
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8_SRGB;	// 使用sRGB格式以确保颜色正确显示
	GetRenderActor()->SetRenderTarget(RenderTarget);

	if (UMaterialInstanceDynamic* DisplayDynamicImageMID = DisplayImage->GetDynamicMaterial())
	{
		DisplayDynamicImageMID->SetTextureParameterValue(DisplayImageRenderTargetParamName, RenderTarget);
	}
	
}

void URenderActorWidget::BeginRenderCapture()
{
	RenderTickInterval = 1.f / static_cast<float>(FrameRate);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RenderTickTimerHandle, this, &URenderActorWidget::UpdateRender, RenderTickInterval, true);
	}
}

void URenderActorWidget::UpdateRender()
{
	if (GetRenderActor())
	{
		GetRenderActor()->UpdateRender();
	}
}

void URenderActorWidget::StopRenderCapture()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RenderTickTimerHandle);
	}
}
