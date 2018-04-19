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

w_err_t create_timer_thread(void)
{
    thread_s *thread;
    thread = wind_thread_create("timer",PRIO_HIGH,timer_thread,
                     0,NULL,timerstk,TIMER_STK_SIZE);
    WIND_ASSERT_RETURN(thread != NULL,ERR_COMMAN);
    return ERR_OK;
}
#endif

