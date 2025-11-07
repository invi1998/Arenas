// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_CharacterDefinition.generated.h"

class AArenasCharacter;
/**
 * 
 */
UCLASS()
class ARENAS_API UPA_CharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetCharacterDefinitionAssetType();
	
	FString GetCharacterName() const { return CharacterName; }
	UTexture2D* LoadCharacterIcon() const;
	TSubclassOf<AArenasCharacter> LoadCharacterClass() const;
	TSubclassOf<UAnimInstance> LoadCharacterDisplayAnimBP() const;
	USkeletalMesh* LoadDisplaySkeletalMesh() const;
	

private:
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FString CharacterName;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> CharacterIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<AArenasCharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<UAnimInstance> CharacterDisplayAnimBP;

	
};
