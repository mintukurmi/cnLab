#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_CONNECTIONS 5
#define BUF_SIZE 1024
#define NAME_LEN 255
#define MSG_LEN 2048

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[NAME_LEN];

void str_trim(char *arr, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        if (arr[i] == '\n')
        {
            arr[i] = '\0';
            break;
        }
    }
}

void *rcvHandler()
{
    char msg[MSG_LEN] = {};
    int rcv;

    while (1)
    {
        rcv = recv(sockfd, msg, BUF_SIZE, 0);

        if (rcv > 0)
        {
            printf("%s", msg);
        }
        else if (rcv == 0)
        {
            break;
        }
        bzero(msg, BUF_SIZE);
        rcv = 0;
    }
}

void *sndHandler()
{
    char buff[BUF_SIZE] = {};
    char msg[MSG_LEN] = {};

    while (1)
    {
        fgets(buff, BUF_SIZE, stdin);
        str_trim(buff, BUF_SIZE);

        if (strcmp(buff, "exit") == 0)
        {
            flag = 1;
            break;
        }
        else
        {
            sprintf(msg, "%s : %s\n", name, buff);
            send(sockfd, msg, strlen(msg), 0);
        }
        bzero(buff, BUF_SIZE);
        bzero(msg, MSG_LEN);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Port no found.\n");
        return -1;
    }

    int port = atoi(argv[1]);

    printf("Your name: ");
    fgets(name, NAME_LEN, stdin);
    str_trim(name, strlen(name));

    struct sockaddr_in ser_add;
    socklen_t cli_add_size;

    // socket settings
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    ser_add.sin_family = AF_INET;
    ser_add.sin_addr.s_addr = inet_addr("10.0.0.1");
    ser_add.sin_port = htons(port);

    // connect to server
    if (connect(sockfd, (struct sockaddr *)&ser_add, sizeof(ser_add)) < 0)
    {
        printf("connection error\n");
        return 0;
    }

    printf("-------------Connected to the server-----------\n");

    send(sockfd, name, NAME_LEN, 0);

    printf("--------------Chat Server Started-------------\n");

    pthread_t sendThread;
    pthread_t recvThread;

    if (pthread_create(&sendThread, NULL, &sndHandler, NULL) != 0)
    {
        printf("Error send thread\n");
        return -1;
    }

    if (pthread_create(&sendThread, NULL, &rcvHandler, NULL) != 0)
    {
        printf("Error recv thread\n");
        return -1;
    }

    while (1)
    {
        if (flag)
        {
            printf("\nBye\n");
            break;
        }
    }

    close(sockfd);

    return 0;
}