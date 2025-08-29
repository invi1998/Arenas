// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasAnimNotify_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "GameplayTagsManager.h"

void UArenasAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp->GetOwner()) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!OwnerASC) return;
	
	FGameplayEventData EventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventData);
}

FString UArenasAnimNotify_SendGameplayEvent::GetNotifyName_Implementation() const
{
	return UArenasBlueprintFunctionLibrary::NativeGetGameplayTagLastName(EventTag).ToString();
}
