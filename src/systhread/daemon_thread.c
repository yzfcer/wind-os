#include "wind_type.h"
#include "wind_thread.h"
#include "wind_debug.h"

#if WIND_DAEMON_THREAD_SUPPORT 
#define DAEMON_STK_SIZE 256
static w_stack_t daemonstk[DAEMON_STK_SIZE];

static void test2(void)
{
    wind_thread_sleep(5);
}
static void test1(void)
{
    test2();
}
static int test(void)
{
    w_int32_t i,ret = 0;
    for(i = 0;i < 30000;i ++)
    {
        ret += ((2436647*376*i)/5467);
    }
    test1();
    return ret;
}


static w_err_t daemon_thread(w_int32_t argc,w_int8_t **argv)
{
    (void)argc;
    while(1)
    {
        //test();
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

