/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_sysinfo.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2018/9/29 21:24:27
** 描        述: 打印系统信息命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018/9/29 21:24:27
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
