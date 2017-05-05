#include "main.h"
#include "comm_protocol.h"
//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "crc16.h"
#include "ble_nus.h"
#include "rtc_app.h"
#include "step_counter.h"
#include "config_storage.h"
#include "mma8452.h"

uint8_t comm_send_buff[128];
uint8_t comm_send_buff_len = 0;
struct {
    queue_ack queue[10];
    uint8_t index;
} comm_ack_stack;

_Bool comm_send_flag = false;
extern ble_nus_t m_nus;

void comm_ack_queue_add(void (*p_handle)(uint8_t), uint8_t data)
{
    comm_ack_stack.queue[comm_ack_stack.index].operation = p_handle;
    comm_ack_stack.queue[comm_ack_stack.index++].data = data;
}

void comm_send_step_data_current(uint8_t index)
{
    packet_common_id data;
    data.type = index;
    switch(index){
        case 0x00:
            data.data = wchData.persist.step_walkdata.walking_slow;
            break;
        case 0x01:
            data.data = wchData.persist.step_walkdata.walking_fast;
            break;
        case 0x02:
            data.data = wchData.persist.step_walkdata.run;
            break;
        case 0x03:
            data.data = wchData.persist.step_walkdata.distance;
            break;
        case 0x04:
            data.data = wchData.temporary.step_lastday.cal;
            break;
        case 0x10:
            data.data = wchData.temporary.step_lastday.walking_slow;
            break;
        case 0x11:
            data.data = wchData.temporary.step_lastday.walking_fast;
            break;
        case 0x12:
            data.data = wchData.temporary.step_lastday.run;
            break;
        case 0x13:
            data.data = wchData.temporary.step_lastday.distance;
            break;
        case 0x14:
            data.data = wchData.temporary.step_lastday.cal;
            break;
        default:
            return;
    }
    comm_send_packet_L1(STEP_DATA, (uint8_t *)&data);
    comm_ack_queue_add(comm_send_step_data_current, index+1);
}

void comm_send_alarm_packet(uint8_t index)
{
    if(index >= MAX_ALARM_COUNT)
        return;
    
    packet_alarm packet;
    packet.id = index;
    packet.hour = wchData.persist.config.alarm_data[index].hour;
    packet.minute = wchData.persist.config.alarm_data[index].minute;
    packet.repeat_mode = wchData.persist.config.alarm_data[index].repeat;
    
    comm_send_packet_L1(STEP_DATA, (uint8_t *)&packet);
    comm_ack_queue_add(comm_send_alarm_packet, index+1);
}

/**
 * Function:
 * Params:
 * Return:
 **/
void comm_send_user_data()
{
    uint8_t p[8];
    memcpy(&p, &wchData.persist.config.step_userdata,8);
    comm_send_packet_L2(USER_DATA, p);
}

/**
 * Function:
 * Params:
 * Return:
 **/
void comm_send_conf_name()
{
    uint8_t p[8];
    memcpy(&p, &wchData.persist.config.ble_name,8);
    comm_send_packet_L2(CONF_NAME, p);
}

/**
 * Function:
 * Params:
 * Return:
 **/
void comm_send_conf_behave()
{
    uint8_t p[8];
    p[0] = wchData.persist.config.display_sleep_time;
    p[1] = wchData.persist.config.alarm_delay_time;
    p[2] = wchData.persist.config.alarm_delay_max_count;
    p[3] = wchData.persist.config.alarm_vibra_time;
    p[4] = wchData.persist.config.debug_enable;   
    
    comm_send_packet_L2(CONF_BEHAVE, p);
}

/**
 * Function:
 * Params:
 * Return:
 **/
void comm_send_conf_page()
{
    uint8_t p[8];
    memcpy(&p, &wchData.persist.config.page_order,8);
    comm_send_packet_L2(USER_DATA, p);
}

void comm_send_sleep_data(uint8_t index);

void comm_send_sleep_data_ack(uint8_t index)
{
    wchData.persist.step_sleepindex--; //do not empty data 
    comm_send_sleep_data(index);
}

/**
 * Function:
 * Params:
 * Return:
 **/
void comm_send_sleep_data(uint8_t index)
{
    uint8_t p[5];
    
    if(wchData.persist.step_sleepindex <= 0)
        return;

    memcpy(&p, &wchData.persist.step_sleepdata[wchData.persist.step_sleepindex - 1],5); 
    comm_send_packet_L2(USER_DATA, p);
    comm_ack_queue_add(comm_send_sleep_data_ack, index+1);
}


void comm_ack_queue_exec(void)
{
    if(comm_ack_stack.index>0)
    {
        comm_ack_stack.index--;
        (*comm_ack_stack.queue[comm_ack_stack.index].operation)(comm_ack_stack.queue[comm_ack_stack.index].data);
    }
    
}

void comm_recv_alarm_packet(packet_alarm * packet)
{
    if(packet->id < MAX_ALARM_COUNT)
    {
        wchData.persist.config.alarm_data[packet->id].hour = packet->hour;
        wchData.persist.config.alarm_data[packet->id].minute = packet->minute;
        wchData.persist.config.alarm_data[packet->id].repeat = packet->repeat_mode;
        comm_send_packet_L0(EVENT_SUCCESS);
    }
    else
    {
        comm_send_packet_L0(EVENT_WRONG_DATA);
    }
}

void comm_recv_packet_L0(packet_L0 * data){
    if(comm_ack_stack.index > 0) //well, if no ack response here, clear ack resp stack
    {
        if(data->operation != EVENT_ACK)
        {
            comm_ack_stack.index = 0;
        }        
    }
    
    switch(data->operation){
        case EVENT_ACK:
            comm_ack_queue_exec();
            break;
        case EVENT_FAIL_WRONG_CRC:
        case EVENT_FAIL_INVALID_OPERATION:
        case EVENT_SUCCESS:
            break;
        case STEP_DATA:
            comm_send_step_data_current(0x00);
            break;
        case ALARM:
            comm_send_alarm_packet(0x00);
            break;
        case USER_DATA:
            comm_send_user_data();
            break;
        case CONF_PAGE:
            comm_send_conf_page();
        break;
        case CONF_NAME:
            comm_send_conf_name();
        break;
        case CONF_BEHAVE:
            comm_send_conf_behave();
        break;
        case SLEEP_DATA:
            comm_send_sleep_data(0);
        break;
        
        default:
            comm_send_packet_L0(EVENT_FAIL_INVALID_OPERATION);
            break;
    }
}


void comm_recv_packet_L1(packet_L1 * data){
    switch(data->operation){
        case TIME:
            rtc_setTimeUnix((data->data[0]<<24) + (data->data[1]<<16) + (data->data[2]<<8) + (data->data[3]) );
            step_health_algorithm_reload();
            comm_send_packet_L0(EVENT_SUCCESS);
        break;
        case ALARM:
        {
            comm_recv_alarm_packet((packet_alarm *)&data->data[0]);
            break;
        }
        case NOTIFY:
            memcpy(&wchData.temporary.message, data->data,5);
            //generate some event here
            comm_send_packet_L0(EVENT_SUCCESS);
        break;
        case USER_DATA:
            memcpy(&wchData.persist.config.step_userdata, data->data,5);
            step_health_algorithm_reload();
            comm_send_packet_L0(EVENT_SUCCESS);
        default:
            comm_send_packet_L0(EVENT_FAIL_INVALID_OPERATION);
        break;
    }
}

void comm_recv_packet_L2(packet_L2 * data){
    switch(data->operation)
    {
        case CONF_NAME:
            memcpy(wchData.persist.config.ble_name, data->data, 8);
            //dosomething
            
            comm_send_packet_L0(EVENT_SUCCESS);
            break;
        case CONF_BEHAVE:
            wchData.persist.config.display_sleep_time = data->data[0];
            wchData.persist.config.alarm_delay_time = data->data[1];
            wchData.persist.config.alarm_delay_max_count = data->data[2];
            wchData.persist.config.alarm_vibra_time = data->data[3];
            wchData.persist.config.debug_enable = data->data[4];      
            comm_send_packet_L0(EVENT_SUCCESS);
            break;
        case CONF_PAGE:
            memcpy(wchData.persist.config.page_order, data->data, 8);
            comm_send_packet_L0(EVENT_SUCCESS);
            break;
        default:
            comm_send_packet_L0(EVENT_FAIL_INVALID_OPERATION);
    }
}

//接收信息并处理
void comm_proto_recv_appsh_handler(void *p_event_data, uint16_t event_size)
{
    if(event_size < 2)
    {
        comm_send_packet_L0(EVENT_FAIL_WRONG_LENGTH);
        return;
    }
    if(((uint8_t *)p_event_data)[0] != PROTOCOL_START_FLAG)
    {
        comm_send_packet_L0(EVENT_WRONG_HEADER);
        return;
    }
    
    if(comm_ack_stack.index > 0) //well, if no ack response here, clear ack resp stack
    {
        if(event_size != sizeof(packet_L0))
        {
            comm_ack_stack.index = 0;
        }        
    }
        
    
    switch(event_size)
    {
        case sizeof(packet_L0):
            comm_recv_packet_L0((packet_L0 *)p_event_data);
            break;
        case sizeof(packet_L1):
            comm_recv_packet_L1((packet_L1 *)p_event_data);
            break;
        case sizeof(packet_L2):
            comm_recv_packet_L2((packet_L2 *)p_event_data);
            break;
        default:
            comm_send_packet_L0(EVENT_FAIL_WRONG_LENGTH);
            break;
    }

}

//发送信息
void comm_proto_send_appsh_handler(void *p_event_data, uint16_t event_size)
{
    UNUSED_PARAMETER(p_event_data);
    UNUSED_PARAMETER(event_size);
    ble_nus_string_send(&m_nus, &comm_send_buff[0], comm_send_buff_len);
    comm_send_flag = false;
    comm_send_buff_len = 0;
}

void comm_send_packet_L0(enum protocol_operation operation){
    packet_L0 p;
    p.start = PROTOCOL_START_FLAG;
    p.operation = (uint8_t)operation;
    
    if(comm_send_buff_len > 123)
        return;
    memcpy(&comm_send_buff[comm_send_buff_len], &p, sizeof(p));
    comm_send_buff_len += sizeof(packet_L0);
    
    if(!comm_send_flag)
    {
        comm_send_flag = true;
        app_sched_event_put(NULL, NULL, comm_proto_send_appsh_handler);
    }
}

void comm_send_packet_L1(enum protocol_operation operation, uint8_t * data){
    packet_L1 p;
    p.start = PROTOCOL_START_FLAG;
    p.operation = (uint8_t)operation;
    
    if(comm_send_buff_len > 118)
        return;
    memcpy(&comm_send_buff[comm_send_buff_len], &p, sizeof(packet_L1));
    comm_send_buff_len += sizeof(packet_L1);
    
    if(!comm_send_flag)
    {
        comm_send_flag = true;
        app_sched_event_put(NULL, NULL, comm_proto_send_appsh_handler);
    }
}

void comm_send_packet_L2(enum protocol_operation operation, uint8_t * data){
    packet_L2 p;
    p.start = PROTOCOL_START_FLAG;
    p.operation = (uint8_t)operation;
    
    if(comm_send_buff_len > 128 - sizeof(p))
        return;
    memcpy(&comm_send_buff[comm_send_buff_len], &p, sizeof(packet_L2));
    comm_send_buff_len += sizeof(packet_L2);
    
    if(!comm_send_flag)
    {
        comm_send_flag = true;
        app_sched_event_put(NULL, NULL, comm_proto_send_appsh_handler);
    }
}

void comm_send_acc_data(mma8452_acc_data data)
{
    if(comm_send_buff_len > 128 - sizeof(data))
        return;

    memcpy(&comm_send_buff[comm_send_buff_len], &data, sizeof(data));
    comm_send_buff_len += sizeof(data);
    comm_send_buff[comm_send_buff_len++] = 0;
    
    if(!comm_send_flag)
    {
        comm_send_flag = true;
        app_sched_event_put(NULL, NULL, comm_proto_send_appsh_handler);
    }
}
