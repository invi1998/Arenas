// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenasCharacter.h"

#include "Components/WidgetComponent.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "Widgets/ArenasUserWidget.h"


// Sets default values
AArenasCharacter::AArenasCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ArenasAbilitySystemComponent = CreateDefaultSubobject<UArenasAbilitySystemComponent>(TEXT("ArenasAbilitySystemComponent"));
	// ArenasAbilitySystemComponent->SetIsReplicated(true);
	// ArenasAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ArenasAttributeSet = CreateDefaultSubobject<UArenasAttributeSet>(TEXT("ArenasAttributeSet"));

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComponent"));
	OverheadWidgetComponent->SetupAttachment(GetRootComponent());
}

void AArenasCharacter::ServerSideInit()
{
	ArenasAbilitySystemComponent->InitAbilityActorInfo(this, this);
	ArenasAbilitySystemComponent->ApplyInitialEffects();
}

void AArenasCharacter::ClientSideInit()
{
	ArenasAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool AArenasCharacter::IsLocallyControlledByPlayer() const
{
	// 检查控制器是否存在，并且是否是本地玩家控制器
	// 这通常用于判断当前角色是否由本地玩家控制
	// 在多人游戏中，可能会有AI控制的角色，这个函数可以用来区分
	// 例如，在客户端上，我们可以使用这个函数来判断当前角色
	// 是否是由本地玩家控制的角色，从而决定是否显示某些UI
	// 或者执行某些操作。
	return GetController() && GetController()->IsLocalPlayerController();
}

void AArenasCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController && !NewController->IsPlayerController())
	{
		// 如果控制器是 AI 控制器，我们可以在这里进行一些初始化操作
		ServerSideInit();
	}
}

UPawnUIComponent* AArenasCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

void AArenasCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnOverheadWidgetComponent();
	
}

void AArenasCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArenasCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* AArenasCharacter::GetAbilitySystemComponent() const
{
	return ArenasAbilitySystemComponent;
}

void AArenasCharacter::SpawnOverheadWidgetComponent()
{
	if (IsLocallyControlledByPlayer())
	{
		OverheadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
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

