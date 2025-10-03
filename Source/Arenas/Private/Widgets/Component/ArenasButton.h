// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArenasButton.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadWrite)
	UButton* ButtonArea;
};
