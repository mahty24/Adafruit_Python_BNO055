import socket
import sys
import json

HOST, PORT = "128.61.25.113", 50007

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))
for x in range(0, 100):
    # make sure the data is sent in 5 digit length
    print("Step 1")
    arr1 = format(1.22*x,'3.2f')
    arr2 = format(2.22 * x,'3.2f')
    someVar = format(300.22 - x,'3.2f')

    data = json.dumps({"r": float(arr1), "p": float(arr2), "y": float(someVar)})
    s.send(data.encode())
    print("Step 2")
    print(str(s.recv(1000)))
    print(x)