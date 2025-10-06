// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RenderActor.generated.h"

/**
 * 该类用于在UI中渲染3D模型，或者其他不会被渲染到场景中的物体
 */

class USceneComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;

UCLASS()
class ARENAS_API ARenderActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARenderActor();

	void SetRenderTarget(UTextureRenderTarget2D* NewRenderTarget);
	void UpdateRender();
	USceneCaptureComponent2D* GetSceneCaptureComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Render Actor")
	USceneComponent* RootSceneComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Render Actor")
	USceneCaptureComponent2D* SceneCaptureComponent;
	
};
