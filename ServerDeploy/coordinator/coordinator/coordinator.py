from flask import Flask, request, jsonify
import subprocess    
from Consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY

# docker 容器化
import re

app = Flask(__name__)

# 通过docker查询可用端口的函数
def GetUesedPorts():
    result = subprocess.run(['docker', 'ps', '--format', '{{.Ports}}'], capture_output=True, text=True)
    output = result.stdout

    usedPorts = set()

    for line in output.strip().split('\n'):
        matches = re.findall(r'0\.0\.0\.0:(\d+)->', line)
        # 提取端口号并添加到集合中
        usedPorts.update(map(int, matches))

    return usedPorts


def FindNextAvailablePort(startPort=7777, endPort=8000):
    usedPorts = GetUesedPorts()
    for port in range(startPort, endPort + 1):
        if port not in usedPorts:
            return port
    raise 0

def CreateServerImpl(sessionName, sessionSearchID):
    port = FindNextAvailablePort()
    subprocess.Popen(
        [
            'docker',
            'run',
            '--rm',                         # 容器停止后自动删除
            '-p', f'{port}:{port}/tcp',           # 端口映射(TCP)
            '-p', f'{port}:{port}/udp',           # 端口映射(UDP)
            'arenasserver',                 # 实际的Docker镜像名称
            '-server',                      # 启动服务器参数
            '-log',                         # 启动日志参数
            f'-epicapp="ServerClient"',     # 应用名称参数
            f'-SESSION_NAME="{sessionName}"',  # 会话名称参数
            f'-SESSION_SEARCH_ID="{sessionSearchID}"',  # 会话搜索ID参数
            f'-PORT={port}'                 # 端口参数
       ]
    )
    return port



# TODO: 在后续接入Docker容器化后，此处应该被删除（因为不再需要本地启动服务器）
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

    # port = CreateServerLocalTest(sessionName, sessionSearchID)
    port = CreateServerImpl(sessionName, sessionSearchID)

    return jsonify({"status": "Session created successfully", PORT_KEY: port}), 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)

