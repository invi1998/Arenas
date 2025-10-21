// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "CrosshairWidget.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "GAS/ArenasAbilitySystemComponent.h"


void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CrosshairImage->SetVisibility(ESlateVisibility::Hidden);

	UArenasAbilitySystemComponent* ASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(GetOwningPlayerPawn());
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(ArenasGameplayTags::Status_Crosshair).AddUObject(this, &UCrosshairWidget::OnCrosshairTagChanged);
	}

	CachedPlayerController = GetOwningPlayer();
	
	CrosshairImageSlot = Cast<UCanvasPanelSlot>(Slot);
	if (!CrosshairImageSlot)
	{
		UE_LOG(LogTemp, Error, TEXT("准心控件需要作为画布面板的子项"));
	}
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CachedPlayerController || !CrosshairImageSlot) return;

	if (CrosshairImage->GetVisibility() == ESlateVisibility::Visible)
	{
		UpdateCrosshairPosition();	
	}
	
}

void UCrosshairWidget::OnCrosshairTagChanged(FGameplayTag INGameplayTag, int32 Count)
{
	if (Count > 0)
	{
		CrosshairImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCrosshairWidget::UpdateCrosshairPosition()
{
	if (!CachedPlayerController || !CrosshairImageSlot) return;

	float ViewScale = UWidgetLayoutLibrary::GetViewportScale(this);

	int32 ViewportX, ViewportY;
	CachedPlayerController->GetViewportSize(ViewportX, ViewportY);

	FVector2D ViewPortSize = FVector2D(static_cast<float>(ViewportX), static_cast<float>(ViewportY));
	CrosshairImageSlot->SetPosition(ViewPortSize * 0.5f / ViewScale);
	
}
