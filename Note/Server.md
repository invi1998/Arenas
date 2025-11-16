# 下载源码

```shell
# 只克隆最后一次提交

$ git clone --release --depth 1 git@github.com:invi1998/UnrealEngine.git
```



## 运行服务端

```shell
ArenasServer.exe /Game/Arenas/Map/Lobby/L_Lobby?listen?port=7789 -log
```



## 运行客户端（以窗口模式）

```shell
Windows>Arenas.exe -log -window
```



## 服务端运行脚本

`%UNREAL_EDITOR% `是一个新加的环境变量，用于指向UE源码里的编辑器启动位置，如我这里指向的就是 `F:/UE_Source/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe`，所以他两是可以进行等效替换的，不过个人不喜欢硬编码写死，所以用环境变量来指代

```shell
%UNREAL_EDITOR% ^
%~dp0../Arenas.uproject ^
-server ^
-log ^
-epicapp="ServerClient" ^
-SESSION_NAME="TestSession" ^
-SESSION_SEARCH_ID="asdfasdfasdf" ^
-PORT=7789

```

## 客户端运行脚本

```shell
%UNREAL_EDITOR% ^
%~dp0../Arenas.uproject ^
-game ^
-log ^
-epicapp="GameClient" ^

```

