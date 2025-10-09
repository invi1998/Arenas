// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "DefenseTowerCharacter.h"

#include "Arenas/Arenas.h"


// Sets default values
ADefenseTowerCharacter::ADefenseTowerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	TowerMesh->SetupAttachment(GetRootComponent());
	TowerMesh->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore); // 胶囊体忽略摄像机碰撞
	TowerMesh->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
}

// Called every frame
void ADefenseTowerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADefenseTowerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Called when the game starts or when spawned
void ADefenseTowerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


