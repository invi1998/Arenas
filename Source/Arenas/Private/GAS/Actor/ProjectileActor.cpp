// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "GameplayCueManager.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AProjectileActor::AProjectileActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HasAuthority())
	{
		if (TargetActor)
		{
			// 如果有目标Actor，则朝向目标Actor移动，每帧更新移动方向
			MoveDirection = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		}
	}
		
	SetActorLocation(GetActorLocation() + MoveDirection * ProjectileSpeed * DeltaTime);
	// 如果只设置弹体位置，会出现子弹移动很死板的问题，所以同时还需要修正弹体的朝向
	SetActorRotation(MoveDirection.Rotation());
	
	
}

void AProjectileActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectileActor, TeamId);
	DOREPLIFETIME(AProjectileActor, ProjectileSpeed);
	DOREPLIFETIME(AProjectileActor, MoveDirection);
	DOREPLIFETIME(AProjectileActor, TeamAttitudeByte);
	
}

void AProjectileActor::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	TeamId = InTeamID;
}

FGenericTeamId AProjectileActor::GetGenericTeamId() const
{
	return TeamId;
}

ETeamAttitude::Type AProjectileActor::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(&Other))
	{
		FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();
		FGenericTeamId MyTeamID = GetGenericTeamId();
		
		if (MyTeamID == OtherTeamID) 
			return ETeamAttitude::Friendly;
		else if (MyTeamID == FGenericTeamId::NoTeam || OtherTeamID == FGenericTeamId::NoTeam)
			return ETeamAttitude::Neutral;
		else
			return ETeamAttitude::Hostile;
	}
	return ETeamAttitude::Neutral;
}

void AProjectileActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (bOnlyOverlapTargetActor)
	{
		// 仅与目标Actor发生碰撞
		if (OtherActor != TargetActor)
		{
			return;
		}
	}
	
	// Super::NotifyActorBeginOverlap(OtherActor);
	if (OtherActor && OtherActor != GetOwner())
	{
		// 仅在服务端处理碰撞逻辑
		// 检查阵营态度
		
		if (GetTeamAttitudeTowards(*OtherActor) == ProjectileTeamAttitudeType)
		{
			// 发送游戏提示
			// 	ENGINE_API FHitResult(AActor* InActor, UPrimitiveComponent* InComponent, FVector const& HitLoc, FVector const& HitNorm);
			FHitResult HitResult(OtherActor, nullptr, GetActorLocation(), FVector::UpVector);
			HitResult.ImpactNormal = GetActorForwardVector();
			HitResult.ImpactPoint = GetActorLocation();
			// 应用效果
			UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
			if (IsValid(OtherASC))
			{
				if (HasAuthority() && HitEffectSpecHandle.IsValid())
				{
					// 为HitEffectSpecHandle添加上下文信息
					HitEffectSpecHandle.Data->GetContext().AddHitResult(HitResult, true);
					
					OtherASC->ApplyGameplayEffectSpecToSelf(*HitEffectSpecHandle.Data.Get());
					GetWorldTimerManager().ClearTimer(ProjectileTravelTimerHandle); // 清除最大飞行距离计时器
				}
			}
			
			// 判断当前弹丸所属的Actor是否是AI角色
			if (AActor* OwnerActor = GetOwner())
			{
				if (UArenasBlueprintFunctionLibrary::IsAIActor(OwnerActor))
				{
					// 如果是AI角色发射的弹丸，则使用GAS的方式触发游戏提示
					TriggerGameplayCue_GAS(OtherActor, HitResult);
				}
				else
				{
					// 否则使用本地方式触发游戏提示
					SendLocalGameplayCue(OtherActor, HitResult);
				}
			}
			
			Destroy();
			
		}
		
	}
}

void AProjectileActor::ShootProjectile(float InSpeed, float InMaxDistance, FVector InitialMoveDir, const AActor* InTargetActor, FGenericTeamId InInstigatorTeamID, FGameplayEffectSpecHandle InHitEffectSpecHandle, ETeamAttitude::Type InTeamAttitudeType)
{

	TargetActor = InTargetActor;
	ProjectileSpeed = InSpeed;
	ProjectileTeamAttitudeType = InTeamAttitudeType;
	MoveDirection = InitialMoveDir.GetSafeNormal();
	TeamAttitudeByte = static_cast<uint8>(InTeamAttitudeType);

	FRotator OwnerRotator = GetActorRotation();
	SetGenericTeamId(InInstigatorTeamID);

	if (GetOwner())
	{
		FVector OwnerViewLocation;
		GetOwner()->GetActorEyesViewPoint(OwnerViewLocation, OwnerRotator);	// 获取拥有者的视点位置
	}

	// MoveDirection = OwnerRotator.Vector();
	HitEffectSpecHandle = InHitEffectSpecHandle;

	float TravelMaxTime = InMaxDistance / ProjectileSpeed;
	GetWorldTimerManager().ClearTimer(ProjectileTravelTimerHandle);
	GetWorldTimerManager().SetTimer(
		ProjectileTravelTimerHandle,
		this,
		&AProjectileActor::TravelMaxDistanceReached,
		TravelMaxTime,
		false);
	
}

void AProjectileActor::TravelMaxDistanceReached()
{
	Destroy();
}


// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileActor::OnRep_TeamAttitude()
{
	if (!HasAuthority())
	{
		// 因为ProjectileTeamAttitudeType这个属性变量只存在服务端，所以这里需要将字节值转换回枚举类型
		ProjectileTeamAttitudeType = static_cast<ETeamAttitude::Type>(TeamAttitudeByte);
	}
	
}

void AProjectileActor::SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult)
{
	
	FGameplayCueParameters CueParameters;
	CueParameters.Location = HitResult.ImpactPoint;
	CueParameters.Normal = HitResult.ImpactNormal;
	
	// UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(CueTargetActor, HitGameplayCueTag, EGameplayCueEvent::Executed, CueParameters);
	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(CueTargetActor, HitGameplayCueTag, CueParameters);
}

void AProjectileActor::TriggerGameplayCue_GAS(AActor* HitActor, const FHitResult& HitResult)
{
	if (!HitActor) return;
	if (UArenasAbilitySystemComponent* ArenasASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(HitActor))
	{
		FGameplayCueParameters CueParameters;
		CueParameters.Location = HitResult.ImpactPoint;
		CueParameters.Normal = HitResult.ImpactNormal;

		ArenasASC->ExecuteGameplayCue(HitGameplayCueTag, CueParameters);
	}
}




