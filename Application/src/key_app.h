#ifndef _KEY_APP_H
#define _KEY_APP_H
#include <stdint.h>

#define KEY_PIN 17
#define INT_PIN 14


void key_init(void);
void key_set_evt_handler(void (*evt)(uint8_t));

#endif
