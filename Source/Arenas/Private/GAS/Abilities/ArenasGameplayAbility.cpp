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

void UArenasGameplayAbility::PlayMontageLocally(UAnimMontage* MontageToPlay)
{
	UAnimInstance* OwningAnimInstance = GetOwnerAnimInstance();
	if (OwningAnimInstance && MontageToPlay && !OwningAnimInstance->Montage_IsPlaying(MontageToPlay))
	{
		OwningAnimInstance->Montage_Play(MontageToPlay);
	}
}

void UArenasGameplayAbility::StopMontageAfterCurrentSection(UAnimMontage* MontageToStop)
{
	UAnimInstance* OwningAnimInstance = GetOwnerAnimInstance();
	if (OwningAnimInstance && MontageToStop)
	{
		FName CurrentSectionName = OwningAnimInstance->Montage_GetCurrentSection(MontageToStop);
		// 设置下一段为无效段，动画将在当前段播放完后停止
		OwningAnimInstance->Montage_SetNextSection(CurrentSectionName, NAME_None, MontageToStop);
	}
}

AArenasCharacter* UArenasGameplayAbility::GetOwningArenasCharacter()
{
	if (!OwningArenasCharacter)
	{
		OwningArenasCharacter = Cast<AArenasCharacter>(GetAvatarActorFromActorInfo());
	}

	return OwningArenasCharacter;
}

AActor* UArenasGameplayAbility::GetAimActorTarget(float AimDistance, ETeamAttitude::Type AimTeamAttitude) const
{
	if (AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo())
	{
		FVector Loc;
		FRotator Rot;
		OwnerAvatarActor->GetActorEyesViewPoint(Loc, Rot);

		FVector AimEnd = Loc + Rot.Vector() * AimDistance;
		// TArray<AActor*> Targets;
		TArray<FHitResult> HitResults;

		if (bShowDebugLine)
		{
			DrawDebugLine(GetWorld(), Loc, AimEnd, FColor::Green, false, 2.f, 0, 2.f);
		}
		
		if (GetWorld()->LineTraceMultiByObjectType(
			HitResults,
			Loc,
			AimEnd,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
			FCollisionQueryParams(NAME_None, false, OwnerAvatarActor)
		))
		{
			for (const FHitResult& Hit : HitResults)
			{
				AActor* HitActor = Hit.GetActor();
				if (HitActor && IsActorTeamAttitude(HitActor, AimTeamAttitude))
				{
					return HitActor;
				}
			}
		}
	}

	return nullptr;
}

bool UArenasGameplayAbility::IsActorTeamAttitude(AActor* OtherActor, ETeamAttitude::Type DesiredAttitude) const
{
	if (!OtherActor) return false;
	if (AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo())
	{
		if (IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(OwnerAvatarActor))
		{
			return OwnerTeamAgent->GetTeamAttitudeTowards(*OtherActor) == DesiredAttitude;
		}
	}
	return false;
}

void UArenasGameplayAbility::SendLocalGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->HandleGameplayEvent(EventTag, &Payload);
	}
}

FGenericTeamId UArenasGameplayAbility::GetOwningTeamId() const
{
	if (IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo()))
	{
		return OwnerTeamAgent->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

