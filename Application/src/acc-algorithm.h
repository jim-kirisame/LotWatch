#ifndef __ACC_ALGORITHM__
#define __ACC_ALGORITHM__

#include <stdint.h>

#define ACC_ALGO_WINDOW_SIZE 8
#define ACC_ALGO_BUFFER_SIZE 128
#define ACC_ALGO_TIME_WINDOW_MIN 5
#define ACC_ALGO_TIME_WINDOW_MAX 50
void acc_algo_add(int16_t x, int16_t y, int16_t z);
extern volatile uint16_t acc_window_buffer[ACC_ALGO_WINDOW_SIZE];
extern volatile uint8_t acc_window_pt;
extern volatile uint16_t acc_sport_buffer[ACC_ALGO_BUFFER_SIZE];
extern volatile uint8_t acc_buffer_pt;

#endif
