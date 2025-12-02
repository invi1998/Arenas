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



## Docker容器化

安装linux

```shell
wsl --install -d Ubuntu
```

国内建议，直接安装到F盘

```shell
wsl --install --web-download --location F:\wsl
```

### 终端美化

> 使用 Shell 命令：
>
> ```
> curl -sS https://starship.rs/install.sh | sh
> ```
>
> 安装 Starship

### 安装Docker

```shell
sudo apt install docker.io
```

### 对sudo用户进行修改

**将当前登录用户添加到名为 `docker` 的用户组中。**

```shell
sudo usermod -aG docker $USERE
```

### 查看docker运行状态

```shell
sudo systemctl status docker
```

### 安装docker compose

.bashrc 下添加如下配置（环境变量）

```shell
export DOCKER_CONFIG="$HOME/.docker"
```

创建.docker文件夹，然后在它里面继续创建.cli-plugins文件夹

```shell
~
❯ mkdir .docker

~
❯ cd .docker/

~/.docker

~/.docker
❯ mkdir cli-plugins

~/.docker
❯ cd cli-plugins/

~/.docker/cli-plugins
❯
```

然后在这里下载Docker compose

```she
curl -SL https://github.com/docker/compose/releases/download/v2.40.3/docker-compose-linux-x86_64 -o docker-compose
```

将下载后的文件改为可执行模式

```shell
chmod +x docker-compose
```

编译dockerfile

```shell
Arenas/ServerDeploy/coordinator on  main
❯ docker build -t coordinator .
```

查看docker编译后的镜像

```sh
Arenas/ServerDeploy/coordinator on  main took 1m36s
❯ docker images
REPOSITORY    TAG         IMAGE ID       CREATED          SIZE
coordinator   latest      6a8e653c6d8d   22 seconds ago   169MB
<none>        <none>      aa96c07e393d   11 minutes ago   169MB
python        3.12-slim   445121148b18   12 days ago      119MB
```

没问题就运行docker文件

```shell
Arenas/ServerDeploy/coordinator on  main
❯ docker run --rm -p 80:80 coordinator
```

查看linux的ip

```shell
Arenas/ServerDeploy/coordinator on  main
❯ ip addr show eth0
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1280 qdisc mq state UP group default qlen 1000
    link/ether ...........
    inet xxxxxxxxx/20 brd 172.29.127.255 scope global eth0
       valid_lft forever preferred_lft forever
    inet6 fe80::215:5dff:fe64:d8ce/64 scope link
       valid_lft forever preferred_lft forever
```



这样我们就在Linux下的docker容器里运行了一个协调程序（专门用来监听游戏客户端的连接并在服务器里启动一个游戏服务端）

测试连接，找到打包好的项目游戏文件，用命令行携带参数的方法启动客户端

```shell
.\Arenas.exe -COORDINATOR_URL="1xxxxxx4" -TESTING_URL="17xxxxxxx:7777" -log
```



## 通过 Docker 查询已占用的端口

首先在Linux下准备好python环境，然后和Windows一样，我们也需要在文件夹里单独虚拟化python环境

```shell
❯ python3 -m venv .venv
```

然后激活启用虚拟环境

```shell
❯ source .venv/bin/activate
```

安装flask

```shell
> pip3 install flask
```



> 个人Linux环境问题（源），在构建coordinator镜像的时候使用国内网络构建，构建arenasserver镜像的时候需要挂载VPN

## 启动协调程序容器

我们在运行`coordinator`容器时使用了`-v /var/run/docker.sock:/var/run/docker.sock`，这会将宿主机的Docker套接字挂载到容器内，这样容器内就可以与宿主机的Docker守护进程通信。但是，这并不代表容器内安装了Docker客户端。

```shell
docker run --rm -p 80:80 -v /var/run/docker.sock:/var/run/docker.sock coordinator
```



## 使用 Docker Compose 定义和构建我们的镜像

首先移除docker现有的镜像

```shell
❯ docker rmi -f $(docker images -q)
```

创建docker_compose.yaml文件

```yaml
services:
  server:
    build: ./server
    image: server

  coordinator:
    build: ./coordinator
    image: coordinator
    container_name: servercoordinator
    volumes:
      - /var/run/docker.sock:/var/run/docker.sock
    ports:
      - "80:80"
```

然后构建compose

```shell
docker compose build
```

