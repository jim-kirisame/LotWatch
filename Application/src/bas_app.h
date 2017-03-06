#ifndef _BAS_APP_
#define _BAS_APP_

//#include "app_scheduler.h"
#include "main.h"
#include "ble_bas.h"

#define ADC_INPUT_CHANNEL  	NRF_ADC_CONFIG_INPUT_5
#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        1200                                     /**< Reference voltage (in  milli volts) used by ADC while doing conversion. */
#define ADC_HW_PRE_SCALING_COMPENSATION      4

#define BATTERY_ADC_RESOLUTION  ( NRF_ADC_CONFIG_RES_10BIT )
#define BATTERY_ADC_DIV         (1023)

#define MIN_BAT_MV (3600)
#define CHARGER_CHECK_INTERVAL_MS (APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER))
#define BATTERY_MEASURE_INTERVAL_MS (APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER))

void bas_update_measure_data(void);
uint16_t bas_get_cur_bat_vot(void);
uint8_t bas_get_cur_bat_lvl(void);

void bas_app_init(void);
void bas_timer_init(void);
void bas_timer_start(void);
void bas_timer_stop(void);
void bas_ble_evt_dispatch(ble_evt_t * p_ble_evt);
uint8_t bas_vot2lvl(uint16_t lvl);

#endif
