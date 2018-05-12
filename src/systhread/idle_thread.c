#include "wind_config.h"
#include "wind_type.h"
#include "wind_var.h"
#include "wind_debug.h"
#include "wind_thread.h"

#define IDLE_STK_SIZE 64
static w_stack_t idlestk[IDLE_STK_SIZE];//ø’œ–»ŒŒÒ∂—’ª
static w_err_t idle_thread(w_int32_t argc,w_int8_t **argv)
{    
    while(1)
    {
        g_core.idle_cnt ++;
    }
}


w_err_t _create_idle_thread(void)
{
    thread_s *thread;
    thread = wind_thread_create("idle",idle_thread,
                    0,NULL,PRIO_LOW,idlestk,IDLE_STK_SIZE);
    WIND_ASSERT_RETURN(thread != NULL,ERR_FAIL);
    wind_thread_set_priority(thread,32767);
    return ERR_OK;
}

