// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasGameplayAbility.h"

UAnimInstance* UArenasGameplayAbility::GetOwnerAnimInstance() const
{
	if (const USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo())
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}

	return nullptr;
}
