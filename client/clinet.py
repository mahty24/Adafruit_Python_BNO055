import socket
import sys
import json


HOST, PORT = "localhost", 50007

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))
for x in range(100, 200):
    # make sure the data is sent in 5 digit length
    print("Step 1")
    arr1 = x
    arr2 = 2*x
    someVar = 300-x
    data = json.dumps({"roll": arr1, "pitch": arr2, "yaw": someVar})
    s.send(data.encode())
    print("Step 2")
    print(str(s.recv(1000)))
    print(x)
