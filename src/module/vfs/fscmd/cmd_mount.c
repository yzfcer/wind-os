/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: cmd_mount.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2018/5/13 20:24:37
** ��        ��: �ļ�ϵͳ��������
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2018/5/13 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_heap.h"
#include "wind_string.h"
#include "wind_fs.h"


#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/

#if (CMD_FS_SUPPORT)
#include "wind_file.h"
/********************************************�ڲ���������**********************************************/


/********************************************�ڲ���������*********************************************/

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

/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
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
