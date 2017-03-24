#include "main.h"
#include "viber_app.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_timer.h"

#define ALARM_TIMER_INTERVAL_MS (APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER))
APP_TIMER_DEF(m_viber_timer);
_Bool isStop = false;

void viber_timer_handler(void * p_handler)
{
    UNUSED_PARAMETER(p_handler);
    if(isStop)
        nrf_gpio_pin_clear(VIBER_PIN);
    else
        nrf_gpio_pin_toggle(VIBER_PIN);
}

void viber_init(void){
    nrf_gpio_cfg_output(VIBER_PIN);
    nrf_gpio_pin_clear(VIBER_PIN);
    uint32_t err_code = app_timer_create(&m_viber_timer,
                                APP_TIMER_MODE_REPEATED,
                                viber_timer_handler);
    APP_ERROR_CHECK(err_code);
}

void viber_start(void)
{
    isStop = false;
    uint32_t err_code = app_timer_start(m_viber_timer, ALARM_TIMER_INTERVAL_MS, NULL);
    APP_ERROR_CHECK(err_code);
}

void viber_stop(void)
{
    uint32_t err_code = app_timer_stop(m_viber_timer);
    APP_ERROR_CHECK(err_code);
    isStop = true;
    nrf_gpio_pin_clear(VIBER_PIN); // clear 
}

void viber_action(uint16_t length)
{
    nrf_gpio_pin_set(VIBER_PIN);
    nrf_delay_ms(length);
    nrf_gpio_pin_clear(VIBER_PIN);
}

void viber_weak(void){
    viber_action(100);
}

void viber_mid(void){
    viber_action(300);
}

void viber_strong(void){
    viber_action(500);
}
