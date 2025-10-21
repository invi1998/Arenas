// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
#include "ArenasPlayerController.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Arenas/Arenas.h"
#include "Component/UI/PlayerUIComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/ArenasHeroAttributeSet.h"
#include "Inventory/InventoryComponent.h"


// Sets default values
AArenasPlayerCharacter::AArenasPlayerCharacter()
{
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true; // 让弹簧臂组件使用控制器的旋转
	CameraBoom->ProbeChannel = ECC_SpringArm;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// 这里我需要指定插槽，如果不这样做，相机实际会附着在相机臂的根部，而不是末端插槽上，我们需要指定该插槽，以便使用弹簧臂组件的插槽名
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	
	bUseControllerRotationYaw = false;		// 让角色不跟随控制器的旋转
	GetCharacterMovement()->bOrientRotationToMovement = true; // 让角色朝向移动方向
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // 设置角色的旋转速率

	PlayerUIComponent = CreateDefaultSubobject<UPlayerUIComponent>(TEXT("PlayerUIComponent"));

	HeroAttributeSet = CreateDefaultSubobject<UArenasHeroAttributeSet>(TEXT("HeroAttributeSet"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	
}

void AArenasPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (AArenasPlayerController* OwningPlayerController = GetController<AArenasPlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			InputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);
		}
	}
	
}

void AArenasPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &AArenasPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &AArenasPlayerCharacter::HandleLookInput);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AArenasPlayerCharacter::HandleMoveInput);
		EnhancedInputComponent->BindAction(UpgradeAbilityLeaderInputAction, ETriggerEvent::Started, this, &AArenasPlayerCharacter::LearnAbilityLeaderDown);
		EnhancedInputComponent->BindAction(UpgradeAbilityLeaderInputAction, ETriggerEvent::Completed, this, &AArenasPlayerCharacter::LearnAbilityLeaderUp);
		
		// ability
		for (const TPair<EArenasAbilityInputID, UInputAction*>& AbilityPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(AbilityPair.Value, ETriggerEvent::Triggered, this, &AArenasPlayerCharacter::HandleAbilityInput, AbilityPair.Key);
		}

		EnhancedInputComponent->BindAction(UseInventoryItemInputAction, ETriggerEvent::Triggered, this, &AArenasPlayerCharacter::UseInventoryItem);

	}
	
}

UPlayerUIComponent* AArenasPlayerCharacter::GetPlayerUIComponent() const
{
	return PlayerUIComponent;
}

FVector AArenasPlayerCharacter::GetLookForwardDir() const
{
	return FollowCamera->GetForwardVector();
}

FVector AArenasPlayerCharacter::GetLookRightDir() const
{
	return FollowCamera->GetRightVector();
}

FVector AArenasPlayerCharacter::GetMoveForwardDir() const
{
	return FVector::CrossProduct(GetLookRightDir(), FVector::UpVector);
}

void AArenasPlayerCharacter::HandleLookInput(const FInputActionValue& Value)
{
	FVector2D LookDirection = Value.Get<FVector2D>();

	AddControllerPitchInput(LookDirection.Y);
	AddControllerYawInput(LookDirection.X);
}

void AArenasPlayerCharacter::HandleMoveInput(const FInputActionValue& Value)
{
	FVector2D InputVal = Value.Get<FVector2D>();
	InputVal.Normalize();

	AddMovementInput(GetMoveForwardDir() * InputVal.Y + GetLookRightDir() * InputVal.X);
	
}

void AArenasPlayerCharacter::HandleAbilityInput(const FInputActionValue& Value, EArenasAbilityInputID AbilityID)
{
	// 因为技能输入可能是按下就触发，也可能是按下后松开触发，所以这里通过检查bPressed来区别这两种情况
	bool bPressed = Value.Get<bool>();
	if (bPressed && bIsLearnAbilityLeaderDown && AbilityID != EArenasAbilityInputID::BasicAttack && AbilityID != EArenasAbilityInputID::Confirm && AbilityID != EArenasAbilityInputID::Cancel)
	{
		// 如果当前正在按住升级技能栏键，并且触发的技能不是普攻，则尝试升级该技能
		UpgradeAbilityWithInputID(AbilityID);
		return;
	}
	
	if (bPressed)
	{
		// 执行本地技能触发(按下)
		GetAbilitySystemComponent()->AbilityLocalInputPressed(static_cast<int32>(AbilityID));
	}
	else
	{
		// 执行本地技能触发(松开)
		GetAbilitySystemComponent()->AbilityLocalInputReleased(static_cast<int32>(AbilityID));
	}

	if (AbilityID == EArenasAbilityInputID::BasicAttack)
	{
		FGameplayTag BasicAttackTag = bPressed ? ArenasGameplayTags::Ability_BasicAttack_Pressed : ArenasGameplayTags::Ability_BasicAttack_Released;
		
		// 确认技能释放
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, BasicAttackTag, FGameplayEventData());
		// 服务器端确认技能释放
		Server_SendGameplayEventToSelf(BasicAttackTag, FGameplayEventData());
	}
	
}

void AArenasPlayerCharacter::LearnAbilityLeaderDown(const FInputActionValue& Value)
{
	bIsLearnAbilityLeaderDown = true;
}

void AArenasPlayerCharacter::LearnAbilityLeaderUp(const FInputActionValue& Value)
{
	bIsLearnAbilityLeaderDown = false;
}

void AArenasPlayerCharacter::UseInventoryItem(const FInputActionValue& InputActionValue)
{
	int Value = FMath::RoundToInt(InputActionValue.Get<float>());
	InventoryComponent->TryActivateItemAbilityInSlot(Value - 1);
}

void AArenasPlayerCharacter::OnDeath()
{
	if (APlayerController* PC = GetController<APlayerController>())
	{
		DisableInput(PC);
	}
}

void AArenasPlayerCharacter::OnRespawn()
{
	if (IsStunning()) return;
	if (APlayerController* PC = GetController<APlayerController>())
	{
		EnableInput(PC);
	}
}

void AArenasPlayerCharacter::OnStun()
{
	// 被眩晕时，禁用输入
	if (APlayerController* PC = GetController<APlayerController>())
	{
		DisableInput(PC);
	}
}

void AArenasPlayerCharacter::OnUnStun()
{
	if (!IsAlive()) return;
	if (APlayerController* PC = GetController<APlayerController>())
	{
		EnableInput(PC);
	}
}

void AArenasPlayerCharacter::OnAimStateChanged(bool bNewAiming)
{
	if (IsLocallyControlledByPlayer())
	{
		FVector Goal = bNewAiming ? CameraAimLocationOffset : FVector::ZeroVector;
		LerpCameraToLocalOffsetLocation(Goal);
	}
	
}

void AArenasPlayerCharacter::LerpCameraToLocalOffsetLocation(const FVector& Goal)
{
	GetWorldTimerManager().ClearTimer(CameraLerpTimerHandle);
	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AArenasPlayerCharacter::TickCameraLerp, Goal));
}

void AArenasPlayerCharacter::TickCameraLerp(FVector Goal)
{
	FVector CurrentLocation = FollowCamera->GetRelativeLocation();
	if (FVector::Dist(CurrentLocation, Goal) < 1.f)
	{
		FollowCamera->SetRelativeLocation(Goal);
		// 因为我们的计时器是下一帧执行，当摄像机偏移到目标位置后，我们就直接return，不再设置下一帧的计时器
		return;
	}

	float LerpAlpha = FMath::Clamp(GetWorld()->GetDeltaSeconds() * CameraLerpSpeed, 0.f, 1.f);
	FVector NewLocation = FMath::Lerp(CurrentLocation, Goal, LerpAlpha);
	FollowCamera->SetRelativeLocation(NewLocation);

	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AArenasPlayerCharacter::TickCameraLerp, Goal));
	
}



	



