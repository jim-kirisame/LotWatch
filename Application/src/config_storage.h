#ifndef _CONFIG_STORAGE_
#define _CONFIG_STORAGE_

#include <stdint.h>
#include "alarm_app.h"
#include "step_counter.h"
#include "comm_protocol.h"
#include "app_util.h"

#define PERSISTENT_DATA_VERSION 0x00010001

/** 手表配置 **/
typedef struct watch_config
{
    uint8_t alarm_delay_max_count;              //再响次数
    uint8_t alarm_delay_time;                   //再响时间间隔
    uint8_t alarm_vibra_time;                   //闹钟震动时长
    _Bool   debug_enable;                       //是否显示调试页面
    
    uint8_t display_sleep_time;                 //显示时长
    
    packet_userdata step_userdata;              //用户数据
    alarm_data_item alarm_data[MAX_ALARM_COUNT];//闹钟数据
} watch_config;

/** 手表留存数据，将会写入ROM **/
typedef struct watch_persistent_data
{
    watch_config        config;
    uint32_t            unix_timestamp;         //上次的时间戳
    
    packet_walkdata     step_walkdata;          //计步器数据
    packet_sleepdata    step_sleepdata[10];     //睡眠数据 
    
    uint8_t             space[2];               //数据补齐

} watch_persistent_data;

STATIC_ASSERT(sizeof(watch_persistent_data)%4 == 0);

/** 手表临时数据，不会写入ROM **/
typedef struct watch_temporary_data
{
    uint8_t battery_level;                      //电量数据
    
    _Bool charge_charging;                      //充电中
    _Bool charge_full;                          //充满
    
    _Bool disp_awake;                           //手表是否唤醒
    
    char pair_passcode[7];                      //配对用密码
    
    uint8_t page_current_screen;                //当前显示页面
    _Bool page_should_render_every_frame;       //是否渲染每帧
    _Bool page_keep_awake;                      //是否保持唤醒
    
    float temp_current_temp;                    //当前手表温度
    
    packet_message message;                     //下发通知信息

} watch_temporary_data;

typedef struct watch_data {
    watch_persistent_data persist;
    watch_temporary_data temporary;
} watch_data;

extern watch_data watch_config_data;
void config_init(void);
void config_write(void);
void config_load_default(void);

#endif
