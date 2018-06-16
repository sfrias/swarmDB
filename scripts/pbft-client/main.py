#!/usr/bin/python3
from websocket import create_connection
from google.protobuf import text_format
import pbft_pb2
import json

request = pbft_pb2.pbft_request()
request.client = "cli client"
request.timestamp = 1
request.operation = "iunno, something"
encoded_request = text_format.MessageToString(request)
wrapped_msg = json.dumps({"bzn-api": "pbft", "pbft-data": encoded_request})

print(wrapped_msg)

ws = create_connection("ws://127.0.0.1:50000")
ws.send(wrapped_msg)
ws.close()
