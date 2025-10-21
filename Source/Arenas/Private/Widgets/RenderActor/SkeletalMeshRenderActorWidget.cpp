// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "SkeletalMeshRenderActorWidget.h"

#include "SkeletalMeshRenderActor.h"
#include "Character/ArenasCharacter.h"

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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SetMeshAnimInstance called in SkeletalMeshRenderActorWidget"));
		SkeletalMeshRenderActor->ConfigureSkeletalMeshComponent(ArenasCharacter->GetMesh()->GetSkeletalMeshAsset(), ArenasCharacter->GetMesh()->GetAnimClass());
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
