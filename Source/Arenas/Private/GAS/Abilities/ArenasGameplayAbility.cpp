// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasGameplayAbility.h"

#include "Kismet/KismetSystemLibrary.h"

UAnimInstance* UArenasGameplayAbility::GetOwnerAnimInstance() const
{
	if (const USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo())
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}

	return nullptr;
}

TArray<FHitResult> UArenasGameplayAbility::GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetData, ETeamAttitude::Type TargetTeamType, float SphereSweepRadius, bool bDebug, bool bIgnoreSelf) const
{
	TArray<FHitResult> OutHitResults;
	TSet<AActor*> HitActors;

	IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

	for (const TSharedPtr<FGameplayAbilityTargetData> TargetDataPtr : TargetData.Data)
	{
		FVector StartLoc = TargetDataPtr->GetOrigin().GetTranslation();
		FVector EndLoc = TargetDataPtr->GetEndPoint();
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<FHitResult> HitResults;
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			this,
			StartLoc,
			EndLoc,
			SphereSweepRadius,
			ObjectTypes,
			false,
			bIgnoreSelf ? TArray<AActor*>{GetOwningActorFromActorInfo()} : TArray<AActor*>(),
			bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			HitResults,
			false,
			FLinearColor::Red,
			FLinearColor::Green, 2.f
		);

		// 去除重复的命中结果
		for (const FHitResult& Hit : HitResults)
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
			OutHitResults.Add(Hit);
		}
		
	}

	return OutHitResults;
}
