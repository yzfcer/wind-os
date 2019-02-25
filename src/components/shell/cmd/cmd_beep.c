/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: cmd_bee.c
**��   ��   ��: Jason Zhou
**����޸�����: 2013.10.19
**��        ��: ��������������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.10.19
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2013.10.19
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_chdev.h"
#if (CMD_BEEP_SUPPORT)


COMMAND_DISC(beep)
{
    wind_printf("control the beep device.\r\n");
}

COMMAND_USAGE(beep)
{
    wind_printf("beep on:--to open the beep device.\r\n");
    wind_printf("beep off:--to close the beep device.\r\n");
}

COMMAND_MAIN(beep,argc,argv)
{
    w_chdev_s *dev;
    w_err_t err;
    w_uint8_t stat;
    if(argc < 2)
        return W_ERR_FAIL;
    dev = wind_chdev_get("beep");
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_INVALID);
    if(0 == wind_strcmp(argv[1],"on"))
    {
        err = wind_chdev_open(dev);
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
        stat = 1;
        wind_chdev_write(dev,&stat,1);
        return W_ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"off"))
    {
        err = wind_chdev_open(dev);
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
        stat = 0;
        wind_chdev_write(dev,&stat,1);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

COMMAND_DEF(beep);

#endif

