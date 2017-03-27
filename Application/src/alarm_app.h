#ifndef _ALARM_APP_
#define _ALARM_APP_

#include <stdint.h>
#include "rtc_app.h"

typedef struct alarm_data{
    uint8_t hour;
    uint8_t minute;
    uint8_t repeat;
} alarm_data_item;


//#define ALARM_DELAY_MINUTES 1
//#define ALARM_DELAY_TIMES   3

/**
 * Function: Check if there is an alarm and alarm it
 * Params: date_t *date;
 * Return: none
 **/
void alarm_check(date_t * date);

void alarm_timer_handler(void *p_context);
/**
 * Function: Init alarm timeout timer
 * Params: 
 * Return: 
 **/
void alarm_timer_init(void);
/**
 * Function: Trig an alarm
 * Params:
 * Return:
 **/
void alarm_enter(void);
/**
 * Function: exit alarm state
 * Params:
 * Return:
 **/
void alarm_exit(void);
/**
 * Function: alarm on the next X minute
 * Params: 
 * Return: 
 **/
void alarm_delay_action(void);
/**
 * Function: for test only
 * Params:
 * Return:
 **/
void alarm_test(void);

#endif
