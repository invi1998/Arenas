// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TargetActor_BlackHole.h"

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

void ATargetActor_BlackHole::OnRep_BlackHoleRadius()
{
	DetectionSphereComponent->SetSphereRadius(BlackHoleRadius);
}

void ATargetActor_BlackHole::ActorInBlackHoleArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ATargetActor_BlackHole::ActorOutBlackHoleArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

