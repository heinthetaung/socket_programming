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
#include "cJSON.h"

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
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);

    char id[10];
    if (argc > 1 && argc < 3) {
        sprintf(id, "%s", argv[1]);
    } else {
        printf("Too many or few arguments\n");
        exit(EXIT_FAILURE);
    }

    send_json_event(ID_STARTUP, "{\"VER\":\"V1.0\"}");
    
    cJSON *object = cJSON_CreateObject();
    if (strcmp(id, "210") == 0) {

        cJSON_AddStringToObject(object, "ID", "1234");
        cJSON_AddStringToObject(object, "TAGID", "0001");
        cJSON_AddStringToObject(object, "AT", "2020/07/19 21:04:44");
        cJSON_AddStringToObject(object, "USER", "user1");
        cJSON_AddStringToObject(object, "PIN", "111111");
        send_json_event(ID_ADD_WHITELIST, cJSON_PrintUnformatted(object));

//        cJSON_free(object);
//        object = cJSON_CreateObject();
//
//        cJSON_AddStringToObject(object, "ID", "1234");
//        cJSON_AddStringToObject(object, "TAGID", "0001");
//        cJSON_AddStringToObject(object, "AT", "2020/07/19 21:04:44");
//        cJSON_AddStringToObject(object, "USER", "user1");
//        cJSON_AddStringToObject(object, "PIN", "222222");
//        send_json_event(ID_ADD_WHITELIST, cJSON_PrintUnformatted(object));
//
//        cJSON_free(object);
//        object = cJSON_CreateObject();
//
//        cJSON_AddStringToObject(object, "ID", "1234");
//        cJSON_AddStringToObject(object, "TAGID", "0001");
//        cJSON_AddStringToObject(object, "AT", "2020/07/19 21:04:44");
//        cJSON_AddStringToObject(object, "USER", "user1");
//        cJSON_AddStringToObject(object, "PIN", "123456");
//        send_json_event(ID_ADD_WHITELIST, cJSON_PrintUnformatted(object));
//        printf("ID %s sent\n", id);

    } else if (strcmp(id, "212") == 0) {
        cJSON_AddStringToObject(object, "ID", "1234");
        cJSON_AddStringToObject(object, "AT", "2020/07/19 21:04:44");
        send_json_event(ID_ADD_BLACKLIST, cJSON_PrintUnformatted(object));

        printf("ID %s sent\n", id);

    } else if (strcmp(id, "214") == 0) {
        cJSON_AddStringToObject(object, "AT", "2020/07/19 21:04:44");
        send_json_event(ID_ERASE_WHITELIST, cJSON_PrintUnformatted(object));

        printf("ID %s sent\n", id);

    } else if (strcmp(id, "216") == 0) {
        cJSON_AddStringToObject(object, "ID", "1234");
        cJSON_AddStringToObject(object, "AT", "2020/07/19 19:04:44");
        cJSON_AddStringToObject(object, "START", "2020/07/19 21:04:44");
        cJSON_AddStringToObject(object, "END", "2020/07/21 22:04:44");
        cJSON_AddStringToObject(object, "PRI", "1");
        cJSON_AddStringToObject(object, "MODE", "1");
        send_json_event(ID_ADD_SCHEDULE, cJSON_PrintUnformatted(object));

        printf("ID %s sent\n", id);

    } else if (strcmp(id, "217") == 0) {
        cJSON_AddStringToObject(object, "ID", "1234");
        cJSON_AddStringToObject(object, "AT", "2020/07/19 21:04:44");
        send_json_event(ID_REMOVE_SCHEDULE, cJSON_PrintUnformatted(object));

        printf("ID %s sent\n", id);

    } else if (strcmp(id, "218") == 0) {
        cJSON_AddStringToObject(object, "FTP", "firmware1.zip");
        send_json_event(ID_FIRMWARE_UPDATE, cJSON_PrintUnformatted(object));

        printf("ID %s sent\n", id);

    } else if (strcmp(id, "220") == 0) {
        cJSON_AddStringToObject(object, "AT", "2020/07/19 21:04:44");
        send_json_event(ID_RESTART_CONTROLLER, cJSON_PrintUnformatted(object));

        printf("ID %s sent\n", id);

    } else {
        printf("ID not found\n");
    }

    cJSON_free(object);
    //    send_event("0001", ID_2FA_SUCCESS, NULL);
    //    for(int i =0; i<4;i++) {
    //        send_heartbeat_event(ID_HEART_BEAT);
    //        send_event("0001")
    //    }
    //    send_heartbeat_event(ID_HEART_BEAT);

    return 0;

}

