/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_umount.c
** Author      : Jason Zhou
** Last Date   : 2018/5/13 20:24:37
** Description : 文件系统卸载命令
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


#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_FS_SUPPORT)
#include "wind_file.h"
/********************************************internal variables**********************************************/


/********************************************internal functions**********************************************/

static w_err_t cmd_umount(w_int32_t argc,char **argv)
{
    return wind_vfs_unmount(argv[1]);
}

/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(umount)
{
    wind_printf("to umount file system.\r\n");
}

COMMAND_USAGE(umount)
{
    wind_printf("umount <fsname>:--to umount a file system.\r\n");
    wind_printf("        fsname :file system object name,use command \"mount\" to see it.\r\n");
}

COMMAND_MAIN(umount,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"umount") == 0)
        return cmd_umount(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(umount);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
