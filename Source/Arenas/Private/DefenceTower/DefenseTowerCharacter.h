// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Character/ArenasCharacter.h"
#include "DefenseTowerCharacter.generated.h"

class AArenasTowerAIController;
class UCameraComponent;
class USphereComponent;

UCLASS()
class ARENAS_API ADefenseTowerCharacter : public AArenasCharacter
{
	GENERATED_BODY()

public:
	ADefenseTowerCharacter();
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;

	void SetDefenseTowerFaceGoal(AActor* NewFaceGoal);

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void BeginPlay() override;

private:
	virtual void OnRep_TeamID() override;

	virtual void OnFinishDeathAnimation();
	virtual void OnDeath();

	// 根据队伍ID设置塔的材质颜色
	void PickSkinBasedOnTeamID();

	// 队伍ID与防御塔材质皮肤映射表
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TMap<FGenericTeamId, UMaterialInstance*> TowerMaterialMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName DefenseTowerAttackRange = "DefenseTowerAttackRange"; // 防御塔攻击范围感知标签
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float DefaultAttackRange = 800.f; // 默认攻击范围

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TowerDefaultFaceGoalName = "TowerDefaultFaceGoal"; // 防御塔默认面向目标的标签

	UPROPERTY()
	AArenasTowerAIController* TowerAIController;

	UPROPERTY(VisibleAnywhere, Category = "Range")
	USphereComponent* AttackRangeSphereComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	UDecalComponent* GroundDecalComponent;

	void TowerBeginAttack(AActor* Actor, bool bIsAttackTarget);
	void TowerStopAttack(AActor* Actor);

	void WhileSameTeamHeroInRangeTakeDamage(AActor* DamageSourceActor, float DamageValue);

	void UpdateTowerAttackTargetOnEnemyDeath(AActor* DeadActor);

	void SelectAttackTarget();
	
	UFUNCTION()
	void ActorEnterTowerAttackRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ActorExitTowerAttackRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	TArray<AActor*> SameTeamHeroInRange; // 范围内的同队伍英雄角色

	UPROPERTY()
	TArray<AActor*> EnemyTeamMinionsInRange; // 范围内的敌对队伍小兵角色

	UPROPERTY()
	TArray<AActor*> EnemyTeamHeroesInRange; // 范围内的敌对队伍英雄角色

	UPROPERTY()
	AActor* CurrentTargetActor;

};
