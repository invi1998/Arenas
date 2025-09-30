// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryComponent.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "PA_ShopItem.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasHeroAttributeSet.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerArenasASC) return;

	Server_Purchase(ItemToPurchase);
	
}

float UInventoryComponent::GetGold() const
{
	if (OwnerArenasASC)
	{
		bool bFound = false;
		float Gold = OwnerArenasASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetGoldAttribute(), bFound);
		if (bFound)
		{
			return Gold;
		}
	}

	return 0.f;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerArenasASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(GetOwner());
	
}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase) return;

	if (GetGold() < ItemToPurchase->GetPrice())
	{
		return;
	}

	OwnerArenasASC->ApplyModToAttribute(UArenasHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	
	
}


