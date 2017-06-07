#include "main.h"
#include "step_counter.h"

#include "app_error.h"
#include "app_timer.h"
#include "mma8452.h"
#include "rtc_app.h"
#include "config_storage.h"
#include "acc-algorithm.h"

APP_TIMER_DEF(m_acc_timer_meas_id);             /**< Battery timer. */
uint8_t ms_counter;

volatile _Bool isInit = false;


void step_counter_timer_handler(void *p_context)
{
    UNUSED_PARAMETER(p_context);
    if(!isInit)
        return;
    ms_counter ++;
    ms_counter %= ACC_MEASURE_HZ;
    mma8452_acc_data acc_data;
    mma8452_read_acc(&acc_data);
    
    acc_algo_add(acc_data.x, acc_data.y, acc_data.z);
    if(wchData.persist.config.debug_enable)
        comm_send_acc_data(acc_data);
}

void step_counter_timer_init(void)
{
    uint8_t err_code = app_timer_create(&m_acc_timer_meas_id,
                                APP_TIMER_MODE_REPEATED,
                                step_counter_timer_handler);
    APP_ERROR_CHECK(err_code);
}

void step_counter_timer_start(void)
{
    uint32_t err_code = app_timer_start(m_acc_timer_meas_id, ACC_MEASURE_TIMER_MS, NULL);
    APP_ERROR_CHECK(err_code);
}

void step_health_algorithm_init(void)
{
    isInit = true;
}

void step_health_algorithm_reload(void)
{
    isInit = false;
}

void step_counter_init(void)
{
    step_health_algorithm_init();
    step_counter_timer_init();
    step_counter_timer_start();
}


void step_nextday(void)
{
    //TODO: add some event to inform mobile phone this data;
    wchData.temporary.step_lastday = wchData.persist.step_walkdata;
    
    wchData.persist.step_walkdata.walking = 0;
}



