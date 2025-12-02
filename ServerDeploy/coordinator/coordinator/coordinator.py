from flask import Flask, request, jsonify
import subprocess    
from Consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY

# docker 容器化
import re

app = Flask(__name__)

# 通过docker查询可用端口的函数
import socket

def GetUesedPorts():
    # 首先获取Docker容器使用的端口
    result = subprocess.run(['docker', 'ps', '--format', '{{.Ports}}'], capture_output=True, text=True)
    output = result.stdout

    usedPorts = set()

    for line in output.strip().split('\n'):
        matches = re.findall(r'0\.0\.0\.0:(\d+)->', line)
        usedPorts.update(map(int, matches))

    # 再检查系统上所有监听的端口
    try:
        # 使用netstat或ss命令检查所有监听端口
        result = subprocess.run(['ss', '-tln', '--numeric'], capture_output=True, text=True)
        if result.returncode == 0:
            for line in result.stdout.strip().split('\n'):
                if 'LISTEN' in line:
                    parts = line.split()
                    if len(parts) >= 4:
                        address = parts[3]
                        if ':' in address:
                            port_str = address.split(':')[-1]
                            if port_str.isdigit():
                                usedPorts.add(int(port_str))
    except:
        # 如果ss命令不可用，尝试netstat
        try:
            result = subprocess.run(['netstat', '-tln', '--numeric'], capture_output=True, text=True)
            if result.returncode == 0:
                for line in result.stdout.strip().split('\n'):
                    if 'LISTEN' in line:
                        parts = line.split()
                        if len(parts) >= 4:
                            address = parts[3]
                            if ':' in address:
                                port_str = address.split(':')[-1]
                                if port_str.isdigit():
                                    usedPorts.add(int(port_str))
        except:
            pass

    return usedPorts


def FindNextAvailablePort(startPort=7787, endPort=8100):
    usedPorts = GetUesedPorts()
    for port in range(startPort, endPort + 1):
        if port not in usedPorts:
            return port
    return 0

def CreateServerImpl(sessionName, sessionSearchID):
    port = FindNextAvailablePort()
    subprocess.Popen(
        [
            'docker',
            'run',
            '--rm',                         # 容器停止后自动删除
            '-p', f'{port}:{port}/tcp',           # 端口映射(TCP)
            '-p', f'{port}:{port}/udp',           # 端口映射(UDP)
            'server',                 # 实际的Docker镜像名称
            '-server',                      # 启动服务器参数
            '-log',                         # 启动日志参数
            f'-epicapp="ServerClient"',     # 应用名称参数
            f'-SESSION_NAME="{sessionName}"',  # 会话名称参数
            f'-SESSION_SEARCH_ID="{sessionSearchID}"',  # 会话搜索ID参数
            f'-PORT={port}'                 # 端口参数
       ]
    )
    return port


@app.route('/Session', methods=['POST'])
def CreateSession():
    print("CreateSession called, request data:", request.json)

    sessionName = request.json.get(SESSION_NAME_KEY)
    sessionSearchID = request.json.get(SESSION_SEARCH_ID_KEY)

    port = CreateServerImpl(sessionName, sessionSearchID)

    return jsonify({"status": "Session created successfully", PORT_KEY: port}), 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)


