#ifndef _APP_PAGE_
#define _APP_PAGE_

#include <stdint.h>

enum app_page_screen {
    CLOCK_PAGE = 0,
    MESSAGE_PAGE,
    WALK_COUNTER,
    ALARM_CLOCK,
    CONN_PASS_PAGE,
    CHARGING_PAGE,
    DEBUG_PAGE
};

void page_disp_current(void);
extern enum app_page_screen current_screen;
extern _Bool page_should_render_every_frame;
extern _Bool page_keep_awake;
#endif
