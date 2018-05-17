/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: cmd_xmodem.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2015/1/24 20:24:37
** ��        ��: 
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2015/1/24 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: �����䴦����庯��
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_heap.h"
#include "treefs.h"
#if WIND_XMODEM_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/

#if WIND_CONSOLE_SUPPORT

/********************************************�ڲ���������**********************************************/




/********************************************�ڲ���������*********************************************/
extern w_int32_t xmodem_send(w_uint8_t *src, w_int32_t srcsz);
extern w_int32_t xmodem_recv(w_uint8_t *dest, w_int32_t destsz);

static w_err_t cmd_xmode_get(int argc,char **argv)
{
    w_int32_t len;
    treefile_s *file;
    w_uint8_t *buff = wind_malloc(4096);
    wind_memset(buff,0,4096);
    len = xmodem_recv(buff,4096);
    if(len <= 0)
    {
        wind_error("xmodem receive failed.");
        return ERR_FAIL;
    }
    file = treefile_open(argv[2],FMODE_CRT|FMODE_W);
    treefile_write(file,buff,len);
    treefile_close(file);
    wind_free(buff);
    return ERR_OK;
}

static w_err_t cmd_xmode_put(int argc,char **argv)
{
    w_int32_t len;
    treefile_s *file;
    w_uint8_t *buff = wind_malloc(4096);
    wind_memset(buff,0,4096);
    
    file = treefile_open(argv[2],FMODE_R);
    if(file == NULL)
    {
        wind_error("file is NOT exist.");
        wind_free(buff);
        return ERR_FILE_NOT_EXIT;
    }
    len = treefile_read(file,buff,4096);
    len = xmodem_send(buff,len);
    treefile_close(file);
    wind_free(buff);
    return ERR_OK;
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
    if(argc < 3)
        return ERR_FAIL;
    if(wind_strcmp(argv[1],"get") == 0)
        return cmd_xmode_get(argc,argv);
    if(wind_strcmp(argv[1],"put") == 0)
        return cmd_xmode_put(argc,argv);
    return ERR_OK;
}

COMMAND_DEF(xmodem);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
