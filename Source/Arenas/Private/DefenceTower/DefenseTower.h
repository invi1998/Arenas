// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "DefenseTower.generated.h"

class USphereComponent;

UCLASS()
class ARENAS_API ADefenseTower : public ACharacter
{
	GENERATED_BODY()

public:
	ADefenseTower();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	USphereComponent* InfluenceSphereComponent;

	UFUNCTION()
	void OnInfluenceSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
