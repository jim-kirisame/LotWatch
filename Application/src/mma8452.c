#include "main.h"
#include "app_error.h"
#include "mma8452.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "twi_master.h"

#include "nrf_drv_config.h"

void mma8452_i2c_init(void){
    twi_master_init();
}

void mma8452_i2c_write_raw(uint8_t * data, uint8_t len)
{
    twi_master_transfer(I2C_SLAVE_ADDRESS, data, len, false);
}

void mma8452_i2c_write(uint8_t addr, uint8_t data)
{
    uint8_t packet[] = {addr, data};
    mma8452_i2c_write_raw(packet, 2);
}

void mma8452_i2c_read(uint8_t addr, uint8_t * buf, uint8_t buflen)
{
    if(twi_master_transfer(I2C_SLAVE_ADDRESS, &addr, 1, false))
        twi_master_transfer(I2C_SLAVE_ADDRESS | TWI_READ_BIT, buf, buflen, false);
}

void mma8452_chip_init(void){
    mma8452_i2c_write(0x0e, 0x00); //2g acc & no 
    mma8452_i2c_write(0x0f, 0x00); //Pulse_LPF_EN = 0
    mma8452_i2c_write(0x21, 0x3c); //double tap and single tap on z-axis only
    mma8452_i2c_write(0x23, 0x30); //Set x Threshold to 32 counts or 2g
    mma8452_i2c_write(0x24, 0x40); //Set y Threshold to 32 counts or 2g
    mma8452_i2c_write(0x25, 0x40); //Set Z Threshold to 32 counts or 2g
    mma8452_i2c_write(0x26, 0x0a); //50ms / 5ms = 10 @50hz, Normal, Pulse_LPF_EN
    mma8452_i2c_write(0x27, 0x14); //200ms / 10ms = 20
    mma8452_i2c_write(0x28, 0x0e); //300ms / 10ms = 30
    
    mma8452_i2c_write(0x2a, 0x30); //50hz, multibyte read, standby
    mma8452_i2c_write(0x2b, 0x00); //normal mode
    //mma8452_i2c_write(0x2c, 0x02); //interrupt active high
    mma8452_i2c_write(0x2d, 0x08); //enable interrupt of tap detection
    mma8452_i2c_write(0x2e, 0x08); //set interrupt on INT1
    mma8452_i2c_write(0x2a, 0x31); //50hz, multibyte read, awake
}

_Bool mma8452_chip_test(void)
{
    uint8_t rx;
    mma8452_i2c_read(0x0d, &rx, 1);
    return rx == 0x2A;
}

void mma8452_init(void){
    mma8452_i2c_init();
    mma8452_chip_init();
}

void mma8452_read_acc(mma8452_acc_data * data)
{
    uint8_t buf[2];
    mma8452_i2c_read(0x01, buf, 2);
    (*data).x = buf[1] + ((int16_t)buf[0]<<8); 
    (*data).x /= 16;
    mma8452_i2c_read(0x03, buf, 2);
    (*data).y = buf[1] + ((int16_t)buf[0]<<8); 
    (*data).y /= 16;
    mma8452_i2c_read(0x05, buf, 2);
    (*data).z = buf[1] + ((int16_t)buf[0]<<8); 
    (*data).z /= 16;
}

_Bool mma8452_read_isDoubleTap(void)
{
    uint8_t buf[1];
    mma8452_i2c_read(0x22, buf, 1);
    return buf[0] & 0x80 == 0x80;
}