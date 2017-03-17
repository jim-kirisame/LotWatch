#include "main.h"
#include "comm_protocol.h"
#include <stdlib.h>
#include <string.h>
#include "crc16.h"
#include "ble_nus.h"
#include "rtc_app.h"

uint8_t comm_send_buff[128];
uint8_t comm_send_buff_len = 0;
_Bool comm_send_flag = false;
extern ble_nus_t m_nus;


void comm_recv_packet_L0(packet_L0 * data){
    if(data->checksum != checksum8((uint8_t *)data, 3))
    {
        comm_send_packet_L0(EVENT_FAIL_WRONG_CRC);
        return;
    }
    switch(data->operation){
        case EVENT_ACK:
        case EVENT_FAIL_WRONG_CRC:
        case EVENT_FAIL_INVALID_OPERATION:
        case EVENT_SUCCESS:
            break;
        
        default:
            comm_send_packet_L0(EVENT_FAIL_INVALID_OPERATION);
            break;
    }
}
void comm_recv_packet_L1(packet_L1 * data){
    uint8_t crc = checksum8((uint8_t *)data, 8);
    if(data->checksum != crc)
    {
        comm_send_packet_L0(EVENT_FAIL_WRONG_CRC);
        return;
    }
    switch(data->operation){
        case TIME:
            rtc_setTimeUnix((data->data[0]<<24) + (data->data[1]<<16) + (data->data[2]<<8) + (data->data[3]) );
            comm_send_packet_L0(EVENT_SUCCESS);
        break;
        default:
            comm_send_packet_L0(EVENT_FAIL_INVALID_OPERATION);
        break;
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
        
    if(((uint8_t *)p_event_data)[1] != PROTOCOL_VERSION)
    {
        comm_send_packet_L0(EVENT_WRONG_VERSION);
        return;
    }
    
    switch(event_size)
    {
        case sizeof(packet_L0):
            comm_recv_packet_L0((packet_L0 *)p_event_data);
            break;
        case sizeof(packet_L1):
            comm_recv_packet_L1((packet_L1 *)p_event_data);
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
    ble_nus_string_send(&m_nus, comm_send_buff, comm_send_buff_len);
    comm_send_flag = false;
    comm_send_buff_len = 0;
}

void comm_send_packet_L0(enum protocol_operation operation){
    packet_L0 p;
    p.start = PROTOCOL_START_FLAG;
    p.operation = operation;
    p.version = PROTOCOL_VERSION;
    p.checksum = checksum8((uint8_t *)&p, 3);
    
    if(comm_send_buff_len > 123)
        return;
    memcpy(&comm_send_buff[comm_send_buff_len], &p, sizeof(packet_L0));
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
    p.operation = operation;
    p.version = PROTOCOL_VERSION;
    memcpy(p.data, data, 5);
    
    p.checksum = checksum8((uint8_t *)&p, 8);
    
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
