#include "wind_types.h"
#include "wind_var.h"
#include "wind_thread.h"

#define DAEMON_STK_SIZE 512
static w_stack_t daemonstk[DAEMON_STK_SIZE];//主任务堆栈
static w_err_t daemon_proc(w_int32_t argc,w_int8_t **argv)
{
    w_err_t err;

    w_uint32_t dmcnt = 0;
    argc = 0;
    while(1)
    {
        wind_thread_sleep(50);
        dmcnt ++;
    }
    return ERR_OK;
}

void create_daemon_thread(void)
{

    g_core.pdaemon = wind_thread_create("daemon",PRIO_HIGH,daemon_proc,
                     0,NULL,daemonstk,DAEMON_STK_SIZE);
}

