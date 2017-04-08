#include "main.h"
#include "viber_app.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include "nrf_drv_config.h"

//#define USING_PWM

#ifdef USING_PWM
#include "app_pwm.h"
#endif


#define ALARM_TIMER_INTERVAL_MS (APP_TIMER_TICKS(500, APP_TIMER_PRESCALER))
APP_TIMER_DEF(m_viber_timer);

_Bool isStop = false;
_Bool state;
#ifdef USING_PWM
APP_PWM_INSTANCE(PWM0,0);                   // Create the instance "PWM0" using TIMER0.
static volatile bool ready_flag;            // A flag indicating PWM status.

void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    ready_flag = true;
}
#endif

void viber_timer_handler(void * p_handler)
{
    UNUSED_PARAMETER(p_handler);
    if(isStop)
    {
    #ifdef USING_PWM
        app_pwm_disable(&PWM0);
    #else
        nrf_gpio_pin_clear(VIBER_PIN); // clear 
    #endif
    }
    else
    {
    #ifdef USING_PWM
        if(state)
        {
            app_pwm_disable(&PWM0);
        }
        else
        {
            app_pwm_enable(&PWM0);
        }
        state = ~state;
    #else
        nrf_gpio_pin_toggle(VIBER_PIN);
    #endif
    }
}



void viber_init(void){
    uint32_t err_code;
#ifdef USING_PWM
    
    /* 1-channel PWM, 200Hz, output on DK LED pins. */
    app_pwm_config_t pwm0_cfg = APP_PWM_DEFAULT_CONFIG_1CH(5000L, VIBER_PIN);
    
    /* Switch the polarity of the channel. */
    pwm0_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    
    /* Initialize and enable PWM. */
    err_code = app_pwm_init(&PWM0,&pwm0_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_pwm_channel_duty_set(&PWM0, 0, 50) ;
    app_pwm_enable(&PWM0);
    
#else
    nrf_gpio_cfg_output(VIBER_PIN);
    nrf_gpio_pin_clear(VIBER_PIN);
#endif
    err_code = app_timer_create(&m_viber_timer,
                                APP_TIMER_MODE_REPEATED,
                                viber_timer_handler);
    APP_ERROR_CHECK(err_code);
}

void viber_start(void)
{
    isStop = false;
    uint32_t err_code = app_timer_start(m_viber_timer, ALARM_TIMER_INTERVAL_MS, NULL);
    APP_ERROR_CHECK(err_code);
#ifdef USING_PWM
    app_pwm_enable(&PWM0);
#endif
}

void viber_stop(void)
{
    uint32_t err_code = app_timer_stop(m_viber_timer);
    APP_ERROR_CHECK(err_code);
    isStop = true;
    
#ifdef USING_PWM
    app_pwm_disable(&PWM0);
#else
    nrf_gpio_pin_clear(VIBER_PIN); // clear 
#endif

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
