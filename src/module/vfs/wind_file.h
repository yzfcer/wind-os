/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_file.h
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 文件系统文件操作标准化API接口
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
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
#include "wind_filepath.h"
#include "fs_def.h"

#if WIND_MODULE_VFS_SUPPORT


w_err_t _wind_file_mod_init(void);


w_file_s *wind_file_get(w_vfs_s *fs,const char *path);
w_file_s *wind_file_get_bypath(char *path);

w_bool_t wind_fexist(const char *path);
w_file_s* wind_fopen(const char *path,w_uint8_t fmode);
w_err_t wind_fclose(w_file_s *file);
w_err_t wind_fremove(const char *path);
w_file_s *wind_freaddir(w_file_s *dir);

w_err_t wind_fseek(w_file_s *file,w_uint32_t offset);
w_int32_t wind_ftell(w_file_s *file);
w_int32_t wind_fread(w_file_s *file,w_uint8_t *buff, w_int32_t size);
w_int32_t wind_fwrite(w_file_s *file,w_uint8_t *buff, w_int32_t size);
w_err_t wind_fgets(w_file_s *file,char *buff, w_int32_t maxlen);
w_err_t wind_fputs(w_file_s *file,char *buff);
w_err_t wind_frename(w_file_s *file,char *newname);
#endif
#endif

