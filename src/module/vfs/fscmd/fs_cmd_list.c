/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: fs_cmd_list.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2018/5/13 20:24:37
** ��        ��: �ļ�ϵͳ�������ע��ӿ�
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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_cmd.h"
#if WIND_MODULE_VFS_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if CMD_FS_SUPPORT
    COMMAND_DECLARE(cat);
    COMMAND_DECLARE(cd);
    COMMAND_DECLARE(ls);
    COMMAND_DECLARE(mkdir);
    COMMAND_DECLARE(pwd);
    COMMAND_DECLARE(rm);
    COMMAND_DECLARE(touch);
    COMMAND_DECLARE(write);
#endif


void _wind_vfs_register_fs_cmd(void)
{
#if CMD_FS_SUPPORT
    wind_cmd_register(COMMAND(cat));
    wind_cmd_register(COMMAND(cd));
    wind_cmd_register(COMMAND(ls));
    wind_cmd_register(COMMAND(mkdir));
    wind_cmd_register(COMMAND(pwd));
    wind_cmd_register(COMMAND(rm));
    wind_cmd_register(COMMAND(touch));
    wind_cmd_register(COMMAND(write));
#endif
}

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#endif
