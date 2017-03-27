#include "main.h"
#include "step_counter.h"

#include "app_error.h"
#include "app_timer.h"
#include "mma8452.h"
#include "rtc_app.h"
#include "config_storage.h"

APP_TIMER_DEF(m_acc_timer_meas_id);             /**< Battery timer. */
//packet_userdata step_userdata;
uint8_t ms_counter;

//packet_sleepdata step_sleepdata[SLEEP_DATA_COUNT];
//packet_walkdata step_walkdata;
//uint8_t step_sleepdata_index;
volatile _Bool isInit = false;


uint32_t SquareRoot(uint32_t a_nInput)
{
    uint32_t op  = a_nInput;
    uint32_t res = 0;
    uint32_t one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type


    // "one" starts at the highest power of four <= than the argument.
    while (one > op) {
        one >>= 2;
    }

    while (one != 0) {
        if (op >= res + one) {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}

void step_counter_timer_handler(void *p_context)
{
    UNUSED_PARAMETER(p_context);
    if(!isInit)
        return;
    uint32_t xyz;
    ms_counter ++;
    ms_counter %= ACC_MEASURE_HZ;
    mma8452_acc_data acc_data;
    mma8452_read_acc(&acc_data);
    
    xyz = acc_data.x*acc_data.x + acc_data.y*acc_data.y +acc_data.z*acc_data.z;
    
    health_algorithm_data_in_accelerate(acc_data.x,
                                        acc_data.y,
                                        acc_data.z,
                                        SquareRoot(xyz),
                                        (long long)rtc_getTimeUnix()*1000 + (1000 / ACC_MEASURE_HZ * ms_counter));
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

void step_health_user_data_init(void)
{
    /*
    wchData.persist.config.step_userdata.age = 23;
    wchData.persist.config.step_userdata.height = 175;
    wchData.persist.config.step_userdata.sex = false;
    wchData.persist.config.step_userdata.weight = 75;
    */
}

void step_health_algorithm_init(void)
{
    uint8_t errcode;
    
    errcode = init_health_algorithm(25,
                                    wchData.persist.config.step_userdata.height,
                                    wchData.persist.config.step_userdata.weight,
                                    wchData.persist.config.step_userdata.age,
                                    wchData.persist.config.step_userdata.sex);
    APP_ERROR_CHECK_BOOL(errcode == 0);
    
    errcode = register_health_algorithm_callback(step_health_algorithm_callback, NULL);
    APP_ERROR_CHECK_BOOL(errcode == 0);
    isInit = true;
}

void step_health_algorithm_reload(void)
{
    isInit = false;
    health_algorithm_finalize();
    step_health_algorithm_init();
}

void step_counter_init(void)
{
    step_health_user_data_init();
    step_health_algorithm_init();
    step_counter_timer_init();
    step_counter_timer_start();
}

void step_distance_event_handler(algorithm_event_t *event)
{
    APP_ERROR_CHECK_BOOL(event->event_common.type == DISTANCE_EVENT);
    switch(event->distance.mode)
    {
        case STEP_MODE_WALK_SLOW:
            wchData.persist.step_walkdata.walking_slow+=event->distance.new_steps;
        break;
        
        case STEP_MODE_WALK_QUICK:
            wchData.persist.step_walkdata.walking_fast+=event->distance.new_steps;
        break;
        
        case STEP_MODE_RUN:
            wchData.persist.step_walkdata.run += event->distance.new_steps;
        break;
    }
    
    wchData.persist.step_walkdata.distance += event->distance.new_distances;
    wchData.persist.step_walkdata.cal += event->distance.new_calory;
}

void step_sleep_event_handler(algorithm_event_t *event)
{
    APP_ERROR_CHECK_BOOL(event->event_common.type == SLEEP_EVENT);
    wchData.persist.step_sleepdata[wchData.persist.step_sleepindex].type = event->sleep.mode;
    wchData.persist.step_sleepdata[wchData.persist.step_sleepindex++].timestamp = event->sleep.starting_time_stamp;
}

void step_health_algorithm_callback(algorithm_event_t *event, void* user_data)
{
    switch(event->event_common.type)
    {
        case SLEEP_EVENT:
            step_sleep_event_handler(event);
            break;

        case DISTANCE_EVENT:
            step_distance_event_handler(event);
            break;
        
        default:
            break;
    }
}

void step_nextday(void)
{
    //TODO: add some event to inform mobile phone this data;
    wchData.temporary.step_lastday = wchData.persist.step_walkdata;
    
    wchData.persist.step_walkdata.cal = 0;
    wchData.persist.step_walkdata.distance = 0;
    wchData.persist.step_walkdata.run = 0;
    wchData.persist.step_walkdata.walking_fast = 0;
    wchData.persist.step_walkdata.walking_slow = 0;
}



