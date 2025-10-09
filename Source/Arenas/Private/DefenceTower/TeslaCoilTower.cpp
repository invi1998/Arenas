// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TeslaCoilTower.h"

#include "ArenasGameplayTags.h"
#include "Arenas/Arenas.h"
#include "Components/WidgetComponent.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/ArenasUserWidget.h"


// Sets default values
ATeslaCoilTower::ATeslaCoilTower()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ArenasAttributeSet = CreateDefaultSubobject<UArenasAttributeSet>(TEXT("ArenasAttributeSet"));

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComponent"));
	OverheadWidgetComponent->SetupAttachment(GetRootComponent());

	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
	TowerMesh->SetupAttachment(GetRootComponent());
	TowerMesh->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore); // 胶囊体忽略摄像机碰撞
	TowerMesh->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	
	ArenasAbilitySystemComponent = CreateDefaultSubobject<UArenasAbilitySystemComponent>(TEXT("ArenasAbilitySystemComponent"));

	PawnUIComponent = CreateDefaultSubobject<UPawnUIComponent>(TEXT("PawnUIComponent"));
}

UPawnUIComponent* ATeslaCoilTower::GetPawnUIComponent() const
{
	return PawnUIComponent;
}

void ATeslaCoilTower::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	IGenericTeamAgentInterface::SetGenericTeamId(InTeamID);
}

FGenericTeamId ATeslaCoilTower::GetGenericTeamId() const
{
	return IGenericTeamAgentInterface::GetGenericTeamId();
}

UAbilitySystemComponent* ATeslaCoilTower::GetAbilitySystemComponent() const
{
	return ArenasAbilitySystemComponent;
}

const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& ATeslaCoilTower::GetAbilities() const
{
	return ArenasAbilitySystemComponent->GetAbilities();
}

bool ATeslaCoilTower::IsAlive() const
{
	return GetAbilitySystemComponent() && !GetAbilitySystemComponent()->HasMatchingGameplayTag(ArenasGameplayTags::Status_Dead);
}

void ATeslaCoilTower::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeslaCoilTower, TeamID);
	
}

void ATeslaCoilTower::ServerSideInit()
{
	ArenasAbilitySystemComponent->InitAbilityActorInfo(this, this);
	ArenasAbilitySystemComponent->ServerSideInit();
}

void ATeslaCoilTower::OnRep_TeamID()
{
}

void ATeslaCoilTower::SpawnOverheadWidgetComponent()
{
	// OverheadWidgetComponent->SetHiddenInGame(true);
	if (OverheadWidgetComponent && OverheadWidgetClass)
	{
		OverheadWidgetComponent->SetWidgetClass(OverheadWidgetClass);
		OverheadWidgetComponent->SetDrawSize(FVector2D(200.0f, 50.0f));
		OverheadWidgetComponent->SetVisibility(true);
		OverheadWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverheadWidgetComponent->SetHiddenInGame(false);
		
		OverheadWidget = Cast<UArenasUserWidget>(OverheadWidgetComponent->GetUserWidgetObject());
		if (OverheadWidget)
		{
			OverheadWidget->InitOverheadWidget(this);
		}
	}
	
}

void ATeslaCoilTower::StartDeathSequence()
{
	// 取消所有GAS
	if (ArenasAbilitySystemComponent)
	{
		ArenasAbilitySystemComponent->CancelAllAbilities();
	}
}

void ATeslaCoilTower::SetAIPerceptionStimuliSourceEnabled(bool bEnabled)
{
}

// Called when the game starts or when spawned
void ATeslaCoilTower::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		ServerSideInit();
	}
	else
	{
		ArenasAbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	
}

