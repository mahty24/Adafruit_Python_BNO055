/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <stdbool.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    _Bool failed = 0;

    while(!failed)
    {
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);

        char *roll [] = {&buffer[9], &buffer[10], &buffer[11]};
        double droll = strtod(*roll,NULL);
        char *pitch [] = {&buffer[23], &buffer[24], &buffer[25]};
        double dpitch = strtod(*pitch,NULL);
        char *yaw [] = {&buffer[35], &buffer[36], &buffer[37]};
        double dyaw = strtod(*yaw,NULL);

//        printf("Roll = %s \n pitch = %s \n yaw = %s \n", *roll, *pitch, *yaw);
        printf("Roll = %3.2f pitch = %3.2f yaw = %3.2f \n", droll, dpitch, dyaw);
//        string pitch(&buffer[23], &buffer[26]);
//        string yaw(&buffer[35], &buffer[39]);

//        printf("Step 1: ");
        if (n < 0)
        {
            error("ERROR reading from socket");
            printf("Error reading from socket\n");
            failed = 1;
        }
        printf("Here is the message: %s\n",buffer);
        n = write(newsockfd,roll,3);
        if (n < 0)
        {
            error("ERROR writing to socket");
            printf("Error writing to socket.\n");

            failed = 1;
        }
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}