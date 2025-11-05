// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TargetActor_Around.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ATargetActor_Around::ATargetActor_Around()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(GetRootComponent());
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATargetActor_Around::ActorInDetectionRange);
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true; // 服务器端生成目标数据，而客户端只是进行模拟和显示
}

void ATargetActor_Around::ConfigureDetection(float DetectionRadius, const FGenericTeamId& InTeamId,
	const FGameplayTag& InLocalGameplayCueTag)
{
	SetGenericTeamId(InTeamId);
	TargetDetectionRadius = DetectionRadius;
	DetectionSphere->SetSphereRadius(DetectionRadius);
	LocalGameplayCueTag = InLocalGameplayCueTag;
}

void ATargetActor_Around::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	TeamId = TeamID;
}

FGenericTeamId ATargetActor_Around::GetGenericTeamId() const
{
	return TeamId;
}

ETeamAttitude::Type ATargetActor_Around::GetTeamAttitudeTowards(const AActor& Other) const
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

void ATargetActor_Around::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATargetActor_Around, TeamId);
	DOREPLIFETIME(ATargetActor_Around, LocalGameplayCueTag);
	DOREPLIFETIME(ATargetActor_Around, TargetDetectionRadius);
}

void ATargetActor_Around::OnRep_TargetDetectionRadiusReplicated()
{
	DetectionSphere->SetSphereRadius(TargetDetectionRadius);
}

void ATargetActor_Around::ActorInDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	AActor* AvatarActor = nullptr;
	if (OwningAbility)
	{
		AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	}

	if (OtherActor == AvatarActor) return;
	if (OtherActor == this) return;
	if (UArenasBlueprintFunctionLibrary::IsTowerActor(OtherActor)) return;
	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile) return;

	FHitResult HitResult(OtherActor, OtherComp, OtherActor->GetActorLocation(), FVector::UpVector);
	HitResult.ImpactPoint = OtherActor->GetActorLocation();
	HitResult.ImpactNormal = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	if (HasAuthority())
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		FGameplayAbilityTargetData_ActorArray* ActorArrayData = new FGameplayAbilityTargetData_ActorArray();
		TArray<TWeakObjectPtr<AActor>> TargetActors;
		TargetActors.Add(OtherActor);
		ActorArrayData->SetActors(TargetActors);
		TargetDataHandle.Add(ActorArrayData);

		FGameplayAbilityTargetData_SingleTargetHit* SingleTargetHitData_HitLocation = new FGameplayAbilityTargetData_SingleTargetHit();
		SingleTargetHitData_HitLocation->HitResult = HitResult;
		TargetDataHandle.Add(SingleTargetHitData_HitLocation);
		
		TargetDataReadyDelegate.Broadcast(TargetDataHandle);
	}

	UArenasBlueprintFunctionLibrary::SendLocalGameplayCue(OtherActor, HitResult, LocalGameplayCueTag);

	
}

