#include "wind_type.h"
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_softint.h"
#define MAIN_STK_SIZE 256
void wind_tick_init(void);
void create_stati_thread(void);
#if WIND_DAEMON_SUPPORT
void create_daemon_thread(void);
#else 
#define create_daemon_thread()
#endif
void create_idle_thread(void);
#if WIND_CONSOLE_SUPPORT
void create_console_thread(void);
#else 
#define create_console_thread()
#endif

void create_timer_thread(void);

extern w_err_t wind_main(void);
static w_stack_t mainstk[MAIN_STK_SIZE];

static void set_idle_cnt(void)
{
    wind_thread_sleep(500);
    IDLE_CNT_PER_SEC = g_core.idle_cnt;
    wind_thread_sleep(1000);
    IDLE_CNT_PER_SEC = g_core.idle_cnt - IDLE_CNT_PER_SEC;
    wind_printf("idle:%d\r\n",g_core.idle_cnt);
    wind_printf("idle count:%d\r\n",IDLE_CNT_PER_SEC);
}

static w_err_t init_thread(w_int32_t argc,w_int8_t **argv)
{   
    wind_tick_init();
    WIND_INFO("create sys thread:\r\n");
    create_idle_thread();
    set_idle_cnt();
#if WIND_SOFTINT_SUPPORT
    wind_create_softint_thread();
#endif
#if WIND_TIMER_SUPPORT
    create_timer_thread();
#endif
    create_stati_thread();
    create_daemon_thread();
    create_console_thread();

    //wind_thread_print(&g_core.threadlist);
    
    wind_main();
    return ERR_OK;
}

int create_init_thread(void)
{
    g_core.pmain = wind_thread_create("init",PRIO_HIGH,init_thread,
                        0,NULL,mainstk,MAIN_STK_SIZE);
    return ERR_OK;
}
