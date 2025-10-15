// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "AttackRangeDecal.h"

#include "Components/DecalComponent.h"


// Sets default values
AAttackRangeDecal::AAttackRangeDecal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RangeDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("RangeDecalComponent"));
	RangeDecalComponent->SetupAttachment(GetRootComponent());
	
}

// Called every frame
void AAttackRangeDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAttackRangeDecal::SetDecalSize(const FVector& NewSize)
{
	if (RangeDecalComponent)
	{
		RangeDecalComponent->DecalSize = NewSize;
	}
}

UMaterialInstanceDynamic* AAttackRangeDecal::GetDecalMaterialInstance()
{
	if (RangeDecalComponent)
	{
		return RangeDecalComponent->CreateDynamicMaterialInstance();
	}
	return nullptr;
}

// Called when the game starts or when spawned
void AAttackRangeDecal::BeginPlay()
{
	Super::BeginPlay();
	
}



