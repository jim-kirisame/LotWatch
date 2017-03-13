#include <stdio.h>
#include <string.h>
#include "main.h"
#include "app_page.h"

#include "ssd1306_app.h"
#include "disp_app.h"
#include "rtc_app.h"
#include "mma8452.h"
#include "temp_app.h"
#include "bas_app.h"

enum app_page_screen current_screen = CLOCK_PAGE;
_Bool page_should_render_every_frame;
_Bool page_keep_awake;

void page_disp_clock_page(void)
{
    //TODO: add battery icon
    char str[24];
    char weekstr[4];
    date_t date;
    rtc_getTime(&date);
    rtc_getWeekStr(weekstr, date.week);
    
    ssd1306_clearDisplay();
    
    snprintf(str, 24, "%02d:%02d", date.hour, date.minute);
    ssd1306_draw48Font(str);
    
    snprintf(str, 24, "%02d-%02d %s", date.month, date.day, weekstr);
    ssd1306_draw5x7Font((128-9*6)/2,7,str);
}

void page_disp_debug_page(void)
{
    char weekstr[4];
    char str2[24];
    mma8452_acc_data acc_data;
    
    date_t date;
    rtc_getTime(&date);
    rtc_getWeekStr(weekstr, date.week);
    
    mma8452_read_acc(&acc_data);
    temp_get();
    
    ssd1306_clearDisplay();
    
    snprintf(str2, 24, "acc_x: %d", acc_data.x);
    ssd1306_draw5x7Font(0,1,str2);
    snprintf(str2, 24, "acc_y: %d", acc_data.y);
    ssd1306_draw5x7Font(0,2,str2);
    snprintf(str2, 24, "acc_z: %d", acc_data.z);
    ssd1306_draw5x7Font(0,3,str2);
    snprintf(str2, 24, "temp: %.2f deg", temp_current_temp);
    ssd1306_draw5x7Font(0,4,str2);
    snprintf(str2, 24, "%d-%d-%d %d:%d:%d %s", date.year, date.month, date.day, date.hour, date.minute, date.second, weekstr);
    ssd1306_draw5x7Font(0,5,str2);
    snprintf(str2, 24, "bat: %d mV", bas_get_cur_bat_vot());
    ssd1306_draw5x7Font(0,6,str2);
}

void page_disp_pass_page()
{
    ssd1306_clearDisplay();
    ssd1306_draw16Font(str_passcode,37,3);
}

void page_disp_current()
{
    switch(current_screen){
        case CLOCK_PAGE:
            page_disp_clock_page();
            page_should_render_every_frame = false;
            page_keep_awake = false;
            break;
        case DEBUG_PAGE:
            page_disp_debug_page();
            page_should_render_every_frame = true;
            page_keep_awake = true;
            break;
        case CONN_PASS_PAGE:
            page_disp_pass_page();
            page_should_render_every_frame = false;
            page_keep_awake = false;
        default:
            page_should_render_every_frame = false;
            page_keep_awake = false;
            break;
            
    }
}

