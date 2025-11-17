// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MainMenuPlayerController.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

AMainMenuPlayerController::AMainMenuPlayerController()
{
	// bAutoManageActiveCameraTarget = true;		// 禁用自动管理活动摄像机目标，因为在主菜单界面，我们希望手动控制摄像机视角
}

void AMainMenuPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName("Default"), FoundCameras);
	if (!FoundCameras.IsEmpty())
	{
		SetViewTarget(FoundCameras[0]);
	}
	else
	{
		// 如果没有找到带有"Default"标签的摄像机，则使用当前Pawn作为视角目标
		SetViewTarget(InPawn);
	}
	
}
