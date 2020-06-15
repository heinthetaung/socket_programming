#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

int connfd = 0;

void intHandler(int signal) {
    printf("System exiting: signal %d\n", signal);
    close(connfd);
    exit(signal);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, intHandler);

    int listenfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof (serv_addr));
    memset(sendBuff, '0', sizeof (sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr));

    listen(listenfd, 10);
    connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
    int ret;
    while (1) {
        ticks = time(NULL);
        snprintf(sendBuff, sizeof (sendBuff), "%.24s\r\n", ctime(&ticks));
        ret = write(connfd, sendBuff, strlen(sendBuff));
        if (ret = !0) {
            printf("write error - error no: %d\n", ret);
        }
        sleep(1);
    }
    printf("exiting program");
    exit(EXIT_FAILURE);
    return 0;
}

