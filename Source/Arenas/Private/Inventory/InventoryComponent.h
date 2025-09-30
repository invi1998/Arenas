// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

/**
 * 库存组件，管理角色的物品和装备
 */

class UPA_ShopItem;
class UArenasAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARENAS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	void TryPurchase(const UPA_ShopItem* ItemToPurchase);
	float GetGold() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UArenasAbilitySystemComponent* OwnerArenasASC;

	/**********************************************************************************/
	/*									Server										 */
	/**********************************************************************************/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase);
	
};
