#include "main.h"
#include "ssd1306_app.h"
#include "disp_app.h"
#include "Font.h"
#include "icons.h"
#include "Font16.h"
#include <string.h>

void ssd1306_draw5x7Font(uint8_t x, uint8_t y, char * string){
    if(y > 8 || x > 128)
        return;
    
    for(int i=0;i<128;i++)
    {
        if(!string[i]) //end of string
            return;
        for(int j=0;j<5&&x<128&&string[i]>=0x20;j++)
            buffer[x+++y*128] = font5x8[(string[i]-0x20)*5+j];
        buffer[x+++y*128] = 0x00; //space
    }
}

void ssd1306_draw48FontInner(uint8_t *data, uint8_t w, uint8_t x)
{
    uint8_t i,j;
    
    for(i=1;i<7;i++){
        for(j=0;j<w;j++){
            buffer[j+x+i*128] = *data++;
        }
    }
}

void ssd1306_draw48Font(char * string)
{
    uint8_t x=7;
    uint8_t width[] = {24,13,24,24,24,24,24,24,24,24,9};
    uint8_t totalWidth=0;
    
    memset(&buffer[128],0,128*6);
    
    for(uint8_t i =0;i<128;i++){
        if(string[i] >= 0x30 && string[i] <= 0x3a){
            totalWidth += width[ string[i] - 0x30 ];
        }
        else if(string[i]==0x00)
        {
            break;
        }
    }
    x = (128 - totalWidth) / 2;
    
    for(uint8_t i =0;i<128;i++){
        switch(string[i]){
            case 0x30:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x30[0],24,x);
                x+=24;
                break;
            case 0x31:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x31[0],13,x);
                x+=13;
                break;
            case 0x32:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x32[0],24,x);
                x+=24;
                break;
            case 0x33:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x33[0],24,x);
                x+=24;
                break;
            case 0x34:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x34[0],24,x);
                x+=24;
                break;
            case 0x35:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x35[0],24,x);
                x+=24;
                break;
            case 0x36:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x36[0],24,x);
                x+=24;
                break;
            case 0x37:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x37[0],24,x);
                x+=24;
                break;
            case 0x38:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x38[0],24,x);
                x+=24;
                break;
            case 0x39:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x39[0],24,x);
                x+=24;
                break;
            case 0x3a:
                ssd1306_draw48FontInner((uint8_t *)&image_data_Font_0x3a[0],9,x);
                x+=9;
                break;
            case 0x00:
                return;
            default:
                break;
            
        }
        
    }
}

uint8_t ssd1306_draw16FontInner(uint8_t *data, uint8_t len, uint8_t x, uint8_t y)
{
    uint8_t i,j;
    
    for(i=y; i<8 && i<=y+1 ;i++){
        for(j=0;j<len/2;j++){
            buffer[j+x+i*128] = *data++;
        }
    }
    return len / 2;
}

void ssd1306_draw16Font(char * string, uint8_t x, uint8_t y)
{
    for(uint8_t i =0;i<128;i++){
        switch(string[i]){
            case 0x25:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x25[0],sizeof(image_data_Font16_0x25),x,y);
                break;
            case 0x2d:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x2d[0],sizeof(image_data_Font16_0x2d),x,y);
                break;
            case 0x2e:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x2e[0],sizeof(image_data_Font16_0x2e),x,y);
                break;
            case 0x30:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x30[0],sizeof(image_data_Font16_0x30),x,y);
                break;
            case 0x31:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x31[0],sizeof(image_data_Font16_0x31),x,y);
                break;
            case 0x32:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x32[0],sizeof(image_data_Font16_0x32),x,y);
                break;
            case 0x33:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x33[0],sizeof(image_data_Font16_0x33),x,y);
                break;
            case 0x34:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x34[0],sizeof(image_data_Font16_0x34),x,y);
                break;
            case 0x35:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x35[0],sizeof(image_data_Font16_0x35),x,y);
                break;
            case 0x36:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x36[0],sizeof(image_data_Font16_0x36),x,y);
                break;
            case 0x37:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x37[0],sizeof(image_data_Font16_0x37),x,y);
                break;
            case 0x38:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x38[0],sizeof(image_data_Font16_0x38),x,y);
                break;
            case 0x39:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x39[0],sizeof(image_data_Font16_0x39),x,y);
                break;
            case 0x3a:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x3a[0],sizeof(image_data_Font16_0x3a),x,y);
                break;
            case 0x3f:
                x+=ssd1306_draw16FontInner((uint8_t *)&image_data_Font16_0x3f[0],sizeof(image_data_Font16_0x3f),x,y);
                break;
            case 0x00:
                return;
            default:
                break;
            
        }
        
    }
}

void ssd1306_draw48IconInner(uint8_t y, uint8_t * icon)
{
    if(y>2)
        return;
    for(int i=0; i<6; i++)
    {
        memcpy(&buffer[(i+y)*128+40], &icon[i*48], 48);
    }
    
}
void ssd1306_drawAlarmIcon48(uint8_t y)
{
    ssd1306_draw48IconInner(y, (uint8_t *)icons_alarm_icon_48x48);
}
void ssd1306_drawChargingIcon48(uint8_t y)
{
    ssd1306_draw48IconInner(y, (uint8_t *)icons_charging_icon_48x48);
}
void ssd1306_drawIcon16(enum icons_type type, uint8_t x, uint8_t y)
{
    switch(type)
    {
        case ICON_LOCK:
            ssd1306_draw16FontInner((uint8_t *)&icons_lock_icon[0],sizeof(icons_lock_icon),x,y);
            break;
        case ICON_ALARM:
            ssd1306_draw16FontInner((uint8_t *)&icons_alarm_icon[0],sizeof(icons_alarm_icon),x,y);
            break;
        case ICON_QQ:
            ssd1306_draw16FontInner((uint8_t *)&icons_qq_icon[0],sizeof(icons_qq_icon),x,y);
            break;
        case ICON_PHONE:
            ssd1306_draw16FontInner((uint8_t *)&icons_phone_icon[0],sizeof(icons_phone_icon),x,y);
            break;
        case ICON_WECHAT:
            ssd1306_draw16FontInner((uint8_t *)&icons_wechat_icon[0],sizeof(icons_wechat_icon),x,y);
            break;
        case ICON_RUN:
            ssd1306_draw16FontInner((uint8_t *)&icons_run_icon[0],sizeof(icons_run_icon),x,y);
            break;
        case ICON_WALK_SLOW:
            ssd1306_draw16FontInner((uint8_t *)&icons_walkslow_icon[0],sizeof(icons_walkslow_icon),x,y);
            break;
        case ICON_WALK_FAST:
            ssd1306_draw16FontInner((uint8_t *)&icons_walkfast_icon[0],sizeof(icons_walkfast_icon),x,y);
            break;
        case ICON_MESSAGE:
            ssd1306_draw16FontInner((uint8_t *)&icons_message_icon[0],sizeof(icons_message_icon),x,y);
            break;
        default:
            break;
    }
    
}
