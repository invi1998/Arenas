// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasPlayerCharacter.h"

#include "ArenasPlayerController.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AArenasPlayerCharacter::AArenasPlayerCharacter()
{
	bUseControllerRotationYaw = false;		// 让角色不跟随控制器的旋转
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true; // 让弹簧臂组件使用控制器的旋转

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// 这里我需要指定插槽，如果不这样做，相机实际会附着在相机臂的根部，而不是末端插槽上，我们需要指定该插槽，以便使用弹簧臂组件的插槽名
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	
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
	}
	
}

void AArenasPlayerCharacter::HandleLookInput(const FInputActionValue& Value)
{
	FVector2D LookDirection = Value.Get<FVector2D>();

	AddControllerPitchInput(LookDirection.Y);
	AddControllerYawInput(LookDirection.X);
}
	



