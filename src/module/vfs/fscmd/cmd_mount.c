/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_mount.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2018/5/13 20:24:37
** 描        述: 文件系统挂载命令
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
#include "wind_fs.h"


#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (CMD_FS_SUPPORT)
#include "wind_file.h"
/********************************************内部变量定义**********************************************/


/********************************************内部函数定义*********************************************/

static w_err_t cmd_mount(w_int32_t argc,char **argv)
{
    w_vfs_s *vfs;
    if(argc == 1)
    {
        return wind_vfs_print();
    }
    WIND_ASSERT_RETURN(argc >= 4,W_ERR_INVALID);
    vfs = wind_vfs_get_free();
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_FAIL);
    return wind_vfs_mount(vfs->obj.name,argv[1],argv[2],argv[3]);
}

/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(mount)
{
    wind_printf("to mount a blkdev as a file system.\r\n");
}

COMMAND_USAGE(mount)
{
    wind_printf("mount <fstype> <blkdev> <path>:--to mount blkdev to the path.\r\n");
    wind_printf("     fstype : file system type,should be treefs or listfs.\r\n");
    wind_printf("     blkdev : block device name,can find it as command \"list blkdev\".\r\n");
    wind_printf("     path   : which path the file system to mount,it must be an existing\r\n");
    //wind_printf("              directory unless the rootfs.\r\n");
}

COMMAND_MAIN(mount,argc,argv)
{
    //WIND_ASSERT_RETURN(argc >= 1,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"mount") == 0)
        return cmd_mount(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(mount);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
