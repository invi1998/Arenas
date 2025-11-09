// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"

class UCineCameraComponent;
class UPA_CharacterDefinition;

UCLASS()
class ARENAS_API ACharacterDisplay : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACharacterDisplay();

	void ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* InCharacterDefinition);
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	USkeletalMeshComponent* SkeletalMeshComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	UCineCameraComponent* ViewCameraComponent;
	
	

};
