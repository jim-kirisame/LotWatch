#ifndef _COMM_PROTOCOL_
#define _COMM_PROTOCOL_

#include <stdint.h>

#define PROTOCOL_START_FLAG 0x55
#define PROTOCOL_VERSION    0x01

enum protocol_operation{
    STATUS = 0x00,
    
    TIME        =   0x02,
    ALARM       =   0x03,
    NOTIFY      =   0x04,
    USER_DATA   =   0x05,
    
    EVENT_ACK                       = 0x10,
    EVENT_SUCCESS                   = 0x11,
    EVENT_FAIL_INVALID_OPERATION    = 0x12,
    EVENT_FAIL_WRONG_LENGTH         = 0x13,
    EVENT_FAIL_WRONG_CRC            = 0x14,
    EVENT_WRONG_HEADER              = 0x15,
    EVENT_WRONG_VERSION             = 0x16
};

void comm_send_packet_L0(enum protocol_operation operation);
void comm_send_packet_L1(enum protocol_operation operation, uint8_t * data);
void comm_proto_send_appsh_handler(void *p_event_data, uint16_t event_size);
void comm_proto_recv_appsh_handler(void *p_event_data, uint16_t event_size);

typedef struct packet_L0{
    uint8_t start;
    uint8_t version;
    enum protocol_operation operation;
    uint8_t checksum;
} packet_L0;

typedef struct packet_L1{
    uint8_t start;
    uint8_t version;
    enum protocol_operation operation;
    uint8_t data[5];
    uint8_t checksum;
} packet_L1;

#endif
