// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

UCLASS()
class ARENAS_API AProjectileActor : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void ShootProjectile(float InSpeed, float InMaxDistance, FVector InitialMoveDir, const AActor* InTargetActor, FGenericTeamId InInstigatorTeamID, FGameplayEffectSpecHandle InHitEffectSpecHandle, ETeamAttitude::Type InTeamAttitudeType = ETeamAttitude::Hostile);

	void TravelMaxDistanceReached();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY(Replicated)
	FVector MoveDirection;

	UPROPERTY(Replicated)
	float ProjectileSpeed;

	FGameplayEffectSpecHandle HitEffectSpecHandle;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_TeamAttitude)
	uint8 TeamAttitudeByte;

	UFUNCTION()
	void OnRep_TeamAttitude();

	// 投射物飞行目标，不需要网络复制，因为只在服务端使用，用于计算投射物的飞行方向，客户端只需要按照服务端发的飞行方向执行运动即可
	UPROPERTY()
	const AActor* TargetActor;

	FTimerHandle ProjectileTravelTimerHandle;

	ETeamAttitude::Type ProjectileTeamAttitudeType;		// 投射物的阵营态度

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Cue")
	FGameplayTag HitGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile" )
	bool bOnlyOverlapTargetActor = false;	// 是否只与目标Actor发生碰撞

	void SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult);

	void TriggerGameplayCue_GAS(AActor* HitActor, const FHitResult& HitResult);

};
