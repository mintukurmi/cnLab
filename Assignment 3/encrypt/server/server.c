#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_LINE 50
#define LINSTENPORT 7788
#define SERVERPORT 8877
#define BUFFSIZE 50
#define KEY 10

void writefile(int sockfd, FILE *fp);
void printFile(FILE *fp);
void decryptData(FILE *fp1, FILE* fp2);
int bitwisesub(int x, int y);
int bitwiseadd(int x, int y);

ssize_t total=0;

int main(int argc, char *argv[]) 
{
    struct timeval start_time;
    struct timeval end_time;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        perror("Can't allocate sockfd");
        exit(1);
    }
    
    struct sockaddr_in clientaddr, serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("10.0.0.1");
    serveraddr.sin_port = htons(SERVERPORT);

    if (bind(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) 
    {
        perror("Bind Error");
        exit(1);
    }

    if (listen(sockfd, LINSTENPORT) == -1) 
    {
        perror("Listen Error");
        exit(1);
    }

    printf("\nServer Listening on port %d \n", SERVERPORT);

    while(1){
        socklen_t addrlen = sizeof(clientaddr);
        int connfd = accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);
        if (connfd == -1) 
        {
            perror("Connect Error");
            exit(1);
        }
        
        FILE *fp = fopen("cipher.txt", "wb");
        if (fp == NULL) 
        {
            perror("Can't open file");
            exit(1);
        }

        char addr[INET_ADDRSTRLEN];
        printf("\n-------------------------------------------------------------\n");
        printf("Uploading file: %s by %s\n", "PlainText", inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN));
        writefile(connfd, fp);
        fclose(fp);
        
        printf("Upload Success, Total Bytes = %ld\n", total);

        // file pointers
        FILE *fp1 = fopen("plaintext.txt","r+");
        FILE *fp2 = fopen("cipher.txt","r+");
        
        // decrypt text
        decryptData(fp2, fp1);
        sleep(3);

        // Move the file pointer to the start.
        fseek(fp1, 0, SEEK_SET);
        fseek(fp2, 0, SEEK_SET);

        printf("\n\nCipher Text: \n");
        printFile(fp2);
        printf("\n\n");

        printf("\n\nPlain Text: \n");
        printFile(fp1);
        printf("\n\n");

        close(connfd);
    }
    
    return 0;
}

// write file fn def
void writefile(int sockfd, FILE *fp)
{
    ssize_t n;
    char buff[MAX_LINE] = {0};
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0) 
    {
	    total+=n;
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }
        
        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, MAX_LINE);
    }
}


int bitwiseadd(int x, int y)
{
    while (y != 0)
    {
        int carry = x & y;
        x = x ^ y;
        y = carry << 1;
    }
    return x;
}

int bitwisesub(int x, int y)
{
    while (y != 0)
    {
        int carry = (~x) & y;
        x = x ^ y;
        y = carry << 1;
    }
    return x;
}

void decryptData(FILE *fp1, FILE* fp2){
    char ch;
    while ((ch = fgetc(fp1)) != EOF)
    {
        fputc(bitwisesub(ch, KEY), fp2);
    }
}   

void printFile(FILE *fp){
   char ch;
    while ((ch = fgetc(fp)) != EOF)
    {
        printf("%c", ch);
    } 
}