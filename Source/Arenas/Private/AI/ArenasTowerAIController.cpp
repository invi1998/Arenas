// Ace of Arenas. (invi_1998 All Rights Reserved)

#include "ArenasTowerAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/Engine.h"

AArenasTowerAIController::AArenasTowerAIController()
{
}

void AArenasTowerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 订阅GAS的Tag生成或者移除事件，以便在角色死亡时禁用AI感知或者重生时启用AI感知
	if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
	{
		OwnerASC->RegisterGameplayTagEvent(ArenasGameplayTags::Status_Dead, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AArenasTowerAIController::OnOwnerDeadTagChanged);
	}
}

void AArenasTowerAIController::SetCurrentTargetActor(AActor* NewTargetActor)
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

AActor* AArenasTowerAIController::GetCurrentTargetActor() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		return Cast<AActor>(BlackboardComponent->GetValueAsObject(BlackboardKeyName_TargetActor));
	}
	return nullptr;
}

ETeamAttitude::Type AArenasTowerAIController::GetTeamAttitudeTowards(const AActor& Other) const
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

void AArenasTowerAIController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);
	
}

void AArenasTowerAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);
}

void AArenasTowerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArenasTowerAIController::OnOwnerDeadTagChanged(FGameplayTag GameplayTag, int Count)
{
	if (Count != 0)
	{
		// 禁用行为树
		GetBrainComponent()->StopLogic(TEXT("Owner Dead"));
	}
	else
	{
		GetBrainComponent()->StartLogic();
	}
}
