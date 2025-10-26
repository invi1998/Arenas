// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TargetActor_Beam.h"
#include "NiagaraComponent.h"
#include "Arenas/Arenas.h"
#include "Components/SphereComponent.h"


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
	
	AvatarActor = nullptr;
}



