// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ArenasAnimInstance.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeUninitializeAnimation() override;
	
protected:

private:
	UPROPERTY()
	ACharacter* OwnerPlayerCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComponent;
	
};
