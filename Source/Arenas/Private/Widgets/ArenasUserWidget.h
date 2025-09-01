// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Blueprint/UserWidget.h"
#include "ArenasUserWidget.generated.h"

class UPawnUIComponent;
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
	void InitOverheadWidget(AActor* InActor);

protected:
	// NativeOnInitialized 先执行，NativeConstruct 后执行
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta=(DisplayName = "On Owning Player UI Component Initialized"))
	void BP_OnOwningPlayerUIComponentInitialized(UPlayerUIComponent* InPlayerUIComponent);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta=(DisplayName = "On OverHead Created Widget Initialized"))
	void BP_OnOverheadWidgetInitialized(UPawnUIComponent* InOverHeadUIComponent, ETeamAttitude::Type InTeamAttitude);

private:
	UPROPERTY()
	UArenasAbilitySystemComponent* CachedArenasASC;
	
};
