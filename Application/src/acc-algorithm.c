#include "acc-algorithm.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h> 
#include "config_storage.h"

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
void acc_algo_proc_step_data(void);
void acc_algo_add(int16_t x, int16_t y, int16_t z)
{
    acc_window_buffer[acc_window_pt] = SquareRoot_2(x*x + y*y + z*z);
    acc_algo_hamming_window();
    acc_window_pt++;
    if(acc_window_pt >= ACC_ALGO_WINDOW_SIZE)
        acc_window_pt = 0;
    acc_algo_proc_step_data();
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


void acc_alog_step_add(uint8_t num)
{
    wchData.persist.step_walkdata.walking += num;
}

uint8_t acc_lastPeakIndex, acc_lastValleyIndex;
_Bool acc_rising;
uint8_t acc_stepSuccess, acc_stepFail = 0;
uint8_t acc_durationWindow_min = ACC_ALGO_TIME_WINDOW_MIN;
uint8_t acc_durationWindow_max = ACC_ALGO_TIME_WINDOW_MAX;

_Bool acc_check_time_window(uint8_t duration)
{
    if(duration > acc_durationWindow_min && duration < acc_durationWindow_max)
    {
        acc_durationWindow_min = duration *4/5;
        acc_durationWindow_max = duration *5/4;
        return true;
    }
    else
        return false;
}

void acc_reset_time_window()
{
    acc_durationWindow_min = ACC_ALGO_TIME_WINDOW_MIN;
    acc_durationWindow_max = ACC_ALGO_TIME_WINDOW_MAX;
}

void acc_algo_proc_step_data()
{
    uint16_t now, prev;
    uint8_t thisIndex = acc_buffer_pt > 1 ? acc_buffer_pt - 2 : ACC_ALGO_BUFFER_SIZE - 1;
    now = acc_sport_buffer[acc_buffer_pt > 0 ? acc_buffer_pt - 1 : ACC_ALGO_BUFFER_SIZE] / 16;
    prev = acc_sport_buffer[thisIndex] / 16;
    
    if(prev == now) return;
    if(!acc_rising ^ (prev > now))
    {
        //极大值判断
        uint16_t lastPeak = acc_sport_buffer[acc_lastPeakIndex] / 16;
        uint16_t lastValley = acc_sport_buffer[acc_lastValleyIndex] / 16;
        uint16_t p2v = lastPeak - lastValley;
        uint16_t v2p = prev - lastValley;
        acc_rising = now > prev;
        
        if(p2v / 4 <= v2p) //判定波形是否正确
        {
            if(v2p <= 3) return;
            if(!acc_rising)
            {
                uint8_t duration = thisIndex > acc_lastPeakIndex ? thisIndex - acc_lastPeakIndex : thisIndex + ACC_ALGO_BUFFER_SIZE - acc_lastPeakIndex;
                //wchData.persist.step_walkdata.run++;
                if(acc_check_time_window(duration))
                {
                    acc_stepSuccess++;
                    acc_stepFail=0;
                    if(acc_stepSuccess > 7)acc_alog_step_add(1);
                    else if(acc_stepSuccess == 7)acc_alog_step_add(7);
                }
                else
                {
                    acc_stepFail++;
                    if(acc_stepFail>2)
                    {
                        acc_reset_time_window();
                        acc_stepSuccess = 0;
                    }
                }
                acc_lastPeakIndex = thisIndex;
            }
            else
            {
                acc_lastValleyIndex = thisIndex;
            }
        }
        // wchData.persist.step_walkdata.walking_fast++;
        
    }

    
    
}
