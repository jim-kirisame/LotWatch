#include "main.h"
#include "alarm_app.h"
#include "rtc_app.h"
#include "key_app.h"
#include "app_timer.h"
#include "viber_app.h"

#include "config_storage.h"

#define ALARM_TIMER_INTERVAL_MS (APP_TIMER_TICKS(wchData.persist.config.alarm_vibra_time*1000, APP_TIMER_PRESCALER))
APP_TIMER_DEF(m_alarm_timer);             /**< rtc timer. */

//alarm_data_item alarm_data[MAX_ALARM_COUNT];
uint8_t alarm_delay;
uint8_t alarm_delay_times;

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
    viber_start();
    key_generate_evt(ALARM_DISP_EVENT);
    alarm_timer_start();
}

void alarm_exit(void)
{
    alarm_timer_stop();
    key_generate_evt(NORMAL_DISP_EVENT);
    viber_stop();
}

void alarm_delay_action(void)
{
    alarm_exit();
    alarm_delay = wchData.persist.config.alarm_delay_time;
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
        if(!alarm_delay && alarm_delay_times)
        {
            alarm_delay_times--;
            alarm_enter();
        }
        
    }
    for(int i=0; i< MAX_ALARM_COUNT; i++)
    {
        if( wchData.persist.config.alarm_data[i].repeat & (1<<7) || wchData.persist.config.alarm_data[i].repeat & ( 1 << date->week ) )
        {
            if(wchData.persist.config.alarm_data[i].hour == date->hour && wchData.persist.config.alarm_data[i].minute == date->minute)
            {
                alarm_enter();
                alarm_delay_times = wchData.persist.config.alarm_delay_max_count;
                wchData.persist.config.alarm_data[i].repeat &= ~(1<<7);
            }
            
        }
    }
}
