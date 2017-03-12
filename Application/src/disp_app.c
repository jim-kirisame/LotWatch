#include "main.h"
#include "ssd1306_app.h"
#include "disp_app.h"
#include "Font.h"
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
