// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "SkeletalMeshRenderActorWidget.h"

#include "SkeletalMeshRenderActor.h"
#include "GameFramework/Character.h"

void USkeletalMeshRenderActorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* PlayerCharacter = GetOwningPlayerPawn<ACharacter>();
	
	if (PlayerCharacter && SkeletalMeshRenderActor)
	{
		SkeletalMeshRenderActor->ConfigureSkeletalMeshComponent(PlayerCharacter->GetMesh()->GetSkeletalMeshAsset(), PlayerCharacter->GetMesh()->GetAnimClass());
		USceneCaptureComponent2D* SceneCaptureComponent = SkeletalMeshRenderActor->GetSceneCaptureComponent();
		
	}
	
}

void USkeletalMeshRenderActorWidget::SpawnRenderActor()
{
	if (!SkeletalMeshRenderActorClass) return;
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SkeletalMeshRenderActor = World->SpawnActor<ASkeletalMeshRenderActor>(SkeletalMeshRenderActorClass, SpawnParameters);
		
	}
}

ARenderActor* USkeletalMeshRenderActorWidget::GetRenderActor() const
{
	return SkeletalMeshRenderActor;
}
