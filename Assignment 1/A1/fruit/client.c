// calc client
#include <stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


int main(int argc, char* argv[]){

  int clientSocket;

  char buffer[1024];

  int result;
  
  int mango_availble = -1;
  int apple_available = -1;

  char* fruit = argv[2];
  int qty = atoi(argv[3]);


  struct sockaddr_in serverAddr, clientAddr;

  socklen_t addr_size;


  /*---- Create the socket. The three arguments are: ----*/

  /* 1) Internet domain 2) Stream socket 3) Dlefault protocol (TCP in this case) */

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);


  /*---- Configure settings of the server address struct ----*/

  /* Address family = Internet */

  serverAddr.sin_family = AF_INET;


  /* Set port number, using htons function to use proper byte order */

  serverAddr.sin_port = htons(7891);


  /* Set IP address to localhost */

  serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");


  /* Set all bits of the padding field to 0 */

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  


  /*---- Connect the socket to the server using the address struct ----*/

  connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));



  /*---- Read the message from the server into the buffer ----*/

  recv(clientSocket, &apple_available, 4, 0);
  recv(clientSocket, &mango_availble, 4, 0);

  printf("\nFrom server: ");
  printf("\nMango available: %d \t Apple Available: %d\n", mango_availble, apple_available);

   /* ----  Send something to server ---- */

  strcpy(buffer, argv[1]);
  send(clientSocket,buffer, 1024, 0);
  strcpy(buffer, argv[2]);
  send(clientSocket,buffer, 1024, 0);
  send(clientSocket,&qty, 4, 0);

  // receive response fomr server
  recv(clientSocket, buffer, 1024, 0);
  printf("\nFrom server: %s\n\n", buffer);


  return 0;

}

