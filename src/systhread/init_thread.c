#include "wind_type.h"
#include "wind_var.h"
#include "wind_thread.h"
#include "wind_softirq.h"
#include "wind_heap.h"
#include "wind_dev.h"
#include "wind_blkdev.h"

#define INIT_STK_SIZE 256

void wind_tick_init(void);
#if WIND_STATI_THREAD_SUPPORT
void create_stati_thread(void);
#else
#define create_stati_thread()
#endif
#if WIND_DAEMON_THREAD_SUPPORT
void create_daemon_thread(void);
#else 
#define create_daemon_thread()
#endif
void create_idle_thread(void);
#if WIND_CONSOLE_SUPPORT
w_err_t create_console_thread(void);
#else 
#define create_console_thread() ERR_OK
#endif

void create_timer_thread(void);

extern w_err_t wind_main(void);
static w_stack_t mainstk[INIT_STK_SIZE];

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
    _wind_dev_init();
    _wind_blkdev_init();
#if WIND_HEAP_SUPPORT
    wind_heaps_init();
#endif
    wind_notice("create sys thread:");
    create_idle_thread();
    set_idle_cnt();
#if WIND_SOFTIRQ_SUPPORT
    wind_create_softirq_thread();
#endif
#if WIND_TIMER_SUPPORT
    create_timer_thread();
#endif
    create_stati_thread();
    create_daemon_thread();
    create_console_thread();
    wind_main();
    return ERR_OK;
}

w_err_t create_init_thread(void)
{
    thread_s *thread;
    thread = wind_thread_create("init",PRIO_HIGH,init_thread,
                        0,NULL,mainstk,INIT_STK_SIZE);
    WIND_ASSERT_RETURN(thread != NULL,ERR_COMMAN);
    return ERR_OK;
}
