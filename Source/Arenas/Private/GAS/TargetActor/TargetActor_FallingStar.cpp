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
	// 该函数只会在服务端被调用
	AOERadius = InAOERadius;
	LoopDuration = InLoopDuration;
	SpawnRate = InSpawnRate;
	SpawnHeight = InSpawnHeight;
	FallingSpeed = InFallingSpeed;
	SetGenericTeamId(OwningTeamId);

	// 打印参数
	UE_LOG(LogTemp, Warning, TEXT(" ATargetActor_FallingStar::ConfigureFallingStarTargetActor: AOERadius = %f, LoopDuration = %f, SpawnRate = %f, SpawnHeight = %f, FallingSpeed = %f"), 
		AOERadius, LoopDuration, SpawnRate, SpawnHeight, FallingSpeed);

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

		UE_LOG(LogTemp, Warning, TEXT(" 909090909090 ATargetActor_FallingStar::StartTargeting: IsServer = %s"), HasAuthority() ? TEXT("True") : TEXT("False"));
		
		UE_LOG(LogTemp, Warning, TEXT(" 000000000 ATargetActor_FallingStar::ConfigureFallingStarTargetActor: Activating FallingStarVFXComp"));
	}
}

void ATargetActor_FallingStar::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	// 打印是否是服务端
	UE_LOG(LogTemp, Warning, TEXT(" 11111111 ATargetActor_FallingStar::StartTargeting: IsServer = %s"), HasAuthority() ? TEXT("True") : TEXT("False"));
	
	if (!OwningAbility) return;

	UE_LOG(LogTemp, Warning, TEXT(" 2222222 ATargetActor_FallingStar::StartTargeting: IsServer = %s"), HasAuthority() ? TEXT("True") : TEXT("False"));
	
	AvatarActor = Ability->GetAvatarActorFromActorInfo();
	
	if (HasAuthority())
	{
		bIsActiveVFX = true;
		if (UWorld* World = GetWorld())
		{
			
			const float OnceFallTime = SpawnHeight / FallingSpeed;
			UE_LOG(LogTemp, Warning, TEXT(" ---------------- ATargetActor_FallingStar::StartTargeting: Setting up timers. LoopDuration = %f, OnceFallTime = %f"), LoopDuration, OnceFallTime);
			World->GetTimerManager().ClearTimer(FallingStarImpactTimerHandle);
			World->GetTimerManager().ClearTimer(FallingStarSpawnTimerHandle);
			World->GetTimerManager().SetTimer(FallingStarSpawnTimerHandle, this, &ATargetActor_FallingStar::StopFallingStar, LoopDuration, false);
			World->GetTimerManager().SetTimer(FallingStarImpactTimerHandle, this, &ATargetActor_FallingStar::DoTargetCheckAndReport, OnceFallTime, true, OnceFallTime);
		}
	}
	
}

void ATargetActor_FallingStar::BeginDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("???????????????????? ATargetActor_FallingStar::BeginDestroy called."));
	Super::BeginDestroy();
}

void ATargetActor_FallingStar::OnRep_IsActiveVFX()
{
	UE_LOG(LogTemp, Warning, TEXT(" 33333333 ATargetActor_FallingStar::StartTargeting: IsServer = %s"), HasAuthority() ? TEXT("True") : TEXT("False"));
	
	if (bIsActiveVFX && FallingStarVFXComp)
	{
		// 设置特效参数
		FallingStarVFXComp->SetVariableFloat(VFXParamName_AOERadius, AOERadius);
		FallingStarVFXComp->SetVariableFloat(VFXParamName_LoopDuration, LoopDuration);
		FallingStarVFXComp->SetVariableFloat(VFXParamName_SpawnRate, SpawnRate);
		const FVector Offset = FVector(0.f, 0.f, SpawnHeight);
		FallingStarVFXComp->SetVariableVec3(VFXParamName_Offset, Offset);
		const FVector Velocity = FVector(0.f, 0.f, -FallingSpeed);
		FallingStarVFXComp->SetVariableVec3(VFXParamName_Velocity, Velocity);

		UE_LOG(LogTemp, Warning, TEXT(" --------------- OnRep_IsActiveVFX: Activating FallingStarVFXComp"));
		FallingStarVFXComp->Activate(true);
	}
	else if (FallingStarVFXComp)
	{
		UE_LOG(LogTemp, Warning, TEXT(" 4444444444 ATargetActor_FallingStar::StartTargeting: IsServer = %s"), HasAuthority() ? TEXT("True") : TEXT("False"));
	
		FallingStarVFXComp->Deactivate();
	}
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
	UE_LOG(LogTemp, Warning, TEXT(" ATargetActor_FallingStar::DoTargetCheckAndReport: IsServer = %s"), HasAuthority() ? TEXT("True") : TEXT("False"));
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

	UE_LOG(LogTemp, Warning, TEXT(" ATargetActor_FallingStar::DoTargetCheckAndReport: Reporting %d targets."), OverlappingActorsWeakPtrArray.Num());

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

