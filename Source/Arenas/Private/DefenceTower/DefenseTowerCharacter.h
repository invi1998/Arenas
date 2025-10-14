// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Character/ArenasCharacter.h"
#include "DefenseTowerCharacter.generated.h"

class AArenasAIController;
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

	void SetTowerAttackRange(float NewRange);

	void SetDefenseTowerFaceGoal(AActor* NewFaceGoal);

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void BeginPlay() override;

private:
	virtual void OnRep_TeamID() override;

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
	AArenasAIController* OwnerAIC;

	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	UDecalComponent* GroundDecalComponent;

	void TowerBeginAttack(AActor* Actor);
	void TowerStopAttack(AActor* Actor);

	// Client
	UFUNCTION(Client, Reliable)
	void ClientTowerBeginAttack(AActor* Actor);

	UFUNCTION(Client, Reliable)
	void ClientTowerStopAttack(AActor* Actor);

};
