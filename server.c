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
#include "db.h"
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

int check_pin(char * pin) {
    int ret = 0;
    char *result[100];
    int count;
    //    char pincodes[100][6]={0};
    cJSON * pincode = NULL;
    cJSON * json = NULL;
    get_pin_from_whitelist("0001", result, &count);
    for (int i = 0; i < count; i++) {
        json = cJSON_Parse(result[i]);
        pincode = cJSON_GetObjectItemCaseSensitive(json, "PIN");
        //        strcpy(pincodes[i], pincode ->valuestring);
        if (strcmp(pincode ->valuestring, pin) == 0) {
            ret++;
        }
    }
    return ret;
}

long int convert_epoch_time(char * time) {
    FILE *fp;
    char cmd[100];
    if (strcmp(time, "now") == 0) {
        fp = popen("date +\"%s\"", "r");
    } else {
        snprintf(cmd, 100, "date --date='%s' +\"%%s\"", time);
        fp = popen(cmd, "r");
    }

    if (fp == NULL) {
        printf("cannot open process\n");
        exit(EXIT_FAILURE);
    }
    else {
        char time[100];
        //printf("Reading and comparing buffer\n");
        if (fgets(time, sizeof (time), fp) > 0) {
            char *pos;
            if ((pos = strchr(time, '\n')) != NULL)
                *pos = '\0';
            return atoi(time);
        } else {
            printf("fgets error\n");
        }
    }
    return -1;
}

int check_time(char *start_time, char * end_time) {
    long int start_epoch;
    long int end_epoch;
    long int curr_epoch;

    start_epoch = convert_epoch_time(start_time);
    end_epoch = convert_epoch_time(end_time);
    curr_epoch = convert_epoch_time("now");

    if(curr_epoch > start_epoch && curr_epoch < end_epoch) {
        return 0; //current time is in between start and end
    }
    printf("Start Time: %ld\nEnd Time: %ld\nNow: %ld\n", start_epoch, end_epoch, curr_epoch);
    return -1;
}

int check_schedule() {
    int ret = 0;
    char *result[100];
    int count;
    cJSON * json = NULL;
    cJSON * startJSON = NULL;
    cJSON * endJSON = NULL;
    char start_time[100][20];
    char end_time[100][20];
    get_schedule(result, &count);
    for (int i = 0; i < count; i++) {
        json = cJSON_Parse(result[i]);
        startJSON = cJSON_GetObjectItemCaseSensitive(json, "START");
        endJSON = cJSON_GetObjectItemCaseSensitive(json, "END");
        strcpy(start_time[i], startJSON ->valuestring);
        strcpy(end_time[i], endJSON ->valuestring);
        if (check_time(start_time[i], end_time[i]) == 0) {
            ret++;
        }
    }
    printf("No of schedule %d\n", count);
    return ret;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);

    pthread_t socket_server_taskthread_id;
    pthread_create(&socket_server_taskthread_id, NULL, socket_server_task, NULL);

    //    printf("Pin matched %d\n", check_pin("123456"));
    printf("Within the schedule %d times\n", check_schedule());

    pthread_join(socket_server_taskthread_id, NULL);
}