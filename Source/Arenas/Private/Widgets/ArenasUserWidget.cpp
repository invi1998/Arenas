// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasUserWidget.h"

#include "Interface/PawnUIInterface.h"
#include "Component/UI/PlayerUIComponent.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "GAS/ArenasAbilitySystemComponent.h"

void UArenasUserWidget::InitOverheadWidget(AActor* InActor)
{
	UArenasAbilitySystemComponent* OverHeadASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(InActor);
	if (!OverHeadASC) return;
	
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(InActor))
	{
		if (UPlayerUIComponent* PlayerUIComponent = PawnUIInterface->GetPlayerUIComponent())
		{
			BP_OnOverheadWidgetInitialized(PlayerUIComponent);
			PlayerUIComponent->SetAndBoundAttributeDelegate(OverHeadASC);
		}
	}
}

void UArenasUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (UPlayerUIComponent* PlayerUIComponent = PawnUIInterface->GetPlayerUIComponent())
		{
			BP_OnOwningPlayerUIComponentInitialized(PlayerUIComponent);
		}
	}
}

void UArenasUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedArenasASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(GetOwningPlayerPawn());
	if (CachedArenasASC)
	{
		if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
		{
			if (UPlayerUIComponent* PlayerUIComponent = PawnUIInterface->GetPlayerUIComponent())
			{
				PlayerUIComponent->SetAndBoundAttributeDelegate(CachedArenasASC);
			}
		}
	}
}

