// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionEntryWidget.generated.h"

/**
 * 会话条目Widget
 */

class UButton;
class UTextBlock;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSesionEntrySelectedDelegate, const FString& /*SessionIDStr*/);


UCLASS()
class ARENAS_API USessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	FOnSesionEntrySelectedDelegate OnSessionEntrySelectedDelegate;
	
	void InitializeEntry(const FString& InSessionName, const FString& InSessionIDString);
	
	FORCEINLINE FString GetCachedSessionIDString() const { return CachedSessionIDString; }
	
private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SessionNameText;
	
	UPROPERTY(meta=(BindWidget))
	UButton* SessionButton;
	
	FString CachedSessionIDString;
	
	UFUNCTION()
	void OnSessionButtonClicked();
	
};
