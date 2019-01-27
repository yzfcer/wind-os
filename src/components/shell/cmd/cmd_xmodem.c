/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_xmodem.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/1/24 20:24:37
** 描        述: xmodem协议传输命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2015/1/24 20:24:37
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (WIND_CONSOLE_SUPPORT && CMD_XMODEM_SUPPORT)
#include "wind_heap.h"
#include "wind_std.h"
#include "wind_xmodem.h"
#include "treefs.h"
/********************************************内部变量定义**********************************************/




/********************************************内部函数定义*********************************************/
//extern w_int32_t xmodem_send(w_uint8_t *src, w_int32_t srcsz);
//extern w_int32_t xmodem_recv_bak(w_uint8_t *dest, w_int32_t destsz);
xm_ctx_s ctx;

w_int32_t xm_write(w_uint8_t ch)
{
    return wind_std_output(&ch,1);
}

static w_int32_t xm_read(w_uint8_t *ch,w_uint32_t time_out)
{
    w_int32_t cnt;
    w_uint32_t i;
    w_uint32_t tick = time_out / 10;
    for(i = 0;i < tick;i ++)
    {
        cnt = wind_std_input(ch,1);
        if(cnt >= 1)
            return 1;
        wind_thread_sleep(10);
    }
    return 0;
}



static w_err_t cmd_xmodem_get(int argc,char **argv)
{
    w_err_t err = W_ERR_OK;
    w_int32_t len;
    treefile_s *file = W_NULL;
    w_uint8_t *buff = W_NULL,*xbuff = W_NULL;
    if(argv[2][0] != '/')
    {
        wind_error("unknown file path.");
        return W_ERR_INVALID;
    }
    buff = wind_malloc(4096);
    if(buff == W_NULL)
    {
        err = W_ERR_MEM;
        goto XM_GET_END;
    }
    xbuff = wind_malloc(XMODEM_BUFF_LEN);
    if(xbuff == W_NULL)
    {
        err = W_ERR_MEM;
        goto XM_GET_END;
    }
    file = treefile_open(argv[2],TF_FMODE_CRT | TF_FMODE_W);
    if(file == W_NULL)
    {
        err = W_ERR_INVALID;
        goto XM_GET_END;
    }
    
    xmodem_init(&ctx,XM_DIR_RECV,xbuff,XMODEM_BUFF_LEN,xm_write,xm_read);
    for(;;)
    {
        wind_memset(buff,0,4096);
        len = xmodem_recv(&ctx,buff,4096);
        if(len < 0)
        {
            wind_error("xmodem receive failed.");
            err = W_ERR_FAIL;
            break;
        }
        if(len == 0)
        {
            break;
        }
        treefile_write(file,buff,len);
    }
    xmodem_end(&ctx);
XM_GET_END:
    treefile_close(file);
    wind_free(buff);
    wind_free(xbuff);
    return err;
}

static w_err_t cmd_xmodem_put(int argc,char **argv)
{
    w_err_t err = W_ERR_OK;
    w_int32_t len;
    treefile_s *file = W_NULL;
    w_uint8_t *buff = W_NULL,*xbuff = W_NULL;
    if(argv[2][0] != '/')
    {
        wind_error("unknown file path.");
        return W_ERR_INVALID;
    }
    
    buff = wind_malloc(4096);
    if(buff == W_NULL)
    {
        err = W_ERR_MEM;
        goto XM_PUT_END;
    }
    xbuff = wind_malloc(XMODEM_BUFF_LEN);
    if(xbuff == W_NULL)
    {
        err = W_ERR_MEM;
        goto XM_PUT_END;
    }
    file = treefile_open(argv[2],TF_FMODE_R);
    if(file == W_NULL)
    {
        err = W_ERR_INVALID;
        goto XM_PUT_END;
    }
    
    xmodem_init(&ctx,XM_DIR_SEND,xbuff,XMODEM_BUFF_LEN,xm_write,xm_read);
    for(;;)
    {
        wind_memset(buff,0,4096);
        len = treefile_read(file,buff,4096);
        if(len <= 0)
            break;
        len = xmodem_send(&ctx,buff,len);
        if(len <= 0)
        {
            wind_error("xmodem send failed.");
            err = W_ERR_FAIL;
            break;
        }
    }
    xmodem_end(&ctx);
XM_PUT_END:
    if(file != W_NULL)
        treefile_close(file);
    if(buff != W_NULL)
        wind_free(buff);
    if(xbuff != W_NULL)
        wind_free(xbuff);
    return err;

}

/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(xm)
{
    console_printf("to translate file data via xmodem protocol.\r\n");
}

COMMAND_USAGE(xm)
{
    console_printf("xm get <localfile>:to receive a file save as localfile.\r\n");
    console_printf("xm put <localfile>:to send localfile to remote host via xmodem.\r\n");
}

COMMAND_MAIN(xm,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"get") == 0)
        return cmd_xmodem_get(argc,argv);
    if(wind_strcmp(argv[1],"put") == 0)
        return cmd_xmodem_put(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(xm);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
