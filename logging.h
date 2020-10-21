/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   logging.h
 * Author: xyz
 *
 * Created on August 4, 2020, 1:52 PM
 */

#ifndef LOGGING_H
#define LOGGING_H
#ifdef __cplusplus
extern "C" {
#endif

    void user_log_init(void);
    void user_write_log(const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif /* LOGGING_H */

