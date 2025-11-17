// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "LobbyGameMode.h"

#include "Network/ArenasGameSession.h"

ALobbyGameMode::ALobbyGameMode()
{
	// 使用无缝旅行以保持玩家连接，以确保PlayerState能够在地图切换时保留，如果不使用无缝旅行，PlayerState将在每次切换地图时被销毁和重新创建
	// 如果是在编辑器中运行游戏，就算设置了使用无缝旅行，编辑器也不会支持无缝旅行功能，所以需要以单独游戏进程测试
	bUseSeamlessTravel = true;
	GameSessionClass = AArenasGameSession::StaticClass();
}
