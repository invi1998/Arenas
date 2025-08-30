// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ArenasCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


// Sets default values
AArenasAIController::AArenasAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;		// 只检测敌人
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;	// 不检测中立
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;	// 不检测友方

	SightConfig->SightRadius = 1000.f;			// 视野范围
	SightConfig->LoseSightRadius = 1200.f;		// 失去视野范围
	SightConfig->PeripheralVisionAngleDegrees = 180.f; // 视野角度

	SightConfig->SetMaxAge(5.f);				// 记忆时间

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());	// 设置主要感官为视觉

	PerceptionComp->OnTargetPerceptionInfoUpdated.AddDynamic(this, &AArenasAIController::OnTargetPerceptionUpdated);
	
}

void AArenasAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetGenericTeamId(FGenericTeamId(0)); // 设置为队伍0

	if (IGenericTeamAgentInterface* PawnAsTeamAgent = Cast<IGenericTeamAgentInterface>(InPawn))
	{
		PawnAsTeamAgent->SetGenericTeamId(GetGenericTeamId()); // 让被控制的Pawn也属于同一队伍
	}
	
}

void AArenasAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree); // 运行行为树
}

void AArenasAIController::OnTargetPerceptionUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	// 当感知到目标时将目标设置为黑板中的目标
	if (UpdateInfo.Stimulus.WasSuccessfullySensed())	// 感知到
	{
		if (!GetCurrentTargetActor())
		{
			SetCurrentTargetActor(UpdateInfo.Target.Get());
		}
	}
	else
	{
		if (GetCurrentTargetActor() == UpdateInfo.Target.Get())
		{
			// 只有当当前目标是失去感知的目标时才清除
			SetCurrentTargetActor(nullptr);
		}
	}
}

const AActor* AArenasAIController::GetCurrentTargetActor() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		return static_cast<const AActor*>(BlackboardComponent->GetValueAsObject(BlackboardKeyName_TargetActor));
	}

	return nullptr;
	
}

void AArenasAIController::SetCurrentTargetActor(AActor* NewTargetActor)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (NewTargetActor)
		{
			BlackboardComponent->SetValueAsObject(BlackboardKeyName_TargetActor, NewTargetActor);
		}
		else
		{
			BlackboardComponent->ClearValue(BlackboardKeyName_TargetActor);
		}
	}
	
}



