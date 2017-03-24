#ifndef _ALARM_APP_
#define _ALARM_APP_

#include <stdint.h>
#include "rtc_app.h"

typedef struct alarm_data{
    uint8_t hour;
    uint8_t minute;
    uint8_t repeat;
} alarm_data_item;

#define MAX_ALARM_COUNT 5
#define ALARM_DELAY_MINUTES 5
void alarm_check(date_t * date);
void alarm_timer_handler(void *p_context);

void alarm_timer_init(void);
void alarm_enter(void);
void alarm_exit(void);
void alarm_delay_action(void);
void alarm_test(void);

#endif
