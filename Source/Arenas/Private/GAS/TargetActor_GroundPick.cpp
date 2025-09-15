// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "GAS/TargetActor_GroundPick.h"

ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;
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
	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLoc, TraceEnd, ECC_Visibility);

	if (!HitResult.bBlockingHit)
	{
		// 如果没有击中任何物体，则返回延伸线的终点位置（即，如果角色视线朝着天空，那么此时定然不会有击中, 此时就以视线最远距离的正下方和地面的交界点作为Actor生成位置）
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceEnd, TraceEnd + FVector::DownVector * TNumericLimits<float>::Max(), ECC_Visibility);
	}

	if (!HitResult.bBlockingHit)
	{
		return GetActorLocation();
	}
	
	return HitResult.ImpactPoint;
	
}
