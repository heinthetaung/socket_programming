/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
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
#include <time.h>
#include "cJSON.h"
#include <stdarg.h>
#include <pthread.h>
#include "socket.h"

int sockfd = 0;
int reinit = 0;
char server_ip[] = SERVER_IP;
int server_port = SERVER_PORT;
char recvBuff[1024];
char sendBuff[1024];
pthread_mutex_t socket_lock;

void socket_signalHandler(int signal) {
    close(sockfd);
    if (signal == 13) {
        reinit = 1;
    }
}

char * build_json(char * tag_id, char * event_time, char * image) {
    cJSON *object = cJSON_CreateObject();

    if (tag_id != NULL) {
        cJSON_AddStringToObject(object, "TAGID", tag_id);
    } else {
        cJSON_AddStringToObject(object, "TAGID", "");
    }

    if (event_time != NULL) {
        cJSON_AddStringToObject(object, "AT", event_time);
    }

    if (image != NULL) {
        cJSON_AddStringToObject(object, "IMAGE", image);
    }
    return cJSON_PrintUnformatted(object);

}

char * build_beaconJSON(char * tag_id, char * bat_status) {
    cJSON *object = cJSON_CreateObject();

    if (tag_id != NULL) {
        cJSON_AddStringToObject(object, "TAGID", tag_id);
    } else {
        cJSON_AddStringToObject(object, "TAGID", "");
    }

    if (bat_status != NULL) {
        cJSON_AddStringToObject(object, "BAT", bat_status);
    }
    return cJSON_PrintUnformatted(object);
}

void get_formatted_time(char * time_tmp) {
    FILE *fp;
    fp = popen("date +\"%d/%m/%Y %H:%M:%S\"", "r");
    if (fp == NULL) {
        printf("cannot open process\n");
        exit(EXIT_FAILURE);
    } else {
        char time[512];
        //printf("Reading and comparing buffer\n");
        if (fgets(time, sizeof (time), fp) > 0) {
            char *pos;
            if ((pos = strchr(time, '\n')) != NULL)
                *pos = '\0';
            strcpy(time_tmp, time);
        } else {
            printf("fgets error\n");
        }
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
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        printf("\n inet_pton error occured\n");
        exit(EXIT_FAILURE);
    }

    if (ret = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("Error : Connect Failed, Error code: %d\n", ret);
        exit(EXIT_FAILURE);
    } else {
        printf("Socket Connected\n");
    }

}

void send_heartbeat_event(char cmd) {
    char data[7] = {0};
    data[0] = (char) 0x01;
    data[1] = cmd_len;
    data[5] = cmd;
    data[6] = 0;
    socket_send_data(data, 7);
}

void socket_send_data(char * data, int buff_len) {
    int ret = 0;
    //    pthread_mutex_lock(&socket_lock); 
    socket_connect();
    ret = write(sockfd, data, buff_len);
    if (ret < 0) {
        printf("Write Error - code: %d\n", ret);
    }
    close(sockfd);
    //    pthread_mutex_unlock(&socket_lock); 
    sleep(1);
}

void socket_sendDataFromServer(char * data, int buff_len) {
    int ret = 0;
    //    pthread_mutex_lock(&socket_lock); 
    //    socket_connect();
    ret = write(sockfd, data, buff_len);
    if (ret < 0) {
        printf("Write Error - code: %d\n", ret);
    }
    //    close(sockfd);
    //    pthread_mutex_unlock(&socket_lock); 
    sleep(1);
}

void socket_receive_data(void) {
    //    Receive data
    int n;
    while ((n = read(sockfd, recvBuff, sizeof (recvBuff) - 1)) > 0) {
        recvBuff[n] = 0;
        //printf("n: %d\n", n);
        if (fputs(recvBuff, stdout) == EOF) {
            printf("\n Error : Fputs error\n");
        }
    }
}

int prepare_data(char * data, char cmd, char * jsonString, int json_len) {

    memcpy(data + data_offset + cmd_len, jsonString, json_len);
    data[0] = (char) 0x01;
    data[1] = cmd_len + json_len;
    data[5] = cmd;
    data[6] = 0;
    int buff_len = data_offset + cmd_len + json_len;
    return buff_len;
}
