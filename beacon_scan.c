/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   beacon_scan.c
 * Author: hein
 *
 * Created on 23 June, 2020, 1:12 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <dirent.h>
#include <signal.h> //opendir func
#include "beacon_scan.h"

int beacon_detected;
/*
 * 
 */

pid_t proc_find(const char* name) {
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof (buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if (fp) {
            if (fgets(buf, sizeof (buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t) lpid;
                }
            }
            fclose(fp);
        }

    }

    closedir(dir);
    return -1;
}

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

int beacon_get_rssi(char *beacon_data) {
    int len = strlen(beacon_data);
    char rssi[2];
    rssi[0] = beacon_data[len - 2];
    rssi[1] = beacon_data[len - 1];
    int rssi_int = (int) strtol(rssi, NULL, 16);
    return rssi_int - 256;
}

int beacon_filter(char * beacon_data) {
    regex_t regex;
    int ret;
    char msgBuf[100];

    ret = regcomp(&regex, "^>043E2A0201.{18}0201.{10}0215.{44}", REG_EXTENDED); //total 90 characters
    if (ret != 0) {
        printf("Error Compiling Reg - error code: %d\n", ret);
        exit(EXIT_FAILURE);
    }
    ret = regexec(&regex, beacon_data, 0, NULL, 0);
    if (!ret) {
        //puts("Match");
    } else if (ret == REG_NOMATCH) {
        //puts("No match");
    } else {
        regerror(ret, &regex, msgBuf, sizeof (msgBuf));
        fprintf(stderr, "Regex match failed: %s\n", msgBuf);
        exit(1);
    }

    /* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);

    return ret;
}

void* beacon_detect_task(void* argv) {
    beacon_detected = 0;
    FILE *fp;
    pid_t hcitool_process_id = proc_find("hcitool");
    if (hcitool_process_id == -1) {
//        system("sudo systemctl restart bluetooth");
        int status = system("sudo hcitool lescan --duplicates > /dev/null &");
        if (status != 0) {
            printf("cannot start hcitool error\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("hcitool already running\n");
    }

    fp = popen("sudo hcidump --raw", "r");

    if (fp == NULL) {
        printf("cannot open process\n");
        exit(EXIT_FAILURE);
    } else {
        char beacon_info[2048];
        char buffer[1024];
        //printf("Reading and comparing buffer\n");
        while (fgets(buffer, sizeof (buffer), fp)) {
            //printf("Reading and comparing buffer\n");
            if (strstr(buffer, ">") != NULL) {
                char *pos;
                if ((pos = strchr(buffer, '\n')) != NULL)
                    *pos = '\0';

                strcpy(beacon_info, buffer);

            } else {
                char *pos;
                if ((pos = strchr(buffer, '\n')) != NULL)
                    *pos = '\0';

                if (buffer[0] == ' ' && buffer[1] == ' ') {
                    memmove(buffer, buffer + 2, strlen(buffer));
                }
                //printf("%s", buffer);
                strcat(beacon_info, buffer);

                remove_spaces(beacon_info);
                //printf("%s\n", beacon_info);
                if (beacon_filter(beacon_info) == 0) {
                    //printf("%s\n", beacon_info);
                    int rssi = beacon_get_rssi(beacon_info);

                    if (rssi > RSSI_THRESHOLD) {
                        printf("RSSI: %d\n", rssi);
                        beacon_detected = 1;
                    }
                }
            }

        }
        pclose(fp);
    }
}