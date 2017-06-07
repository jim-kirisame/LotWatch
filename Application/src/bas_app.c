#include <stdint.h>
#include <string.h>
#include "bas_app.h"
#include "app_error.h"
#include "ble_bas.h"
#include "nrf_adc.h"
#include "key_app.h"
#include "config_storage.h"
#include "comm_protocol.h"

#define ADC_RESULT_QUEUE_SIZE 7
uint16_t adc_result_queue[ADC_RESULT_QUEUE_SIZE];   /**中值滤波**/
uint8_t adc_result_queue_index;         
uint8_t tick = 0, tickCount = 1;

ble_bas_t m_bas;                                    /**< Structure used to identify the battery service. */
uint32_t currVot;                                   /**< Current Vottage of battery. */            

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

uint16_t adc_result_calc()
{
    uint16_t min=0xFFFF,max=0x0000,curr;
    uint32_t total = 0;
    for(int i=0;i<ADC_RESULT_QUEUE_SIZE;i++)
    {
        curr = adc_result_queue[i];
        if(curr > max)
            max = curr;
        if(curr < min)
            min = curr;
    }
    for(int i=0;i<ADC_RESULT_QUEUE_SIZE;i++)
    {
        total += adc_result_queue[i];
    }
    total -= max;
    total -= min;
    return total / (ADC_RESULT_QUEUE_SIZE - 2);
}


void ADC_appsh_mes_evt_handler(void *p_event_data, uint16_t event_size)
{
    UNUSED_PARAMETER(p_event_data);
    UNUSED_PARAMETER(event_size);
    
    if(adc_result_queue_index >= ADC_RESULT_QUEUE_SIZE)
    {
        adc_result_queue_index = 0;
    }
    adc_result_queue[adc_result_queue_index++] = nrf_adc_result_get();
    
    if(currVot == adc2vottage(adc_result_calc()) && currVot > 0) // 数据稳定后才延长测量时间
    {
        tickCount = 10;
    }
    currVot = adc2vottage(adc_result_calc());
    bas_update_measure_data();
}
/** ADC测量完毕*/
void ADC_IRQHandler(){
    nrf_adc_conversion_event_clean();
    nrf_adc_stop();
    app_sched_event_put(NULL, NULL, ADC_appsh_mes_evt_handler);
}
/** ADC测量时钟*/
void bas_measure_timer_handler(void *p_context){
    UNUSED_PARAMETER(p_context);
        nrf_adc_start();
}
void adc_tick()
{
    tick++;
    if(tick >= tickCount)
    {
        tick = 0;
        bas_measure_timer_handler(NULL);
    }
}
/** 初始化电量服务 */
void bas_app_init(){
    adc_init();
}
/* 启动电量测量时钟*/
void bas_timer_start(){
        //do something
}
/*停止电量测量时钟*/
void bas_timer_stop(){
    //do something
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
    return wchData.temporary.battery_level;
}
/*获取当前电压*/
uint16_t bas_get_cur_bat_vot(){
    return currVot;
}
/*手工同步电量信息*/
void bas_update_measure_data(){
    if(wchData.temporary.battery_level != bas_vot2lvl(currVot))
    {
        wchData.temporary.battery_level = bas_vot2lvl(currVot);
        comm_send_status();
    }
}
