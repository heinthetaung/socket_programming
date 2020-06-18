#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <signal.h>

int sockfd = 0;
int reinit = 0;
char server_ip[]="0.0.0.0";
void intHandler(int signal) {
    printf("intHanlder(): signal %d\n", signal);
    close(sockfd);
    if(signal == 13) {
        reinit = 1;
    }
    
    if(signal == 2) {
        exit(signal);
    }
    
}

int main(int argc, char *argv[])
{
    signal(SIGINT, intHandler);
    signal(SIGPIPE, intHandler);
    int n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 
    int ret;
    if(ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed, Error code: %d\n", ret);
       return 1;
    } 

    while(1) {
        while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
        {
            recvBuff[n] = 0;
            printf("n: %d\n", n);
            if(fputs(recvBuff, stdout) == EOF)
            {
                printf("\n Error : Fputs error\n");
            }
        }
        close(sockfd);
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Error : Could not create socket \n");
            return 1;
        }
        if(ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
           printf("\n Error : Connect Failed, Error code: %d\n", ret);
           //return 1;
        } 
        printf("Read error code: %d\n", n);
        sleep(1);
    }
   

    printf("Received: %s\n", recvBuff);
    if(n < 0)
    {
        printf("\n Read error \n");
    } 

    return 0;
}
