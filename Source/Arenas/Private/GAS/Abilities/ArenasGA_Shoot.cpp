// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_Shoot.h"

#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Actor/ProjectileActor.h"
// #include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"

UArenasGA_Shoot::UArenasGA_Shoot()
{
	ActivationOwnedTags.AddTag(ArenasGameplayTags::Status_Aiming);
}

void UArenasGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	// 射击技能，当按住右键时，激活该技能，松开右键时，结束该技能
	// 技能激活期间，左键可以发射子弹
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 服务器端逻辑
		UAbilityTask_WaitGameplayEvent* WaitStartShootEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ArenasGameplayTags::Ability_BasicAttack_Pressed);
		WaitStartShootEventTask->EventReceived.AddDynamic(this, &UArenasGA_Shoot::OnStartShoot);
		WaitStartShootEventTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitStopShootEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ArenasGameplayTags::Ability_BasicAttack_Released);
		WaitStopShootEventTask->EventReceived.AddDynamic(this, &UArenasGA_Shoot::OnStopShoot);
		WaitStopShootEventTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetShootEventTag(), nullptr, false, false);
		WaitShootProjectileEventTask->EventReceived.AddDynamic(this, &UArenasGA_Shoot::ShootProjectile);
		WaitShootProjectileEventTask->ReadyForActivation();
		
	}
	
}

void UArenasGA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot Ability Input Released"));
	K2_EndAbility();
}

void UArenasGA_Shoot::OnStartShoot(FGameplayEventData Payload)
{
	// 此处射击动画我们不在客户端做预测，只在服务器端做蒙太奇事件，然后客户端只用于单纯的动画播放
	if (HasAuthority(&CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayShootMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
		PlayShootMontageTask->ReadyForActivation();
	}
	else
	{
		// 客户端逻辑，可以播放射击动画
		PlayMontageLocally(ShootMontage);
	}

	// 当然，如果要是非要做客户端预测，可以通过AbilityTask_NetworkSyncPoint来做（添加同步）
	// UAbilityTask_NetworkSyncPoint* NetworkSyncPointTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyServerWait);
	// NetworkSyncPointTask->ReadyForActivation();
}

void UArenasGA_Shoot::OnStopShoot(FGameplayEventData Payload)
{
	if (ShootMontage)
	{
		StopMontageAffterCurrentSection(ShootMontage);
	}
}

void UArenasGA_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = OwnerAvatarActor;
		SpawnParameters.Instigator = Cast<APawn>(OwnerAvatarActor);
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
		if (USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo())
		{
			TArray<FName> OutNames;
			UGameplayTagsManager::Get().SplitGameplayTagFName(Payload.EventTag, OutNames);
			if (OutNames.Num() > 0)
			{
				FName SocketName = OutNames.Last();
				SocketLocation = MeshComp->GetSocketLocation(SocketName);
			}
		}
		if (AProjectileActor* SpawnedProjectile = GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, SocketLocation, OwnerAvatarActor->GetActorRotation(), SpawnParameters))
		{
			FGameplayEffectSpecHandle HitEffectSpecHandle = MakeOutgoingGameplayEffectSpec(ProjectileHitEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			
			SpawnedProjectile->ShootProjectile(ProjectileSpeed, ShootProjectileRange, nullptr, GetOwnerTeamId(), HitEffectSpecHandle);
		}
		
	}
}

FGameplayTag UArenasGA_Shoot::GetShootEventTag() const
{
	return FGameplayTag::RequestGameplayTag(FName("Event.Ability.Shoot"));
}

FGenericTeamId UArenasGA_Shoot::GetOwnerTeamId() const
{
	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetOwningActorFromActorInfo()))
	{
		return TeamAgent->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}
