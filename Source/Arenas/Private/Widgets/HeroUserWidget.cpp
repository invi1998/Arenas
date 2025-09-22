// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "HeroUserWidget.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "Component/UI/PlayerUIComponent.h"
#include "Interface/PawnUIInterface.h"

void UHeroUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UHeroUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* OwnnerPawn = GetOwningPlayerPawn();
	if (!OwnnerPawn) return;

	UArenasAbilitySystemComponent* OwnerPlayerArenasASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(OwnnerPawn);
	if (!OwnerPlayerArenasASC) return;

	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwnnerPawn))
	{
		if (UPlayerUIComponent* PlayerUIComponent = PawnUIInterface->GetPlayerUIComponent())
		{
			BP_OnOwningPlayerUIComponentInitialized(PlayerUIComponent);
			PlayerUIComponent->SetAndBoundAttributeDelegate(OwnerPlayerArenasASC);
		}
	}
}
