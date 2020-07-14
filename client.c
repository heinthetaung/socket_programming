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
#include "socket.h"

char recvBuff[1024];

void signal_handler(int signal) {
    socket_signalHandler(signal);
    if (signal == SIGQUIT) {
        //quit_sig = 1;
        printf("\n Got Quit Signal\n");
    } else if ((signal == SIGINT) || (signal == SIGTERM)) {
        //exit_sig = 1;
        printf("\n Got Exit Signal\n");
        exit(0);
    }

    //pclose(fp);
}

void send_event(char *tag_id, char cmd, char * image) {
    char data[512] = {0};
    char event_time[512] = {0};
    char jsonString[512] = {0};

    get_formatted_time(event_time);
    int json_len = snprintf(jsonString, sizeof (jsonString), "%s", build_json(tag_id, event_time, image));
    int buff_len = prepare_data(data, cmd, jsonString, json_len);

    socket_send_data(data, buff_len);
}

#define cmd_len 2
#define cmd 254
int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);
    int n = 0;
    int ret;
    char data[7] = {0};
    data[0] = (char) 0x01;
    data[1] = 2;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = cmd;
    data[6] = 0;
    int buff_len = 7;

    
    send_event("0001", ID_2FA_SUCCESS, NULL);

    return 0;

}

