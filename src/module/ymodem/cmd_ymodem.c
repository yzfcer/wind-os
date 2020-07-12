/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_ymodem.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : YMODEM operation command
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2015/1/24 20:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_heap.h"
#include "wind_std.h"
#include "wind_ymodem.h"
#include "wind_file.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if CMD_YMODEM_SUPPORT







//extern w_int32_t wind_ymodem_send(w_uint8_t *src, w_int32_t srcsz);
//extern w_int32_t ymodem_recv_bak(w_uint8_t *dest, w_int32_t destsz);
ym_ctx_s ctx;

w_int32_t ym_write(w_uint8_t ch)
{
    return wind_std_output(&ch,1);
}

static w_int32_t ym_read(w_uint8_t *ch,w_uint32_t time_out)
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



static w_err_t cmd_ymodem_get(int argc,char **argv)
{
    w_err_t err = W_ERR_OK;
    w_int32_t len;
    w_file_s *file = (w_file_s *)W_NULL;
    w_uint8_t *buff = (w_uint8_t *)W_NULL;
    w_uint8_t *xbuff = (w_uint8_t *)W_NULL;
    if(argv[2][0] != '/')
    {
        wind_error("unknown file path.");
        return W_ERR_INVALID;
    }
    buff = wind_malloc(4096);
    if(buff == W_NULL)
    {
        err = W_ERR_MEM;
        goto YM_GET_END;
    }
    xbuff = wind_malloc(YMODEM_BUFF_LEN);
    if(xbuff == W_NULL)
    {
        err = W_ERR_MEM;
        goto YM_GET_END;
    }
    file = wind_fopen(argv[2],FMODE_CRT | FMODE_W);
    if(file == W_NULL)
    {
        err = W_ERR_INVALID;
        goto YM_GET_END;
    }
    
    wind_ymodem_init(&ctx,YM_DIR_RECV,xbuff,YMODEM_BUFF_LEN,ym_write,ym_read);
    for(;;)
    {
        wind_memset(buff,0,4096);
        len = wind_ymodem_recv(&ctx,buff,4096);
        if(len < 0)
        {
            wind_error("ymodem receive failed.");
            err = W_ERR_FAIL;
            break;
        }
        if(len == 0)
        {
            break;
        }
        wind_fwrite(file,buff,len);
    }
    wind_ymodem_end(&ctx);
YM_GET_END:
    wind_fclose(file);
    wind_free(buff);
    wind_free(xbuff);
    return err;
}

static w_err_t cmd_ymodem_put(int argc,char **argv)
{
    w_err_t err = W_ERR_OK;
    w_int32_t len;
    w_file_s *file = (w_file_s *)W_NULL;
    w_uint8_t *buff = (w_uint8_t *)W_NULL;
    w_uint8_t *xbuff = (w_uint8_t *)W_NULL;
    if(argv[2][0] != '/')
    {
        wind_error("unknown file path.");
        return W_ERR_INVALID;
    }
    
    buff = wind_malloc(4096);
    if(buff == W_NULL)
    {
        err = W_ERR_MEM;
        goto YM_PUT_END;
    }
    xbuff = wind_malloc(YMODEM_BUFF_LEN);
    if(xbuff == W_NULL)
    {
        err = W_ERR_MEM;
        goto YM_PUT_END;
    }
    file = wind_fopen(argv[2],FMODE_R);
    if(file == W_NULL)
    {
        err = W_ERR_INVALID;
        goto YM_PUT_END;
    }
    
    wind_ymodem_init(&ctx,YM_DIR_SEND,xbuff,YMODEM_BUFF_LEN,ym_write,ym_read);
    for(;;)
    {
        wind_memset(buff,0,4096);
        len = wind_fread(file,buff,4096);
        if(len <= 0)
            break;
        len = wind_ymodem_send(&ctx,buff,len);
        if(len <= 0)
        {
            wind_error("ymodem send failed.");
            err = W_ERR_FAIL;
            break;
        }
    }
    wind_ymodem_end(&ctx);
YM_PUT_END:
    if(file != W_NULL)
        wind_fclose(file);
    if(buff != W_NULL)
        wind_free(buff);
    if(xbuff != W_NULL)
        wind_free(xbuff);
    return err;

}






COMMAND_DISC(ymodem)
{
    wind_printf("to translate file data via ymodem protocol.\r\n");
}

COMMAND_USAGE(ymodem)
{
    wind_printf("ymodem get <localfile>:--to receive a file save as localfile.\r\n");
    wind_printf("ymodem put <localfile>:--to send localfile to remote host via ymodem.\r\n");
}

COMMAND_MAIN(ymodem,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"get") == 0)
        return cmd_ymodem_get(argc,argv);
    if(wind_strcmp(argv[1],"put") == 0)
        return cmd_ymodem_put(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(ymodem);

#endif // #if CMD_YMODEM_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
