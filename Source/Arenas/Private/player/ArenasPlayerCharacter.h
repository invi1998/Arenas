// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Character/ArenasCharacter.h"
#include "Types/ArenaStructTypes.h"
#include "ArenasPlayerCharacter.generated.h"

class UInventoryComponent;
class UArenasHeroAttributeSet;
class UPlayerUIComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ARENAS_API AArenasPlayerCharacter : public AArenasCharacter
{
	GENERATED_BODY()

public:
	AArenasPlayerCharacter();

	// 该函数是官方推荐用于初始化输入映射设置时需要重写的函数
	// 这是因为当客户端重启时该函数会被调用，这是在多人游戏中首次在客户端生成实际角色的时机
	// 此时角色已经准备好在BeginPlay中与输入进行绑定
	virtual void PawnClientRestart() override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	// Interface IPawnUIInterface Begin
	virtual UPlayerUIComponent* GetPlayerUIComponent() const override;
	// Interface IPawnUIInterface End
	
private:
	UPROPERTY()
	UArenasHeroAttributeSet* HeroAttributeSet;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	UCameraComponent* FollowCamera;

	FVector GetLookForwardDir() const;
	FVector GetLookRightDir() const;
	FVector GetMoveForwardDir() const;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* UpgradeAbilityLeaderInputAction;	// 升级技能栏

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* UseInventoryItemInputAction;	// 使用物品栏
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* GameplayInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<EArenasAbilityInputID, UInputAction*> GameplayAbilityInputActions;

	void HandleLookInput(const FInputActionValue& Value);
	void HandleMoveInput(const FInputActionValue& Value);
	void HandleAbilityInput(const FInputActionValue& Value, EArenasAbilityInputID AbilityID);
	void LearnAbilityLeaderDown(const FInputActionValue& Value);
	void LearnAbilityLeaderUp(const FInputActionValue& Value);
	void UseInventoryItem(const FInputActionValue& InputActionValue);

	bool bIsLearnAbilityLeaderDown = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UPlayerUIComponent* PlayerUIComponent;

	virtual void OnDeath() override;
	virtual void OnRespawn() override;

	virtual void OnStun() override;
	virtual void OnUnStun() override;

	virtual void OnAimStateChanged(bool bNewAiming) override;

	// 瞄准时相机位置的偏移
	UPROPERTY(EditDefaultsOnly, Category = "View")
	FVector CameraAimLocationOffset = FVector(50.f, 0.f, 50.f);

	UPROPERTY(EditDefaultsOnly, Category = "View")
	float CameraLerpSpeed = 10.f;

	// 相机差值计时器
	FTimerHandle CameraLerpTimerHandle;

	// 将相机位置差值到指定位置
	void LerpCameraToLocalOffsetLocation(const FVector& Goal);
	void TickCameraLerp(FVector Goal);


	UPROPERTY()
	UInventoryComponent* InventoryComponent;
	
};

