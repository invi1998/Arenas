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
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());

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

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &AArenasPlayerCharacter::Jump);
	}
	
}
	



