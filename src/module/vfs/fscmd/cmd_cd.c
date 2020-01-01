/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_cd.c
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

static w_err_t cmd_cd(w_int32_t argc,char **argv)
{
    w_err_t err;
    char *fullpath = W_NULL;
    char *curpath;
    w_file_s *file;

    do
    {
        err = W_ERR_OK;
        curpath = wind_filepath_get_current();
        if(argc >= 2)
           fullpath = wind_filepath_generate(curpath,argv[1],1);
        else
            fullpath = wind_filepath_generate(curpath,curpath,1);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_MEM,"generate fullpath failed");
        file = wind_fopen(fullpath,FMODE_R);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_NOFILE,"directory is NOT exist.");
        WIND_ASSERT_BREAK(file->isdir == 1,W_ERR_FAIL,"%s is not a directory",fullpath);
        wind_filepath_set_current(fullpath);
        
    }while(0);
    if(fullpath != W_NULL)
        wind_filepath_release(fullpath);
    if(file != W_NULL)
        wind_fclose(file);
    return err;
}


/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(cd)
{
    wind_printf("to set current user directory.\r\n");
}

COMMAND_USAGE(cd)
{
    wind_printf("cd [dir]:--to change current user path.\r\n");
    wind_printf("    dir : the directory to set.\r\n");
}

COMMAND_MAIN(cd,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 1,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"cd") == 0)
        return cmd_cd(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(cd);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
