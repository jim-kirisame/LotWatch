#include "main.h"
#include "charge_app.h"
#include "nrf_gpio.h"
#include "app_timer.h"

_Bool charge_charging;
_Bool charge_fulled;

APP_TIMER_DEF(charge_check_timer_id);             /**< Battery timer. */

void charge_timer_handler(void *p_context){
    UNUSED_PARAMETER(p_context);
    charge_charging = !nrf_gpio_pin_read(CHRG_PIN);
    charge_fulled = !nrf_gpio_pin_read(STDBY_PIN);
}

void charge_init(void){
    nrf_gpio_cfg_input(CHRG_PIN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(CHRG_PIN, NRF_GPIO_PIN_PULLUP);
    uint8_t err_code = app_timer_create(&charge_check_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                charge_timer_handler);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_timer_start(charge_check_timer_id, CHARGER_CHECK_INTERVAL_MS, NULL);
    APP_ERROR_CHECK(err_code);
}
