// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
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
	
}

void AProjectileActor::Destroyed()
{
	
	Super::Destroyed();
}

void AProjectileActor::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	TeamId = InTeamID;
}

FGenericTeamId AProjectileActor::GetGenericTeamId() const
{
	return IGenericTeamAgentInterface::GetGenericTeamId();
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
		UE_LOG(LogTemp, Warning, TEXT("ProjectileActor overlapped with %s, Is Server = %s"), *OtherActor->GetName(), HasAuthority() ? TEXT("True") : TEXT("False"));
		// 仅在服务端处理碰撞逻辑
		// 检查阵营态度
		if (GetTeamAttitudeTowards(*OtherActor) == ProjectileTeamAttitudeType)
		{
			// 应用效果
			UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
			if (IsValid(OtherASC))
			{
				if (HasAuthority() && HitEffectSpecHandle.IsValid())
				{
					OtherASC->ApplyGameplayEffectSpecToSelf(*HitEffectSpecHandle.Data.Get());
					GetWorldTimerManager().ClearTimer(ProjectileTravelTimerHandle); // 清除最大飞行距离计时器
				}
			}

			// 发送本地游戏提示
			FHitResult HitResult;
			HitResult.ImpactNormal = GetActorForwardVector();
			HitResult.ImpactPoint = GetActorLocation();
			SendLocalGameplayCue(OtherActor, HitResult);
			
			Destroyed();
			
		}
	}
}

void AProjectileActor::ShootProjectile(float InSpeed, float InMaxDistance, FVector InitialMoveDir, const AActor* InTargetActor, FGenericTeamId InInstigatorTeamID, FGameplayEffectSpecHandle InHitEffectSpecHandle, ETeamAttitude::Type InTeamAttitudeType)
{
	TargetActor = InTargetActor;
	ProjectileSpeed = InSpeed;
	ProjectileTeamAttitudeType = InTeamAttitudeType;
	MoveDirection = InitialMoveDir.GetSafeNormal();

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

void AProjectileActor::SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult)
{
	FGameplayCueParameters CueParameters;
	CueParameters.Location = HitResult.ImpactPoint;
	CueParameters.Normal = HitResult.ImpactNormal;
	
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(CueTargetActor, HitGameplayCueTag, EGameplayCueEvent::Executed, CueParameters);
	
}




