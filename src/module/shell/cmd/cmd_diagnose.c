/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_diagnose.c
** Author      : Jason Zhou
** Last Date   : 2019/2/24
** Description : 系统故障诊断命令
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019/2/24
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
