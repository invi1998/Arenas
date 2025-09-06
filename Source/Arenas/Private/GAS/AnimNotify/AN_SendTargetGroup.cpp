// Ace of Arenas. (invi1998 All Rights Reserved)


#include "AN_SendTargetGroup.h"

#include "AbilitySystemBlueprintLibrary.h"

void UAN_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if (TargetSocketNames.Num() <= 1) return;

	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;

	FGameplayEventData PayloadData;
	for (int32 i = 1; i < TargetSocketNames.Num(); ++i)
	{
		// 堆分配内存
		FGameplayAbilityTargetData_LocationInfo* LocationInfo = new FGameplayAbilityTargetData_LocationInfo();

		FVector StartLocation = MeshComp->GetSocketLocation(TargetSocketNames[i-1]);
		FVector EndLocation = MeshComp->GetSocketLocation(TargetSocketNames[i]);

		LocationInfo->SourceLocation.LiteralTransform.SetLocation(StartLocation);
		LocationInfo->TargetLocation.LiteralTransform.SetLocation(EndLocation);

		// 添加到目标数据数组中，Add内部会使用智能指针进行管理，所以这里不需要担心内存泄漏
		// 同时也需要注意，因为Add内部会使用智能指针进行管理，所以上面我们创建的LocationInfo必须是原始指针
		PayloadData.TargetData.Add(LocationInfo);
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, PayloadData);
}
