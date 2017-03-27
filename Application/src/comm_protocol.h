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
    STEP_DATA   =   0x06,
    SLEEP_DATA  =   0x07,
    CONF_NAME   =   0x08,
    CONF_BEHAVE =   0x09,
    CONF_PAGE   =   0x0a,
    
    EVENT_ACK                       = 0x10,
    EVENT_SUCCESS                   = 0x11,
    EVENT_FAIL_INVALID_OPERATION    = 0x12,
    EVENT_FAIL_WRONG_LENGTH         = 0x13,
    EVENT_FAIL_WRONG_CRC            = 0x14,
    EVENT_WRONG_HEADER              = 0x15,
    EVENT_WRONG_VERSION             = 0x16,
    EVENT_WRONG_DATA                = 0x17,
};

enum step_type{
    STEP_WALK_SLOW = 0x00,
    STEP_WALK_FAST,
    STEP_RUN,
    STEP_DISTANCE,
    STEP_CAL
};

void comm_send_packet_L0(enum protocol_operation operation);
void comm_send_packet_L1(enum protocol_operation operation, uint8_t * data);
void comm_send_packet_L2(enum protocol_operation operation, uint8_t * data);
void comm_proto_send_appsh_handler(void *p_event_data, uint16_t event_size);
void comm_proto_recv_appsh_handler(void *p_event_data, uint16_t event_size);

typedef struct packet_L0{
    uint8_t start;
    uint8_t version;
    uint8_t operation;
    uint8_t checksum;
} packet_L0;

typedef struct packet_L1{
    uint8_t start;
    uint8_t version;
    uint8_t operation;
    uint8_t data[5];
    uint8_t checksum;
} packet_L1;

typedef struct packet_L2{
    uint8_t start;
    uint8_t version;
    uint8_t operation;
    uint8_t data[8];
    uint8_t checksum;
} packet_L2;

typedef struct packet_alarm{
    uint8_t id;
    uint8_t repeat_mode;
    uint8_t hour;
    uint8_t minute;
    uint8_t mode;
} packet_alarm;

typedef struct packet_message{
    uint8_t call;
    uint8_t message;
    uint8_t qq;
    uint8_t wechat;
    uint8_t others;
} packet_message;


typedef struct packet_common_id{
    uint8_t type;
    uint32_t data;
} packet_common_id;

typedef struct queue_ack{
    void (*operation)(uint8_t);
    uint8_t data;
} queue_ack;

#endif
