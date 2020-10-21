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
#include <stdarg.h>
#include <pthread.h>
#include "cJSON.h"
#include "slog.h"
#include "socket.h"
#include "db.h"

#define JSON_SIZE 10000000

//for listening on tcp port
int reinit_listen = 0;
int connfd = 0;
int listenfd = 0;
int socket_connected = 0;
char *recvBuff;
char *data;
int listener_port = LISTENER_PORT;


//for connecting to tcp socket
int sockfd = 0;
int reinit = 0;
char server_ip[32] = SERVER_IP;
int server_port = SERVER_PORT;
//char recvBuff[1024];
char sendBuff[1024];
pthread_mutex_t socket_lock;

void socket_signalHandler(int signal) {
    close(sockfd);
    if (signal == 13) {
        reinit = 1;
    }
    if (signal == 2) {
        socket_connected = 0;
        shutdown(connfd, 2);
        shutdown(listenfd, 2);
        close(connfd);
        close(listenfd);
    }
}

void send_json_event(int cmd, char * input_string) {
    char * data = malloc(sizeof (char) * JSON_SIZE);
    char * jsonString = malloc(sizeof (char) * JSON_SIZE);

    int json_len = snprintf(jsonString, sizeof (char) * JSON_SIZE, "%s", input_string);
    int buff_len = prepare_data(data, cmd, jsonString, json_len);
    //    printf("%s\n", jsonString);
    socket_send_data(data, buff_len);
    free(data);
    free(jsonString);
}

void send_event(char *tag_id, char cmd, char * image, char * battery) {
    char * data = malloc(sizeof (char) * JSON_SIZE);
    char event_time[512] = {0};
    char * jsonString = malloc(sizeof (char) * JSON_SIZE);

    get_formatted_time(event_time);
    int json_len = snprintf(jsonString, sizeof (char) * JSON_SIZE, "%s", build_json(tag_id, event_time, image, battery));
    int buff_len = prepare_data(data, cmd, jsonString, json_len);
    //    printf("%s\n", jsonString);
    socket_send_data(data, buff_len);
    free(data);
    free(jsonString);
}

void socket_listen(void) {
    int yes = 1; // For setsockopt() SO_REUSEADDR, below
    struct sockaddr_in serv_addr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof (serv_addr));
    memset(sendBuff, '0', sizeof (sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(listener_port);

    // Lose the pesky "address already in use" error message - ref: beej's guide
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int));

    bind(listenfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr));
    printf("listening on socket\n");
    listen(listenfd, 10);

    connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
    socket_connected = 1;
    printf("connection accepted\n");
}

char * build_json(char * tag_id, char * event_time, char * image, char * battery) {
    cJSON *object = cJSON_CreateObject();

    if (tag_id != NULL) {
        cJSON_AddStringToObject(object, "TAGID", tag_id);
    } else {
        cJSON_AddStringToObject(object, "TAGID", "");
    }

    if (event_time != NULL) {
        cJSON_AddStringToObject(object, "AT", event_time);
    } else {
        cJSON_AddStringToObject(object, "AT", "");
    }

    if (image != NULL) {
        cJSON_AddStringToObject(object, "IMAGE", image);
    } else {
        cJSON_AddStringToObject(object, "IMAGE", "");
    }

    if (battery != NULL) {
        cJSON_AddStringToObject(object, "BAT", battery);
    } else {
        cJSON_AddStringToObject(object, "BAT", "");
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
        //        printf("Socket Connected\n");
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
    pthread_mutex_lock(&socket_lock);
    socket_connect();
    ret = write(sockfd, data, buff_len);
    if (ret < 0) {
        printf("Write Error - code: %d\n", ret);
    }
    close(sockfd);
    pthread_mutex_unlock(&socket_lock);
    sleep(1);
}

void * socket_server_task(void* arg) {
    slog_init("socket_server", "slog.cfg", 1, 0);
    time_t ticks;
    socket_listen();
    recvBuff = (char*) malloc(JSON_SIZE * sizeof (char));
    data = (char*) malloc(JSON_SIZE * sizeof (char));
    memset(recvBuff, 0, JSON_SIZE * sizeof (char));
    memset(data, 0, JSON_SIZE * sizeof (char));
    int ret;
    int n = 0;
    int socket_len = 0;
    int cmd_id;
    int data_length;
    while (1) {
        if (reinit_listen == 1) {
            reinit_listen = 0;
            //            socket_listen();
            connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
        }
        memset(recvBuff, 0, JSON_SIZE * sizeof (char));
        memset(data, 0, JSON_SIZE * sizeof (char));
        n = 0;
        socket_len = 0;
        n = recv(connfd, (char*) recvBuff + socket_len, JSON_SIZE * sizeof (char) - 1 - socket_len, 0);
        socket_len += n;
        if (n > 0) {
            recvBuff[socket_len] = 0;
            while (n != 0) {
                n = recv(connfd, (char*) recvBuff + socket_len, JSON_SIZE * sizeof (char) - 1 - socket_len, 0);
                socket_len += n;
                if (n > 0) {
                    recvBuff[socket_len] = 0;
                }
            }
        } else if (n == 0) {
            printf("n == 0\n");
        } else {
            printf("Exit Failure\n");
            exit(EXIT_FAILURE);
        }

        char * recvBuffPtr = (char*) recvBuff;
        char * dataPtr = (char*) data;
        if (recvBuff[0] == 0x01) {
            printf("if (recvBuff[0] == 0x01)\n");
            printf("%02x %02x %02x %02x\n", (char) recvBuff[4], (char) recvBuff[3], (char) recvBuff[2], (char) recvBuff[1]);
            data_length = (int) ((((unsigned int) recvBuff[4] << 24) & 0xFF000000 |
                    ((unsigned int) recvBuff[3] << 16) & 0x00FF0000 |
                    ((unsigned int) recvBuff[2] << 8) & 0x0000FF00 |
                    ((unsigned int) recvBuff[1]) & 0x000000FF) - cmd_len);
            cmd_id = (int) ((uint16_t) recvBuff[6] << 8 | (uint8_t) recvBuff[5]);
            printf("socket_len: %d\n", socket_len);
            printf("cmd_id: %d\n", cmd_id);
            printf("Data length: %d \n", data_length);

            memcpy((char*) dataPtr, ((char*) recvBuffPtr) + data_offset + cmd_len, (JSON_SIZE * sizeof (char)) - data_offset - cmd_len);
            char * tmp = (char*) malloc(JSON_SIZE * sizeof (char));
            int tmp_len = snprintf(tmp, JSON_SIZE * sizeof (char), "%s\n", (char*) data);
            printf("Compare Length: %d\n", tmp_len - 1 + 7);
            //            printf("%s\n", data);
            //            fflush(stdout);
            //                                            int i=7;
            //                                            while(data_length) {
            //                                                
            //                                                data_length--;
            //                                                printf("%c", recvBuffPtr[i]);
            //                                                i++;
            //                                            }
            //                                            slog(0, SLOG_NONE, "Command ID: %d Data: %s", cmd_id, data);
        } else {
            printf("if (recvBuff[0] != 0x01)\n");
        }

        //        close(listenfd);
        //        close(connfd);
        reinit_listen = 1;
        if (data_length > 7) {
            cJSON * json = cJSON_Parse(data);
            cJSON * tagID = NULL;
            cJSON * scheduleID = NULL;
            cJSON * whitelistID = NULL;
            char *string = cJSON_PrintUnformatted(json);
//            db_insert_json(string, NULL);
//            printf("%s\n", string);
            switch (cmd_id) {
                case ID_CONTROLLER_RESYNC:
                    break;
                case ID_CONTROLLER_SEND_STATUS:
                    break;
                case ID_ADD_WHITELIST:
                    save_whitelist(string);
                    break;
                case ID_ADD_BLACKLIST:
                    whitelistID = cJSON_GetObjectItemCaseSensitive(json, "ID");
                    delete_whitelist(whitelistID ->valuestring);
                    break;
                case ID_ERASE_WHITELIST:
                    break;
                case ID_ADD_SCHEDULE:
                    save_schedule(string);
                    break;
                case ID_REMOVE_SCHEDULE:
                    scheduleID = cJSON_GetObjectItemCaseSensitive(json, "ID");
                    delete_schedule(scheduleID->valuestring);
                    break;
                case ID_FIRMWARE_UPDATE:
                    break;
                case ID_RESTART_CONTROLLER:
                    break;
                default:
                    printf("Invalid command ID\n");
                    break;
            }
            tagID = cJSON_GetObjectItemCaseSensitive(json, "TAGID");
            //        if (cJSON_IsString(tagID) && (tagID->valuestring != NULL)) {
            //            printf("Beacon ID: %s\n", tagID->valuestring);
            //        } else {
            //            printf("No TAG ID");
            //        }

            //            fflush(stdout);
            cJSON_free(scheduleID);
            cJSON_free(tagID);
            cJSON_free(json);
        }
        //        exit(EXIT_SUCCESS);
    }
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

int prepare_data(char * data, char cmd, char * jsonString, int json_len) {

    memcpy(data + data_offset + cmd_len, jsonString, json_len);
    //    printf("JSON Length: %d\n", json_len);
    int data_len = cmd_len + json_len;
    data[0] = (char) 0x01;
    data[1] = data_len & 0x000000FF;
    data[2] = (data_len & 0x0000FF00) >> 8;
    data[3] = (data_len & 0x00FF0000) >> 16;
    data[4] = (data_len & 0xFF000000) >> 24;
    data[5] = cmd;
    data[6] = 0;
    //    printf("Data Len: %.2x %.2x %.2x %.2x\n", data[4] , data[3] , data[2] ,data[1] );
    int buff_len = data_offset + data_len;
    return buff_len;
}
