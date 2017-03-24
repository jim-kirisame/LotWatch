#include "main.h"
#include "alarm_app.h"
#include "rtc_app.h"
#include "key_app.h"
#include "app_timer.h"
#include "viber_app.h"

#define ALARM_TIMER_INTERVAL_MS (APP_TIMER_TICKS(10000, APP_TIMER_PRESCALER))
APP_TIMER_DEF(m_alarm_timer);             /**< rtc timer. */

alarm_data_item alarm_data[MAX_ALARM_COUNT];
uint8_t alarm_delay;

void alarm_timer_handler(void *p_context);

void alarm_timer_init(void)
{
    uint32_t err_code = app_timer_create(&m_alarm_timer,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                alarm_timer_handler);
    APP_ERROR_CHECK(err_code);
}

void alarm_test(void)
{
    alarm_data[0].hour = 18;
    alarm_data[0].minute = 33;
    alarm_data[0].repeat = 0xFF;
}

void alarm_timer_start(void)
{
    uint32_t err_code = app_timer_start(m_alarm_timer, ALARM_TIMER_INTERVAL_MS, NULL);
    APP_ERROR_CHECK(err_code);
}

void alarm_timer_stop(void)
{
    uint32_t err_code = app_timer_stop(m_alarm_timer);
    APP_ERROR_CHECK(err_code);
}

void alarm_enter(void)
{

    //viber_start();
    key_generate_evt(ALARM_DISP_EVENT);
    alarm_timer_start();
}

void alarm_exit(void)
{
    alarm_timer_stop();
    viber_stop();
}

void alarm_delay_action(void)
{
    alarm_exit();
    alarm_delay = ALARM_DELAY_MINUTES;
}

void alarm_timer_handler(void *p_context)
{
    UNUSED_PARAMETER(p_context);
    alarm_delay_action();
}
void alarm_check(date_t * date)
{
    if(alarm_delay)
    {
        alarm_delay--;
        if(!alarm_delay)
        {
            alarm_enter();
        }
        
    }
    for(int i=0; i< MAX_ALARM_COUNT; i++)
    {
        if( alarm_data[i].repeat & (1<<7) || alarm_data[i].repeat & ( 1 << date->week ) )
        {
            if(alarm_data[i].hour == date->hour && alarm_data[i].minute == date->minute)
            {
                alarm_enter();
                alarm_data[i].repeat &= ~(1<<7);
            }
            
        }
    }
}
