#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_LINE 50
#define LINSTENPORT 7788
#define SERVERPORT 8877
#define BUFFSIZE 50
#define KEY 10

void sendfile(FILE *fp, int sockfd);
int bitwiseadd(int x, int y);
int bitwisesub(int x, int y);
void encryptData(FILE *fp1, FILE* fp2);
void printFile(FILE *fp);

ssize_t total=0;

int main(int argc, char* argv[])
{
    char buff[BUFFSIZE] = {0};
    struct timeval start_time;
    struct timeval end_time;

    // if (argc != 3) 
    // {
    //     perror("usage:./client upload <filepath>");
    //     exit(1);
    // }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("Can't allocate sockfd");
        exit(1);
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERPORT);
    serveraddr.sin_addr.s_addr = inet_addr("10.0.0.1");


    if (connect(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("Connect Error");
        exit(1);
    }
    
    // file pointers
    FILE *fp1 = fopen("plaintext.txt","r");
    FILE *fp2 = fopen("cipher.txt","r+");
    
    printf("\nPlain Text: \n");
    printFile(fp1);

    // Move the file pointer to the start.
    fseek(fp1, 0, SEEK_SET);

    // encrypting data
    encryptData(fp1, fp2);    

    // Move the file pointer to the start.
    fseek(fp2, 0, SEEK_SET);

    printf("\n\nCipher Text: \n");
    printFile(fp2);
    printf("\n\n");


    FILE *fp = fopen("cipher.txt", "rb");
    if (fp == NULL) 
    {
        perror("Can't open file");
        exit(1);
    }

    gettimeofday(&start_time, NULL); 
    sendfile(fp, sockfd);
    gettimeofday(&end_time, NULL);
   
    printf("Encrypted File Upload Success..");

    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    close(sockfd);
    return 0;
}

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

void encryptData(FILE *fp1, FILE* fp2){
    char ch;
    while ((ch = fgetc(fp1)) != EOF)
    {
        fputc(bitwiseadd(ch, KEY), fp2);
    }
}   

void printFile(FILE *fp){
   char ch;
    while ((ch = fgetc(fp)) != EOF)
    {
        printf("%c", ch);
    } 
}