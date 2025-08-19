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
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE ACharacter* GetOwnerPlayerCharacter() const { return OwnerPlayerCharacter; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE UCharacterMovementComponent* GetOwnerMovementComponent() const { return OwnerMovementComponent; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed != 0.0f; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsNotMoving() const { return Speed == 0.0f; }
	
protected:

private:
	UPROPERTY()
	ACharacter* OwnerPlayerCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComponent;

	float Speed;
	
};
