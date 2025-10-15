// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackRangeDecal.generated.h"

class UDecalComponent;

UCLASS()
class ARENAS_API AAttackRangeDecal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAttackRangeDecal();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetDecalSize(const FVector& NewSize);
	UMaterialInstanceDynamic* GetDecalMaterialInstance();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> RangeDecalComponent;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
};
