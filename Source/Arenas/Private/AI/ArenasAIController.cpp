// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasAIController.h"

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



