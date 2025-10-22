// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "RenderActor.h"
#include "SkeletalMeshRenderActor.generated.h"

class USkeletalMeshComponent;

UCLASS()
class ARENAS_API ASkeletalMeshRenderActor : public ARenderActor
{
	GENERATED_BODY()

public:
	ASkeletalMeshRenderActor();
	void ConfigureSkeletalMeshComponent(USkeletalMesh* InSkeletalMesh, TSubclassOf<UAnimInstance> AnimBlueprintClass);

private:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* MeshComp;
	

};
