/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_pwd.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2018/5/13 20:24:37
** 描        述: 文件系统操作命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018/5/13 20:24:37
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
#include "wind_string.h"


#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (CMD_FS_SUPPORT)
#include "wind_file.h"
/********************************************内部变量定义**********************************************/


/********************************************内部函数定义*********************************************/

static w_err_t cmd_pwd(w_int32_t argc,char **argv)
{
    char *fullpath;
    char *curpath = wind_filepath_get_current();
	if(argc >= 2)
	   fullpath = wind_filepath_generate(curpath,argv[1],1);
	else
		fullpath = wind_filepath_generate(curpath,curpath,1);
    wind_printf("%s\r\n",curpath);
    return W_ERR_OK;
}

/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(pwd)
{
    wind_printf("to operate pwd file system.\r\n");
}

COMMAND_USAGE(pwd)
{
    wind_printf("pwd:--to show current user path.\r\n");
}

COMMAND_MAIN(pwd,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 1,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"pwd") == 0)
        return cmd_pwd(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(pwd);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
