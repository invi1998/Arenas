// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "CharacterDisplay.h"

#include "Camera/CameraComponent.h"
#include "Character/PA_CharacterDefinition.h"


// Sets default values
ACharacterDisplay::ACharacterDisplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("RootComponent")));

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	ViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(FName("ViewCameraComponent"));
	ViewCameraComponent->SetupAttachment(GetRootComponent());

	
}

void ACharacterDisplay::ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* InCharacterDefinition)
{
	if (!InCharacterDefinition) return;

	SkeletalMeshComponent->SetSkeletalMeshAsset(InCharacterDefinition->LoadDisplaySkeletalMesh());
	SkeletalMeshComponent->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
	SkeletalMeshComponent->SetAnimInstanceClass(InCharacterDefinition->LoadCharacterDisplayAnimBP());
	
}





