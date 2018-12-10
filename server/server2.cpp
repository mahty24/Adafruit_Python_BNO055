#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

using namespace std;

#define SERVER_PORT htons(50007)

int main() {

    char buffer[1000];
    _ssize_t n;

    int serverSock=socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = SERVER_PORT;
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    /* bind (this socket, local address, address length)
       bind server socket (serverSock) to server address (serverAddr).
       Necessary so that server can use a specific port */
    bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));

    // wait for a client
    /* listen (this socket, request queue length) */
    listen(serverSock,1);

    sockaddr_in clientAddr;
    socklen_t sin_size=sizeof(struct sockaddr_in);
    int clientSock=accept(serverSock,(struct sockaddr*)&clientAddr, &sin_size);

    bool failed = 0;

    while (!failed) {

        bzero(buffer, 1000);

        //receive a message from a client
        n = read(clientSock, buffer, 500);
        cout << "Confirmation code  " << n << endl;
        cout << "Server received:  " << buffer << endl;

        string data(buffer);

        unsigned long int aIndex = data.find('A');
        unsigned long int bIndex = data.find('B');
        unsigned long int endIndex = data.find('}');

        unsigned long  int astart = aIndex + 4;
        unsigned long int aend = bIndex - 3;
        unsigned long int bstart = bIndex + 4;
        unsigned long int bend = endIndex;

        string pwmA = data.substr(astart,aend-astart);
        string pwmB = data.substr(bstart,bend-bstart);

//        cout << "Roll = " << droll << " pitch = " << dpitch << " yaw = " << dyaw<< "\n" << endl;


        strcpy(buffer, "test");
        n = write(clientSock, buffer, strlen(buffer));
        cout << "Confirmation code  " << n << endl;

        if (n < 0)
        {
            cout << "Error reading from socket\n" << endl;
            failed = 1;
        }
//        printf("Here is the message: %s\n",buffer);
//        n = write(newsockfd,roll,3);
        if (n < 0)
        {
            cout << "Error writing to socket.\n" << endl;

            failed = 1;
        }
    }
    close(clientSock);
    close(serverSock);
    return 0;
}
