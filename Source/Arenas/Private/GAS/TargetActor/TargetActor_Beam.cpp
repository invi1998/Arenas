// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TargetActor_Beam.h"
#include "NiagaraComponent.h"
#include "Arenas/Arenas.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ATargetActor_Beam::ATargetActor_Beam()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	TargetEndDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TargetEndDetectionSphere"));
	TargetEndDetectionSphere->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore);	// 忽略弹簧臂通道的碰撞
	TargetEndDetectionSphere->SetupAttachment(GetRootComponent());

	BeamVFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BeamVFXComp"));
	BeamVFXComp->SetupAttachment(GetRootComponent());
	bReplicates = true;
	
	ShouldProduceTargetDataOnServer = true;		// 服务器端生成目标数据，而客户端只是进行模拟和显示

	// 设置了只在服务端生成后，测试发现，在多人模式下，施法者客户端没有生成目标Actor，只有服务端和其他客户端生成了该Actor
	// 这是因为在父类的IsNetRelevantFor函数里，有一段逻辑是这样的：
	// bool AGameplayAbilityTargetActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
	// The player who created the ability doesn't need to be updated about it - there should be local prediction in place.
	// if (RealViewer == PrimaryPC)
	// {
	// 	return false;
	// }
	// 创建该能力的玩家不需要获得更新信息——应该有本地预测机制。
	// 因此，施法者客户端不会接收到该Actor的更新信息，自然也就不会生成该Actor了。
	// 这样做的好处是减少了网络流量，因为施法者客户端不需要知道该Actor的存在，只需要在本地进行预测和显示即可。
	// 其他客户端和服务端则需要知道该Actor的存在，以便进行正确的游戏逻辑处理。
	
	AvatarActor = nullptr;
}

void ATargetActor_Beam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATargetActor_Beam, TargetRange);
	DOREPLIFETIME(ATargetActor_Beam, DetectionCylinderRadius);
	DOREPLIFETIME(ATargetActor_Beam, TeamId);
	DOREPLIFETIME(ATargetActor_Beam, AvatarActor);
	
}

void ATargetActor_Beam::ConfigureTargetSetting(float InTargetRange, float InDetectionCylinderRadius, float InTargetingInterval, FGenericTeamId OwningTeamId, bool bShouldDrawDebug)
{
	TargetRange = InTargetRange;
	DetectionCylinderRadius = InDetectionCylinderRadius;
	TargetingInterval = InTargetingInterval;
	TeamId = OwningTeamId;
	bDrawDebug = bShouldDrawDebug;
	
}

void ATargetActor_Beam::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	TeamId = TeamID;
}

FGenericTeamId ATargetActor_Beam::GetGenericTeamId() const
{
	return TeamId;
}





