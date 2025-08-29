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

TArray<FHitResult> UArenasGameplayAbility::GetHitResultsFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetData, float SphereSweepRadius, bool bDebug, bool bIgnoreSelf) const
{
	TArray<FHitResult> HitResults;

	for (const TSharedPtr<FGameplayAbilityTargetData> TargetDataPtr : TargetData.Data)
	{
		FVector StartLoc = TargetDataPtr->GetOrigin().GetTranslation();
		FVector EndLoc = TargetDataPtr->GetEndPoint();
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
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
	}

	return HitResults;
}
