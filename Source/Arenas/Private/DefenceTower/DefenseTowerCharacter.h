// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Character/ArenasCharacter.h"
#include "DefenseTowerCharacter.generated.h"

UCLASS()
class ARENAS_API ADefenseTowerCharacter : public AArenasCharacter
{
	GENERATED_BODY()

public:
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;

	void SetTowerAttackRange(float NewRange);

	void SetDefenseTowerFaceGoal(AActor* NewFaceGoal);

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
	float DefaultAttackRange = 600.f; // 默认攻击范围

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TowerDefaultFaceGoalName = "TowerDefaultFaceGoal"; // 防御塔默认面向目标的标签
	
};
