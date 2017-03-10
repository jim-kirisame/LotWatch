#include "main.h"
#include "viber_app.h"
#include "nrf_gpio.h"


void viber_init(void){
    nrf_gpio_cfg_output(VIBER_PIN);
    nrf_gpio_pin_clear(VIBER_PIN);
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