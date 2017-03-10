#ifndef _CHARGE_APP_
#define _CHARGE_APP_

extern _Bool charge_charging;
extern _Bool charge_fulled;

#define CHRG_PIN 9
#define STDBY_PIN 10
#define CHARGER_CHECK_INTERVAL_MS (APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER))

void charge_init(void);

#endif
