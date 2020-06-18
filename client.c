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
char server_ip[] = "0.0.0.0";
char recvBuff[1024];

void intHandler(int signal) {
    printf("intHanlder(): signal %d\n", signal);
    close(sockfd);
    if (signal == 13) {
        reinit = 1;
    }

    if (signal == 2) {
        exit(signal);
    }

}

void socket_connect(void) {
    int ret = 0;
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, '0', sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        printf("\n inet_pton error occured\n");
        exit(EXIT_FAILURE);
    }

    if (ret = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("Error : Connect Failed, Error code: %d\n", ret);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, intHandler);
    signal(SIGPIPE, intHandler);
    int n = 0;

    socket_connect();

    while (1) {
        while ((n = read(sockfd, recvBuff, sizeof (recvBuff) - 1)) > 0) {
            recvBuff[n] = 0;
            //printf("n: %d\n", n);
            if (fputs(recvBuff, stdout) == EOF) {
                printf("\n Error : Fputs error\n");
            }
        }
        close(sockfd);
        socket_connect();
        printf("Read error - code: %d\n", n);
        sleep(1);
    }
    if (n < 0) {
        printf("\n Read error \n");
    }

    return 0;
}
