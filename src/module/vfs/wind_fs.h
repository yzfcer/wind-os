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

