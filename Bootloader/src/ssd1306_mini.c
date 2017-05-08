#include "ssd1306_mini.h"

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

void ssd1306_drv_init(void)
{
    nrf_gpio_cfg_output(SPI_RES);
    nrf_gpio_cfg_output(SPI_DC);
    nrf_gpio_cfg_output(SPI_SCLK);
    nrf_gpio_cfg_output(SPI_SDA);
}
void ssd1306_spi_write_raw(uint8_t data)
{
    //nrf_gpio_pin_clear(SPI_CS);
    for(int i=7;i>=0;i--)
    {
        nrf_gpio_pin_clear(SPI_SCLK);
        nrf_gpio_pin_write(SPI_SDA, data & (1 << i));
        __nop();
        nrf_gpio_pin_set(SPI_SCLK);
    }
    //nrf_gpio_pin_set(SPI_CS);
}
    
void ssd1306_spi_write(uint8_t * p_tx_data, uint16_t len, _Bool dc){
    nrf_gpio_pin_write(SPI_DC, dc); //data: true; command: false;
        
    //nrf_delay_ms(1);
    for(int i=0;i<len;i++)
    {
        ssd1306_spi_write_raw(p_tx_data[i]);
    }
}
void ssd1306_write_command(uint8_t command)
{
    ssd1306_spi_write(&command, 1, false);
    nrf_delay_ms(1);
}

void ssd1306_write_data(uint8_t * data, uint32_t len)
{
    ssd1306_spi_write(data, len, true);
}

void ssd1306_chip_init(){
    //some code
    nrf_gpio_pin_clear(SPI_RES);
    nrf_delay_ms(10);
    nrf_gpio_pin_set(SPI_RES);
    
    ssd1306_write_command(SSD1306_DISPLAYOFF);          //????
    ssd1306_write_command(SSD1306_SETDISPLAYCLOCKDIV);  //????
    ssd1306_write_command(0x80);                        //100???
    
    ssd1306_write_command(SSD1306_SETLOWCOLUMN);  // 0x00
    ssd1306_write_command(SSD1306_SETHIGHCOLUMN);   // 0x10

    ssd1306_write_command(SSD1306_SETMULTIPLEX);        // 0xA8
    ssd1306_write_command(0x3f);                        //64??

    ssd1306_write_command(SSD1306_SETDISPLAYOFFSET);    // 0xD3
    ssd1306_write_command(0x0);                         // no offset
    ssd1306_write_command(SSD1306_SETSTARTLINE);  // line #0
    ssd1306_write_command(SSD1306_CHARGEPUMP);          // 0x8D
    ssd1306_write_command(0x14);

    ssd1306_write_command(SSD1306_MEMORYMODE);          // 0x20
    ssd1306_write_command(0x00);                        // 0x0 act like ks0108
    ssd1306_write_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_write_command(SSD1306_COMSCANDEC);
    ssd1306_write_command(SSD1306_SETCOMPINS);          // 0xDA
    ssd1306_write_command(0x12);
    ssd1306_write_command(SSD1306_SETCONTRAST);         // 0x81
    ssd1306_write_command(0xCF);                        //???????????CF

    ssd1306_write_command(SSD1306_SETPRECHARGE);        // 0xd9
    ssd1306_write_command(0xF1);
    ssd1306_write_command(SSD1306_SETVCOMDETECT);       // 0xDB
    ssd1306_write_command(0x40);
    ssd1306_write_command(SSD1306_DISPLAYALLON_RESUME); // 0xA4
    ssd1306_write_command(SSD1306_NORMALDISPLAY);       // 0xA6

    ssd1306_write_command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306_write_command(SSD1306_DISPLAYON);           //--turn on oled panel
}

void ssd1306_init(void){
    ssd1306_drv_init();
    ssd1306_chip_init();
}

void ssd1306_drawByte(uint8_t x, uint8_t y, uint8_t * data, uint8_t len)
{
    if(x >=128 || y >=8)
        return;
    ssd1306_write_command(SSD1306_COLUMNADDR);
    ssd1306_write_command(x);   // Column start address (0 = reset)
    ssd1306_write_command(0x7F); // Column end address (127 = reset)

    ssd1306_write_command(SSD1306_PAGEADDR);
    ssd1306_write_command(y); // Page start address (0 = reset)
    ssd1306_write_command(7); // Page end address
    ssd1306_write_data(data, len);
}

void ssd1306_cls(void)
{
    uint8_t data[64] = {0};
    ssd1306_write_command(SSD1306_COLUMNADDR);
    ssd1306_write_command(0);   // Column start address (0 = reset)
    ssd1306_write_command(0x7F); // Column end address (127 = reset)

    ssd1306_write_command(SSD1306_PAGEADDR);
    ssd1306_write_command(0); // Page start address (0 = reset)
    ssd1306_write_command(7); // Page end address
    
    for(int i=0;i<1024;i+=64)
    {
        ssd1306_write_data(&data[0], 64);
    }
}

void ssd1306_draw5x7Font(uint8_t x, uint8_t y, char * string){
    uint8_t data[6];
    if(y > 8 || x > 128)
        return;
    
    for(int i=0;i<128;i++)
    {
        if(!string[i]) //end of string
            return;
        for(int j=0; j<5 && x<128 && string[i]>=0x20 ;j++)
            data[j] = font5x8[(string[i]-0x20)*5+j];
        data[5] = 0x00; //space
        
        ssd1306_drawByte(x + i * 6,y, data, 6);
    }
}

