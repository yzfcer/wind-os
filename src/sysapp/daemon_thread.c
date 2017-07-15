#include "wind_types.h"
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_debug.h"
#define DAEMON_STK_SIZE 512
static w_stack_t daemonstk[DAEMON_STK_SIZE];


static int test(void)
{
    w_uint32_t i,ret = 0;
    for(i = 0;i < 30000;i ++)
    {
        ret += ((244536647*3476*i)/5467);
    }
    return ret;
}

static void print_thread(void)
{
    pnode_s node = g_core.pcblist.head;
    pthread_s thr;
    while(node)
    {
        thr = (pthread_s)node->obj;
        wind_printf("%s:%d\r\n",thr->name,thr->proc_status);
        node = node->next;
    }
}
static w_err_t daemon_proc(w_int32_t argc,w_int8_t **argv)
{
    w_int32_t cnt;
    w_err_t err;
    w_uint32_t dmcnt = 0;
    argc = 0;
    while(1)
    {
        wind_thread_sleep(10);
        //print_thread();
        test();
    }
    return ERR_OK;
}

void create_daemon_thread(void)
{

    g_core.pdaemon = wind_thread_create("daemon",PRIO_HIGH,daemon_proc,
                     0,NULL,daemonstk,DAEMON_STK_SIZE);
}

