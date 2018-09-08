#include "wind_type.h"
#include "wind_thread.h"
#include "wind_debug.h"
//#include "wind_watchdog.h"

#if WIND_DAEMON_THREAD_SUPPORT 
#define DAEMON_STK_SIZE 256
static w_stack_t daemonstk[DAEMON_STK_SIZE];


static w_err_t daemon_thread(w_int32_t argc,w_int8_t **argv)
{
    (void)argc;
    //wind_watchdog_create("daemon",0,10);
	
    while(1)
    {
        wind_thread_sleep(1000);
    }
}

w_err_t _create_daemon_thread(void)
{
    thread_s *thread;
    thread = wind_thread_create("daemon",daemon_thread,
                     0,NULL,PRIO_HIGH,daemonstk,DAEMON_STK_SIZE);
    WIND_ASSERT_RETURN(thread != NULL,ERR_FAIL);
    return ERR_OK;
}

#endif

