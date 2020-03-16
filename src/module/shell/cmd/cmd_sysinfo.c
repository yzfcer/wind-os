/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: cmd_sysinfo.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2018/9/29 21:24:27
** ��        ��: ��ӡϵͳ��Ϣ����
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2018/9/29 21:24:27
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
#include "wind_sysinfo.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_SYSINFO_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/
static void print_byte_width(void)
{
	wind_printf("sizeof char:%d\r\n",sizeof(char));
	wind_printf("sizeof short:%d\r\n",sizeof(short));
	wind_printf("sizeof int:%d\r\n",sizeof(int));
	wind_printf("sizeof long:%d\r\n",sizeof(long));
	wind_printf("sizeof long long:%d\r\n",sizeof(long long));
}


/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(sysinfo)
{
    wind_printf("to display system infomation.\r\n");
}

COMMAND_USAGE(sysinfo)
{
    wind_printf("sysinfo:--to show system infomation of the device.\r\n");
    wind_printf("sysinfo -b:--to show system byte width.\r\n");
}

COMMAND_MAIN(sysinfo,argc,argv)
{
   if((argc >= 2) && (wind_strcmp(argv[1],"-b") == 0))
    {
        print_byte_width();
        return W_ERR_OK;
    }
   _wind_print_sysinfo();
   return W_ERR_OK;
}

COMMAND_DEF(sysinfo);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
