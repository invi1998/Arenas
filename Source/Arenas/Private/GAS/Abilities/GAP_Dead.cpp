// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "GAP_Dead.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "Engine/OverlapResult.h"
#include "GAS/ArenasHeroAttributeSet.h"

UGAP_Dead::UGAP_Dead()
{

	ActivationBlockedTags.RemoveTag(ArenasGameplayTags::Status_Stun);
	ActivationBlockedTags.RemoveTag(ArenasGameplayTags::Status_Dead);
	
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;		// 仅在服务器上执行
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;		// 每个角色实例化
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = ArenasGameplayTags::Status_Dead;
	AbilityTriggers.Add(TriggerData);

}

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority())
	{
		// 经验等奖励公式，如果有击杀者，则将奖励的百分之50发给击杀者，然后将剩下的百分之50均分给附近的敌方英雄单位（包括击杀者）
		// 如果没有击杀者，则将奖励的100%均分给附近的敌方英雄单位
		// 奖励范围内的敌方英雄单位必须是存活状态
		// 奖励范围内的敌方英雄单位必须是英雄单位
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
		if (!Killer || !UArenasBlueprintFunctionLibrary::IsHeroActor(Killer))
		{
			Killer = nullptr;
		}
		TArray<AActor*> RewardTargets = GetRewardTargets();
		if (RewardTargets.Num() == 0 && !Killer)
		{
			K2_EndAbility();
			return;
		}
		if (Killer && !RewardTargets.Contains(Killer))
		{
			RewardTargets.Add(Killer);
		}

		bool bFound = false;
		float SelfOwnerExp = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetExperienceAttribute(), bFound);
		float TotalExpReward = BaseExperienceReward + ExperienceRewardExperience * SelfOwnerExp;
		float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfOwnerExp;

		// 对于奖励发放，此处我们通过GE来实现，而非直接修改角色属性集，因为在某些情况下，当我们尝试奖励击杀者时，击杀者可能已经死亡
		// 这种情况下，我们不应该发送任何奖励，GE可以很容易的阻止这种情况的发生

		if (Killer)
		{
			float KillerExpReward = TotalExpReward * KillerRewardPercent;
			float KillerGoldReward = TotalGoldReward * KillerRewardPercent;

			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffectClass, 1.f);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_Reward_Exp, KillerExpReward);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_Reward_Gold, KillerGoldReward);

			ApplyGameplayEffectSpecToTarget(
				GetCurrentAbilitySpecHandle(),
				GetCurrentActorInfo(),
				GetCurrentActivationInfo(),
				EffectSpecHandle,
				UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
		}
		
		float TargetExpReward = TotalExpReward * (1.f - KillerRewardPercent) / RewardTargets.Num();
		float TargetGoldReward = TotalGoldReward * (1.f - KillerRewardPercent) / RewardTargets.Num();

		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffectClass, 1.f);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_Reward_Exp, TargetExpReward);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_Reward_Gold, TargetGoldReward);

		K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets, true));

		K2_EndAbility();
	}
}

TArray<AActor*> UGAP_Dead::GetRewardTargets() const
{
	TSet<AActor*> RewardTargets;
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld())
	{
		return RewardTargets.Array();
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	if (GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		AvatarActor->GetActorLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
		FCollisionShape::MakeSphere(RewardRange),
		QueryParams
	))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AActor* OverlapActor = Overlap.GetActor())
			{
				if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OverlapActor))
				{
					if (TeamAgent->GetTeamAttitudeTowards(*AvatarActor) == ETeamAttitude::Hostile)
					{
						if (UArenasBlueprintFunctionLibrary::IsHeroActor(OverlapActor) && 
							UArenasBlueprintFunctionLibrary::IsAlive(OverlapActor))
						{
							// 只奖励存活的敌方英雄单位
							RewardTargets.Add(OverlapActor);
						}
					}
				}
			}
		}
	}

	return RewardTargets.Array();
	
}
