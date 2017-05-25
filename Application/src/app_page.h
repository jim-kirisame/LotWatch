#ifndef _APP_PAGE_
#define _APP_PAGE_

#include <stdint.h>

enum app_page_screen {
    CLOCK_PAGE = 0x80,
    MESSAGE_PAGE = 0x81,
    WALK_COUNTER_PAGE,
    ALARM_CLOCK_PAGE,
    ALARM_DISP_PAGE,
    CONN_PASS_PAGE,
    CHARGING_PAGE,
    DEBUG_PAGE = 0x87,
    ACC_PRINT_PAGE
};

void page_disp_current(void);
#endif
