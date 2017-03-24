#ifndef _APP_PAGE_
#define _APP_PAGE_

#include <stdint.h>

enum app_page_screen {
    CLOCK_PAGE = 0x80,
    MESSAGE_PAGE,
    WALK_COUNTER_PAGE,
    ALARM_CLOCK_PAGE,
    ALARM_DISP_PAGE,
    CONN_PASS_PAGE,
    CHARGING_PAGE,
    DEBUG_PAGE
};

void page_disp_current(void);
extern enum app_page_screen current_screen;
extern _Bool page_should_render_every_frame;
extern _Bool page_keep_awake;
#endif
