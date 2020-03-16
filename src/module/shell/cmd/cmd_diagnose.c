/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_diagnose.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2019/2/24
** 描        述: 系统故障诊断命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019/2/24
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
#include "wind_diagnose.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_DIAGNOSE_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(diagnose)
{
    wind_printf("to manage diagnose object.\r\n");
}

COMMAND_USAGE(diagnose)
{
    wind_printf("diagnose list:--to show all diagnose thread of list.\r\n");
    wind_printf("diagnose check :--to start checking diagnose list.\r\n");
}

COMMAND_MAIN(diagnose,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"list") == 0)
    {
        return wind_diagnose_print();
    }
    else if(wind_strcmp(argv[1],"check") == 0)
    {
        return wind_diagnose_check();
    }
    return W_ERR_FAIL;
}

COMMAND_DEF(diagnose);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
