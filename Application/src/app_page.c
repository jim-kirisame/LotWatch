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
#include "acc-algorithm.h"

//const uint8_t bar_data[9] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
const uint8_t bar_data[9] = {0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00};

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
    
    snprintf(str2, 24, "Acc: %d %d %d", acc_data.x, acc_data.y, acc_data.z);
    ssd1306_draw5x7Font(0,0,str2);
    snprintf(str2, 24, "Step: %d", wchData.persist.step_walkdata.walking);
    ssd1306_draw5x7Font(0,1,str2);
    snprintf(str2, 24, "%d-%d-%d %d:%d:%d %s", date.year, date.month, date.day, date.hour, date.minute, date.second, weekstr);
    ssd1306_draw5x7Font(0,3,str2);
    snprintf(str2, 24, "Bat: %d mV %02d%%", bas_get_cur_bat_vot(),wchData.temporary.battery_level);
    ssd1306_draw5x7Font(0,4,str2);
    snprintf(str2, 24, "Wakeup: %02X,%d", wchData.temporary.debug_wakeup_evt, wchData.temporary.wakeup_counter);
    ssd1306_draw5x7Font(0,5,str2);
    snprintf(str2, 24,"Compile on:");
    ssd1306_draw5x7Font(0,6,str2);
    snprintf(str2, 24,"%s %s" , __DATE__, __TIME__);
    ssd1306_draw5x7Font(0,7,str2);
}

void page_disp_pass_page()
{
    ssd1306_clearDisplay();
    ssd1306_draw16Font(wchData.temporary.pair_passcode,37,3);
}

void page_disp_step_page(void)
{
    char str[8];
    ssd1306_clearDisplay();
    snprintf(str,8,"%d",wchData.persist.step_walkdata.walking);
    ssd1306_draw16Font(str,37,3);
    ssd1306_drawIcon16(ICON_WALK_SLOW, 17, 3);
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
    
    snprintf(str,8,"%d",wchData.temporary.message.call);
    ssd1306_draw16Font(str,25,1);
    ssd1306_drawIcon16(ICON_PHONE, 0, 1);
    
    snprintf(str,8,"%d",wchData.temporary.message.message);
    ssd1306_draw16Font(str,89,1);
    ssd1306_drawIcon16(ICON_MESSAGE, 64, 1);
    
    snprintf(str,8,"%d",wchData.temporary.message.qq);
    ssd1306_draw16Font(str,25,4);
    ssd1306_drawIcon16(ICON_QQ, 0, 4);
    
    snprintf(str,8,"%d",wchData.temporary.message.wechat);
    ssd1306_draw16Font(str,89,4);
    ssd1306_drawIcon16(ICON_WECHAT, 64, 4);
}

/**
 * Function:
 * Params:
 * Return:
 **/
void page_disp_charging_page()
{
    char str[5];
    ssd1306_clearDisplay();
    ssd1306_drawChargingIcon48(0);
    snprintf(str,5,"%02d%%",wchData.temporary.battery_level);
    ssd1306_draw16Font(str,48,6);
}

/**
 * Function:
 * Params:
 * Return:
 **/
void page_acc_print_page()
{
    for(uint8_t i=0;i<ACC_ALGO_BUFFER_SIZE;i++)
    {
        uint8_t data;
         uint16_t temp = acc_sport_buffer[(i + acc_buffer_pt) % ACC_ALGO_BUFFER_SIZE].total / 16 - 32;
        // uint16_t temp = acc_sport_buffer[(i + acc_buffer_pt) % ACC_ALGO_BUFFER_SIZE] / 2 - 480;
        data = temp;
        for(uint8_t j=0;j<8;j++)
        {
            uint8_t disp;
#define FLOAT_POINT
#ifdef FLOAT_POINT
            if(data <= j * 8)
            {
                disp = 0x00;
            }
            else if(data > (j+1) * 8)
            {
                disp = 0x00;
            }
            else
            {
                //disp = bar_data[(j+1) * 8 - data];
                disp = 1 << ((j+1) * 8 - data);
            }
#else
            if(data <= j * 8)
            {
                disp = 0x00;
            }
            else if(data >= (j+1) * 8)
            {
                disp = 0xff;
            }
            else
            {
                disp = bar_data[(j+1) * 8 - data];
                //disp = 0xff >> ((j+1) * 8 - data);
            }
#endif
            ssd1306_drawByte(i,7-j,disp);
        }
    }
}

/**
 * Function: Display current page
 * Params: none
 * Return: none
 **/
void page_disp_current()
{
    switch(wchData.temporary.page_current_screen){
        case CLOCK_PAGE:
            page_disp_clock_page();
            wchData.temporary.page_should_render_every_frame = false;
            wchData.temporary.page_keep_awake = false;
            break;
        case DEBUG_PAGE:
            page_disp_debug_page();
            wchData.temporary.page_should_render_every_frame = true;
            wchData.temporary.page_keep_awake = true;
            break;
        case CONN_PASS_PAGE:
            page_disp_pass_page();
            wchData.temporary.page_should_render_every_frame = false;
            wchData.temporary.page_keep_awake = true;
            break;
        case WALK_COUNTER_PAGE:
            page_disp_step_page();
            wchData.temporary.page_should_render_every_frame = false;
            wchData.temporary.page_keep_awake = false;
            break;
        case ALARM_DISP_PAGE:
            wchData.temporary.page_should_render_every_frame = false;
            wchData.temporary.page_keep_awake = true;
            page_disp_alarming_page();
            break;
        case MESSAGE_PAGE:
            wchData.temporary.page_should_render_every_frame = false;
            wchData.temporary.page_keep_awake = false;
            page_disp_message_page();
            break;
        case CHARGING_PAGE:
            page_disp_charging_page();
            wchData.temporary.page_should_render_every_frame = true;
            wchData.temporary.page_keep_awake = false;
            break;
        case ACC_PRINT_PAGE:
            page_acc_print_page();
            wchData.temporary.page_should_render_every_frame = true;
            wchData.temporary.page_keep_awake = true;
            break;
        default:
            wchData.temporary.page_should_render_every_frame = false;
            wchData.temporary.page_keep_awake = false;
            break;
            
    }
}

