// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UArenasAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerPlayerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerPlayerCharacter)
	{
		OwnerMovementComponent = OwnerPlayerCharacter->GetCharacterMovement();
	}
	else
	{
		OwnerMovementComponent = nullptr;
	}
	
}

void UArenasAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerPlayerCharacter)
	{
		Speed = OwnerPlayerCharacter->GetVelocity().Length();
		FRotator BodyRot = OwnerPlayerCharacter->GetActorRotation();
		FRotator BodyRotDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRot, BodyPrevRot);	// 计算身体旋转的增量，同时该增量会被规范化到0-180度范围内
		BodyPrevRot = BodyRot;

		YawSpeed = BodyRotDelta.Yaw / DeltaSeconds;	// 计算身体旋转的角速度

		// 因为我们使用鼠标控制旋转移动的时候，会导致身体旋转角速度的变化非常大，使用这种方法会有些不稳定
		// 我们会看到身体动作变得很急促，所以我们需要平滑处理身体旋转角速度
		// 这里我们使用插值来平滑处理身体旋转角速度
		// 这里的插值速度可以根据需要进行调整，默认值为1
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawSpeedSmoothLerpSpeed);
	}

	if (OwnerMovementComponent)
	{
		bIsJumping = OwnerMovementComponent->IsFalling();
	}
	
}

void UArenasAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

