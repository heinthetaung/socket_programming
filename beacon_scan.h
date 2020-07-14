/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   beacon_scan.h
 * Author: hein
 *
 * Created on 23 June, 2020, 1:16 AM
 */

#ifndef BEACON_SCAN_H
#define BEACON_SCAN_H

#ifdef __cplusplus
extern "C" {
#endif

    //code here
#include <unistd.h> //sleep func

    int beacon_filter(char * beacon_data);
    int beacon_get_rssi(char *beacon_data);
    pid_t proc_find(const char* name);
    void* beacon_detect_task(void*);

    extern int beacon_detected;


#ifdef __cplusplus
}
#endif

#endif /* BEACON_SCAN_H */

