#include "wind_types.h"
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_debug.h"
#define DAEMON_STK_SIZE 512
static w_stack_t daemonstk[DAEMON_STK_SIZE];


static int test(void)
{
    w_int32_t i,ret = 0;
    for(i = 0;i < 30000;i ++)
    {
        ret += ((2436647*376*i)/5467);
    }
    return ret;
}


static w_err_t daemon_proc(w_int32_t argc,w_int8_t **argv)
{
    (void)argc;
    while(1)
    {
        wind_thread_sleep(10);
        test();
    }
}

void create_daemon_thread(void)
{

    g_core.pdaemon = wind_thread_create("daemon",PRIO_HIGH,daemon_proc,
                     0,NULL,daemonstk,DAEMON_STK_SIZE);
}

