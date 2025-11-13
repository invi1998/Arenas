// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "ArenasAnimInstance.generated.h"

class UArenasAbilitySystemComponent;
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

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawSpeed() const { return YawSpeed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSmoothedYawSpeed() const { return SmoothedYawSpeed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsJumping() const { return bIsJumping; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsOnGround() const { return !bIsJumping; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookYawOffset() const { return LookRotOffset.Yaw; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookPitchOffset() const { return LookRotOffset.Pitch; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetForwardSpeed() const { return ForwardSpeed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetRightSpeed() const { return RightSpeed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	bool ShouldDoFullBody() const;
	
protected:

private:
	UPROPERTY()
	ACharacter* OwnerPlayerCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComponent;

	UPROPERTY()
	UArenasAbilitySystemComponent* OwnerArenasASC;

	float Speed;
	float YawSpeed;
	float SmoothedYawSpeed;		// 平滑的身体旋转角速度
	bool bIsJumping;
	bool bIsAiming;		// 是否在瞄准
	float ForwardSpeed;
	float RightSpeed;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float YawSpeedSmoothLerpSpeed = 1.f;	// 平滑的身体旋转角速度的插值速度

	UPROPERTY(EditAnywhere, Category = "Animation")
	float YawSpeedLerpToZeroSpeed = 30.f;	// 当角色停止移动时，身体旋转角速度插值到0的速度
	
	FRotator BodyPrevRot;	// 角色身体的上一个旋转角度
	FRotator LookRotOffset;	// 角色头部的旋转偏移
	void OnOwnerAimingTagChanged(FGameplayTag GameplayTag, int32 Count);
};
