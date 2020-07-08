/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_mount.c
** Author      : Jason Zhou
** Last Date   : 2018/5/13 20:24:37
** Description : Mount file system command
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
#include "wind_fs.h"
#include "wind_file.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if CMD_MOUNT_SUPPORT


static w_err_t cmd_mount(w_int32_t argc,char **argv)
{
    w_vfs_s *vfs;
    if(argc == 1)
    {
        return wind_vfs_print_detail();
    }
    WIND_ASSERT_RETURN(argc >= 4,W_ERR_INVALID);
    vfs = wind_vfs_get_free();
    WIND_ASSERT_RETURN(vfs != W_NULL,W_ERR_FAIL);
    return wind_vfs_mount(vfs->obj.name,argv[1],argv[2],argv[3]);
}






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
