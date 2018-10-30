/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2017/04/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "boot_port.h"
#include "wind_debug.h"
#include "boot_hw_if.h"
#include "boot_param.h"
#include <stdarg.h>
#include "wind_config.h"

#ifdef __cplusplus
extern "C" {
#endif

//通用缓存器，一般在拷贝数据时做缓存，或者接收命令字符

void boot_delay(w_uint32_t ms)
{
    w_uint32_t tick = boot_get_sys_ms();
    while(boot_get_sys_ms() - tick < ms);
}

w_err_t wait_for_key_input(w_int32_t to_sec,char *ch,w_int32_t print_flag)
{
    w_int32_t second;
    w_uint32_t tick;
    w_err_t err = W_ERR_FAIL;
    if(to_sec <= 0)
        return W_ERR_OK;
    second = to_sec > 99?99:to_sec;
    tick = boot_get_sys_ms();
    if(print_flag)
        wind_printf("\r\nwaiting:%ds",second);
    while(1)
    {
        feed_watchdog();
        if(wind_std_input(ch,1) > 0)
        {
            err = W_ERR_OK;
            break;
        }
        if(boot_get_sys_ms() - tick >= 1000)
        {
            tick = boot_get_sys_ms();
            second --;
            if(second <= 0)
            {
                break;
            }
            if(print_flag)
            {
                wind_printf("%c%c%c",8,8,8);
                wind_printf("%2ds",second);
            }
        }
    }
    wind_printf("\r\n");
    return err;
}





w_err_t read_char_blocking(char *ch)
{
    if(0 == wait_for_key_input(60,ch,0))
        return W_ERR_OK;
    wind_printf("you have NOT input any key in a 60 seconds,boot exit the menu list.\r\n");
    if(0 == wait_for_key_input(30,ch,1))
        return W_ERR_OK;
    return W_ERR_FAIL;
}

w_int32_t read_line_blockig(char *buff,w_int32_t len)
{
    w_int32_t idx = 0;
    char ch;
    while(1)
    {
        if(0 < wind_std_input(&ch,1))
        {
            if((ch == '\r') || 
                (ch == '\n') ||
                ((idx >= len - 1)))
            {
                buff[idx++] = 0;
                return idx;
            }
            else 
            {
                buff[idx++] = ch;
            }
        }
    }   
}

#if 0
typedef enum
{
    RECV_START,
    RECV_HANDLE,
    RECV_END
}recv_e;

typedef struct
{
    w_uint32_t stat;
    w_uint32_t idx;
    w_uint32_t mstick;
}recv_stat_s;

recv_stat_s g_recvstat;
void init_recv_stat(recv_stat_s *stat)
{
    stat->idx = 0;
    stat->mstick = boot_get_sys_ms();
    stat->stat = RECV_START;
}
static void wait_file_send_compete(void)
{
    w_err_t err;
    char ch;
    g_recvstat.stat = boot_get_sys_ms();
    while(1)
    {
        //err = wait_for_key_input(3,&ch,0);
        feed_watchdog();
        boot_delay(100);
        if(0 < wind_std_input(&ch))
        {
            g_recvstat.stat = boot_get_sys_ms();
        }
        if(boot_get_sys_ms() - g_recvstat.stat >= 2000)
        return;
    }
}

w_int32_t boot_receive_img(w_uint32_t addr,w_uint32_t maxlen)
{
    w_err_t err;
    w_int32_t i,end;
    char *buf = (char*)addr;
    init_recv_stat(&g_recvstat);
    while(1)
    {
        switch(g_recvstat.stat)
        {
            case RECV_START:
                err = wait_for_key_input(30,&buf[0],1);
                if(0 == err)
                {
                    
                    g_recvstat.stat = RECV_HANDLE;
                    g_recvstat.idx = 1;
                    g_recvstat.mstick = boot_get_sys_ms();
                }
                else
                {
                    wind_notice("wait for file data time out.");
                    return -1;
                }
                break;
            case RECV_HANDLE:
                err = wind_std_input(&buf[g_recvstat.idx]);
                if(0 < err)
                {
                    g_recvstat.idx ++;
                    if(g_recvstat.idx >= maxlen)
                    {
                        wind_warn("img file is too long.");
                                              wait_file_send_compete();
                                              return -1;
                    }
                    g_recvstat.mstick = boot_get_sys_ms();
                    if((g_recvstat.idx & 0x1ff) == 0)
                    {
                        wind_printf("#");
                    }
                    if((g_recvstat.idx & 0x7fff) == 0)
                    {
                        wind_printf("\r\n");
                    }
                }
                else
                {
                    if(boot_get_sys_ms() - g_recvstat.mstick > 1000)
                    {
                        g_recvstat.stat = RECV_END;
                    }
                }
                feed_watchdog();
                break;
            case RECV_END:
                wind_printf("\r\n");
                end = (g_recvstat.idx + COMMBUF_SIZE - 1)/COMMBUF_SIZE*COMMBUF_SIZE;
                for(i = g_recvstat.idx;i < end; i ++)
                    buf[i] = 0;
                return g_recvstat.idx;
        }
    }
}
#endif



#ifdef __cplusplus
}
#endif
