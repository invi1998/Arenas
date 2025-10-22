// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "SkeletalMeshRenderActorWidget.h"

#include "SkeletalMeshRenderActor.h"
#include "Character/ArenasCharacter.h"
#include "Components/SceneCaptureComponent2D.h"

void USkeletalMeshRenderActorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AArenasCharacter* ArenasCharacter = GetOwningPlayerPawn<AArenasCharacter>())
	{
		ArenasCharacter->OnAnimInstanceReady.AddUObject(this, &USkeletalMeshRenderActorWidget::SetMeshAnimInstance);
	}
	
}

void USkeletalMeshRenderActorWidget::SetMeshAnimInstance()
{
	AArenasCharacter* ArenasCharacter = GetOwningPlayerPawn<AArenasCharacter>();
	
	if (ArenasCharacter && SkeletalMeshRenderActor)
	{
		SkeletalMeshRenderActor->ConfigureSkeletalMeshComponent(ArenasCharacter->GetMesh()->GetSkeletalMeshAsset(), ArenasCharacter->GetMesh()->GetAnimClass());

		if (IRenderActorTargetInterface* TargetInterface = Cast<IRenderActorTargetInterface>(ArenasCharacter))
		{
			if (USceneCaptureComponent2D* SceneCaptureComp = SkeletalMeshRenderActor->GetSceneCaptureComponent())
			{
				SceneCaptureComp->SetRelativeLocation(TargetInterface->GetCaptureTargetLocalPosition());
				SceneCaptureComp->SetRelativeRotation(TargetInterface->GetCaptureTargetLocalRotation());
			}
		}
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
