/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_board_port.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的CPU体系相关的代码
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_config.h"
#include "wind_type.h"
#include "wind_std.h"
#include "wind_debug.h"
#include "misc.h"
#include "usart1.h"
#if WIND_DEBUG_SUPPORT

#define UART_RCV_BUFLEN 256
typedef struct
{
    w_int32_t size;

    w_int32_t wridx;
    w_int32_t rdidx;
    char buf[UART_RCV_BUFLEN];
}usrt_buf_s;
usrt_buf_s uartbuf;
static void uartbuf_init(void)
{
    w_int32_t i;
    uartbuf.size = UART_RCV_BUFLEN;
    uartbuf.wridx = 0;
    uartbuf.rdidx = 0;
    for(i = 0;i < uartbuf.size;i ++)
        uartbuf.buf[i] = 0;
}


void wind_stdin_irq(char data)
{
    uartbuf.buf[uartbuf.wridx] = data;
    uartbuf.wridx ++;
    if(uartbuf.wridx >= uartbuf.size)
        uartbuf.wridx = 0;
}

void _wind_std_init(void)
{
    uartbuf_init();
    uart1_init(115200);
}

w_int32_t wind_std_input(w_uint8_t *buf,w_int32_t len)
{
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        if(uartbuf.rdidx != uartbuf.wridx)
        {
            buf[i] = uartbuf.buf[uartbuf.rdidx];
            uartbuf.rdidx ++;
            if(uartbuf.rdidx >= uartbuf.size)
                uartbuf.rdidx = 0;
        }
        else
            break;
    }
    return i;
}

w_int32_t wind_std_output(w_uint8_t *str,w_int32_t len)
{
    uart1_send_chars(str,len);
    return len;
}

#endif



