// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "UArenaGA_AIShoot.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "GameplayTagsManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GAS/Actor/ProjectileActor.h"
#include "Interface/ArenasCombatInterface.h"

UUArenaGA_AIShoot::UUArenaGA_AIShoot()
{
	FGameplayTagContainer TempAbilityTags;
	TempAbilityTags.AddTag(ArenasGameplayTags::Ability_BasicAttack);
	SetAssetTags(TempAbilityTags);
	BlockAbilitiesWithTag.AddTag(ArenasGameplayTags::Ability_BasicAttack);
}

FGameplayTag UUArenaGA_AIShoot::GetComboChangeEventTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Event.Ability.Combo.Change"));
}

void UUArenaGA_AIShoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	// 等价于
	if (!K2_CommitAbility())
	{
		// EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		// 等价于
		K2_EndAbility();
		return;
	}

	// 在多人游戏中，我们需要确保蒙太奇在服务器上播放，然后再通过复制将其同步到客户端
	// HasAuthorityOrPredictionKey 会检查我们是否在服务器上，或者我们是否拥有预测密钥（客户端预测即本地客户端立即执行）
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayComboMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage, 1.f, NAME_None, false);
		// 订阅蒙太奇淡出混合事件
		PlayComboMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UUArenaGA_AIShoot::K2_EndAbility);
		// 订阅蒙太奇完成事件
		PlayComboMontageAndWaitTask->OnCompleted.AddDynamic(this, &UUArenaGA_AIShoot::K2_EndAbility);
		// 订阅蒙太奇中断事件
		PlayComboMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UUArenaGA_AIShoot::K2_EndAbility);
		// 订阅蒙太奇取消事件
		PlayComboMontageAndWaitTask->OnCancelled.AddDynamic(this, &UUArenaGA_AIShoot::K2_EndAbility);
		
		PlayComboMontageAndWaitTask->ReadyForActivation();	// 准备好被激活（实际并未被激活）

		// 等待动画通知事件（AnimNotify Event）
		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			GetComboChangeEventTag(),
			nullptr,
			false,	// 因为连招会被多次触发，所以这里不需要只触发一次
			false);	// 同时，我们这里监听的是Combo.Change，它下面有多个具体的连招段Tag，所以这里不需要只匹配精确标签

		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &UUArenaGA_AIShoot::OnComboChangeEventReceived);
		WaitComboChangeEventTask->ReadyForActivation();
	}

	// 伤害事件
	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitComboDamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			GetDamageEventTag(),
			nullptr,
			false,
			false);

		WaitComboDamageEventTask->EventReceived.AddDynamic(this, &UUArenaGA_AIShoot::ShootProjectile);
		WaitComboDamageEventTask->ReadyForActivation();
	}
	

	SetupWaitComboInputPressTask();
}

void UUArenaGA_AIShoot::SetupWaitComboInputPressTask()
{
	UAbilityTask_WaitInputPress* WaitComboInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitComboInputPressTask->OnPress.AddDynamic(this, &UUArenaGA_AIShoot::OnComboInputPressed);
	WaitComboInputPressTask->ReadyForActivation();
}

void UUArenaGA_AIShoot::TryCommitCombo()
{
	if (NextComboName == NAME_None)
	{
		return;
	}

	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
		
	}
}

int32 UUArenaGA_AIShoot::GetCurrentComboIndex() const
{
	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage);
		return FCString::Atoi(*CurrentSectionName.ToString());
	}
	return 0; // 默认返回0，表示无效索引
}

void UUArenaGA_AIShoot::OnComboChangeEventReceived(FGameplayEventData PayloadData)
{
	FGameplayTag EventTag = PayloadData.EventTag;

	if (EventTag == ArenasGameplayTags::Event_Ability_Combo_Change_End)
	{
		NextComboName = NAME_None;
		return;
	}

	NextComboName = UArenasBlueprintFunctionLibrary::NativeGetGameplayTagLastName(EventTag);
}

void UUArenaGA_AIShoot::OnComboInputPressed(float TimeWaited)
{
	SetupWaitComboInputPressTask();
	TryCommitCombo();
}

FGameplayTag UUArenaGA_AIShoot::GetDamageEventTag() const
{
	return FGameplayTag::RequestGameplayTag(FName("Event.Ability.Shoot"));
}

void UUArenaGA_AIShoot::ShootProjectile(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = OwnerAvatarActor;
		SpawnParameters.Instigator = Cast<APawn>(OwnerAvatarActor);
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// OwnerAvatarActor是否实现了IArenasCombatInterface接口
		if (IArenasCombatInterface* CombatInterface = Cast<IArenasCombatInterface>(OwnerAvatarActor))
		{
			AActor* TargetActor = CombatInterface->GetCurrentTargetActor();

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

				FVector Loc;
				FRotator Rot;
				OwnerAvatarActor->GetActorEyesViewPoint(Loc, Rot);
				FVector ShootTargetEndLoc = Loc + Rot.Vector() * ShootProjectileRange;
				FVector InitialProjectileDirection = (ShootTargetEndLoc - SocketLocation).GetSafeNormal();

				SpawnedProjectile->ShootProjectile(ProjectileSpeed, ShootProjectileRange, InitialProjectileDirection, TargetActor, GetOwnerTeamId(), HitEffectSpecHandle);
			}
		}
		
	}
}

FGenericTeamId UUArenaGA_AIShoot::GetOwnerTeamId() const
{
	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetOwningActorFromActorInfo()))
	{
		return TeamAgent->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}
