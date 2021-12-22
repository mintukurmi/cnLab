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

void writefile(int sockfd, FILE *fp);
void sendfile(FILE *fp, int sockfd);

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
        // close(sockfd); 

        char operation[BUFFSIZE] = {0};
        recv(connfd, operation, BUFFSIZE, 0);
    
        char filename[BUFFSIZE] = {0}; 
        
        if(strcmp(operation, "upload") == 0){
            if (recv(connfd, filename, BUFFSIZE, 0) == -1) {
            perror("Can't receive filename");
            exit(1);
        }

            FILE *fp = fopen(filename, "wb");
            if (fp == NULL) 
            {
                perror("Can't open file");
                exit(1);
            }

            char addr[INET_ADDRSTRLEN];
            printf("\n-------------------------------------------------------------\n");
            printf("Uploading file: %s by %s\n", filename, inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN));
            gettimeofday(&start_time, NULL); 
            writefile(connfd, fp);
            gettimeofday(&end_time, NULL); 
            fclose(fp);
            
            printf("Upload Success, Total Bytes = %ld\n", total);
            printf("Time taken: %ld secs \n\n", end_time.tv_sec - start_time.tv_sec);
        }

        if(strcmp(operation, "download") == 0){
            if (recv(connfd, filename, BUFFSIZE, 0) == -1) {
                perror("Can't receive filename");
                exit(1);
            }

            FILE *fp = fopen(filename, "rb");
            if (fp == NULL) 
            {
                perror("Can't open file");
                exit(1);
            }

            char addr[INET_ADDRSTRLEN];
            printf("\n-------------------------------------------------------------\n");
            printf("Downloading file: %s by %s\n", filename, inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN));

            gettimeofday(&start_time, NULL);
            sendfile(fp, connfd);
            gettimeofday(&end_time, NULL); 
            fclose(fp);

            printf("File download success, Total Bytes = %ld\n", total);
            printf("Time taken: %ld secs \n\n", end_time.tv_sec - start_time.tv_sec);
        }
        
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

// send file for download

void sendfile(FILE *fp, int sockfd) 
{
    int n; 
    char sendline[MAX_LINE] = {0}; 
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) 
    {
	    total+=n;
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }
        
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
}