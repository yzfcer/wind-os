/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_xmodem.c
** 创   建   人: 周江村
** 最后修改日期: 2015/1/24 20:24:37
** 描        述: xmodem协议传输命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
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
#include "wind_heap.h"
#include "treefs.h"
#include "xmodem.h"
#if WIND_XMODEM_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (WIND_CONSOLE_SUPPORT && WIND_FS_SUPPORT)

/********************************************内部变量定义**********************************************/




/********************************************内部函数定义*********************************************/
extern w_int32_t xmodem_send(w_uint8_t *src, w_int32_t srcsz);
extern w_int32_t xmodem_recv_bak(w_uint8_t *dest, w_int32_t destsz);

static w_err_t cmd_xmodem_get(int argc,char **argv)
{
    w_err_t err = ERR_OK;
    w_int32_t len;
    treefile_s *file;
    w_uint8_t *buff;
    if(argv[2][0] != '/')
    {
        wind_error("unknown file path.");
        return ERR_INVALID_PARAM;
    }
    xmodem_recv_start();
    buff = wind_malloc(4096);
    file = treefile_open(argv[2],TF_FMODE_CRT | TF_FMODE_W);
    for(;;)
    {
        wind_memset(buff,0,4096);
        len = xmodem_recv_data(buff,4096);
        if(len < 0)
        {
            wind_error("xmodem receive failed.");
            err = ERR_FAIL;
            break;
        }
        if(len == 0)
        {
            break;
        }
        treefile_write(file,buff,len);
    }
    xmodem_recv_end();
    treefile_close(file);
    wind_free(buff);
    return err;
}

static w_err_t cmd_xmodem_put(int argc,char **argv)
{
    w_int32_t len;
    treefile_s *file;
    w_uint8_t *buff;
    if(argv[2][0] != '/')
    {
        wind_error("unknown file path.");
        return ERR_INVALID_PARAM;
    }
    file = treefile_open(argv[2],TF_FMODE_R);
    if(file == NULL)
    {
        wind_error("file is NOT exist.");
        return ERR_FILE_NOT_EXIT;
    }
    buff = wind_malloc(file->filelen);
    wind_memset(buff,0,file->filelen);
    len = treefile_read(file,buff,file->filelen);
    len = xmodem_send(buff,len);
    treefile_close(file);
    wind_free(buff);
    return ERR_OK;
}

/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
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
    if(argc < 3)
        return ERR_FAIL;
    if(wind_strcmp(argv[1],"get") == 0)
        return cmd_xmodem_get(argc,argv);
    if(wind_strcmp(argv[1],"put") == 0)
        return cmd_xmodem_put(argc,argv);
    return ERR_OK;
}

COMMAND_DEF(xmodem);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
