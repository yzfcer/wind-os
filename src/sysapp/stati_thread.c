#include "wind_types.h"
#include "wind_var.h"
#include "wind_thread.h"

static w_uint32_t core_get_ticks_of_idle(w_uint32_t ms)
{
    pthread_s pthread;
    pnode_s node;
    w_uint32_t cnts;
    pthread_s pproc = wind_get_cur_proc();
    wind_close_interrupt();
    node = g_core.pcblist.head;
    
    WIND_DEBUG("RUN_FLAG=%d\r\n",RUN_FLAG);
    while(node)
    {
        pthread = (pthread_s)node->obj;
        if((pthread != pproc) && (pthread->prio != 32767))
        {
            pthread->proc_status = PROC_STATUS_SUSPEND;
            pthread->cause = CAUSE_COM;
        }
        node = node->next;
    }
    wind_open_interrupt();
    
    cnts = g_core.idle_cnt;
    wind_thread_sleep(ms);
    cnts = g_core.idle_cnt - cnts;
    
    node = g_core.pcblist.head;
    while(node)
    {
        pthread = (pthread_s)node->obj;
        if((pthread != pproc) && (pthread->prio != 32767))
        {
            pthread->proc_status = PROC_STATUS_READY;
            pthread->cause = CAUSE_COM;
        }
        node = node->next;
    }
    return cnts;
}


#define STATI_STK_SIZE 1024
static w_stack_t statisstk[STATI_STK_SIZE];
static w_err_t stati_proc(w_int16_t argc,w_int8_t **argv)
{
    
    w_uint32_t statcnt = 0;
    w_uint32_t stati_ms = 1000;
    argc = 0;
    while(1)
    {
        statcnt = g_core.idle_cnt;
        wind_thread_sleep(stati_ms);
        statcnt = g_core.idle_cnt - statcnt;
        WIND_CPU_USAGE = (IDLE_CNT_PER_SEC - statcnt) * 100 / IDLE_CNT_PER_SEC;
        wind_printf("idle cnt:%d/%d\r\n",statcnt,IDLE_CNT_PER_SEC);
        if(statcnt >= IDLE_CNT_PER_SEC)
            wind_printf("\r\ncpu:%d.%d%%\r\n",0,0);
        else
            wind_printf("\r\ncpu:%d%%\r\n",WIND_CPU_USAGE);
    }
    return ERR_OK;
}

void create_stati_thread(void)
{
    g_core.pstat = wind_thread_create("statistics",PRIO_HIGH,stati_proc,
                     0,NULL,statisstk,STATI_STK_SIZE);
}

