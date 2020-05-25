/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: cmd_rm.c
** Author      : Jason Zhou
** Last Date: 2018/5/13 20:24:37
** Description : 文件系统操作命令
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2018/5/13 20:24:37
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
