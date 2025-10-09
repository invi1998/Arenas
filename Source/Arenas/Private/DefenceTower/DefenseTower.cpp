// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "DefenseTower.h"

#include "Components/SphereComponent.h"


ADefenseTower::ADefenseTower()
{
	PrimaryActorTick.bCanEverTick = false;

	InfluenceSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InfluenceSphereComponent"));
	InfluenceSphereComponent->SetupAttachment(GetRootComponent());

	InfluenceSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ADefenseTower::OnInfluenceSphereBeginOverlap);

	
}

void ADefenseTower::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADefenseTower::OnInfluenceSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

