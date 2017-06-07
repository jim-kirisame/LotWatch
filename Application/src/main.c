/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup
 * from button, advertise, get a connection restart advertising on disconnect and if no new
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
 */

#include <stdint.h>
#include <string.h>
#include "app_error.h"
#include "main.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_timer_appsh.h"
#include "app_scheduler.h"
#include "device_manager.h"
#include "pstorage.h"
//#include "app_trace.h"
#include "nrf_drv_wdt.h"

#include "ble_nus.h"

#include "dfu_app.h"
#include "bas_app.h"

#include "ssd1306_app.h"
#include "disp_app.h"
#include "nrf_gpio.h"
#include "mma8452.h"
#include "temp_app.h"
#include "rtc_app.h"
#include "key_app.h"
#include "app_page.h"
#include "comm_protocol.h"
#include "step_counter.h"
#include "alarm_app.h"
#include "viber_app.h"
#include "config_storage.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                           /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define DEVICE_NAME "LotWatch"                                                      /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME "Lotlab"                                                  /**< Manufacturer. Will be passed to Device Information Service. */

#define APP_ADV_FAST_INTERVAL 160                                                   /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_FAST_TIMEOUT_IN_SECONDS 20                                          /**< The advertising timeout in units of seconds. */

#define APP_ADV_SLOW_INTERVAL 1600                                                  /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_SLOW_TIMEOUT_IN_SECONDS 0                                         /**< The advertising timeout in units of seconds. */

#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define MIN_CONN_INTERVAL MSEC_TO_UNITS(100, UNIT_1_25_MS)                          /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(200, UNIT_1_25_MS)                          /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY 0                                                             /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS)                            /**< Connection supervisory timeout (4 seconds). */

#define SCREEN_SAVER_INTERVAL_MS (APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER))
#define SECURITY_REQUEST_DELAY         APP_TIMER_TICKS(4000, APP_TIMER_PRESCALER)   /**< Delay after connection until Security Request is sent, if necessary (ticks). */
#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER)   /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3                                              /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_TIMEOUT                 30                                        /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND 1                                                            /**< Perform bonding. */
#define SEC_PARAM_MITM 1                                                            /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES BLE_GAP_IO_CAPS_DISPLAY_ONLY                      /**< No I/O capabilities. */
#define SEC_PARAM_OOB 0                                                             /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE 7                                                    /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE 16                                                   /**< Maximum encryption key size. */

/*****************************************************************************
* add codes for enalbe app schedule
******************************************************************************/
#define SCHED_MAX_EVENT_DATA_SIZE       16                                          /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE                30                                          /**< Maximum number of events in the scheduler queue. */

#define DEAD_BEEF 0xDEADBEEF                                                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define APP_IRQ_PRIORITY_HIGH   NRF_APP_PRIORITY_HIGH

STATIC_ASSERT(IS_SRVC_CHANGED_CHARACT_PRESENT);                                     /** When having DFU Service support in application the Service Changed Characteristic should always be present. */  
APP_TIMER_DEF(m_screen_saver_timer);                                                /**< Battery timer. */          

static dm_application_instance_t m_app_handle;                                      /**< Application identifier allocated by device manager */

uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                                   /**< Handle of the current connection. */

static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
                                   {BLE_UUID_NUS_SERVICE,                BLE_UUID_TYPE_BLE}}; /**< Universally unique service identifiers. */
                                   
ble_nus_t                        m_nus;                                             /**< Structure to identify the Nordic UART Service. */                                  
int8_t                   m_adv_state;
                                   
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // This call can be used for debug purposes during application development.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    //ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    viber_action(error_code);
    NVIC_SystemReset();
}
                                   
/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

void screen_enter_sleep_mode(void)
{
    wchData.temporary.disp_awake= false;
    ssd1306_displayOff();
    ssd1306_spi_uninit();   //stop spi to save power
}
static void adv_start_switch(ble_adv_mode_t mode);
void screen_exit_sleep_mode(void)
{
    ssd1306_spi_init();     
    wchData.temporary.disp_awake = true;
    ssd1306_displayOn();
    adv_start_switch(BLE_ADV_MODE_FAST);
}

void screen_saver(void *p_context){
    UNUSED_PARAMETER(p_context);
    if(!wchData.temporary.page_keep_awake)
    {
        screen_enter_sleep_mode();
    }
        //app_sched_event_put(NULL, NULL, screen_saver_appsh_handler);
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    // Initialize timer module.
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
}

static void timers_create(void)
{
    // Create timers.
    alarm_timer_init();
    
    uint8_t err_code = app_timer_create(&m_screen_saver_timer,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                screen_saver);
    APP_ERROR_CHECK(err_code);
    
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)wchData.persist.config.ble_name,
                                          strlen(wchData.persist.config.ble_name));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
                                          
    err_code = sd_ble_gap_tx_power_set(-8);
    APP_ERROR_CHECK(err_code);

}
/**@snippet [Handling the data received over UART] */

/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
    app_sched_event_put(p_data, length, comm_proto_recv_appsh_handler);
}
/**@snippet [Handling the data received over BLE] */

static void nus_init(void)
{
    uint32_t       err_code;
    ble_nus_init_t nus_init;
    
    
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;
    
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    dfu_serv_init(&m_app_handle);
	bas_app_init();
    nus_init();
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = on_conn_params_evt;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting timers.
*/
static void application_timers_start(void)
{
    /* YOUR_JOB: Start your timers. below is an example of how to start a timer.
    uint32_t err_code;
    err_code = app_timer_start(m_app_timer_id, TIMER_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code); */
	//bas_timer_start();
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.


static void sleep_mode_enter(void)
{
    uint32_t err_code;

    //err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    //APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    //err_code = bsp_btn_ble_sleep_mode_prepare();
    //APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    //mma8452_sleep();
    
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
    
    
    
	///bas_timer_stop();
}
 */
static void adv_start_switch(ble_adv_mode_t mode)
{
    uint32_t err_code;
    if(m_adv_state != 0)
        return;
    err_code = ble_advertising_start(mode);
    APP_ERROR_CHECK(err_code);
    m_adv_state = 1;
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    //uint32_t err_code;

    switch (ble_adv_evt)
    {
    case BLE_ADV_EVT_FAST:
        //err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
        //APP_ERROR_CHECK(err_code);
        break;
    case BLE_ADV_EVT_IDLE:
        //adv_start_switch(NULL);
        //sleep_mode_enter();
        break;
    default:
        break;
    }
}

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t *p_ble_evt)
{
    //uint32_t err_code;
    char str2[8] = {0};

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        //err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
        //APP_ERROR_CHECK(err_code);
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        m_adv_state = -1;                   //no longer needs to adv
        //key_generate_evt(NORMAL_DISP_EVENT);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        m_conn_handle = BLE_CONN_HANDLE_INVALID;
        m_adv_state = 0;                    //adv now off
        adv_start_switch(BLE_ADV_MODE_FAST);
        break;
    
    case BLE_GAP_EVT_PASSKEY_DISPLAY:    
        for(int i=0;i<6;i++)
            snprintf(str2, 8, "%s%c", str2, p_ble_evt->evt.gap_evt.params.passkey_display.passkey[i]);
        snprintf(wchData.temporary.pair_passcode, 7, "%s", str2);
        key_generate_evt(PASSCODE_DISP_EVENT);
        break;
     case BLE_GAP_EVT_TIMEOUT:
        if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISING)
        {
            m_adv_state = 0; // If advertising times out
        }
        break;
    
    default:
        // No implementation needed.
        break;
    }
}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t *p_ble_evt)
{
    dm_ble_evt_handler(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    dfu_ble_evt_dispatch(p_ble_evt);
}

/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
    ble_advertising_on_sys_evt(sys_evt);
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_4000MS_CALIBRATION, NULL);

#if defined(S110) || defined(S130) || defined(S132)
    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
#if (defined(S130) || defined(S132))
    ble_enable_params.gatts_enable_params.attr_tab_size = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
#endif
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
#endif

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
    
    // Disable DCDC mode to achieve lower power consuption
    err_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_DISABLE);
    APP_ERROR_CHECK(err_code);
    
    // using low power mode
    err_code = sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Device Manager events.
 *
 * @param[in] p_evt  Data associated to the device manager event.
 */
static uint32_t device_manager_evt_handler(dm_handle_t const *p_handle,
                                           dm_event_t const *p_event,
                                           ret_code_t event_result)
{
    uint32_t err_code;
    dfu_device_manager_event_handler(p_handle, p_event);
    switch (event_result)
    {
        case BLE_GAP_SEC_STATUS_PASSKEY_ENTRY_FAILED: //handle passkey pairing fail event
        {
            err_code = sd_ble_gap_disconnect(m_conn_handle , BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            key_generate_evt(NORMAL_DISP_EVENT);
            return NRF_SUCCESS;
        }
        case BLE_GAP_SEC_STATUS_SUCCESS:
        case BLE_GAP_SEC_STATUS_TIMEOUT:
             key_generate_evt(NORMAL_DISP_EVENT);
             return NRF_SUCCESS;
            
        default:
            APP_ERROR_CHECK(event_result);
            return NRF_SUCCESS;
    }
}

/**@brief Function for the Device Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Device Manager.
 */
static void device_manager_init(bool erase_bonds)
{
    uint32_t err_code;
    dm_init_param_t init_param = {.clear_persistent_data = erase_bonds};
    dm_application_param_t register_param;

    // Initialize persistent storage module.
    err_code = pstorage_init();
    APP_ERROR_CHECK(err_code);

    err_code = dm_init(&init_param);
    APP_ERROR_CHECK(err_code);

    memset(&register_param.sec_param, 0, sizeof(ble_gap_sec_params_t));

    register_param.sec_param.bond = SEC_PARAM_BOND;
    register_param.sec_param.mitm = SEC_PARAM_MITM;
    register_param.sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
    register_param.sec_param.oob = SEC_PARAM_OOB;
    register_param.sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    register_param.sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    register_param.evt_handler = device_manager_evt_handler;
    register_param.service_type = DM_PROTOCOL_CNTXT_GATT_SRVR_ID;

    err_code = dm_register(&m_app_handle, &register_param);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t err_code;
    ble_advdata_t advdata;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = true;
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids = m_adv_uuids;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval = APP_ADV_FAST_INTERVAL;
    options.ble_adv_fast_timeout = APP_ADV_FAST_TIMEOUT_IN_SECONDS;
    options.ble_adv_slow_enabled = true;
    options.ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL;
    options.ble_adv_slow_interval = APP_ADV_FAST_TIMEOUT_IN_SECONDS;
    
    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool *p_erase_bonds)
{
	UNUSED_PARAMETER(p_erase_bonds);
}

/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
//#define NATIVE_SLEEP
#ifdef NATIVE_SLEEP
    __SEV();
    __WFE();
    __WFE();
#else  
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
#endif
#undef NATIVE_SLEEP
}

static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

static void wdt_event_handler(void)
{
}

static void wdt_init(void)
{
    uint32_t err_code;
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    config.reload_value = 5000;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&wchData.temporary.wdt_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
}

void key_evt(uint8_t evt)
{
    if(wchData.temporary.disp_awake){
        switch(evt)
        {
            /*
            case WRIST_ROTATE_EVENT: 
                if(current_screen == ALARM_DISP_PAGE)
                    alarm_delay_action(); //too sensitive now
            */
            case TAP_ONCE_EVENT:
            //case TAP_TWICE_EVENT:
            case TOUCH_KEY_EVENT:   
            // 
            {
                                
                switch(wchData.temporary.page_current_screen)
                {
                    case ALARM_DISP_PAGE:
                        alarm_exit();
                        break;
                    case CHARGING_PAGE:
                        charge_fulled = false;
                        break;
                    default:
                        break;
                }
                
                for(int i=0;i<sizeof(wchData.persist.config.page_order);i++)
                {
                    if(wchData.persist.config.page_order[i] == wchData.temporary.page_current_screen)
                    {
                        if(i+1 == sizeof(wchData.persist.config.page_order))//lastone
                        {
                            wchData.temporary.page_current_screen = wchData.persist.config.page_order[0];
                        } 
                        else
                        {
                            uint8_t next = wchData.persist.config.page_order[i+1];
                            if(next == 0)
                            {
                                wchData.temporary.page_current_screen = wchData.persist.config.page_order[0];
                                break;
                            }
                            else
                            {
                                wchData.temporary.page_current_screen = next;
                                break;
                            }
                        }
                    }
                    else if(i+1 == sizeof(wchData.persist.config.page_order))//lastone
                    {
                        wchData.temporary.page_current_screen = wchData.persist.config.page_order[0];
                    }
                }        
                break;  
            }
        
            default: 
                break;
        }
        app_timer_stop(m_screen_saver_timer);
        app_timer_start(m_screen_saver_timer, SCREEN_SAVER_INTERVAL_MS, NULL);
    }
    else
    {
        wchData.temporary.page_current_screen = wchData.persist.config.page_order[0];
        wchData.temporary.debug_wakeup_evt = evt;
        screen_exit_sleep_mode();
        app_timer_start(m_screen_saver_timer, SCREEN_SAVER_INTERVAL_MS, NULL);
    }
    
    switch(evt)
    {
        case PASSCODE_DISP_EVENT:
            wchData.temporary.page_current_screen = CONN_PASS_PAGE;
            break;
        case ALARM_DISP_EVENT:
            wchData.temporary.page_current_screen = ALARM_DISP_PAGE;
            break;
        case NOT_CHARGING_EVENT:
        case NOT_FULLED_EVENT:
        case NORMAL_DISP_EVENT:
            wchData.temporary.page_current_screen = wchData.persist.config.page_order[0];
            break;
        case CHARGING_EVENT:
        case FULLED_EVENT:
            wchData.temporary.page_current_screen = CHARGING_PAGE;
            break;
        default: 
            //wchData.temporary.page_current_screen = CLOCK_PAGE;
            break;
    }
    page_disp_current();
    //ssd1306_display();
}

void lotwatch_service_init(void)
{  
    ssd1306_init();
    mma8452_init();
    temp_init();
    viber_init();
    
    rtc_init();
    key_init();
    step_counter_init();
     
    key_set_evt_handler(&key_evt);
}

/**@brief Function for application main entry.
 */
int main(void)
{
    //uint32_t err_code;
    bool erase_bonds = false;

    // Initialize.   
    timers_init();
    scheduler_init();
    timers_create();
    
    buttons_leds_init(&erase_bonds);
    ble_stack_init();
    device_manager_init(erase_bonds);
    
    wdt_init();
    
    config_init();
    
    gap_params_init();
    advertising_init();
    services_init();
    conn_params_init();
    
    lotwatch_service_init();

    // Start execution.
    application_timers_start();
    adv_start_switch(BLE_ADV_MODE_FAST);

    //for test only.
    rtc_setTimeUnix(1483200000);
    
    page_disp_current();
    app_timer_start(m_screen_saver_timer, SCREEN_SAVER_INTERVAL_MS, NULL);
    
    // Enter main loop.
    for (;;)
    {
        app_sched_execute();
        
        if(wchData.temporary.page_should_render_every_frame)
        {
            page_disp_current();
        }
        if(wchData.temporary.disp_awake)
        {
            ssd1306_display();
        }
        else
        {
            wchData.temporary.wakeup_counter++;
        }
        power_manage();
        
    }
}

/**
 * @}
 */
