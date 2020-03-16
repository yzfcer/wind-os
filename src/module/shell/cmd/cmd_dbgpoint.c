/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_dbgp.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2019/1/29 21:45:25
** 描        述: 调试点读写操作命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019/1/29 21:45:25
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
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_dbgpoint.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_DBGPOINT_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(dbgp)
{
    wind_printf("to set or get a dbgpoint status.\r\n");
}

COMMAND_USAGE(dbgp)
{
    wind_printf("dbgp list:--to set a dbgpoint status via str.\r\n");
    wind_printf("dbgp read <dbgpoint>:--to set a dbgpoint status via str.\r\n");
    wind_printf("dbgp write <dbgpoint> <str>:--to set a dbgpoint status via str.\r\n");
}

COMMAND_MAIN(dbgp,argc,argv)
{
    w_int32_t len;
    w_dbgpoint_s *dbgp;
    w_uint8_t *buff = W_NULL;
   WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"list") == 0)
    {
        wind_dbgpoint_print();
        return W_ERR_OK;
    }
    else if(wind_strcmp(argv[1],"read") == 0)
    {
        WIND_ASSERT_RETURN(argc == 3,W_ERR_INVALID);
        dbgp = wind_dbgpoint_get(argv[2]);
        WIND_ASSERT_RETURN(dbgp != W_NULL,W_ERR_INVALID);
        buff = (w_uint8_t *)wind_malloc(dbgp->lenth+1);
        WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_MEM);
        wind_memset(buff,0,dbgp->lenth+1);
        len = wind_dbgpoint_read(dbgp,buff,dbgp->lenth);
        if(len > 0)
            wind_printf("%s\r\n",buff);
        wind_free(buff);
        return W_ERR_OK;
    }
    else if(wind_strcmp(argv[1],"write") == 0)
    {
        WIND_ASSERT_RETURN(argc == 4,W_ERR_INVALID);
        dbgp = wind_dbgpoint_get(argv[2]);
        WIND_ASSERT_RETURN(dbgp != W_NULL,W_ERR_INVALID);
        len = wind_dbgpoint_read(dbgp,buff,dbgp->lenth);
        WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

COMMAND_DEF(dbgp);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
