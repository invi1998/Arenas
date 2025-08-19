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

		// GetBaseAimRotation 若存在控制器，默认情况下会瞄准玩家的视线方向（即摄像机朝向）
		FRotator ControlRot = OwnerPlayerCharacter->GetBaseAimRotation();
		// 若存在控制器，默认情况下我们会瞄准玩家视线方向（即摄像机朝向）正对着的方向。
		// 这样就很完美了。
		// 对于pawn来说，默认情况下当它的视角旋转时就是这样的。
		// 但如果是人眼视角的摄像机，效果会更好。不过这只是从实现层面来看。这有点奇怪。
		// 我们来看看这里的实现。
		// 它试图获取控制器。
		// 如果控制器不为空，那么它就不是空的。然后我们只需让控制器提供视点位置，也就是摄像机的位置。没问题。我们就返回这个值。
		// 不过，若没有控制器，我们将采用角色的旋转角度——这样处理也很不错 对于 AI 而言。
		// 但随后会出现这种奇怪的俯仰角更新问题。
		// 因此当俯仰角为零时，我们会用同步复制的视角俯仰角来更新它。
		// 我不明白他们为何要采用这种方式，因为你看。
		// 他们确实有一个名为 remoteViewPitch 的变量，并且这个变量是同步复制的，对吧？
		// 但他们没有同步复制远程视角偏转。这会导致问题，因为在多人游戏中我们将只有同步的俯仰角数据。
		// 这样就不会同步偏航角了，对吧？所以在多人游戏中这个功能无法 100%生效。
		// 只有俯仰角会生效，因为在多人游戏中控制器并不存在于其他客户端上，对吧？
		// 这意味着只有我们的客户端能看到俯仰角在起作用，但偏航角没有效果

		LookRotOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, BodyRot);	// 计算头部旋转的偏移量
		
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

