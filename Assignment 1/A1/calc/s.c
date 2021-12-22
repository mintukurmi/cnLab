#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


int calc(char ch[255]);


int main(){

  int welcomeSocket, newSocket;

  char buffer[1024];
  int result;

  struct sockaddr_in serverAddr;

  struct sockaddr_in serverStorage;

  socklen_t addr_size;



  /*---- Create the socket. The three arguments are: ----*/

  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */

  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

  

  /*---- Configure settings of the server address struct ----*/

  /* Address family = Internet */

  serverAddr.sin_family = AF_INET;


  /* Set port number, using htons function to use proper byte order */

  serverAddr.sin_port = htons(7891);


  /* Set IP address to localhost */

  serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");


  /* Set all bits of the padding field to 0 */

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  


  /*---- Bind the address struct to the socket ----*/

  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));


  /*---- Listen on the socket, with 5 max connection requests queued ----*/

  if(listen(welcomeSocket,5)==0)

    printf("Listening\n");

  else

    printf("Error\n");

  /*---- Accept call creates a new socket for the incoming connection ----*/

 while(1) {

  addr_size = sizeof serverStorage;

  newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);

  /*---- Identify clients like this. The following information of client are taken from client due to connect function ----*/
  //Change of the following information at client side can not be done. However, padding field may be changed and that may be tried 

  struct sockaddr_in* cliIP = (struct sockaddr_in*)&serverStorage;

  struct in_addr ipAddr = cliIP->sin_addr;

  char str[INET_ADDRSTRLEN];

  inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);

  char* ID = cliIP->sin_zero;

  char str2[8];

  inet_ntop(AF_INET, &ID, str2, 8);


  int i;

  for (i=0;i<8;i++){
	printf("%c", serverStorage.sin_zero[i]);
  }

  /* ---- Receive message from client, if any ---- */

  recv(newSocket, buffer, 1024, 0);

  printf("\n From Client: calc %s\n", buffer);  


  /*---- Send message to the socket of the incoming connection ----*/

//   strcpy(res, calc(buffer));
    result = calc(buffer);

  send(newSocket,&result,4,0);

  close(newSocket);

 }
  return 0;

}


// function defs

int calc(char ch[255]){

int num1 = 0,num2 = 0;
char op;
int state = 0;
int result =0;

    int i=0;
    for (i = 0; i < strlen(ch); i++) {

            if (ch[i] != '\n'){

                if (ch[i] >= '0' && ch[i]  <= '9'){ //Checks if the character is a number
                    if (state == 0) 
                    num1 = num1*10 + ch[i] - '0'; // Convert ASCII to decimal
                    else 
                    num2 = num2*10 + ch[i] - '0'; // Convert ASCII to decimal
            }
            else{
                op = ch[i] ;
                state = 1;
            }

        }     

    }

       switch(op)
       {
         case '+':
            result = num1 + num2;
         break;
         case '-':
            result = num1 - num2;
         break;
         case '*':
            result = num1 * num2;
         break;
         case '/':
            result = num1 / num2;
         break;
       }


   return result;

}