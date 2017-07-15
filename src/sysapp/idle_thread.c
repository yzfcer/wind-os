#include "wind_types.h"
#include "wind_var.h"
#include "wind_thread.h"

#define IDLE_STK_SIZE 256
static w_stack_t idlestk[IDLE_STK_SIZE];//ø’œ–»ŒŒÒ∂—’ª
static w_err_t idle_thread(w_int16_t argc,w_int8_t **argv)
{    
    while(1)
    {
        g_core.idle_cnt ++;
    }
    return ERR_OK;
}


void create_idle_thread(void)
{
    g_core.pidle = wind_thread_create("idle",PRIO_LOW,idle_thread,
                    0,NULL,idlestk,IDLE_STK_SIZE);
    wind_thread_changeprio(g_core.pidle,32767);
}
