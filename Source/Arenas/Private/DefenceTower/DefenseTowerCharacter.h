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
	// Sets default values for this character's properties
	ADefenseTowerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tower")
	UStaticMeshComponent* TowerMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	

	
};
