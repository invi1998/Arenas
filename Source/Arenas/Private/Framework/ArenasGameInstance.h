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
	virtual void Init() override;

	
/********************************************************/
/*					Session Server						*/
/********************************************************/
private:
	void CreateSession();		// 创建会话
	
	FString ServerSessionName;		// 服务器会话名称
	int SessionServerPort;			// 服务器会话端口
	

private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevelMap;

	void LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad);
	
};
