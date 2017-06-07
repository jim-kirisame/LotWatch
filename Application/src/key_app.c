#include "main.h"
#include "key_app.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "mma8452.h"
#include "config_storage.h"

_Bool charge_charging;
_Bool charge_fulled;

typedef struct key_pin_event_data
{
    nrf_drv_gpiote_pin_t pin;
    nrf_gpiote_polarity_t action;
    
} key_pin_event_data;

void (* press_evt_handler)(uint8_t);

void key_appsh_evt_handler(void *p_event_data, uint16_t event_size)
{
    enum key_evt_type event;
    key_pin_event_data * temp = (key_pin_event_data * )p_event_data;
    switch(temp->pin){
        case INT_PIN:
            switch(mma8452_read_int())
            {
                case 0xC0:
                    event = TAP_ONCE_EVENT;
                    break;
                case 0xC8:
                    event = TAP_TWICE_EVENT;
                    break;
                case 0xA2:
                    event = WRIST_ROTATE_EVENT;
                    break;
                default:
                    break;
            }
            break;
            /*
        case KEY_PIN:
            event = TOUCH_KEY_EVENT;
            break;*/
        case CHRG_PIN:
        case STDBY_PIN:
            charge_charging = !nrf_gpio_pin_read(CHRG_PIN_2);
            charge_fulled = !nrf_gpio_pin_read(STDBY_PIN_2);
            if(charge_charging && ! charge_fulled) event = FULLED_EVENT;
            else if(!charge_charging && charge_fulled) event = CHARGING_EVENT;
            break;
        default:
            event = (enum key_evt_type)temp->pin;
            break;
    }

    (*press_evt_handler)((uint8_t)event);
}

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    key_pin_event_data event;
    event.pin = pin;
    event.action = action;
    app_sched_event_put(&event, sizeof(event), key_appsh_evt_handler);
}

void key_set_evt_handler(void (*evt)(uint8_t))
{
    press_evt_handler = evt;
}

void key_generate_evt(enum key_evt_type event)
{
    key_pin_event_data temp;
    temp.pin = event;
    app_sched_event_put(&temp, sizeof(temp), key_appsh_evt_handler);
}

void key_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    /*
    //Touch key
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
    in_config.pull = NRF_GPIO_PIN_PULLDOWN;
    

    err_code = nrf_drv_gpiote_in_init(KEY_PIN, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(KEY_PIN, true);
    */
    
    //acc interrupt
    nrf_drv_gpiote_in_config_t in_config_2 = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
    in_config_2.pull = NRF_GPIO_PIN_PULLUP;
    
    err_code = nrf_drv_gpiote_in_init(INT_PIN, &in_config_2, in_pin_handler);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_in_event_enable(INT_PIN, true);

    
    //charging interrupt
    nrf_drv_gpiote_in_config_t in_config_3 = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
    in_config_3.pull = NRF_GPIO_PIN_PULLUP;
    
    err_code = nrf_drv_gpiote_in_init(CHRG_PIN, &in_config_3, in_pin_handler);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_in_event_enable(CHRG_PIN, true);
    
    //full interrupt
    err_code = nrf_drv_gpiote_in_init(STDBY_PIN, &in_config_3, in_pin_handler);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_in_event_enable(STDBY_PIN, true);
    nrf_gpio_cfg_input(CHRG_PIN_2, NRF_GPIO_PIN_PULLUP );
    nrf_gpio_cfg_input(STDBY_PIN_2, NRF_GPIO_PIN_PULLUP );
    nrf_gpio_cfg_input(INT2_PIN, NRF_GPIO_PIN_PULLUP );
}

