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
int listenfd = 0;
int reinit = 0;
char sendBuff[1025];

void intHandler(int signal) {
    printf("intHanlder(): signal %d\n", signal);
    close(listenfd);
    close(connfd);

    if (signal == 13) {
        reinit = 1;
    }

    if (signal == 2) {
        exit(signal);
    }

}

void socket_listen(void) {
    int yes = 1; // For setsockopt() SO_REUSEADDR, below
    struct sockaddr_in serv_addr;
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof (serv_addr));
    memset(sendBuff, '0', sizeof (sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    // Lose the pesky "address already in use" error message - ref: beej's guide
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int));

    bind(listenfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr));
    printf("listening on socket\n");
    listen(listenfd, 10);

    connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
    printf("connection accepted\n");
}

int main(int argc, char *argv[]) {
    signal(SIGINT, intHandler);
    signal(SIGPIPE, intHandler);
    
    time_t ticks;
    socket_listen();

    int ret;
    while (1) {
        if (reinit == 1) {
            reinit = 0;
            socket_listen();
        }

        ticks = time(NULL);
        snprintf(sendBuff, sizeof (sendBuff), "%.24s\r\n", ctime(&ticks));
        int buff_len = strlen(sendBuff);
        //printf("buffer length: %d\n", buff_len);
        ret = write(connfd, sendBuff, buff_len);
        if (ret < 0) {
            printf("Write Error - code: %d\n", ret);
        } 
//        else {
//            printf("Write Fun - Return code: %d\n", ret);
//        }
        sleep(1);
    }
    printf("exiting program");
    exit(EXIT_FAILURE);
    return 0;
}

