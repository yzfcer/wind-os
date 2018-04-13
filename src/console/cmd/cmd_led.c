/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_led.c
**��   ��   ��: �ܽ���
**����޸�����: 2013.10.19
**��        ��: ϵͳ�Ŀ���̨����stat����������ͳ����Դʹ�����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2013.10.19
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2013.10.19
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_cmd.h"
#include "wind_dev.h"
#if WIND_CONSOLE_SUPPORT


COMMAND_DISC(led)
{
    console_printf("conreol a led device.\r\n");
}

COMMAND_USAGE(led)
{
    console_printf("led on <ledname>:to open a led device.\r\n");
    console_printf("led off <ledname>:to close a led device.\r\n");
}

COMMAND_MAIN(led,argc,argv)
{
    w_err_t err;
    dev_s *led;
    w_uint8_t stat;
    WIND_ASSERT_RETURN(argc == 3,ERR_INVALID_PARAM);
    led = wind_dev_get(argv[2]);
    WIND_ASSERT_RETURN(led != NULL,ERR_INVALID_PARAM);
    if(0 == wind_strcmp(argv[1],"on"))
    {
        err = wind_dev_open(led);
        WIND_ASSERT_RETURN(err == ERR_OK,ERR_COMMAN);
        stat = 1;
        wind_dev_write(led,&stat,1);
        return ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"off"))
    {
        err = wind_dev_open(led);
        WIND_ASSERT_RETURN(err == ERR_OK,ERR_COMMAN);
        stat = 0;
        wind_dev_write(led,&stat,1);
        return ERR_OK;
    }
    return ERR_COMMAN;
}

COMMAND_DEF(led);

#endif
