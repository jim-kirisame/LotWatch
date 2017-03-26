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
#include "step_counter.h"
#include "config_storage.h"

void page_disp_clock_page(void)
{
    
    char str[24];
    char weekstr[4];
    date_t date;
    uint8_t batt = bas_get_cur_bat_lvl();
    rtc_getTime(&date);
    rtc_getWeekStr(weekstr, date.week);
    
    ssd1306_clearDisplay();
    
    snprintf(str, 24, "%02d:%02d", date.hour, date.minute);
    ssd1306_draw48Font(str);
    
    snprintf(str, 24, "%02d-%02d %s", date.month, date.day, weekstr);
    ssd1306_draw5x7Font((128-9*6)/2,7,str);
    
    //Draw battery icon
    ssd1306_drawByte(110, 0, 0xFF);
    ssd1306_drawByte(111, 0, 0x81);
    for(int i=0;i<10;i++)
    {
        if(batt>=(i*10+5))
            ssd1306_drawByte(112+i, 0, 0xbd);
        else
            ssd1306_drawByte(112+i, 0, 0x81);
    }
    ssd1306_drawByte(121, 0, 0x81);
    ssd1306_drawByte(122, 0, 0xFF);
    ssd1306_drawByte(123, 0, 0x24);
    ssd1306_drawByte(124, 0, 0x3C);
    
    
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
    snprintf(str2, 24, "temp: %.2f deg", watch_config_data.temporary.temp_current_temp);
    ssd1306_draw5x7Font(0,4,str2);
    snprintf(str2, 24, "%d-%d-%d %d:%d:%d %s", date.year, date.month, date.day, date.hour, date.minute, date.second, weekstr);
    ssd1306_draw5x7Font(0,5,str2);
    snprintf(str2, 24, "bat: %d mV", bas_get_cur_bat_vot());
    ssd1306_draw5x7Font(0,6,str2);
}

void page_disp_pass_page()
{
    ssd1306_clearDisplay();
    ssd1306_draw16Font(watch_config_data.temporary.pair_passcode,37,3);
}

void page_disp_step_page(void)
{
    char str[8];
    ssd1306_clearDisplay();
    snprintf(str,8,"%d",watch_config_data.persist.step_walkdata.walking_slow);
    ssd1306_draw16Font(str,37,1);
    ssd1306_drawIcon16(ICON_WALK_SLOW, 17, 1);
    snprintf(str,8,"%d",watch_config_data.persist.step_walkdata.walking_fast);
    ssd1306_draw16Font(str,37,3);
    ssd1306_drawIcon16(ICON_WALK_FAST, 17, 3);
    snprintf(str,8,"%d",watch_config_data.persist.step_walkdata.run);
    ssd1306_draw16Font(str,37,5);
    ssd1306_drawIcon16(ICON_RUN, 17, 5);
}
void page_disp_alarming_page(void)
{
    char str[6];
    date_t date;
    rtc_getTime(&date);
    snprintf(str, 6, "%02d:%02d", date.hour, date.minute);
    
    ssd1306_clearDisplay();
    ssd1306_drawAlarmIcon48(0);
    
    ssd1306_draw16Font(str,44,6);
}

/**
 * Function:
 * Params:
 * Return:
 **/
void page_disp_message_page(void)
{
    char str[8];
    ssd1306_clearDisplay();
    
    snprintf(str,8,"%d",255);
    ssd1306_draw16Font(str,20,1);
    ssd1306_drawIcon16(ICON_PHONE, 0, 1);
    
    snprintf(str,8,"%d",255);
    ssd1306_draw16Font(str,84,1);
    ssd1306_drawIcon16(ICON_MESSAGE, 64, 1);
    
    snprintf(str,8,"%d",255);
    ssd1306_draw16Font(str,20,4);
    ssd1306_drawIcon16(ICON_QQ, 0, 4);
    
    snprintf(str,8,"%d",255);
    ssd1306_draw16Font(str,84,4);
    ssd1306_drawIcon16(ICON_WECHAT, 64, 4);
}


/**
 * Function: Display current page
 * Params: none
 * Return: none
 **/
void page_disp_current()
{
    switch(watch_config_data.temporary.page_current_screen){
        case CLOCK_PAGE:
            page_disp_clock_page();
            watch_config_data.temporary.page_should_render_every_frame = false;
            watch_config_data.temporary.page_keep_awake = false;
            break;
        case DEBUG_PAGE:
            page_disp_debug_page();
            watch_config_data.temporary.page_should_render_every_frame = true;
            watch_config_data.temporary.page_keep_awake = true;
            break;
        case CONN_PASS_PAGE:
            page_disp_pass_page();
            watch_config_data.temporary.page_should_render_every_frame = false;
            watch_config_data.temporary.page_keep_awake = false;
            break;
        case WALK_COUNTER_PAGE:
            page_disp_step_page();
            watch_config_data.temporary.page_should_render_every_frame = false;
            watch_config_data.temporary.page_keep_awake = false;
            break;
        case ALARM_DISP_PAGE:
            watch_config_data.temporary.page_should_render_every_frame = false;
            watch_config_data.temporary.page_keep_awake = true;
            page_disp_alarming_page();
            break;
        case MESSAGE_PAGE:
            watch_config_data.temporary.page_should_render_every_frame = false;
            watch_config_data.temporary.page_keep_awake = false;
            page_disp_message_page();
            break;
        default:
            watch_config_data.temporary.page_should_render_every_frame = false;
            watch_config_data.temporary.page_keep_awake = false;
            break;
            
    }
}

