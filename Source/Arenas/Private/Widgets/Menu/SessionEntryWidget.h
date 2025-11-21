// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
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
	virtual void NativeDestruct() override;
	
	FOnSesionEntrySelectedDelegate OnSessionEntrySelectedDelegate;

	void InitializeEntry(const FString& InSessionName, const FString& InSessionIDString, const FOnlineSessionSearchResult& InSessionSearchResult);
	
	FORCEINLINE FString GetCachedSessionIDString() const { return CachedSessionIDString; }
	
private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SessionNameText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerAmount;
	
	UPROPERTY(meta=(BindWidget))
	UButton* SessionButton;
	
	FString CachedSessionIDString;
	
	UFUNCTION()
	void OnSessionButtonClicked();
	
	FOnlineSessionSearchResult CachedSessionSearchResult;
	
	void OnSessionUpdated(FName SessionName, bool bWasSuccessful);
	
	void OnSessionParticipantJoin(FName SessionName, const FUniqueNetId& NetId);
	void OnSessionParticipantLeft(FName SessionName, const FUniqueNetId& NetId, EOnSessionParticipantLeftReason OnSessionParticipantLeftReason);
	
	void UpdatePlayerCountDisplay();
	
};
