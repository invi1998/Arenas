// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "RenderActor.h"

#include "Components/SceneCaptureComponent2D.h"


// Sets default values
ARenderActor::ARenderActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(RootSceneComponent);

	SceneCaptureComponent->bCaptureEveryFrame = false;	// 不需要每帧捕获
	SceneCaptureComponent->FOVAngle = 30.f;	// 设置视野角度
	
	
}

void ARenderActor::SetRenderTarget(UTextureRenderTarget2D* NewRenderTarget)
{
	SceneCaptureComponent->TextureTarget = NewRenderTarget;
}

void ARenderActor::UpdateRender()
{
	if (IsValid(SceneCaptureComponent))
	{
		SceneCaptureComponent->CaptureScene();
	}
}

USceneCaptureComponent2D* ARenderActor::GetSceneCaptureComponent()
{
	return SceneCaptureComponent;
}

// Called when the game starts or when spawned
void ARenderActor::BeginPlay()
{
	Super::BeginPlay();

	// 只显示该Actor的组件，环境中其他物体都不显示
	SceneCaptureComponent->ShowOnlyActorComponents(this);
	SetActorLocation(FVector(0, 100000.f, 0));	// 将渲染Actor放置在远离场景的位置
}


