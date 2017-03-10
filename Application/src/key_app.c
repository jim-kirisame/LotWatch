#include "main.c"
#include "key_app.h"
#include "nrf_gpiote.h"

void * key_pressHandler;

void key_init(void)
{

}

void key_setPressHandler(void * handler)
{
    key_pressHandler = handler;
}