#ifndef _KEY_APP_H
#define _KEY_APP_H
#include <stdint.h>

#define KEY_PIN 17
#define INT_PIN 14
#define CHRG_PIN 9
#define STDBY_PIN 10

extern _Bool charge_charging;
extern _Bool charge_fulled;

void key_init(void);
void key_set_evt_handler(void (*evt)(uint8_t));

enum key_evt_type{
    TOUCH_KEY_EVENT,
    TAP_ONCE_EVENT,
    TAP_TWICE_EVENT,
    CHARGING_EVENT,
    NOT_CHARGING_EVENT,
    FULLED_EVENT,
    NOT_FULLED_EVENT
};

#endif
