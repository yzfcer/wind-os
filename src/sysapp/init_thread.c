#include "wind_types.h"
#include "wind_var.h"
#include "wind_thread.h"

#define MAIN_STK_SIZE 512
void wind_tick_init(void);
void create_stati_thread(void);
void create_daemon_thread(void);
void create_idle_thread(void);
void create_console_thread(void);

extern w_err_t wind_main(void);
static w_stack_t mainstk[MAIN_STK_SIZE];

static w_err_t init_thread(w_int16_t argc,w_int8_t **argv)
{   
    wind_tick_init();
    WIND_INFO("create sys thread:\r\n");
#if WIND_SOFTINT_SUPPORT > 0    
    wind_create_softint_proc();
#endif
    create_stati_thread();
    create_daemon_thread();
    create_idle_thread();
#if WIND_CONSOLE_SUPPORT > 0
    create_console_thread();
#endif
    wind_thread_showlist(g_core.pcblist.head);
    
    wind_main();
    //while(1)
    //    wind_thread_sleep(1000);
    return ERR_OK;
}

int create_init_thread(void)
{
    g_core.pmain = wind_thread_create("entry",PRIO_HIGH,init_thread,
                        0,NULL,mainstk,MAIN_STK_SIZE);

}
