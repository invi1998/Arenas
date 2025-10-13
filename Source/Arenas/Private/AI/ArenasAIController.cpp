// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "BrainComponent.h"
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

	if (IGenericTeamAgentInterface* PawnAsTeamAgent = Cast<IGenericTeamAgentInterface>(InPawn))
	{
		SetGenericTeamId(PawnAsTeamAgent->GetGenericTeamId()); // 让AIController属于被控制Pawn的队伍
		// 注意这里：我们在生成AI角色时，要先赋予（确定）角色的队伍ID，然后再调用FinishSpawning去生成角色
		// 这样OnPossess这里才能正确获取到角色的队伍ID

		// 同时，AI在生成时，可能已经有某些感知信息了，所以我们需要让感知组件更新一次
		ClearAndDisablePerception();
		EnableAllPerception();
		
	}

	// 订阅GAS的Tag生成或者移除事件，以便在角色死亡时禁用AI感知或者重生时启用AI感知
	if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
	{
		OwnerASC->RegisterGameplayTagEvent(ArenasGameplayTags::Status_Dead, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AArenasAIController::OnOwnerDeadTagChanged);
		OwnerASC->RegisterGameplayTagEvent(ArenasGameplayTags::Status_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AArenasAIController::OnOwnerStunTagChanged);
	}
	
}

ETeamAttitude::Type AArenasAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(&Other))
	{
		FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();
		FGenericTeamId MyTeamID = GetGenericTeamId();
        
		UE_LOG(LogTemp, Warning, TEXT("Team Attitude: MyTeam(%d) vs OtherTeam(%d)"), 
			   MyTeamID.GetId(), OtherTeamID.GetId());
        
		if (MyTeamID == OtherTeamID) 
			return ETeamAttitude::Friendly;
		else if (MyTeamID == FGenericTeamId::NoTeam || OtherTeamID == FGenericTeamId::NoTeam)
			return ETeamAttitude::Neutral;
		else
			return ETeamAttitude::Hostile;
	}
	return ETeamAttitude::Neutral;
}

void AArenasAIController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);

	ClearAndDisablePerception();
	EnableAllPerception();
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

void AArenasAIController::OnOwnerDeadTagChanged(FGameplayTag GameplayTag, int Count)
{
	if (Count != 0)
	{
		// 禁用行为树
		GetBrainComponent()->StopLogic(TEXT("Owner Dead"));
		ClearAndDisablePerception(); // 如果角色死亡，清除并禁用感知
		bIsPawnDead = true;
	}
	else
	{
		GetBrainComponent()->StartLogic();
		EnableAllPerception(); // 如果角色重生，启用所有感知
		bIsPawnDead = false;
	}
}

void AArenasAIController::OnOwnerStunTagChanged(FGameplayTag GameplayTag, int Count)
{
	if (bIsPawnDead) return; // 如果角色已经死亡，则不处理眩晕状态

	if (UArenasBlueprintFunctionLibrary::IsTowerActor(GetPawn()))
	{
		return; // 如果是防御塔，不处理眩晕状态
	}
	
	if (Count != 0)
	{
		// 禁用行为树
		GetBrainComponent()->StopLogic(TEXT("Owner Stunned"));
		// 这里如果只是被眩晕，我们不清除感知，因为可能只是暂时性的眩晕，在眩晕结束后，AI应该继续追踪之前的目标
		// ClearAndDisablePerception(); // 如果角色被眩晕，清除并禁用感知
	}
	else
	{
		GetBrainComponent()->StartLogic();
		// EnableAllPerception(); // 如果角色解除眩晕，启用所有感知
	}
}

void AArenasAIController::ClearAndDisablePerception()
{
	// 要清除感知，首先我们需要将感知Age设置为最大值
	AIPerceptionComponent->AgeStimuli(TNumericLimits<float>::Max());

	// 然后禁用所有感官
	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), false); // 禁用所有感官
	}
	
	// 然后清除黑板中的目标
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->ClearValue(BlackboardKeyName_TargetActor);
	}
}

void AArenasAIController::EnableAllPerception()
{
	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), true); // 启用所有感官
	}
}



