// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "SkeletalMeshRenderActor.h"


ASkeletalMeshRenderActor::ASkeletalMeshRenderActor()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(GetRootComponent());
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetLightingChannels(false, true, false); // 只接受第二个光照通道的光照
	
}

void ASkeletalMeshRenderActor::ConfigureSkeletalMeshComponent(USkeletalMesh* InSkeletalMesh, TSubclassOf<UAnimationAsset> AnimBlueprintClass)
{
	MeshComp->SetSkeletalMesh(InSkeletalMesh);
	if (AnimBlueprintClass)
	{
		MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		MeshComp->SetAnimInstanceClass(AnimBlueprintClass);
	}
	else
	{
		MeshComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	}
}

void ASkeletalMeshRenderActor::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetVisibleInSceneCaptureOnly(true);	// 只在场景捕捉中可见
	
	
}
