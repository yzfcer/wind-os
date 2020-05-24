/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: wind_fsops.h
**Author: Jason Zhou
**Last Date: 
**Description: 文件系统标准化操作集API接口
**              
**--------------History----------------------------------------------------------------------------
Author: 
** Version: v1.0
** Date: 
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_FSOPS_H__
#define WIND_FSOPS_H__
#include "wind_config.h"
#include "wind_blkdev.h"
#include "wind_filepath.h"
#include "fs_def.h"

#if WIND_MODULE_VFS_SUPPORT

w_fsops_s *wind_fsops_get(const char *name);
char *wind_vfs_checktype(w_blkdev_s *blkdev,char *type);
w_err_t wind_fsops_register(w_fsops_s *ops);
w_err_t wind_fsops_unregister(w_fsops_s *ops);
w_err_t wind_fsops_init(void);
#endif
#endif

