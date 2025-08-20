// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ValueGauge.h"

#include "Kismet/KismetMathLibrary.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UValueGauge::SetValue(float NewValue, float MaxValue)
{
	float NewPercent = UKismetMathLibrary::SafeDivide(NewValue, MaxValue);

	FNumberFormattingOptions NumberFormat = FNumberFormattingOptions()
		.SetMaximumFractionalDigits(0);

	
	
}
