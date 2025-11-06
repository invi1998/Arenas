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
		// 获取当前贴花的旋转
		FRotator DecalRot = ChargeDecalComponent->GetComponentRotation();
		// 只修改Yaw值，使其与控制器的朝向一致
		DecalRot.Yaw = StartRot.Yaw + 90.f; // Decal的朝向需要加90度偏移
		
		ChargeDecalComponent->SetWorldRotation(DecalRot);

		if (bNeedsSizeUpdate)
		{
			ChargeDecalComponent->DecalSize = FVector(500.F, MaxChargeDistance, 500.f);
			ChargeDecalComponent->MarkRenderStateDirty();
			bNeedsSizeUpdate = false;
		}
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
	bNeedsSizeUpdate = true;
}

void ATargetActor_ChargeIndicator::StopAttaching()
{
	bStopAttaching = true;
}



