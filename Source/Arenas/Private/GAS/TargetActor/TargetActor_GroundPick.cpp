// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TargetActor_GroundPick.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Arenas/Arenas.h"
#include "Components/DecalComponent.h"
#include "Engine/OverlapResult.h"

ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(GetRootComponent());

}

void ATargetActor_GroundPick::ConfirmTargetingAndContinue()
{
	TArray<FOverlapResult> Overlaps;	// 用于存储重叠结果
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn); // 只查询Pawn类型
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(TargetAreaRadius); // 以目标区域半径创建一个球形碰撞体
	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		CollisionShape
	);

	TSet<AActor*> OverlappingActors;
	if (OwningAbility)
	{
		if (IGenericTeamAgentInterface* OwnerAsTeamAgent = Cast<IGenericTeamAgentInterface>(OwningAbility->GetAvatarActorFromActorInfo()))
		{
			for (const FOverlapResult& OverlapResult : Overlaps)
			{
				// 过滤掉防御塔
				if (UArenasBlueprintFunctionLibrary::IsTowerActor(OverlapResult.GetActor()))
				{
					continue;
				}
				
				if (bShouldTargetEnemies)
				{
					// 选择敌人
					if (OwnerAsTeamAgent->GetTeamAttitudeTowards(*OverlapResult.GetActor()) == ETeamAttitude::Hostile)
					{
						OverlappingActors.Add(OverlapResult.GetActor());
					}
				}
				if (bShouldTargetAllies)
				{
					// 选择友军
					if (OwnerAsTeamAgent->GetTeamAttitudeTowards(*OverlapResult.GetActor()) == ETeamAttitude::Friendly)
					{
						OverlappingActors.Add(OverlapResult.GetActor());
					}
				}
			}
		}
	}
	// 生成目标数据句柄
	FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(OverlappingActors.Array(), false);

	// 将技能施法位置也加入目标数据中
	FGameplayAbilityTargetData_SingleTargetHit* SingleTargetHitData_HitLocation = new FGameplayAbilityTargetData_SingleTargetHit();
	SingleTargetHitData_HitLocation->HitResult.ImpactPoint = GetActorLocation();
	TargetDataHandle.Add(SingleTargetHitData_HitLocation);

	// 广播目标数据准备好的委托（该委托会被AbilityTask_WaitTargetData监听到），并将目标数据句柄传递过去（WaitTargetDataTask->ValidData）
	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
	
}

void ATargetActor_GroundPick::SetTargetAreaRadius(float InRadius)
{
	TargetAreaRadius = InRadius;
	DecalComponent->DecalSize = FVector(InRadius, InRadius, InRadius);
}

void ATargetActor_GroundPick::SetTargetOptions(bool bInShouldTargetEnemies, bool bInShouldTargetAllies)
{
	bShouldTargetEnemies = bInShouldTargetEnemies;
	bShouldTargetAllies = bInShouldTargetAllies;
}

void ATargetActor_GroundPick::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 只在客户端发生
	if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
	{
		SetActorLocation(GetTargetPoint());
	}
	
}

FVector ATargetActor_GroundPick::GetTargetPoint() const
{
	if (!PrimaryPC || !PrimaryPC->IsLocalPlayerController()) return GetActorLocation();
	
	FHitResult HitResult;
	FVector ViewLoc;
	FRotator ViewRot;
	PrimaryPC->GetPlayerViewPoint(ViewLoc, ViewRot);	// 获取玩家视点位置和旋转
	FVector TraceEnd = ViewLoc + ViewRot.Vector() * TargetTraceDistance; // 视点方向向前延伸10000单位

	// 进行线性追踪（射线检测）
	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLoc, TraceEnd, ECC_Target);

	if (!HitResult.bBlockingHit)
	{
		// 如果没有击中任何物体，则返回延伸线的终点位置（即，如果角色视线朝着天空，那么此时定然不会有击中, 此时就以视线最远距离的正下方和地面的交界点作为Actor生成位置）
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceEnd, TraceEnd + FVector::DownVector * TNumericLimits<float>::Max(), ECC_Target);
	}

	if (!HitResult.bBlockingHit)
	{
		return GetActorLocation();
	}

	if (bShouldDrawDebugSphere)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, TargetAreaRadius, 32, FColor::Red, false, -1.f, 0, 2.f);
	}
	
	return HitResult.ImpactPoint;
	
}
