/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_rm.c
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
/*********************************************header file***********************************************/

#if (CMD_FS_SUPPORT)
#include "wind_file.h"
/********************************************internal variables**********************************************/
/********************************************internal functions**********************************************/

static w_err_t cmd_rm(w_int32_t argc,char **argv)
{
    w_err_t err;
    w_int32_t i,len;
    char *curpath;
    char * fullpath;
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    curpath = wind_filepath_get_current();
    for(i = 1;i < argc;i ++)
    {
        err = W_ERR_OK;
        len = wind_strlen(argv[i]);
        fullpath = wind_filepath_generate(curpath,argv[i],argv[i][len-1] == '/'?1:0);
        err = wind_fremove(fullpath);
        wind_notice("remove file :%s %s",fullpath,err == W_ERR_OK?"successed":"failed");
        wind_filepath_release(fullpath);
    }
    return err;
}

/********************************************global variables**********************************************/

/********************************************global functions**********************************************/
COMMAND_DISC(rm)
{
    wind_printf("to operate rm file system.\r\n");
}

COMMAND_USAGE(rm)
{
    wind_printf("rm <file>:--to remove a directory or file.\r\n");
    wind_printf("file : the file or directory need to remove.\r\n");
}

COMMAND_MAIN(rm,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"rm") == 0)
        return cmd_rm(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(rm);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
