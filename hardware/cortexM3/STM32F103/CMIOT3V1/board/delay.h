#ifndef _DELAY_H_
#define _DELAY_H_
#include "wind_thread.h"
#include "wind_core.h"

//#include "includes.h"




#define RTOS_TimeDly(ticks) 						wind_thread_sleep(ticks)
//#define RTOS_TimeDlyHMSM(hour, min, sec, ms)		OSTimeDlyHMSM(hour, min, sec, ms)

#define RTOS_EnterInt()								wind_enter_irq()
#define RTOS_ExitInt()								wind_exit_irq()

#define RTOS_ENTER_CRITICAL()						wind_disable_interrupt()
#define RTOS_EXIT_CRITICAL()						wind_enable_interrupt()




void Delay_Init(void);

void DelayUs(unsigned short us);

void DelayXms(unsigned short ms);

#endif
