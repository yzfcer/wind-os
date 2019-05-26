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
#if WIND_FS_SUPPORT
char *wind_filepath_generate(char *pre_path,char *relative_path,w_uint16_t isdir);
w_err_t wind_filepath_release(char *path);
char *  wind_filepath_get_current(void);
w_err_t wind_filepath_set_current(char *path);
w_err_t wind_filepath_check_valid(char *path);
w_err_t wind_filepath_get_parent(char *path);
w_err_t wind_pathfile_get_filename(char *path);


#endif
#endif

