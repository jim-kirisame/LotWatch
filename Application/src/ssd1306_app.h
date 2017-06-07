#ifndef _SSD1306_APP_
#define _SSD1306_APP_
#include <stdint.h>
#define SPI_RES 27
#define SPI_DC 26
#define SPI_SCLK 25
#define SPI_MOSI 24
#define SPI_MISO 18
#define OLED_EN 23
#define OLED_CS 28

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

extern uint8_t buffer[];

void ssd1306_clearDisplay(void);
void ssd1306_display(void);
void ssd1306_stopScroll(void);
void ssd1306_startScrollDiagLeft(uint8_t start, uint8_t stop);
void ssd1306_startScrollDiagRight(uint8_t start, uint8_t stop);
void ssd1306_startScrollLeft(uint8_t start, uint8_t stop);
void ssd1306_startScrollRight(uint8_t start, uint8_t stop);
void ssd1306_invertDisplay(_Bool i);
void ssd1306_drawPixel(uint8_t x, uint8_t y, _Bool white);
void ssd1306_drawByte(uint8_t x, uint8_t y, uint8_t byte);
void ssd1306_init(void);
void ssd1306_write_data(uint8_t * data, uint32_t len);
void ssd1306_displayOff(void);
void ssd1306_displayOn(void);
void ssd1306_spi_init(void);
void ssd1306_spi_uninit(void);

#endif
