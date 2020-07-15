/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   socket.h
 * Author: hein
 *
 * Created on 28 June, 2020, 3:12 PM
 */

#ifndef SOCKET_H
#define SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

//#define SERVER_IP "192.168.1.134"
        #define SERVER_IP  "0.0.0.0"
    //    #define SERVER_IP "10.0.105.10"
    //#define SERVER_IP  "10.0.0.37"

#define SERVER_PORT 35001

#define ID_STARTUP 1
#define ID_DOOR_OPEN 10
#define ID_DOOR_CLOSE 11
#define ID_UNLOCK_FAIL 12
#define ID_LOCK_FAIL 13
#define ID_2FA_SUCCESS 14
#define ID_2FA_NOT_MATCH 15
#define ID_3RD_ERR 16
#define ID_TAMPER 17
#define ID_TAMPER_OK 18
#define ID_UNAUTHORIZED 19
#define ID_OVERWRITE 20
#define ID_LOWBAT 21
#define ID_CAMERA_ERROR 22
#define ID_1FA_SUCCESS 23
#define ID_RESTARTED 24
#define ID_ROUTING 100
#define ID_ACCESS_ERR 253
#define ID_HEART_BEAT 254

#define ID_TAG_ENROLL 222

#define data_offset 5
#define cmd_len 2

    void socket_signalHandler(int signal);
    void socket_send_data(char * data, int buff_len);
    int prepare_data(char * data, char cmd, char * jsonString, int json_len);
    char * build_beaconJSON(char * tag_id, char * bat_status);
    char * build_json(char * tag_id, char * event_time, char * image);
    void get_formatted_time(char * time_tmp);
    void socket_connect(void);
    void send_heartbeat_event(char cmd);
    void socket_sendDataFromServer(char * data, int buff_len);
#ifdef __cplusplus
}
#endif

#endif /* SOCKET_H */

