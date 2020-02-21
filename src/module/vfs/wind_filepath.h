/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_filepath.h
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 文件路径处理相关函数
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
#ifndef WIND_FILEPATH_H__
#define WIND_FILEPATH_H__
#include "wind_config.h"
#include "wind_type.h"
#if WIND_MODULE_VFS_SUPPORT
char *wind_filepath_generate(char *pre_path,char *relative_path,w_uint16_t isdir);
char *wind_filepath_remove_tail(char *path);
char * wind_filepath_copy(char *path);
w_bool_t wind_filepath_isdir(char *path);
w_err_t wind_filepath_release(char *path);
char * wind_filepath_get_current(void);
w_err_t wind_filepath_set_current(char *path);
w_int32_t wind_filepath_split(char *path,char **layers,w_int32_t layercnt);
w_err_t wind_filepath_check_valid(char *path);
char* wind_filepath_get_parent(char *path);
char* wind_filepath_get_filename(char *path);
char* wind_filepath_to_directory(char *path);


#endif
#endif

