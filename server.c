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
#include "slog.h"
#include "socket.h"
#include "cJSON.h"

#define JSON_SIZE 10000000

void signal_handler(int signal) {
    printf("intHanlder(): signal %d\n", signal);
    socket_signalHandler(signal);
    if (signal == SIGQUIT) {
        //quit_sig = 1;
        printf("\n Got Quit Signal\n");
    } else if ((signal == SIGINT) || (signal == SIGTERM)) {
        //exit_sig = 1;
        printf("\n Got Exit Signal\n");
        exit(0);
    }

}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);

    pthread_t socket_server_taskthread_id;
    pthread_create(&socket_server_taskthread_id, NULL, socket_server_task, NULL);
    
    pthread_join(socket_server_taskthread_id, NULL);
}