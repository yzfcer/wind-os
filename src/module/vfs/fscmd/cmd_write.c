/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_write.c
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
static w_err_t cmd_write(w_int32_t argc,char **argv)
{
    w_file_s *file;
    char * fullpath;
    w_int32_t len,filelen;
    char *curpath = wind_filepath_get_current();
    if(argc < 3)
        return W_ERR_INVALID;
    fullpath = wind_filepath_generate(curpath,argv[1],0);
    file = wind_fopen(fullpath,FMODE_W | FMODE_CRT);
    if(file == W_NULL)
    {
        wind_printf("open directory or file failed.\r\n",fullpath);
        wind_filepath_release(fullpath);
        return W_ERR_NOFILE;
    }

    len = wind_strlen(argv[2]);
    filelen = wind_fwrite(file,(w_uint8_t*)argv[2],len);
    wind_fclose(file);
    if(filelen == len)
    {
        wind_printf("write file OK.\r\n");
        wind_filepath_release(fullpath);
        return W_ERR_OK;
    }
    wind_printf("write file failed.\r\n");
    wind_filepath_release(fullpath);
    return W_ERR_FAIL;
}



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(write)
{
    wind_printf("to operate write file system.\r\n");
}

COMMAND_USAGE(write)
{
    wind_printf("write <file> <content>:--to write content into a file.\r\n");
    wind_printf("file : the destination file to write in.\r\n");
    wind_printf("content : what to write into the file.\r\n");
	
}

COMMAND_MAIN(write,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"write") == 0)
        return cmd_write(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(write);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
