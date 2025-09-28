// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
#include "Character/ArenasCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

UArenasGameplayAbility::UArenasGameplayAbility()
{
	ActivationBlockedTags.AddTag(ArenasGameplayTags::Status_Stun); // 眩晕状态下无法激活该能力
	ActivationBlockedTags.AddTag(ArenasGameplayTags::Status_Dead); // 死亡状态下无法激活该能力
}

bool UArenasGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (!Spec || Spec->Level <= 0)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

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

void UArenasGameplayAbility::PushSelf(const FVector& PushVelocity)
{
	if (AArenasCharacter* ArenasCharacter = GetOwningArenasCharacter())
	{
		// 推开角色
		ArenasCharacter->LaunchCharacter(PushVelocity, true, true);
	}
}

void UArenasGameplayAbility::PushTarget(AActor* TargetActor, const FVector& PushVelocity)
{
	if (!TargetActor) return;

	FGameplayEventData EventData;

	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit();
	FHitResult HitResult;
	HitResult.ImpactNormal = PushVelocity;
	HitData->HitResult = HitResult;
	EventData.TargetData.Add(HitData);

	// 使用 GameplayEvent 来触发击飞被动技能
	EventData.EventTag = ArenasGameplayTags::Ability_Passive_Launch_Activated;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, EventData.EventTag, EventData);
	
}

void UArenasGameplayAbility::PushTargets(const TArray<AActor*>& TargetActors, const FVector& PushVelocity)
{
	for (AActor* TargetActor : TargetActors)
	{
		PushTarget(TargetActor, PushVelocity);
	}
}

void UArenasGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& PushVelocity)
{
	TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetData);
	PushTargets(TargetActors, PushVelocity);
}

AArenasCharacter* UArenasGameplayAbility::GetOwningArenasCharacter()
{
	if (!OwningArenasCharacter)
	{
		OwningArenasCharacter = Cast<AArenasCharacter>(GetAvatarActorFromActorInfo());
	}

	return OwningArenasCharacter;
}

