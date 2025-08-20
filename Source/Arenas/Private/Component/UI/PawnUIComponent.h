// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Component/PawnExtensionComponentBase.h"
#include "PawnUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPercentChangedDelegate, float, NewPercent, float, NewValue, float, MaxValue);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARENAS_API UPawnUIComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnPercentChanged;	// 百分比变化委托
	
};
