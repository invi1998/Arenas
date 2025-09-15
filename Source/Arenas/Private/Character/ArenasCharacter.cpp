// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenasCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasGameplayTags.h"
#include "Arenas/Arenas.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Widgets/ArenasUserWidget.h"


// Sets default values
AArenasCharacter::AArenasCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore); // 胶囊体忽略摄像机碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	
	ArenasAbilitySystemComponent = CreateDefaultSubobject<UArenasAbilitySystemComponent>(TEXT("ArenasAbilitySystemComponent"));
	// ArenasAbilitySystemComponent->SetIsReplicated(true);
	// ArenasAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ArenasAttributeSet = CreateDefaultSubobject<UArenasAttributeSet>(TEXT("ArenasAttributeSet"));

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComponent"));
	OverheadWidgetComponent->SetupAttachment(GetRootComponent());
	
	BindGASChangedDelegate();

	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSourceComponent"));
	
	PawnUIComponent = CreateDefaultSubobject<UPawnUIComponent>(TEXT("PawnUIComponent"));
}

void AArenasCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenasCharacter, TeamID);
}

void AArenasCharacter::ServerSideInit()
{
	ArenasAbilitySystemComponent->InitAbilityActorInfo(this, this);
	ArenasAbilitySystemComponent->ApplyInitialEffects();		// 初始化角色属性
	ArenasAbilitySystemComponent->GiveInitialAbilities();		// 赋予初始技能
}

void AArenasCharacter::ClientSideInit()
{
	ArenasAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool AArenasCharacter::IsLocallyControlledByPlayer() const
{
	// 检查控制器是否存在，并且是否是本地玩家控制器
	// 这通常用于判断当前角色是否由本地玩家控制
	// 在多人游戏中，可能会有AI控制的角色，这个函数可以用来区分
	// 例如，在客户端上，我们可以使用这个函数来判断当前角色
	// 是否是由本地玩家控制的角色，从而决定是否显示某些UI
	// 或者执行某些操作。
	return GetController() && GetController()->IsLocalPlayerController();
}

bool AArenasCharacter::IsAlive() const
{
	return GetAbilitySystemComponent() && !GetAbilitySystemComponent()->HasMatchingGameplayTag(ArenasGameplayTags::Status_Dead);
}

void AArenasCharacter::RespawnImmediately()
{
	if (HasAuthority())
	{
		if (GetAbilitySystemComponent())
		{
			FGameplayTagContainer TempTags;
			TempTags.AddTag(ArenasGameplayTags::Status_Dead);
			GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(TempTags);
		}
	}
}

bool AArenasCharacter::IsStunning() const
{
	return GetAbilitySystemComponent() && GetAbilitySystemComponent()->HasMatchingGameplayTag(ArenasGameplayTags::Status_Stun);
}

void AArenasCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController && !NewController->IsPlayerController())
	{
		// 如果控制器是 AI 控制器，我们可以在这里进行一些初始化操作
		ServerSideInit();
	}
}

UPawnUIComponent* AArenasCharacter::GetPawnUIComponent() const
{
	return PawnUIComponent;
}

FGenericTeamId AArenasCharacter::GetOwningGenericTeamId() const
{
	return TeamID;
}

const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& AArenasCharacter::GetAbilities() const
{
	return ArenasAbilitySystemComponent->GetAbilities();
}

void AArenasCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnOverheadWidgetComponent();
	MeshRelativeTransform = GetMesh()->GetRelativeTransform();

	// 注册视觉感知，建议在BeginPlay中注册，如果在构造函数中注册那么可能会因为组件还未初始化而失败
	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
	
}

void AArenasCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArenasCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* AArenasCharacter::GetAbilitySystemComponent() const
{
	return ArenasAbilitySystemComponent;
}

void AArenasCharacter::Server_SendGameplayEventToSelf_Implementation(const FGameplayTag& EventTag, const FGameplayEventData& Payload)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, Payload);
}

bool AArenasCharacter::Server_SendGameplayEventToSelf_Validate(const FGameplayTag& EventTag, const FGameplayEventData& Payload)
{
	return true;
}

void AArenasCharacter::DeathTagUpdated(FGameplayTag InGameplayTag, int32 NewCount)
{
	if (NewCount != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

void AArenasCharacter::StunTagUpdated(FGameplayTag GameplayTag, int32 NewCount)
{
	if (!IsAlive()) return;

	if (NewCount != 0)
	{
		// 被眩晕
		OnStun();
		PlayAnimMontage(StunMontage);
	}
	else
	{
		OnUnStun();
		StopAnimMontage(StunMontage);
	}
}

void AArenasCharacter::AimingTagUpdated(FGameplayTag GameplayTag, int32 NewCount)
{
	SetIsAiming(NewCount != 0);
}

void AArenasCharacter::SetIsAiming(bool bNewAiming)
{
	// 切换瞄准状态时，调整角色的旋转行为
	// 瞄准时角色面向控制器方向，非瞄准时角色面向移动方向
	bUseControllerRotationYaw = bNewAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bNewAiming;
	OnAimStateChanged(bNewAiming);
}

void AArenasCharacter::OnAimStateChanged(bool bNewAiming)
{
}

void AArenasCharacter::BindGASChangedDelegate()
{
	if (ArenasAbilitySystemComponent)
	{
		ArenasAbilitySystemComponent->RegisterGameplayTagEvent(
			ArenasGameplayTags::Status_Dead,
			EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AArenasCharacter::DeathTagUpdated);

		ArenasAbilitySystemComponent->RegisterGameplayTagEvent(
			ArenasGameplayTags::Status_Stun,
			EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AArenasCharacter::StunTagUpdated);

		ArenasAbilitySystemComponent->RegisterGameplayTagEvent(
			ArenasGameplayTags::Status_Aiming,
			EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AArenasCharacter::AimingTagUpdated);
		
	}
}

void AArenasCharacter::SpawnOverheadWidgetComponent()
{
	if (IsLocallyControlledByPlayer())
	{
		OverheadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
	if (OverheadWidgetComponent && OverheadWidgetClass)
	{
		OverheadWidgetComponent->SetWidgetClass(OverheadWidgetClass);
		OverheadWidgetComponent->SetDrawSize(FVector2D(200.0f, 50.0f));
		OverheadWidgetComponent->SetVisibility(true);
		OverheadWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverheadWidgetComponent->SetHiddenInGame(false);
		
		OverheadWidget = Cast<UArenasUserWidget>(OverheadWidgetComponent->GetUserWidgetObject());
		if (OverheadWidget)
		{
			OverheadWidget->InitOverheadWidget(this);
		}

		// 启动定时器定期更新头顶UI的可见性
		if (OverheadTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(OverheadTimerHandle);
		}
		GetWorld()->GetTimerManager().SetTimer(OverheadTimerHandle, this, &AArenasCharacter::UpdateOverheadWidgetVisibility, OverheadWidgetVisibilityCheckDelay, true);
	}
}

void AArenasCharacter::UpdateOverheadWidgetVisibility()
{
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		float DistanceSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverheadWidgetComponent->SetHiddenInGame(DistanceSquared > OverheadWidgetVisibilityRangeSquared);
	}
}

void AArenasCharacter::SetStatusGaugeEnabled(bool bEnabled)
{
	GetWorld()->GetTimerManager().ClearTimer(OverheadTimerHandle);
	if (bEnabled)
	{
		SpawnOverheadWidgetComponent();
	}
	else
	{
		OverheadWidgetComponent->SetHiddenInGame(true);
	}
}

void AArenasCharacter::StartDeathSequence()
{
	OnDeath();

	// 取消所有GAS
	if (ArenasAbilitySystemComponent)
	{
		ArenasAbilitySystemComponent->CancelAllAbilities();
	}
	
	PlayDeathAnimation();
	SetStatusGaugeEnabled(false);
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 停止AI感知
	SetAIPerceptionStimuliSourceEnabled(false);
	
}

void AArenasCharacter::Respawn()
{
	OnRespawn();
	
	SetAIPerceptionStimuliSourceEnabled(true);	// 重新启用AI感知

	SetRagdollPhysics(false);
	
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetStatusGaugeEnabled(true);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);

	if (HasAuthority() && GetController())
	{
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		if (StartSpot.IsValid())
		{
			// 如果控制器有出生点信息，则将角色传送到出生点位置
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}

	if (ArenasAbilitySystemComponent)
	{
		ArenasAbilitySystemComponent->ApplyFullStateEffect();
	}
	
}

void AArenasCharacter::DeathMontageFinished()
{
	if (!IsAlive())
	{
		// 死亡动画播放完毕后添加布娃娃物理效果
		SetRagdollPhysics(true);
	}
	
}

void AArenasCharacter::SetRagdollPhysics(bool bEnabled)
{
	if (bEnabled)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);	// 取消附加,保持世界变换
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); // 重新附加到胶囊体，保持相对变换
		GetMesh()->SetRelativeTransform(MeshRelativeTransform); // 重置相对变换
	}
}

void AArenasCharacter::PlayDeathAnimation()
{
	if (DeathMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		float MontageDuration = GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
		GetWorld()->GetTimerManager().ClearTimer(DeathMontageTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(DeathMontageTimerHandle, this, &AArenasCharacter::DeathMontageFinished, MontageDuration + DeathMontageFinishTimeOffset, false);
	}
}

void AArenasCharacter::OnDeath()
{
}

void AArenasCharacter::OnRespawn()
{
}

void AArenasCharacter::OnStun()
{
}

void AArenasCharacter::OnUnStun()
{
}

void AArenasCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	TeamID = InTeamID;
}

FGenericTeamId AArenasCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void AArenasCharacter::OnRep_TeamID()
{
	// 这里可以添加一些逻辑，当TeamID在客户端发生变化时触发
}

void AArenasCharacter::SetAIPerceptionStimuliSourceEnabled(bool bEnabled)
{
	if (PerceptionStimuliSourceComponent)
	{
		if (bEnabled)
		{
			PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();	// 注册到感知系统
		}
		else
		{
			PerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem(); // 从感知系统注销
			// 这里只是注销掉AI感知，但是并不意味着我们会立即丢失掉Target，因为AI感知系统会有一个记忆时间（默认是5秒），所以我们还需要在AI感知更新处更新我的Target
			// void AArenasAIController::OnTargetPerceptionUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
		}
	}
}

