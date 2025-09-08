// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Interface/PawnUIInterface.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "ArenasCharacter.generated.h"

struct FGameplayEventData;
class UAIPerceptionStimuliSourceComponent;
class UArenasUserWidget;
class UWidgetComponent;
class UArenasAttributeSet;
class UArenasAbilitySystemComponent;

UCLASS()
class ARENAS_API AArenasCharacter : public ACharacter, public IAbilitySystemInterface, public IPawnUIInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArenasCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void ServerSideInit();
	void ClientSideInit();
	bool IsLocallyControlledByPlayer() const;

	bool IsAlive() const;
	void RespawnImmediately();

	bool IsStunning() const;

	// 当角色被控制器控制时调用(只会在服务端被调用)
	virtual void PossessedBy(AController* NewController) override;

	// Interface IPawnUIInterface Begin
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	FGenericTeamId GetOwningGenericTeamId() const;
	// Interface IPawnUIInterface End

	const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/********************************************************************************************/
	/*								 	Gameplay Ability										*/
	/********************************************************************************************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 标记为服务端可靠并且带有验证，以确保只有经过验证的客户端请求才能被服务器接受，意味着这是一个RPC(远程过程调用)函数
	// 即当客户端调用此函数时，它实际会通知服务器在服务端调用相同的函数
	// WithValidation 表示我们需要提供一个验证函数来验证调用请求的合法性
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendGameplayEventToSelf(const FGameplayTag& EventTag, const FGameplayEventData& Payload);

private:
	void DeathTagUpdated(FGameplayTag InGameplayTag, int32 NewCount);
	void StunTagUpdated(FGameplayTag GameplayTag, int32 NewCount);
	void BindGASChangedDelegate();
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	UArenasAbilitySystemComponent* ArenasAbilitySystemComponent;

	UPROPERTY()
	UArenasAttributeSet* ArenasAttributeSet;	// 属性集


	/********************************************************************************************/
	/*								 		UI													*/
	/********************************************************************************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UPawnUIComponent* PawnUIComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	UWidgetComponent* OverheadWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UArenasUserWidget> OverheadWidgetClass;

	UPROPERTY()
	UArenasUserWidget* OverheadWidget;

	void SpawnOverheadWidgetComponent();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float OverheadWidgetVisibilityCheckDelay = 1.f; // 显示头顶UI的检测时间

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float OverheadWidgetVisibilityRangeSquared = 1000000.f; // 头顶UI的显示范围，单位是平方
	
	FTimerHandle OverheadTimerHandle;
	void UpdateOverheadWidgetVisibility();
	void SetStatusGaugeEnabled(bool bEnabled);

	/********************************************************************************************/
	/*								 		Death and Respawn									*/
	/********************************************************************************************/
	FTransform MeshRelativeTransform;
	
	void StartDeathSequence();
	void Respawn();

	UPROPERTY(EditDefaultsOnly, Category = "Death and Respawn")
	float DeathMontageFinishTimeOffset = -0.8f; // 死亡动画播放完成后，延迟或者提前多少秒后触发布娃娃物理

	UPROPERTY(EditDefaultsOnly, Category = "Death and Respawn")
	UAnimMontage* DeathMontage;

	FTimerHandle DeathMontageTimerHandle;

	void DeathMontageFinished();
	void SetRagdollPhysics(bool bEnabled);

	void PlayDeathAnimation();

	virtual void OnDeath();
	virtual void OnRespawn();

	/********************************************************************************************/
	/*								 		Stun 眩晕											*/
	/********************************************************************************************/
	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	UAnimMontage* StunMontage;

	virtual void OnStun();
	virtual void OnUnStun();

public:
	/********************************************************************************************/
	/*								 		Team												*/
	/********************************************************************************************/
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamID)
	FGenericTeamId TeamID;

	UFUNCTION()
	virtual void OnRep_TeamID();

	/********************************************************************************************/
	/*								 			AI												*/
	/********************************************************************************************/
	void SetAIPerceptionStimuliSourceEnabled(bool bEnabled);	// 启用或禁用AI感知组件（在角色死亡和复活时）
	
	// AI感知组件
	UPROPERTY()
	UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent;
	
	
};

