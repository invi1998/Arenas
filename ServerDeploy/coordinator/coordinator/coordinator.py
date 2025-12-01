from flask import Flask, request, jsonify
import subprocess    
from Consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY

# docker 容器化
import re

app = Flask(__name__)

# 通过docker查询可用端口的函数
def GetUesedPortS():
    result = subprocess.run(['docker', 'ps', '--format', '{{.Ports}}'], capture_output=True, text=True)
    output = result.stdout
    print("Docker ps output:", output)


def CreateServerImpl(sessionName, sessionSearchID):
    port = GetUesedPortS()


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
    # app.run(host='0.0.0.0', port=80)
    CreateServerImpl("TestSession", "TestSearchID")

