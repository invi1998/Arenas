// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasAnimInstance.h"

#include "GameFramework/Character.h"

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
	}
}

void UArenasAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

