from flask import Flask, request, jsonify
import subprocess    
from Consts import SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY, PORT_KEY

# docker 容器化
import re

app = Flask(__name__)

# TODO: 在后续接入Docker容器化后，此处应该被删除
nextAvailablePort = 7777

@app.route('/Session', methods=['POST'])
def CreateSession():
    print("CreateSession called, request data:", request.json)

    port = nextAvailablePort

    return jsonify({"status": "Session created successfully"}, {PORT_KEY: port}), 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)

