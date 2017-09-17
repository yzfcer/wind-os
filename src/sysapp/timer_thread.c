#include "wind_type.h"
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_timer.h"

#define STATI_STK_SIZE 256
static w_stack_t statisstk[STATI_STK_SIZE];
static w_err_t timer_thread(w_int32_t argc,w_int8_t **argv)
{
    while(1)
    {
        wind_thread_sleep(10);
#if WIND_TTIMER_SUPPORT > 0
        wind_timer_handler();//首先处理定时中断的事情
#endif
    }
}

void create_timer_thread(void)
{
    g_core.ptimer = wind_thread_create("timer",PRIO_HIGH,timer_thread,
                     0,NULL,statisstk,STATI_STK_SIZE);
}

