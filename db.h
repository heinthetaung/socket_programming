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

#define DB_NAME "mydb"
#define COLL_NAME "mycoll"

    int db_insert_json(char * json, char * description);
    int db_delete_json(char * id);
    int db_query_json(char * key, char * val, char * result[], int * size);

    int save_schedule(char *json);
    int delete_schedule(char *id);
    int get_schedule(char * result[], int *size);

    int save_whitelist(char * json);
    int delete_whitelist(char *id);
    int get_pin_from_whitelist(char * tagid, char * result[], int *size);


#ifdef __cplusplus
}
#endif

#endif /* DB_H */

