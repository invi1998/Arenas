// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TargetActor_Beam.h"
#include "NiagaraComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Arenas/Arenas.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

void ATargetActor_Beam::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateTargetTrace();
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

void ATargetActor_Beam::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	if (!OwningAbility) return;

	// 获取持有该TargetActor的角色，这在其他代理客户端上将不可用，代理客户端只能通过复制的AvatarActor来获取
	// 而拥有者的客户端和服务器则可以获取
	AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	if (HasAuthority())
	{
		GetWorldTimerManager().ClearTimer(PeriodicalTargetingTimerHandle);
		GetWorldTimerManager().SetTimer(PeriodicalTargetingTimerHandle, this, &ATargetActor_Beam::DoTargetCheckAndReport, TargetingInterval, true, 0.f);
	}
	
	
}

void ATargetActor_Beam::DoTargetCheckAndReport()
{
}

void ATargetActor_Beam::UpdateTargetTrace()
{
	FVector ViewLocation = GetActorLocation();
	FRotator ViewRotation = GetActorRotation();
	if (AvatarActor)
	{
		AvatarActor->GetActorEyesViewPoint(ViewLocation, ViewRotation);
	}

	// 视野方向的远点（即摄像机（控制器）视角下的远点）
	FVector LookEndPoint = ViewLocation + ViewRotation.Vector() * 100000.f;
	// 让TargetActor面朝视野方向，这样激光束特效才能正确对齐，因为在瞄准状态下，摄像机被我们做了一定的偏移，所以需要重新计算角色人物模型视角下朝向瞄准远点的旋转
	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookEndPoint);
	// 然后设置TargetActor的朝向
	SetActorRotation(LookRotation);

	// 使用特定通道将形状在世界中扫过，并返回所有初始重叠（包括阻挡）如果需要，然后返回重叠击中结果，再返回第一个阻挡击中。
	// 结果会被排序，因此阻挡击中（如果找到）将是数组中的最后一个元素。只会生成最近的单个阻挡结果，之后不会进行任何测试。
	// 参数:OutHits — 在射线与世界之间发现的击中数组
	// Start — 形状的起始位置
	// End — 形状的结束位置
	// Rot — 形状的旋转
	// TraceChannel — 该射线所属的“通道”，用于确定应击中的组件
	// CollisionShape — 碰撞形状，支持盒子、球体、胶囊
	// Params — 用于追踪的其他附加参数
	// ResponseParam — 用于该追踪的响应容器
	FVector SweepEndLocation = GetActorLocation() + LookRotation.Vector() * TargetRange;
	TArray<FHitResult> SweepHitResults;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.AddIgnoredActor(AvatarActor);
	FCollisionResponseParams CollisionResponseParams(ECR_Overlap);		// 设置为重叠响应，以便检测到所有重叠的Actor
	GetWorld()->SweepMultiByChannel(SweepHitResults,
		GetActorLocation(),
		SweepEndLocation,
		FQuat::Identity,	// 无旋转，安全的默认值：在函数需要传入一个四元数参数，但你又不希望发生旋转时，传递它是一个安全的选择。
		ECC_WorldDynamic,
		FCollisionShape::MakeSphere(DetectionCylinderRadius),
		CollisionQueryParams,
		CollisionResponseParams
	);

	FVector BeamEndLocation = SweepEndLocation;
	float BeamLength = TargetRange;

	for (FHitResult HitResult : SweepHitResults)
	{
		if (HitResult.GetActor())
		{
			if (GetTeamAttitudeTowards(*HitResult.GetActor()) != ETeamAttitude::Friendly)
			{
				// 遇到第一个非友军的Actor时，说明遇到了地形或者其他敌方等阻挡物，激光束到此为止
				BeamEndLocation = HitResult.ImpactPoint;
				BeamLength = FVector::Distance(GetActorLocation(), BeamEndLocation);
				break;
			}
		}
		
	}

	TargetEndDetectionSphere->SetWorldLocation(BeamEndLocation);
	if (BeamVFXComp)
	{
		// Niagara里，长度参数是以米为单位的，而我们这里的单位是厘米，所以需要除以100
		BeamVFXComp->SetFloatParameter(BeamFXLengthParamName, BeamLength * 0.01f);
	}
	
}





