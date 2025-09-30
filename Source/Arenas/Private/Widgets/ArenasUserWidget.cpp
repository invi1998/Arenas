// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasUserWidget.h"

#include "Interface/PawnUIInterface.h"
#include "Component/UI/PlayerUIComponent.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "Component/AbilityListView.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "Shop/ShopWidget.h"

void UArenasUserWidget::InitOverheadWidget(AActor* InActor)
{
	UArenasAbilitySystemComponent* OverHeadASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(InActor);
	if (!OverHeadASC) return;
	
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(InActor))
	{
		if (UPawnUIComponent* PawnUICom = PawnUIInterface->GetPawnUIComponent())
		{
			// 获取传入InActor和当前客户端所属玩家的Character的阵营的关系（敌对、友方、中立）
			ETeamAttitude::Type TeamAttitude = ETeamAttitude::Neutral;
			if (IPawnUIInterface* LocalPlayerControllerPawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
			{
				FGenericTeamId InActorTeamID = PawnUIInterface->GetOwningGenericTeamId();
				FGenericTeamId LocalPlayerControllerPawnTeamID = LocalPlayerControllerPawnUIInterface->GetOwningGenericTeamId();
				TeamAttitude = InActorTeamID == LocalPlayerControllerPawnTeamID ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
			}
			
			BP_OnOverheadWidgetInitialized(PawnUICom, TeamAttitude);
			PawnUICom->SetAndBoundAttributeDelegate(OverHeadASC);
		}
	}
}

void UArenasUserWidget::ConfigureAbilities(const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& InAbilities)
{
	if (bIsPlayerWidget && AbilityListView)
	{
		AbilityListView->ConfigureAbilities(InAbilities);
	}
}

void UArenasUserWidget::ToggleShopPopup()
{
	if (ShopWidget)
	{
		if (ShopWidget->GetVisibility() == ESlateVisibility::HitTestInvisible)
		{
			ShopWidget->SetVisibility(ESlateVisibility::Visible);
			PlayShopPopupAnim(true);
		}
		else
		{
			ShopWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			PlayShopPopupAnim(false);
		}
	}
}

void UArenasUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!bIsPlayerWidget) return;
	
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

	if (!bIsPlayerWidget) return;

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

void UArenasUserWidget::PlayShopPopupAnim(bool bPlayForward)
{
	if (ShopPopupAnim)
	{
		if (bPlayForward)
		{
			PlayAnimationForward(ShopPopupAnim);
		}
		else
		{
			PlayAnimationReverse(ShopPopupAnim);
		}
	}
}

