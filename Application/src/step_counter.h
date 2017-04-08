#ifndef _STEP_COUNTER_
#define _STEP_COUNTER_

#include "health-algorithm.h"
#include <stdint.h>
#define ACC_MEASURE_HZ 5
#define ACC_MEASURE_TIMER_MS (APP_TIMER_TICKS(1000/ACC_MEASURE_HZ, APP_TIMER_PRESCALER)) //10hz
#define SLEEP_DATA_COUNT 10

typedef struct packet_userdata{
    uint16_t height;
    uint8_t weight;
    uint8_t age;
    uint8_t sex;
} packet_userdata;

typedef struct packet_sleepdata {
    uint8_t type;
    uint32_t timestamp;
} packet_sleepdata;

typedef struct packet_walkdata {
    uint32_t walking_slow;
    uint32_t walking_fast;
    uint32_t run;
    uint32_t distance;
    uint32_t cal;
} packet_walkdata;


void step_health_algorithm_callback(algorithm_event_t *event, void* user_data);
void step_counter_init(void);
void step_nextday(void);
void step_health_algorithm_reload(void);

//extern packet_sleepdata step_sleepdata[SLEEP_DATA_COUNT];
//extern packet_walkdata step_walkdata;
//extern uint8_t step_sleepdata_index;

#endif
