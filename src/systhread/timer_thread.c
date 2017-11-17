#include "wind_config.h"
#include "wind_type.h"
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_timer.h"

#if WIND_TIMER_SUPPORT
#define TIMER_STK_SIZE 256

//void wind_timer_event(void);
static w_stack_t timerstk[TIMER_STK_SIZE];
static w_err_t timer_thread(w_int32_t argc,w_int8_t **argv)
{
    while(1)
    {
        wind_thread_sleep(10);
        wind_timer_event();
    }
}

void create_timer_thread(void)
{
    g_core.ptimer = wind_thread_create("timer",PRIO_HIGH,timer_thread,
                     0,NULL,timerstk,TIMER_STK_SIZE);
}
#endif

