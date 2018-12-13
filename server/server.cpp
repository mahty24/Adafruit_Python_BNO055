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
#include <stdlib.h>
#include <pigpio.h> // include GPIO library
#include <signal.h> // needed to clean up CTL C abort
#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define SERVER_PORT htons(50007)

#define pwmA 16
#define fwA 27
#define reA 22
#define pwmB 23
#define fwB 24
#define reB 25
#define servo 17


int duty;
// Called when CTL C or STOP button hit
static void err_handler (int sig){
gpioTerminate(); //release GPIO locks & resources
signal(SIGINT, SIG_DFL); //exit program
kill(getppid(), SIGINT); //kill it off
kill(getpid(), SIGINT);
exit(0);
}
static void exit_handler(void) {
gpioTerminate(); //release GPIO locks & resources on exit
}

void setAngle(int angle) {
gpioPWM(servo,angle/(18+2));
sleep(1);
gpioPWM(servo,0);
} 
int main(int argc, char *argv[])
{
   if (gpioInitialise()<0) return 1; // init I/O library
   signal (SIGQUIT, err_handler);// CTL C and STOP button
   signal (SIGINT, err_handler); // GPIO exit & cleanup
   signal (SIGTERM, err_handler);
   signal (SIGABRT, err_handler);
   atexit(exit_handler);  // exit handler cleanup 
   //IO code starts here
   gpioSetMode(pwmA, PI_OUTPUT); 
   gpioSetMode(pwmB, PI_OUTPUT);
   gpioSetMode(fwA, PI_OUTPUT);
   gpioSetMode(reA, PI_OUTPUT);
   gpioSetMode(fwB, PI_OUTPUT);
   gpioSetMode(reB, PI_OUTPUT);
   gpioSetMode(servo, PI_OUTPUT);
   
 
   gpioSetPWMrange(pwmA, 200);
   gpioSetPWMrange(pwmB, 200);
   
   gpioPWM(pwmA,0);
   gpioPWM(pwmB,0); 
   gpioServo(servo, 2500);
   //setAngle(180);
   
   //system("raspivid -o - -t 0 -hf -rot 270 -w 800 -h 400 -fps 24 | cvlc -vvv stream:///dev/stdin --sout '#standard{access=http,mux=ts,dst=:8160}' :demux=h264");
   ///////////
   char buffer[1000];
   int n;

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
   ////////
   //system("raspivid -o - -t 0 -hf -rot 270 -w 800 -h 400 -fps 24 | cvlc -vvv stream:///dev/stdin --sout '#standard{access=http,mux=ts,dst=:8160}' :demux=h264");

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

       string aside = data.substr(astart,aend-astart);
       
       string bside = data.substr(bstart,bend-bstart);
       
       int Aside = stoi(aside);
       int Bside = stoi(bside);

//        cout << "Roll = " << droll << " pitch = " << dpitch << " yaw = " << dyaw<< "\n" << endl;


       strcpy(buffer, "test");
       n = write(clientSock, buffer, strlen(buffer));
       cout << "Confirmation code  " << n << endl;

       if (n < 0)
       {
           cout << "Error reading from socket\n" << endl;
           failed = 1;
       }
//       printf("Here is the message: %s\n",buffer);
//       n = write(newsockfd,roll,3);
       if (n < 0)
       {
           cout << "Error writing to socket.\n" << endl;
           failed = 1;
        }
 printf("PWMA = %d\n", Aside);
     if ( Aside > 0 ) {
  gpioWrite(reA, PI_LOW);
         gpioWrite(fwA, PI_HIGH);
 } else {
  printf("in here reverse A\n");
  gpioWrite(fwA, PI_LOW);
  gpioWrite(reA, PI_HIGH); 
  Aside = -1*Aside;       
     }
     
     printf("PWMB = %d\n", Bside);
     if ( Bside > 0 ) {
  gpioWrite(reB, PI_LOW);
         gpioWrite(fwB, PI_HIGH);
 } else {
  printf("in here reverse B\n");
  gpioWrite(fwB, PI_LOW);
  gpioWrite(reB, PI_HIGH);
  Bside = -1*Bside;         
     }
     
     
     
     
     gpioPWM(pwmA, Aside);
     gpioPWM(pwmB, Bside);
     //gpioSetPWMrange(servo, 100);
     //gpioPWM(servo, 40); 
     
     //printf("The PWM Frequency for the servo is %d\n", gpioGetPWMfrequency(servo));
     //gpioSetPWMfrequency(servo, );


   }
   printf("Outside the loop!\n");
   gpioPWM(pwmA, 0);
   gpioPWM(pwmB, 0);
   gpioTerminate();
   return 0;
}