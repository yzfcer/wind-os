/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_fs.h
** Author      : Jason Zhou
** Last Date   : 
** Description : VFS file system standardized API interface
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_FS_H__
#define WIND_FS_H__
#include "wind_config.h"
#include "wind_blkdev.h"
#include "wind_filepath.h"
#include "fs_def.h"

#if WIND_MODULE_VFS_SUPPORT

w_err_t _wind_vfs_mod_init(void);
void _wind_vfs_register_fs_cmd(void);

w_vfs_s *wind_vfs_get(char *name);
w_vfs_s *wind_vfs_get_free(void);
w_vfs_s *wind_vfs_get_byidx(w_int32_t index);

w_vfs_s *wind_vfs_get_bypath(const char *path);
w_err_t wind_vfs_print_list(void);
w_err_t wind_vfs_print_detail(void);

w_err_t wind_vfs_mount(char *fsname,char *fstype,char *devname,char *path);
w_err_t wind_vfs_unmount(char *fsname);
w_err_t wind_vfs_format(w_vfs_s *fs);

#endif
#endif

