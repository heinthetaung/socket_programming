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
#include "cJSON.h"
#include "beacon_scan.h"
#include "socket.h"
#define JSON_SIZE 10000000

int socket_connected = 0;
int connfd = 0;
int listenfd = 0;
int reinit_listen = 0;
char sendBuff[1025];
char *recvBuff;
char *data;

void signal_handler(int signal) {
    printf("intHanlder(): signal %d\n", signal);
    //    socket_connected = 0;
    //    shutdown(connfd, 2);
    //    shutdown(listenfd, 2);
    //    close(connfd);
    //    close(listenfd);

    if (signal == 13) {
        //        reinit_listen = 1;
    }

    if (signal == 2) {
        socket_connected = 0;
        shutdown(connfd, 2);
        shutdown(listenfd, 2);
        close(connfd);
        close(listenfd);
        free(recvBuff);
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
    serv_addr.sin_port = htons(35001);

    // Lose the pesky "address already in use" error message - ref: beej's guide
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int));

    bind(listenfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr));
    printf("listening on socket\n");
    listen(listenfd, 10);

    connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
    socket_connected = 1;
    printf("connection accepted\n");
}

void send_eventFromServer(char *tag_id, char cmd, char * image) {
    char * data = malloc(sizeof (char) * JSON_SIZE);
    char event_time[512] = {0};
    char * jsonString = malloc(sizeof (char) * JSON_SIZE);

    if (socket_connected == 1) {
        //    get_formatted_time(event_time);
        int json_len = snprintf(jsonString, sizeof (char) * JSON_SIZE, "%s", build_beaconJSON(tag_id, "OK"));
        int buff_len = prepare_data(data, cmd, jsonString, json_len);
        printf("%s\n", jsonString);

        int ret = write(connfd, data, buff_len);
        if (ret < 0) {
            connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
            printf("Write Error - code: %d\n", ret);
        }
    }
    free(data);
    free(jsonString);
}

void * socket_server_task(void* arg) {
    while (1) {
        if (reinit_listen == 1) {
            reinit_listen = 0;
            socket_listen();
        }
        //        printf("In While\n");
        if (beacon_detected == 1) {
            beacon_detected = 0;
            printf("beacon_detected == 1\n");

            send_eventFromServer("0001", ID_TAG_ENROLL, NULL);
        }
    }

}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);

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

    pthread_t socket_server_thread_id;
    pthread_create(&socket_server_thread_id, NULL, socket_server_task, NULL);

    pthread_t beacon_det_thread_id;
    pthread_create(&beacon_det_thread_id, NULL, beacon_detect_task, NULL);



    pthread_join(beacon_det_thread_id, NULL);
    pthread_join(socket_server_thread_id, NULL);

    return 0;

    //    while (1) {
    //        if (reinit_listen == 1) {
    //            reinit_listen = 0;
    //            socket_listen();
    //        }
    //        memset(recvBuff, 0, JSON_SIZE * sizeof (char));
    //        memset(data, 0, JSON_SIZE * sizeof (char));
    //        n = 0;
    //        socket_len = 0;
    //        n = recv(connfd, (char*) recvBuff + socket_len, JSON_SIZE * sizeof (char) - 1 - socket_len, 0);
    //        socket_len += n;
    //        if (n > 0) {
    //            recvBuff[socket_len] = 0;
    //            while (n != 0) {
    //                n = recv(connfd, (char*) recvBuff + socket_len, JSON_SIZE * sizeof (char) - 1 - socket_len, 0);
    //                socket_len += n;
    //                if (n > 0) {
    //                    recvBuff[socket_len] = 0;
    //                }
    //            }
    //        } else if (n == 0) {
    //            //printf("n == 0\n");
    //        } else {
    //            printf("Exit Failure\n");
    //            exit(EXIT_FAILURE);
    //        }
    //
    //        char * recvBuffPtr = (char*) recvBuff;
    //        char * dataPtr = (char*) data;
    //        if (recvBuff[0] == 0x01) {
    //            printf("if (recvBuff[0] == 0x01)\n");
    //            printf("%02x %02x %02x %02x\n", (char) recvBuff[4], (char) recvBuff[3], (char) recvBuff[2], (char) recvBuff[1]);
    //            data_length = (int) ((((unsigned int) recvBuff[4] << 24) & 0xFF000000 |
    //                    ((unsigned int) recvBuff[3] << 16) & 0x00FF0000 |
    //                    ((unsigned int) recvBuff[2] << 8) & 0x0000FF00 |
    //                    ((unsigned int) recvBuff[1]) & 0x000000FF) - cmd_len);
    //            cmd_id = (int) ((uint16_t) recvBuff[6] << 8 | (uint8_t) recvBuff[5]);
    //            printf("socket_len: %d\n", socket_len);
    //            printf("cmd_id: %d\n", cmd_id);
    //            printf("Data length: %d \n", data_length);
    //
    //            memcpy((char*) dataPtr, ((char*) recvBuffPtr) + data_offset + cmd_len, (JSON_SIZE * sizeof (char)) - data_offset - cmd_len);
    //            char * tmp = (char*) malloc(JSON_SIZE * sizeof (char));
    //            int tmp_len = snprintf(tmp, JSON_SIZE * sizeof (char), "%s\n", (char*) data);
    //            printf("Compare Length: %d\n", tmp_len - 1 + 7);
    //            //            printf("%s\n", data);
    //            //            fflush(stdout);
    //            //                                            int i=7;
    //            //                                            while(data_length) {
    //            //                                                
    //            //                                                data_length--;
    //            //                                                printf("%c", recvBuffPtr[i]);
    //            //                                                i++;
    //            //                                            }
    //            //                                            slog(0, SLOG_NONE, "Command ID: %d Data: %s", cmd_id, data);
    //        } else {
    //            printf("if (recvBuff[0] != 0x01)\n");
    //        }
    //
    //        close(listenfd);
    //        close(connfd);
    //        reinit_listen = 1;
    //
    //        cJSON * json = cJSON_Parse(data);
    //        cJSON * tagID = NULL;
    //        //        char *string = cJSON_PrintUnformatted(json);
    //        //        printf("%s\n", string);
    //        tagID = cJSON_GetObjectItemCaseSensitive(json, "TAGID");
    //        if (cJSON_IsString(tagID) && (tagID->valuestring != NULL)) {
    //            printf("Beacon ID: %s\n", tagID->valuestring);
    //        } else {
    //            printf("No TAG ID");
    //        }
    //
    //        fflush(stdout);
    //        exit(EXIT_SUCCESS);
    //    }
}