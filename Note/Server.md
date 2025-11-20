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



## 启用EOS子系统

[Online Subsystem EOS Plugin in Unreal Engine | Unreal Engine 5.7 Documentation | Epic Developer Community](https://dev.epicgames.com/documentation/en-us/unreal-engine/online-subsystem-eos-plugin-in-unreal-engine)

```shell

[OnlineSubsystemEOS]
bEnabled=true

[OnlineSubsystem]
DefaultPlatformService=EOS

[/Script/Engine.Engine]
!NetDriverDefinitions=ClearArray
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/SocketSubsystemEOS.NetDriverEOSBase",DriverClassNameFallback="/Script/OnlineSubsystemUtils.IpNetDriver")
+NetDriverDefinitions=(DefName="DemoNetDriver",DriverClassName="/Script/Engine.DemoNetDriver",DriverClassNameFallback="/Script/Engine.DemoNetDriver")

# 下面这行是启用P2P套接字，因为我们使用的是专用服务器，所以，用不到这项配置，所以下面这里无需添加到项目ini里
[/Script/SocketSubsystemEOS.NetDriverEOSBase]
bIsUsingP2PSockets=true

```



## Python环境

用于创建 Python 虚拟环境的命令。虚拟环境是一种隔离的 Python 运行环境，可以让不同项目使用不同的依赖包和 Python 版本，而不会互相干扰。

```python
python -m venv .venv
```

运行python虚拟环境

```python
.\.venv\Scripts\activate
```



## 安装flask库

注意，我们是在python虚拟隔离环境下安装的库，所以该库不会被安装到系统环境里，只会在我们项目里存在

```shell
pip install flask
```



## 使用 HTTP 服务器在请求时启动一个服务器实例（本地启动）

```python
from flask import Flask, request, jsonify
import subprocess    
from Consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY

# docker 容器化
import re

app = Flask(__name__)

# TODO: 在后续接入Docker容器化后，此处应该被删除
nextAvailablePort = 7777

def CreateServerLocalTest(sessionName, sessionSearchID):
    global nextAvailablePort
    subprocess.Popen(
        [
            'F:/UE_Source/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe',
            'F:/study/Arenas/Arenas.uproject',
            '-server',
            '-log',
            '-epicapp="ServerClient"',
            f'-SESSION_NAME="{sessionName}"',
            f'-SESSION_SEARCH_ID="{sessionSearchID}"',
            f'-PORT={nextAvailablePort}',
        ]
    )
    usedPort = nextAvailablePort
    nextAvailablePort += 1
    return usedPort

@app.route('/Session', methods=['POST'])
def CreateSession():
    print("CreateSession called, request data:", request.json)

    sessionName = request.json.get(SESSION_NAME_KEY)
    sessionSearchID = request.json.get(SESSION_SEARCH_ID_KEY)

    port = CreateServerLocalTest(
        sessionName,
        sessionSearchID,
    )

    return jsonify({"status": "Session created successfully", PORT_KEY: port}), 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)


```

