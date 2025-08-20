// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArenasUserWidget.generated.h"

class UArenasAbilitySystemComponent;
struct FGameplayAttribute;
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
	// NativeOnInitialized 先执行，NativeConstruct 后执行
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta=(DisplayName = "On Owning Player UI Component Initialized"))
	void BP_OnOwningPlayerUIComponentInitialized(UPlayerUIComponent* InPlayerUIComponent);

private:
	UPROPERTY()
	UArenasAbilitySystemComponent* CachedArenasASC;
	
};
