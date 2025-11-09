// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "CharacterDisplay.h"

#include "CineCameraComponent.h"
#include "Character/PA_CharacterDefinition.h"


// Sets default values
ACharacterDisplay::ACharacterDisplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("RootComponent")));

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	ViewCameraComponent = CreateDefaultSubobject<UCineCameraComponent>(FName("ViewCameraComponent"));
	ViewCameraComponent->SetupAttachment(GetRootComponent());
	// 摄像机不裁剪画面
	ViewCameraComponent->bUseFieldOfViewForLOD = false;
	// 设置手动对焦距离
	ViewCameraComponent->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;	// 手动对焦
	
}

void ACharacterDisplay::ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* InCharacterDefinition)
{
	if (!InCharacterDefinition) return;

	SkeletalMeshComponent->SetSkeletalMeshAsset(InCharacterDefinition->LoadDisplaySkeletalMesh());
	SkeletalMeshComponent->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
	SkeletalMeshComponent->SetAnimInstanceClass(InCharacterDefinition->LoadCharacterDisplayAnimBP());

	// 如果摄像机组件是CineCameraComponent类，则需要调整焦距以适应不同的角色模型大小
	ViewCameraComponent->SetWorldLocation(InCharacterDefinition->GetCineCameraLocation());
	ViewCameraComponent->SetWorldRotation(InCharacterDefinition->GetCineCameraRotation());
	
	ViewCameraComponent->FocusSettings.ManualFocusDistance = InCharacterDefinition->GetCineCameraManualFocusDistance();
	
	
	
}





