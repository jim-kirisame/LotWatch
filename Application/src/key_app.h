#ifndef _KEY_APP_H
#define _KEY_APP_H
#include <stdint.h>

//#define KEY_PIN 17
#define INT_PIN 16
#define INT2_PIN 15

#define CHRG_PIN_2 8
#define CHRG_PIN 9
#define STDBY_PIN 10
#define STDBY_PIN_2 11

extern _Bool charge_charging;
extern _Bool charge_fulled;

enum key_evt_type{
    TOUCH_KEY_EVENT=0xC0,
    TAP_ONCE_EVENT,
    TAP_TWICE_EVENT,
    WRIST_ROTATE_EVENT,
    CHARGING_EVENT,
    NOT_CHARGING_EVENT=0xc5,
    FULLED_EVENT,
    NOT_FULLED_EVENT,
    PASSCODE_DISP_EVENT,
    ALARM_DISP_EVENT,
    NORMAL_DISP_EVENT=0xca,
};

void key_init(void);
void key_set_evt_handler(void (*evt)(uint8_t));
void key_generate_evt(enum key_evt_type event);


#endif
