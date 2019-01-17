/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: cmd_xmodem.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2015/1/24 20:24:37
** ��        ��: xmodemЭ�鴫������
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2015/1/24 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_heap.h"
#include "wind_std.h"
#include "xmodem.h"
#if WIND_XMODEM_SUPPORT
#include "treefs.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/

#if (WIND_CONSOLE_SUPPORT && CMD_XMODEM_SUPPORT)

/********************************************�ڲ���������**********************************************/




/********************************************�ڲ���������*********************************************/
//extern w_int32_t xmodem_send(w_uint8_t *src, w_int32_t srcsz);
//extern w_int32_t xmodem_recv_bak(w_uint8_t *dest, w_int32_t destsz);
static xm_ctx_s ctx;

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
    treefile_s *file;
    w_uint8_t *buff,*xbuff;
    if(argv[2][0] != '/')
    {
        wind_error("unknown file path.");
        return W_ERR_INVALID;
    }
    buff = wind_malloc(4096);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_MEM);
    xbuff = wind_malloc(XMODEM_BUFF_LEN);
    WIND_ASSERT_TODO(xbuff != W_NULL,wind_free(buff),W_ERR_MEM);
    file = treefile_open(argv[2],TF_FMODE_CRT | TF_FMODE_W);
    xmodem_init_port(&ctx,xm_write,xm_read);
    xmodem_start(&ctx,XM_RECV,xbuff,XMODEM_BUFF_LEN);
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
    treefile_close(file);
    wind_free(buff);
    wind_free(xbuff);
    return err;
}

static w_err_t cmd_xmodem_put(int argc,char **argv)
{
    w_int32_t len;
    treefile_s *file;
    w_uint8_t *buff,*xbuff;
    if(argv[2][0] != '/')
    {
        wind_error("unknown file path.");
        return W_ERR_INVALID;
    }
    file = treefile_open(argv[2],TF_FMODE_R);
    if(file == W_NULL)
    {
        wind_error("file is NOT exist.");
        return W_ERR_NOFILE;
    }
    buff = wind_malloc(file->filelen);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_MEM);
    xbuff = wind_malloc(XMODEM_BUFF_LEN);
    WIND_ASSERT_TODO(xbuff != W_NULL,wind_free(buff),W_ERR_MEM);
    
    xmodem_init_port(&ctx,xm_write,xm_read);
    xmodem_start(&ctx,XM_SEND,xbuff,XMODEM_BUFF_LEN);
    wind_memset(buff,0,file->filelen);
    len = treefile_read(file,buff,file->filelen);
    len = xmodem_send(&ctx,buff,len);
    xmodem_end(&ctx);
    treefile_close(file);
    wind_free(buff);
    return W_ERR_OK;
}

/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
COMMAND_DISC(xmodem)
{
    console_printf("to translate file data via xmodem protocol.\r\n");
}

COMMAND_USAGE(xmodem)
{
    console_printf("xmodem get <localfile>:to receive a file save as localfile.\r\n");
    console_printf("xmodem put <localfile>:to send localfile to remote host via xmodem.\r\n");
}

COMMAND_MAIN(xmodem,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"get") == 0)
        return cmd_xmodem_get(argc,argv);
    if(wind_strcmp(argv[1],"put") == 0)
        return cmd_xmodem_put(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(xmodem);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
