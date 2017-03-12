#include <stdint.h>
#include <string.h>
#include "bas_app.h"
#include "app_error.h"
#include "app_timer.h"
#include "ble_bas.h"
#include "nrf_adc.h"
#include "key_app.h"

ble_bas_t m_bas;                                    /**< Structure used to identify the battery service. */
uint32_t currVot;                                   /**< Current Vottage of battery. */

APP_TIMER_DEF(m_battery_timer_meas_id);             /**< Battery timer. */

/**@brief Function for initializing Battery Service.
 */
void bas_init(void)
{
    uint32_t err_code;
    ble_bas_init_t bas_init_obj;

    memset(&bas_init_obj, 0, sizeof(bas_init_obj));

    bas_init_obj.evt_handler = NULL;
    bas_init_obj.support_notification = true;
    bas_init_obj.p_report_ref = NULL;
    bas_init_obj.initial_batt_level = 100;

    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&bas_init_obj.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&bas_init_obj.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init_obj.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&bas_init_obj.battery_level_report_read_perm);

    err_code = ble_bas_init(&m_bas, &bas_init_obj);
    APP_ERROR_CHECK(err_code);
}
/**@brief adc服务初始化
 */
void adc_init(void){
    // Todo: add some code here!
    nrf_adc_config_t nrf_adc_config = {BATTERY_ADC_RESOLUTION,                // 10bit 精度
                                       NRF_ADC_CONFIG_SCALING_INPUT_FULL_SCALE, // 完整输入
                                       NRF_ADC_CONFIG_REF_VBG};                 // 内置1.2V基准源

    // Initialize and configure ADC
    nrf_adc_configure((nrf_adc_config_t *)&nrf_adc_config);
    nrf_adc_input_select(ADC_INPUT_CHANNEL);
    nrf_adc_int_enable(ADC_INTENSET_END_Enabled << ADC_INTENSET_END_Pos);
        NVIC_SetPriority(ADC_IRQn, 3); //1: High; 3: Low
    NVIC_EnableIRQ(ADC_IRQn);
}

/** ADC结果转换到实际电压 */
uint16_t adc2vottage(int32_t adcResult){
    // Vmes = Vreal * 2.2M / (10M + 2.2M)
    // result = Vmes / Vref * BATTERY_ADC_DIV
    // 22/122约等于2/11
    return (uint32_t)(adcResult * ADC_REF_VOLTAGE_IN_MILLIVOLTS * 11 >> 11);
}
/** ADC测量完毕*/
void ADC_IRQHandler(){
    nrf_adc_conversion_event_clean();
    currVot = adc2vottage(nrf_adc_result_get());
    bas_update_measure_data();
}
/** ADC测量时钟*/
void bas_measure_timer_handler(void *p_context){
    UNUSED_PARAMETER(p_context);
    nrf_adc_start();
}
/** 初始化电量服务 */
void bas_app_init(){
    bas_init();
    adc_init();
}
/* 初始化电量测量时钟*/
void bas_timer_init(){
    uint8_t err_code = app_timer_create(&m_battery_timer_meas_id,
                                APP_TIMER_MODE_REPEATED,
                                bas_measure_timer_handler);
    APP_ERROR_CHECK(err_code);
}
/* 启动电量测量时钟*/
void bas_timer_start(){
    uint32_t err_code;

    err_code = app_timer_start(m_battery_timer_meas_id, BATTERY_MEASURE_INTERVAL_MS, NULL);
    APP_ERROR_CHECK(err_code);
}
/*停止电量测量时钟*/
void bas_timer_stop(){
    uint32_t err_code = app_timer_stop(m_battery_timer_meas_id);
    APP_ERROR_CHECK(err_code);
}
/*电量服务事件处理器*/
void bas_ble_evt_dispatch(ble_evt_t * p_ble_evt){
    ble_bas_on_ble_evt(&m_bas, p_ble_evt);
}
/*电压转换至电量*/
uint8_t bas_vot2lvl(uint16_t lvl){
    if(charge_fulled)
        return 100;
    if(lvl>4200)
        return 100;
    else if(lvl>4000)
        return 90 + (lvl-4000)/20;
    else if(lvl>3600)
        return 10 + (lvl-3600)/5;
    else if(lvl>3200)
        return (lvl-3200)/40;
    else
        return 0;
}
/*获取当前电量*/
uint8_t bas_get_cur_bat_lvl(){
    return bas_vot2lvl(currVot);
}
/*获取当前电压*/
uint16_t bas_get_cur_bat_vot(){
    return currVot;
}
/*手工同步电量信息*/
void bas_update_measure_data(){
    ble_bas_battery_level_update(&m_bas, bas_get_cur_bat_lvl());
}
