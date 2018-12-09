import socket
import sys
import json

HOST, PORT = "128.61.25.113", 50007

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))
for x in range(0, 100):
    # make sure the data is sent in 5 digit length
    print("Step 1")
    arr1 = x
    arr2 = 2 * x
    someVar = 300 - x
    data = json.dumps({"r": arr1, "p": arr2, "y": someVar})
    s.send(data.encode())
    print("Step 2")
    print(str(s.recv(1000)))
    print(x)