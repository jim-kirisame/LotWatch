#include "main.h"
#include "rtc_app.h"
#include "app_error.h"
#include "app_timer.h"
#include "nrf_drv_config.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "alarm_app.h"
#include "step_counter.h"
#include <string.h>

#define COMPARE_COUNTERTIME  (3UL)                                        /**< Get Compare event COMPARE_TIME seconds after the counter starts from 0. */
#define RTC_TIMER_INTERVAL_MS (APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER))
APP_TIMER_DEF(m_rtc_timer);             /**< rtc timer. */

uint32_t rtc_localts;

static const uint8_t rtc_dayOfMonthTable[] = {31,28,31,30,31,30,31,31,30,31,30,31};
static const char rtc_weekStrTable[][4] = {"SUN", "MON", "TUE", "WED", "THR", "FRI", "SAT"};
const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */

static date_t date_temp;

void rtc_add1s(void)
{
    rtc_localts++;
}

/** @brief: Function for handling the RTC0 interrupts.
 * Triggered on TICK and COMPARE0 match.

static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_TICK)
    {
        rtc_add1s();
    }
}
 */


void rtc_timer_handler(void *p_context){
    UNUSED_PARAMETER(p_context);
    
    date_t date;
    
    rtc_add1s();
    rtc_getTime(&date);
    if(date.minute != date_temp.minute)
    {
        alarm_check(&date);
    }
    if(date.day != date_temp.day)
    {
        step_nextday();
    }
    date_temp = date;
}

void rtc_init()
{
    uint32_t err_code;
    
    /**
    //err_code = nrf_drv_clock_init(NULL);
    //APP_ERROR_CHECK(err_code);

    //nrf_drv_clock_lfclk_request();

    //Initialize RTC instance
    err_code = nrf_drv_rtc_init(&rtc, NULL, rtc_handler);
    APP_ERROR_CHECK(err_code);

    //Enable tick event & interrupt
    nrf_drv_rtc_tick_enable(&rtc,true);

    //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
    //err_code = nrf_drv_rtc_cc_set(&rtc, 0, COMPARE_COUNTERTIME*RTC0_CONFIG_FREQUENCY, true);
    //APP_ERROR_CHECK(err_code);

    //Power on RTC instance
    nrf_drv_rtc_enable(&rtc);
    **/
    err_code = app_timer_create(&m_rtc_timer,
                                APP_TIMER_MODE_REPEATED,
                                rtc_timer_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_rtc_timer, RTC_TIMER_INTERVAL_MS, NULL);
    APP_ERROR_CHECK(err_code);
    
}

void rtc_setTimeUnix(uint32_t unixts)
{
    rtc_localts = unixts - RTC_UNIX_TS_BASE;
}

uint32_t rtc_getTimeUnix(void)
{
    return rtc_localts + RTC_UNIX_TS_BASE;
}

void rtc_setTimeLocal(uint32_t localts)
{
    rtc_localts = localts;
}

uint32_t rtc_getTimeLocal(void)
{
    return rtc_localts;
}

void rtc_getWeekStr(char * string, uint8_t week)
{
    memcpy(string, rtc_weekStrTable[week], 4);
}

uint8_t rtc_getDayOfMonth(uint16_t year, uint8_t month)
{
    uint8_t result = rtc_dayOfMonthTable[month-1];
    if(month == 2){
        if(year%4 == 0){ //ignore 100 year, because next is 2100.
            result++;
        }
    }
    return result;
}

uint16_t rtc_getDayOfYear(uint16_t year)
{
    return year%4 ? 365 : 366;
}

void rtc_setTime(date_t * date){
    uint16_t cday;
    uint16_t temp;
    
    for(temp=2017;temp<(*date).year;temp--)
    {
        cday += rtc_getDayOfYear(temp);
    }

    for(temp=1;temp<(*date).month;temp++)
    {
        cday += rtc_getDayOfMonth(temp, (*date).year);
    }

    cday += ((*date).day - 1);

    rtc_localts = (uint32_t)cday * 24 * 60 * 60 + (uint32_t)(*date).hour * 60 * 60 + (uint32_t)(*date).minute * 60 + (*date).second;
}

void rtc_getTime(date_t * date)
{
    uint16_t cday = rtc_localts / 24 / 60 / 60;
    (*date).second = rtc_localts % 60;
    (*date).minute = rtc_localts % 3600 / 60;
    (*date).hour = rtc_localts % (24*60*60) / 3600;
    
    uint16_t year = 2017;
    uint8_t month = 1, day = 1;
    
    (*date).week = (cday % 7 + RTC_WEEK_BASE) % 7;
    
    while(cday>=rtc_getDayOfYear(year))
    {
        cday -= rtc_getDayOfYear(year);
        year++;
    }
    
    while(cday >= rtc_getDayOfMonth(year,month))
    {
        cday -= rtc_getDayOfMonth(year, month);
        month++;
    }
    day += cday;
    
    (*date).year = year;
    (*date).month = month;
    (*date).day = day;
}
