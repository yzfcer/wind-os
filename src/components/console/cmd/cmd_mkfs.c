/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_mkfs.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2018/11/09 20:24:37
** 描        述: 创建文件系统命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018/11/09 20:24:37
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
#include "efs.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (WIND_CONSOLE_SUPPORT && CMD_ECHO_SUPPORT)

/********************************************内部变量定义**********************************************/
static EmbeddedFileSystem efs;
extern w_err_t fat32_format(EmbeddedFileSystem *fs,char *blkname);
static void mkfat32(char *dev,char *path)
{
    dev = fat32_format(&efs,dev);
}


/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(mkfs)
{
    console_printf("to display mkfs string that you input.\r\n");
}

COMMAND_USAGE(mkfs)
{
    console_printf("mkfs fat32 <dev> <fsfile>:to  make a file system image.\r\n");
}

//mkfs fat32 <dev> <fsfile>
COMMAND_MAIN(mkfs,argc,argv)
{
    char *str;
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    str = argv[1];
    console_printf("mkfs is NOT support.");
    return W_ERR_OK;
}

COMMAND_DEF(mkfs);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
