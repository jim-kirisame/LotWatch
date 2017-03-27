#ifndef _DISP_APP_
#define _DISP_APP_

#include <stdint.h>
enum icons_type{
    ICON_LOCK,
    ICON_ALARM,
    ICON_QQ,
    ICON_PHONE,
    ICON_MESSAGE,
    ICON_WECHAT,
    ICON_WALK_SLOW,
    ICON_WALK_FAST,
    ICON_RUN,
    
}; 
void ssd1306_draw5x7Font(uint8_t x, uint8_t y, char * string);
void ssd1306_draw48Font(char * string);
void ssd1306_draw16Font(char * string, uint8_t x, uint8_t y);
void ssd1306_drawAlarmIcon48(uint8_t y);
void ssd1306_drawChargingIcon48(uint8_t y);
void ssd1306_drawIcon16(enum icons_type type, uint8_t x, uint8_t y);

#endif
