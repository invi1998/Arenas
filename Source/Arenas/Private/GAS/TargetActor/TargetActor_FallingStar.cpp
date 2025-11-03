// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TargetActor_FallingStar.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ATargetActor_FallingStar::ATargetActor_FallingStar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	DetectionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphereComponent"));
	DetectionSphereComponent->SetupAttachment(GetRootComponent());
	DetectionSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 只与Pawn通道重叠检测

	FallingStarVFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FallingStarVFXComp"));
	FallingStarVFXComp->SetupAttachment(GetRootComponent());
	FallingStarVFXComp->SetAutoActivate(true);	// 默认激活特效

	DreamImpactVFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DreamImpactVFXComp"));
	DreamImpactVFXComp->SetupAttachment(GetRootComponent());
	DreamImpactVFXComp->SetAutoActivate(true);	// 默认激活特效
	
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;

	AvatarActor = nullptr;
}

// Called every frame
void ATargetActor_FallingStar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATargetActor_FallingStar::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	TeamId = TeamID;
}

FGenericTeamId ATargetActor_FallingStar::GetGenericTeamId() const
{
	return TeamId;
}

ETeamAttitude::Type ATargetActor_FallingStar::GetTeamAttitudeTowards(const AActor& Other) const
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

void ATargetActor_FallingStar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATargetActor_FallingStar, TeamId);
	DOREPLIFETIME(ATargetActor_FallingStar, AOERadius);
	DOREPLIFETIME(ATargetActor_FallingStar, LoopDuration);
	DOREPLIFETIME(ATargetActor_FallingStar, SpawnRate);
	DOREPLIFETIME(ATargetActor_FallingStar, SpawnHeight);
	DOREPLIFETIME(ATargetActor_FallingStar, FallingSpeed);
	DOREPLIFETIME_CONDITION_NOTIFY(ATargetActor_FallingStar, bIsActiveVFX, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(ATargetActor_FallingStar, AvatarActor);
	
}

void ATargetActor_FallingStar::ConfigureFallingStarTargetActor(float InAOERadius, float InLoopDuration,
	float InSpawnRate, float InSpawnHeight, float InFallingSpeed, const FGenericTeamId& OwningTeamId)
{
	AOERadius = InAOERadius;
	LoopDuration = InLoopDuration;
	SpawnRate = InSpawnRate;
	SpawnHeight = InSpawnHeight;
	FallingSpeed = InFallingSpeed;
	SetGenericTeamId(OwningTeamId);
	
	DetectionSphereComponent->SetSphereRadius(AOERadius);
	// 激活特效
	if (FallingStarVFXComp)
	{
		// 设置特效参数
		FallingStarVFXComp->SetVariableFloat(VFXParamName_AOERadius, AOERadius);
		FallingStarVFXComp->SetVariableFloat(VFXParamName_LoopDuration, LoopDuration);
		FallingStarVFXComp->SetVariableFloat(VFXParamName_SpawnRate, SpawnRate);
		const FVector Offset = FVector(0.f, 0.f, SpawnHeight);
		FallingStarVFXComp->SetVariableVec3(VFXParamName_Offset, Offset);
		const FVector Velocity = FVector(0.f, 0.f, -FallingSpeed);
		FallingStarVFXComp->SetVariableVec3(VFXParamName_Velocity, Velocity);

		FallingStarVFXComp->Activate(true);

	}

	if (DreamImpactVFXComp)
	{
		DreamImpactVFXComp->SetVariableFloat(VFXParamName_LoopDuration_DreamImpact, LoopDuration);
		DreamImpactVFXComp->Activate(true);
	}
	
}

void ATargetActor_FallingStar::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	
	if (!OwningAbility) return;

	AvatarActor = Ability->GetAvatarActorFromActorInfo();
	
	if (HasAuthority())
	{
		bIsActiveVFX = true;
		if (UWorld* World = GetWorld())
		{
			
			const float OnceFallTime = SpawnHeight / FallingSpeed;
			World->GetTimerManager().ClearTimer(FallingStarImpactTimerHandle);
			World->GetTimerManager().ClearTimer(FallingStarSpawnTimerHandle);
			World->GetTimerManager().SetTimer(FallingStarSpawnTimerHandle, this, &ATargetActor_FallingStar::CancelTargeting, LoopDuration, false);
			World->GetTimerManager().SetTimer(FallingStarImpactTimerHandle, this, &ATargetActor_FallingStar::DoTargetCheckAndReport, OnceFallTime, true, OnceFallTime);
		}
	}
	
}

void ATargetActor_FallingStar::CancelTargeting()
{
	StopFallingStar();
	Super::CancelTargeting();
}

void ATargetActor_FallingStar::BeginDestroy()
{
	Super::BeginDestroy();
}

void ATargetActor_FallingStar::OnRep_IsActiveVFX()
{
}

void ATargetActor_FallingStar::StopFallingStar()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FallingStarImpactTimerHandle);
		World->GetTimerManager().ClearTimer(FallingStarSpawnTimerHandle);
		
	}
}

void ATargetActor_FallingStar::DoTargetCheckAndReport()
{
	if (!HasAuthority()) return;
	
	TSet<AActor*> OverlappingActorSet;
	DetectionSphereComponent->GetOverlappingActors(OverlappingActorSet);

	TArray<TWeakObjectPtr<AActor>> OverlappingActorsWeakPtrArray;
	for (AActor* OverlappingActor : OverlappingActorSet)
	{
		if (ShouldReportActorAsTarget(OverlappingActor))
		{
			OverlappingActorsWeakPtrArray.Add(OverlappingActor);
		}
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_ActorArray* TargetData = new FGameplayAbilityTargetData_ActorArray();
	TargetData->SetActors(OverlappingActorsWeakPtrArray);
	TargetDataHandle.Add(TargetData);
	
	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
	
}

bool ATargetActor_FallingStar::ShouldReportActorAsTarget(AActor* CheckedActor) const
{
	if (!CheckedActor) return false;
	if (AvatarActor && AvatarActor == CheckedActor) return false;
	if (CheckedActor == this) return false;
	if (UArenasBlueprintFunctionLibrary::IsTowerActor(CheckedActor)) return false;

	if (GetTeamAttitudeTowards(*CheckedActor) == ETeamAttitude::Hostile)
	{
		return true;
	}

	return false;
}

