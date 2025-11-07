// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "PA_CharacterDefinition.h"

#include "ArenasCharacter.h"

FPrimaryAssetId UPA_CharacterDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetCharacterDefinitionAssetType(), GetFName());
}

FPrimaryAssetType UPA_CharacterDefinition::GetCharacterDefinitionAssetType()
{
	return FPrimaryAssetType("CharacterDefinition");
}

UTexture2D* UPA_CharacterDefinition::LoadCharacterIcon() const
{
	// 同步加载角色图标资源
	CharacterIcon.LoadSynchronous();
	if (CharacterIcon.IsValid())
	{
		return CharacterIcon.Get();
	}
	
	return nullptr;
}

TSubclassOf<AArenasCharacter> UPA_CharacterDefinition::LoadCharacterClass() const
{
	CharacterClass.LoadSynchronous();
	if (CharacterClass.IsValid())
	{
		return CharacterClass.Get();
	}
	return TSubclassOf<AArenasCharacter>();
}

TSubclassOf<UAnimInstance> UPA_CharacterDefinition::LoadCharacterDisplayAnimBP() const
{
	CharacterDisplayAnimBP.LoadSynchronous();
	if (CharacterDisplayAnimBP.IsValid())
	{
		return CharacterDisplayAnimBP.Get();
	}
	return TSubclassOf<UAnimInstance>();
}

USkeletalMesh* UPA_CharacterDefinition::LoadDisplaySkeletalMesh() const
{
	TSubclassOf<AArenasCharacter> CharacterBPClass = LoadCharacterClass();
	if (!CharacterBPClass) return nullptr;

	AArenasCharacter* CharacterCDO = CharacterBPClass->GetDefaultObject<AArenasCharacter>();
	if (!CharacterCDO) return nullptr;

	return CharacterCDO->GetMesh()->GetSkeletalMeshAsset();
	
}
