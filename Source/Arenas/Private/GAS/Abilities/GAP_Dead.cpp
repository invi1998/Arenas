// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "GAP_Dead.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "Engine/OverlapResult.h"

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
		if (AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser())
		{
			UE_LOG(LogTemp, Warning, TEXT("****** UGAP_Dead::ActivateAbility Killer: %s"), *Killer->GetName());
		}
		TArray<AActor*> RewardTargets = GetRewardTargets();
		for (AActor* RewardTarget : RewardTargets)
		{
			UE_LOG(LogTemp, Warning, TEXT("***-------*** UGAP_Dead::ActivateAbility RewardTarget: %s"), *RewardTarget->GetName());
		}
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
