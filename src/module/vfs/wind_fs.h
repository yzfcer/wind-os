/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_fs.h
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: �ļ�ϵͳ��׼��API�ӿ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_FILE_H__
#define WIND_FILE_H__
#include "wind_config.h"
#include "wind_blkdev.h"
#include "wind_filepath.h"
#include "fs_def.h"

#if WIND_FS_SUPPORT

#define WIND_FS_DEF(name,type,ops) \
{{~WIND_FS_MAGIC,#name,{W_NULL,W_NULL},0,0},W_NULL,type,W_NULL,W_NULL,&ops}

w_err_t _wind_fs_mod_init(void);
w_fs_s *wind_fs_get(char *name);
w_fs_s *wind_fs_get_bypath(const char *path);
w_err_t wind_fs_print(void);

w_fs_ops_s *wind_fs_ops_get(const char *name);
w_err_t wind_fs_ops_register(w_fs_ops_s *ops);
w_err_t wind_fs_ops_unregister(w_fs_ops_s *ops);

w_err_t wind_fs_mount(char *fsname,w_fstype_e type,char *devname,char *path);
w_err_t wind_fs_unmount(char *fsname);
w_err_t wind_fs_format(w_fs_s *fs);

#endif
#endif

