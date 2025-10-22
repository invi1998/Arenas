// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

struct FGameplayEventData;
class UCanvasPanelSlot;
class UImage;
/**
 * 准心UI
 */
UCLASS()
class ARENAS_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


private:
	UPROPERTY(meta=(BindWidget))
	UImage* CrosshairImage;

	void OnCrosshairTagChanged(FGameplayTag INGameplayTag, int32 Count);

	UPROPERTY()
	UCanvasPanelSlot* CrosshairImageSlot;

	UPROPERTY()
	APlayerController* CachedPlayerController;

	void UpdateCrosshairPosition();

	UPROPERTY()
	const AActor* CurrentAimTargetActor;

	void TargetUpdated(const FGameplayEventData* Data);

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor CrosshairDefaultColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor CrosshairHitTargetColor = FLinearColor::Red;
	
};
