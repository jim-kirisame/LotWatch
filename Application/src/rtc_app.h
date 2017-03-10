#ifndef _RTC_APP_
#define _RTC_APP_

#include <stdint.h>

#define RTC_UNIX_TS_BASE 1483200000
#define RTC_WEEK_BASE 0

typedef struct time_t{
    uint32_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t week;
} date_t;

void rtc_init(void);
void rtc_setTime(date_t * date);
void rtc_getTime(date_t * date);
void rtc_setTimeUnix(uint32_t unixts);

#endif
