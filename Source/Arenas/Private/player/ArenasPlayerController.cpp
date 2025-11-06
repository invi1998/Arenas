// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenasPlayerController.h"

#include "ArenasPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Actor/AttackRangeDecal.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/ArenasUserWidget.h"

void AArenasPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ArenasPlayerCharacter = Cast<AArenasPlayerCharacter>(InPawn);
	if (ArenasPlayerCharacter)
	{
		ArenasPlayerCharacter->ServerSideInit();
		ArenasPlayerCharacter->SetGenericTeamId(TeamID);
	}
}

void AArenasPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenasPlayerController, TeamID);
}

void AArenasPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);
	ArenasPlayerCharacter = Cast<AArenasPlayerCharacter>(P);
	if (ArenasPlayerCharacter)
	{
		ArenasPlayerCharacter->ClientSideInit();
		SpawnPlayerUIWidget();
		ArenasPlayerCharacter->OnAnimInstanceReady.Broadcast();
	}
	
}

void AArenasPlayerController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	TeamID = InTeamID;
}

FGenericTeamId AArenasPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void AArenasPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		InputSubsystem->RemoveMappingContext(UIInputMappingContext);
		InputSubsystem->AddMappingContext(UIInputMappingContext, 1);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ShopToggleInputAction, ETriggerEvent::Triggered, this, &AArenasPlayerController::OnShopToggleActionTriggered);
		EnhancedInputComponent->BindAction(ToggleGameplayMenuInputAction, ETriggerEvent::Triggered, this, &AArenasPlayerController::OnToggleGameplayMenuActionTriggered);
	}
}

void AArenasPlayerController::DrawDefenseTowerRangeDecal(const FName& DefenseTowerName, const FVector& Location, float Range, bool bUnderAttack)
{
	Client_ShowTowerAttackRangeDecal(DefenseTowerName, Location, Range, bUnderAttack);
}

void AArenasPlayerController::ClearDefenseTowerRangeDecal(const FName& DefenseTowerName)
{
	Client_HideTowerAttackRangeDecal(DefenseTowerName);
}

void AArenasPlayerController::ShowCombatText(float ValueNumber, EArenasComboTextType TextType, const FVector& HitLocation)
{
	if (!HasAuthority()) return;
	Client_ShowCombatText(ValueNumber, TextType, HitLocation);
}

void AArenasPlayerController::MatchFinished(AActor* ViewTarget, FGenericTeamId LooseTeamID)
{
	if (!HasAuthority()) return;

	ArenasPlayerCharacter->DisableInput(this);

	Client_MatchFinished(ViewTarget, LooseTeamID);
	
}

void AArenasPlayerController::UpdateTeamOnePlayerKillCount(int32 NewKillCount)
{
	if (IsLocalPlayerController())
	{
		OnTeamOnePlayerKillChanged.Broadcast(NewKillCount);
	}
}

void AArenasPlayerController::UpdateTeamTwoPlayerKillCount(int32 NewKillCount)
{
	if (IsLocalPlayerController())
	{
		OnTeamTwoPlayerKillChanged.Broadcast(NewKillCount);
	}
}

void AArenasPlayerController::SpawnPlayerUIWidget()
{
	if (!IsLocalPlayerController()) return;

	PlayerUIWidget = CreateWidget<UArenasUserWidget>(this, PlayerUIWidgetClass.Get());
	if (PlayerUIWidget)
	{
		PlayerUIWidget->AddToViewport();
		PlayerUIWidget->ConfigureAbilities(ArenasPlayerCharacter->GetAbilities());
	}
	
}

void AArenasPlayerController::OnShopToggleActionTriggered()
{
	if (PlayerUIWidget)
	{
		PlayerUIWidget->ToggleShopPopup();
	}
}

void AArenasPlayerController::OnToggleGameplayMenuActionTriggered()
{
	if (PlayerUIWidget)
	{
		PlayerUIWidget->ToggleGameplayMenu();
	}
}

void AArenasPlayerController::Client_ShowTowerAttackRangeDecal_Implementation(const FName& DefenseTowerName, const FVector& GroundLocation, float Range, bool bUnderAttack)
{
	if (!IsLocalPlayerController()) return;
	// 如果已经存在该防御塔的贴花，则更新其位置和范围
	if (AAttackRangeDecal* DefenseTowerDecal = ActiveDefenseTowerAttackRangeDecals.FindRef(DefenseTowerName))
	{
		DefenseTowerDecal->SetActorLocation(GroundLocation); // 保持贴花在地面上
		FVector DecalSize = FVector(500.f, Range, Range);
		DefenseTowerDecal->SetDecalSize(DecalSize);
		if (UMaterialInstanceDynamic* DecalMID = DefenseTowerDecal->GetDecalMaterialInstance())
		{
			FLinearColor NewColor = bUnderAttack ? AttackColor : InRangeColor;
			DecalMID->SetVectorParameterValue(DecalDynamicMaterialColorParamName, NewColor);
		}
	}
	else
	{
		// 否则创建一个新的贴花
		if (AttackRangeDecalClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetPawn();
			if (AAttackRangeDecal* NewDecal = GetWorld()->SpawnActor<AAttackRangeDecal>(AttackRangeDecalClass, GroundLocation, FRotator::ZeroRotator, SpawnParams))
			{
				FVector DecalSize = FVector(500.f, Range, Range);
				NewDecal->SetDecalSize(DecalSize);
				if (UMaterialInstanceDynamic* DecalMID = NewDecal->GetDecalMaterialInstance())
				{
					FLinearColor NewColor = bUnderAttack ? AttackColor : InRangeColor;
					DecalMID->SetVectorParameterValue(DecalDynamicMaterialColorParamName, NewColor);
				}
				ActiveDefenseTowerAttackRangeDecals.Add(DefenseTowerName, NewDecal);

			}
		}
	}
}

void AArenasPlayerController::Client_HideTowerAttackRangeDecal_Implementation(const FName& DefenseTowerName)
{
	if (!IsLocalPlayerController()) return;
	if (AAttackRangeDecal* DefenseTowerDecal = ActiveDefenseTowerAttackRangeDecals.FindRef(DefenseTowerName))
	{
		DefenseTowerDecal->Destroy();
		ActiveDefenseTowerAttackRangeDecals.Remove(DefenseTowerName);
	}
}

void AArenasPlayerController::Client_ShowCombatText_Implementation(float ValueNumber, EArenasComboTextType TextType, const FVector& HitLocation)
{
	if (!IsLocalPlayerController()) return;
	BP_ShowCombatText(ValueNumber, TextType, HitLocation);
}

void AArenasPlayerController::Client_MatchFinished_Implementation(AActor* ViewTarget, FGenericTeamId LooseTeamID)
{
	// if (!IsLocalPlayerController()) return;
	
	bIsWin = (TeamID != LooseTeamID);
	SetViewTargetWithBlend(ViewTarget, FinishedMatchViewBlendTimeDuration);

	// PlayerUIWidget->ShowMatchResult(MatchResultText);
	FTimerHandle ShowMatchResultTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ShowMatchResultTimerHandle, this, &AArenasPlayerController::ShowMatchFinishedState, FinishedViewBlendTimeDuration, false);
	
}


void AArenasPlayerController::ShowMatchFinishedState()
{
	PlayerUIWidget->ShowMatchResult(bIsWin);
}


