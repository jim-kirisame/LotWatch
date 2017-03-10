#ifndef _VIBER_APP_H_
#define _VIBER_APP_H_


#define VIBER_PIN 19

void viber_init(void);
void viber_action(uint16_t length);
void viber_weak(void);
void viber_mid(void);
void viber_strong(void);

#endif
