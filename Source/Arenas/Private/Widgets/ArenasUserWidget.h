// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArenasUserWidget.generated.h"

class UPlayerUIComponent;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta=(DisplayName = "On Owning Player UI Component Initialized"))
	void BP_OnOwningPlayerUIComponentInitialized(UPlayerUIComponent* InPlayerUIComponent);
	
	
};
