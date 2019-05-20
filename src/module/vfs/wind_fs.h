/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_fs.h
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 文件系统标准化API接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
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
{{~WIND_FS_MAGIC,#name,{W_NULL,W_NULL},0,0},W_NULL,#name,W_NULL,W_NULL,&ops}

w_err_t _wind_vfs_mod_init(void);
w_vfs_s *wind_vfs_get(char *name);
w_vfs_s *wind_vfs_get_bypath(const char *path);
w_err_t wind_vfs_print(void);

w_fstype_s *wind_fstype_get(const char *name);
w_err_t wind_fstype_register(w_fstype_s *ops);
w_err_t wind_fstype_unregister(w_fstype_s *ops);

w_err_t wind_vfs_mount(char *fsname,char *fstype,char *devname,char *path);
w_err_t wind_vfs_unmount(char *fsname);
w_err_t wind_vfs_format(w_vfs_s *fs);

#endif
#endif

