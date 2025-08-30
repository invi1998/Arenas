// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/ArenasCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


// Sets default values
AArenasAIController::AArenasAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;		// 只检测敌人
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;	// 不检测中立
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;	// 不检测友方

	SightConfig->SightRadius = 1000.f;			// 视野范围
	SightConfig->LoseSightRadius = 1200.f;		// 失去视野范围
	SightConfig->PeripheralVisionAngleDegrees = 180.f; // 视野角度

	SightConfig->SetMaxAge(5.f);				// 记忆时间

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());	// 设置主要感官为视觉

	AIPerceptionComponent->OnTargetPerceptionInfoUpdated.AddDynamic(this, &AArenasAIController::OnTargetPerceptionUpdated);	// 绑定感知更新事件
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &AArenasAIController::OnPerceptionForgottenTarget);		// 绑定感知遗忘事件
	
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
		ForgetActorIfDead(UpdateInfo.Target.Get());
	}
}

void AArenasAIController::OnPerceptionForgottenTarget(AActor* Actor)
{
	if (!Actor) return;

	if (GetCurrentTargetActor() == Actor)
	{
		SetCurrentTargetActor(GetNextPerceivedActor());
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

AActor* AArenasAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		TArray<AActor*> PerceivedActors;
		AIPerceptionComponent->GetPerceivedHostileActors(PerceivedActors);	// 获取所有感知到的敌对角色
		if (PerceivedActors.Num() > 0)
		{
			return PerceivedActors[0]; // 返回第一个感知到的敌对角色
		}
	}
	return nullptr;
}

void AArenasAIController::ForgetActorIfDead(AActor* Actor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	// 如果目标存在并且已经死亡，那么让AI忘记该目标
	if (TargetASC && TargetASC->HasMatchingGameplayTag(ArenasGameplayTags::Status_Dead))
	{
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator It = AIPerceptionComponent->GetPerceptualDataIterator(); It; ++It)
		{
			// 遍历感知数据，将指定Actor的感知数据的刺激时间设置为最大值，从而让AI忘记该Actor
			if (It->Key != Actor) continue;

			for (FAIStimulus& Stimulus : It->Value.LastSensedStimuli)
			{
				Stimulus.SetStimulusAge(TNumericLimits<float>::Max());
				break;
			}
		}
	}
}



