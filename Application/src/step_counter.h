#ifndef _STEP_COUNTER_
#define _STEP_COUNTER_

#include <stdint.h>
#define ACC_MEASURE_HZ 25
#define ACC_MEASURE_TIMER_MS (APP_TIMER_TICKS(1000/ACC_MEASURE_HZ, APP_TIMER_PRESCALER)) //10hz
#define SLEEP_DATA_COUNT 10

typedef struct packet_sleepdata {
    uint8_t type;
    uint32_t timestamp;
} packet_sleepdata;

typedef struct packet_walkdata {
    uint32_t walking;
} packet_walkdata;


void step_counter_init(void);
void step_nextday(void);
void step_health_algorithm_reload(void);

//extern packet_sleepdata step_sleepdata[SLEEP_DATA_COUNT];
//extern packet_walkdata step_walkdata;
//extern uint8_t step_sleepdata_index;

#endif
