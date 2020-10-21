/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   db.h
 * Author: hein
 *
 * Created on 20 July, 2020, 12:56 AM
 */

#ifndef DB_H
#define DB_H

#ifdef __cplusplus
extern "C" {
#endif

#define DB_NAME "D2FA"
#define DB_DATA_COLLECTION "DATA"
#define DB_EVENT_COLLECTION "EVENT"
#define DB_LOG_COLLECTION "LOG"

    int add_event_to_db(char *json, char *cmd);
    int read_event_from_db(char * result[], int * size);
    int delete_event_from_db(char *id);

    int db_insert_event_json(char * json, char * key, char * val, const char*db_name, const char*collection_name);
    
    int db_insert_json(char * json, char * key, char * val, const char*db_name, const char*collection_name);
    int db_delete_json(char * key, char * val, char * oid_string, const char*db_name, const char*collection_name);
    int db_query_json(char * key, char * val, char * result[], int * size, const char*db_name, const char*collection_name);

    int save_schedule(char *json);
    int delete_schedule(char *id);
    int get_schedule(char * result[], int *size);

    int save_whitelist(char * json);
    int delete_whitelist(char *id);
    int get_data_from_whitelist(char * tagid, char * result[], int *size);


#ifdef __cplusplus
}
#endif

#endif /* DB_H */

