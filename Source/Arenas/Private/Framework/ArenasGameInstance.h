// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ArenasGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void StartMatch();


private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevelMap;

	void LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad);
	
};
