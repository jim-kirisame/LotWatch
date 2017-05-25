#include "acc-algorithm.h"
#include <stdint.h>
#include <math.h> 

volatile uint16_t acc_window_buffer[ACC_ALGO_WINDOW_SIZE];
volatile uint8_t acc_window_pt=0;
volatile uint16_t acc_sport_buffer[ACC_ALGO_BUFFER_SIZE];
volatile uint8_t acc_buffer_pt = 0;

int abs(int a)
{
   return a >=0 ? a : -a;
}

void acc_algo_hamming_window(void);

uint32_t SquareRoot_2(uint32_t a_nInput)
{
    uint32_t op  = a_nInput;
    uint32_t res = 0;
    uint32_t one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type


    // "one" starts at the highest power of four <= than the argument.
    while (one > op) {
        one >>= 2;
    }

    while (one != 0) {
        if (op >= res + one) {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}

void acc_algo_add(int16_t x, int16_t y, int16_t z)
{
    acc_window_buffer[acc_window_pt] = SquareRoot_2(x*x + y*y + z*z);
    
    acc_algo_hamming_window();
    acc_window_pt++;
    if(acc_window_pt >= ACC_ALGO_WINDOW_SIZE)
        acc_window_pt = 0;
}

void acc_algo_hamming_window()
{
    uint8_t top = ACC_ALGO_WINDOW_SIZE/2;
    //uint8_t offset = top + 1 > acc_window_pt ? ACC_ALGO_WINDOW_SIZE + acc_window_pt - top - 1 : acc_window_pt - top - 1;
    uint8_t offset = acc_window_pt + 1;
    volatile uint32_t result = 0;
    float weight,total = 0;
    #define DELTA_WINDOW
#ifdef DELTA_WINDOW

    if(ACC_ALGO_WINDOW_SIZE % 2)
    {
        for(uint8_t i=0;i<top;i++)
        {
            weight = (float)(i + 1) / (float)(top + 1);
            total += (float)weight;
            result += (float)acc_window_buffer[(offset + i) % ACC_ALGO_WINDOW_SIZE] * (float)weight;
        }
        for(uint8_t i=ACC_ALGO_WINDOW_SIZE-1;i>top;i--)
        {
            weight = (float)(2 * top - i + 1) / (float)(top + 1);
            total += (float)weight;
            result += (float)acc_window_buffer[(offset + i) % ACC_ALGO_WINDOW_SIZE] * (float)weight;
        }
        total += 1;
        result += acc_window_buffer[top];
        result /= (float)total;
    }
    else
    {
        for(uint8_t i=0;i<=top;i++)
        {
            weight = (float)(i + 1) / (float)top;
            total += (float)weight;
            result += (float)acc_window_buffer[(offset + i) % ACC_ALGO_WINDOW_SIZE] * (float)weight;
        }
        for(uint8_t i=ACC_ALGO_WINDOW_SIZE-1;i>top;i--)
        {
            weight = (float)(2 * top - i + 1) / (float)top;
            total += (float)weight;
            result += (float)acc_window_buffer[(offset + i) % ACC_ALGO_WINDOW_SIZE] * (float)weight;
        }
        
        result /= total;
    }

#else    
        for(uint8_t i=0;i<ACC_ALGO_WINDOW_SIZE;i++)
        {
            result += acc_window_buffer[(offset + i) % ACC_ALGO_WINDOW_SIZE];
        }
        result /= ACC_ALGO_WINDOW_SIZE;
#endif
    acc_sport_buffer[acc_buffer_pt] = result;
    
    acc_buffer_pt++;
    if(acc_buffer_pt >= ACC_ALGO_BUFFER_SIZE)
        acc_buffer_pt = 0;
}
