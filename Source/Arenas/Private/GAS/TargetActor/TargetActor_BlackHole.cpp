// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TargetActor_BlackHole.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"




// Sets default values
ATargetActor_BlackHole::ATargetActor_BlackHole()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	DetectionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphereComponent"));
	DetectionSphereComponent->SetupAttachment(GetRootComponent());
	DetectionSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 只与Pawn通道重叠检测
	DetectionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATargetActor_BlackHole::ActorInBlackHoleArea);
	DetectionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ATargetActor_BlackHole::ActorOutBlackHoleArea);

	VFXComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VFXComp"));
	VFXComp->SetupAttachment(GetRootComponent());

	bReplicates = true;
	ShouldProduceTargetDataOnServer = true; // 服务器端生成目标数据，而客户端只是进行模拟和显示
}

// Called every frame
void ATargetActor_BlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 吸引范围内的目标
	if (HasAuthority())
	{
		for (auto& Elem : ActorInRangeMap)
		{
			AActor* TargetActor = Elem.Key;
			UNiagaraComponent* TargetLinkVFXComp = Elem.Value;
			if (TargetActor)
			{
				FVector Direction = (GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal();
				FVector NewLocation = TargetActor->GetActorLocation() + Direction * PullSpeed * DeltaTime;
				TargetActor->SetActorLocation(NewLocation, true);
			}

			if (TargetLinkVFXComp)
			{
				// 更新特效参数，链接到黑洞中心位置
				TargetLinkVFXComp->SetVariablePosition(BlackHoleVFXOriginParamName, VFXComp->GetComponentLocation());
			}
		}
	}
}

void ATargetActor_BlackHole::ConfigureBlackHoleTargetActor(float InBlackHoleRadius, float InPullSpeed,
	float InBlackHoleDuration, const FGenericTeamId& OwningTeamId)
{
	BlackHoleRadius = InBlackHoleRadius;
	PullSpeed = InPullSpeed;
	SetGenericTeamId(OwningTeamId);
	BlackHoleDuration = InBlackHoleDuration;

	DetectionSphereComponent->SetSphereRadius(BlackHoleRadius);



}

void ATargetActor_BlackHole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ATargetActor_BlackHole, BlackHoleRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(ATargetActor_BlackHole, TeamId);
	
}

void ATargetActor_BlackHole::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	TeamId = TeamID;
}

FGenericTeamId ATargetActor_BlackHole::GetGenericTeamId() const
{
	return TeamId;
}

ETeamAttitude::Type ATargetActor_BlackHole::GetTeamAttitudeTowards(const AActor& Other) const
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

void ATargetActor_BlackHole::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(BlackHoleDurationTimerHandle, this, &ATargetActor_BlackHole::StopBlackHole, BlackHoleDuration, false);
	}
	
}

void ATargetActor_BlackHole::ConfirmTargetingAndContinue()
{
	StopBlackHole();
}

void ATargetActor_BlackHole::CancelTargeting()
{
	StopBlackHole();
	Super::CancelTargeting();
}

void ATargetActor_BlackHole::OnRep_BlackHoleRadius()
{
	DetectionSphereComponent->SetSphereRadius(BlackHoleRadius);
}

void ATargetActor_BlackHole::ActorInBlackHoleArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UArenasBlueprintFunctionLibrary::IsTowerActor(OtherActor)) return;
	
	TryAddTarget(OtherActor);
}

void ATargetActor_BlackHole::ActorOutBlackHoleArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (UArenasBlueprintFunctionLibrary::IsTowerActor(OtherActor)) return;
	
	TryRemoveTarget(OtherActor);
}

void ATargetActor_BlackHole::TryAddTarget(AActor* OtherTarget)
{
	if (!OtherTarget || ActorInRangeMap.Contains(OtherTarget)) return;

	if (GetTeamAttitudeTowards(*OtherTarget) == ETeamAttitude::Hostile)
	{
		UNiagaraComponent* LinkVFX = nullptr;
		if (BlackHoleLinkVFX)
		{
			LinkVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(BlackHoleLinkVFX, OtherTarget->GetRootComponent(),
				NAME_None, FVector::ZeroVector, FRotator::ZeroRotator,
				EAttachLocation::Type::KeepRelativeOffset, false);

			if (LinkVFX)
			{
				// 设置特效参数，链接到黑洞中心位置
				LinkVFX->SetVariablePosition(BlackHoleVFXOriginParamName, VFXComp->GetComponentLocation());
			}
		}
		ActorInRangeMap.Add(OtherTarget, LinkVFX);
	}
	
}

void ATargetActor_BlackHole::TryRemoveTarget(AActor* OtherTarget)
{
	if (!OtherTarget || !ActorInRangeMap.Contains(OtherTarget)) return;

	if (UNiagaraComponent* LinkVFXPtr = ActorInRangeMap.FindRef(OtherTarget))
	{
		LinkVFXPtr->DestroyComponent();
	}

	ActorInRangeMap.Remove(OtherTarget);
}

void ATargetActor_BlackHole::StopBlackHole()
{
	TArray<TWeakObjectPtr<AActor>> FinalTargets;
	for (TPair<AActor*, UNiagaraComponent*>& Elem : ActorInRangeMap)
	{
		AActor* TargetActor = Elem.Key;
		UNiagaraComponent* TargetLinkVFXComp = Elem.Value;
		if (TargetActor)
		{
			FinalTargets.Add(TargetActor);
		}

		if (IsValid(TargetLinkVFXComp))
		{
			TargetLinkVFXComp->DestroyComponent();
		}
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_ActorArray* TargetData = new FGameplayAbilityTargetData_ActorArray();
	TargetData->SetActors(FinalTargets);
	TargetDataHandle.Add(TargetData);

	FGameplayAbilityTargetData_SingleTargetHit* HitLocationData = new FGameplayAbilityTargetData_SingleTargetHit();
	HitLocationData->HitResult.ImpactPoint = GetActorLocation();		// 将黑洞位置信息传递给GA
	TargetDataHandle.Add(HitLocationData);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);

	
}

