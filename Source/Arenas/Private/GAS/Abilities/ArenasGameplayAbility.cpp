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

AArenasCharacter* UArenasGameplayAbility::GetOwningArenasCharacter()
{
	if (!OwningArenasCharacter)
	{
		OwningArenasCharacter = Cast<AArenasCharacter>(GetAvatarActorFromActorInfo());
	}

	return OwningArenasCharacter;
}

