// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Character/ArenasCharacter.h"
#include "MinionCharacter.generated.h"

UCLASS()
class ARENAS_API AMinionCharacter : public AArenasCharacter
{
	GENERATED_BODY()

public:
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	
private:
	virtual void OnRep_TeamID() override;
	
	// 根据队伍ID选择对应的皮肤
	void PickSkinBasedOnTeamID();
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TMap<FGenericTeamId, USkeletalMesh*> TeamSkinMap; // 队伍骨骼网格（皮肤）映射表，Key为队伍ID，Value为对应的骨骼网格
	
	
	
};
