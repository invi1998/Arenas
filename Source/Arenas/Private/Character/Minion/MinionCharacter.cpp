// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MinionCharacter.h"


void AMinionCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);
}

void AMinionCharacter::SetGoal(AActor* InGoal)
{
}

void AMinionCharacter::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}

void AMinionCharacter::PickSkinBasedOnTeamID()
{
	if (USkeletalMesh** Skin = TeamSkinMap.Find(GetGenericTeamId()))
	{
		GetMesh()->SetSkeletalMesh(*Skin);
	}
}
