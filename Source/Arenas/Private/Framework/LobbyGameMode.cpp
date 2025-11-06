// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "LobbyGameMode.h"

ALobbyGameMode::ALobbyGameMode()
{
	// 使用无缝旅行以保持玩家连接，以确保PlayerState能够在地图切换时保留，如果不使用无缝旅行，PlayerState将在每次切换地图时被销毁和重新创建
	bUseSeamlessTravel = true;
}
