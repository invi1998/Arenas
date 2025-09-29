// Ace of Arenas. (invi1998 All Rights Reserved)


#include "AN_SendTargetGroup.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Kismet/KismetSystemLibrary.h"

void UAN_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if (TargetSocketNames.Num() <= 1) return;

	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;

	FGameplayEventData PayloadData;
	TSet<AActor*> HitActors;
	AActor* OwnerActor = MeshComp->GetOwner();
	const IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(OwnerActor);

	TArray<AActor*> IgnoredActors;
	if (bIgnoreSelf)
	{
		IgnoredActors.Add(OwnerActor);
	}
	for (int32 i = 1; i < TargetSocketNames.Num(); ++i)
	{
		FVector StartLocation = MeshComp->GetSocketLocation(TargetSocketNames[i-1]);
		FVector EndLocation = MeshComp->GetSocketLocation(TargetSocketNames[i]);
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<FHitResult> Hits;

		UKismetSystemLibrary::SphereTraceMultiForObjects(
			MeshComp,
			StartLocation,
			EndLocation,
			SphereSweepRadius,
			ObjectTypes,
			false,
			IgnoredActors,
			bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			Hits,
			false
		);

		for (const FHitResult& Hit : Hits)
		{
			if (HitActors.Contains(Hit.GetActor()))
			{
				continue;
			}
			if (OwnerTeamAgent)
			{
				ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamAgent->GetTeamAttitudeTowards(*Hit.GetActor());
				if (OtherActorTeamAttitude != TargetTeamType)
				{
					// 如果目标不符合指定的队伍关系，则跳过
					continue;
				}
			}

			HitActors.Add(Hit.GetActor());
			PayloadData.TargetData.Add(new FGameplayAbilityTargetData_SingleTargetHit(Hit));

			SendLocalGameplayCue(Hit);
		}
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, PayloadData);
}

void UAN_SendTargetGroup::SendLocalGameplayCue(const FHitResult& HitResult) const
{
	// 只在本地客户端发送GameplayCue，不需要服务器和其他客户端接收
	FGameplayCueParameters Parameters;
	Parameters.Location = HitResult.ImpactPoint;
	Parameters.Normal = HitResult.ImpactNormal;

	for (const FGameplayTag& CueTag : TriggerGameplayCueTags)
	{
		// 通过GameplayCueManager来处理GameplayCue
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
			HitResult.GetActor(),
			CueTag,
			EGameplayCueEvent::Executed,
			Parameters
		);
	}
}
