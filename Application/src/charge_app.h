#ifndef _CHARGE_APP_
#define _CHARGE_APP_


#define CHARGER_CHECK_INTERVAL_MS (APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER))

void charge_init(void);

#endif
