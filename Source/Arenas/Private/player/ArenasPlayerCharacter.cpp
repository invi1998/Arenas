// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasPlayerCharacter.h"

#include "Camera/CameraComponent.h"
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
	



