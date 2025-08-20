// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValueGauge.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UValueGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	void SetValue(float NewValue, float MaxValue);

private:
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UUserWidget* ProgressBar;
	
};
