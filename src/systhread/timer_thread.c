#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"
#include "wind_timer.h"
#include "wind_debug.h"

#if WIND_TIMER_SUPPORT
#define TIMER_STK_SIZE 256

static w_stack_t timerstk[TIMER_STK_SIZE];
static w_err_t timer_thread(w_int32_t argc,w_int8_t **argv)
{
    while(1)
    {
        wind_thread_sleep(10);
        _wind_timer_event();
    }
}

w_err_t _create_timer_thread(void)
{
    thread_s *thread;
    thread = wind_thread_create("timer",PRIO_HIGH,timer_thread,
                     0,NULL,timerstk,TIMER_STK_SIZE);
    WIND_ASSERT_RETURN(thread != NULL,ERR_COMMAN);
    wind_thread_set_priority(thread,3);
    return ERR_OK;
}
#endif

