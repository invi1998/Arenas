// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "Interface/PawnUIInterface.h"
#include "Types/ArenaStructTypes.h"
#include "TeslaCoilTower.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UGameplayAbility;
class UArenasUserWidget;
class UWidgetComponent;
class UArenasAttributeSet;
class UArenasAbilitySystemComponent;
class USphereComponent;

UCLASS()
class ARENAS_API ATeslaCoilTower : public AActor, public IGenericTeamAgentInterface, public IAbilitySystemInterface, public IPawnUIInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATeslaCoilTower();

	virtual UPawnUIComponent* GetPawnUIComponent() const override;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;

	bool IsAlive() const;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void ServerSideInit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(ReplicatedUsing = OnRep_TeamID)
	FGenericTeamId TeamID;

	UFUNCTION()
	virtual void OnRep_TeamID();
	
	UPROPERTY(EditDefaultsOnly, Category = "Tower")
	UStaticMeshComponent* TowerMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	UArenasAbilitySystemComponent* ArenasAbilitySystemComponent;

	UPROPERTY()
	UArenasAttributeSet* ArenasAttributeSet;	// 属性集
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UPawnUIComponent* PawnUIComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	UWidgetComponent* OverheadWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UArenasUserWidget> OverheadWidgetClass;

	UPROPERTY()
	UArenasUserWidget* OverheadWidget;

	void SpawnOverheadWidgetComponent();
	void StartDeathSequence();

	UPROPERTY()
	AActor* CurrentTargetActor; // 当前目标的Actor
	
	void SetAIPerceptionStimuliSourceEnabled(bool bEnabled);	// 启用或禁用AI感知组件（在角色死亡和复活时）
	
	// AI感知组件
	UPROPERTY()
	UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent;
};
