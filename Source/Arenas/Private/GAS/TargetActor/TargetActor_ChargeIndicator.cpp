// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TargetActor_ChargeIndicator.h"

#include "Abilities/GameplayAbility.h"
#include "Components/DecalComponent.h"


// Sets default values
ATargetActor_ChargeIndicator::ATargetActor_ChargeIndicator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	ChargeDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("ChargeDecalComponent"));
	ChargeDecalComponent->SetupAttachment(GetRootComponent());

	bStopAttaching = false;
}

// Called every frame
void ATargetActor_ChargeIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bStopAttaching) return;
	if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
	{
		if (AvatarActor)
		{
			SetActorLocation(AvatarActor->GetActorLocation());
		}
		// 修改蓄力贴花的朝向，使其始终朝向控制器当前的指向
		FVector StartLoc;
		FRotator StartRot;
		PrimaryPC->GetPlayerViewPoint(StartLoc, StartRot);
		FRotator DecalRotation = StartRot;
		DecalRotation.Pitch -= 90.f; // 贴花需要向下倾斜90度
		DecalRotation.Yaw += 90.f;
		ChargeDecalComponent->SetWorldRotation(DecalRotation);
	}
}

void ATargetActor_ChargeIndicator::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	bStopAttaching = false;
	if (!OwningAbility) return;
	if (Ability)
	{
		AvatarActor = Ability->GetAvatarActorFromActorInfo();
	}
}

void ATargetActor_ChargeIndicator::ConfirmTargetingAndContinue()
{
	Super::ConfirmTargetingAndContinue();
}

void ATargetActor_ChargeIndicator::SetTargetDistance(float Distance)
{
	MaxChargeDistance = Distance;
	ChargeDecalComponent->DecalSize = FVector(MaxChargeDistance, MaxChargeDistance, 500.f);
}

void ATargetActor_ChargeIndicator::StopAttaching()
{
	bStopAttaching = true;
}



