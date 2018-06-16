#!/usr/bin/python3
from websocket import create_connection
from google.protobuf import text_format
import pbft_pb2
import json
import sys

timestamp = 1

for line in sys.stdin:
    request = pbft_pb2.pbft_request()
    request.client = "cli client"
    request.timestamp = timestamp
    timestamp += 1
    request.operation = line.strip()

    message = pbft_pb2.pbft_msg()
    message.request.CopyFrom(request)
    message.type = pbft_pb2.PBFT_MSG_TYPE_REQUEST

    encoded_message = text_format.MessageToString(message)
    wrapped_message = json.dumps({"bzn-api": "pbft", "pbft-data": encoded_message})

    ws = create_connection("ws://127.0.0.1:50000")
    print("Sending message: ", wrapped_message)
    ws.send(wrapped_message)
    ws.close()
