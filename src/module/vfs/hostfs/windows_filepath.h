/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : windows_filepath.h
** Author      : Jason Zhou
** Last Date   : 2019.09.08
** Description : File system adaptation function in Windows
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.09.08
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.09.08
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WINDOWS_FILEPATH_H__
#define WINDOWS_FILEPATH_H__
#include "wind_config.h"
#include "wind_type.h"
#if WIND_HOSTFS_SUPPORT

#if HOST_OS_TYPE == HOST_OS_WINDOWS)

hfileattr_e host_file_type(char *path);
char *host_filepath_remove_tail(char *path);
char *host_filepath_generate(const char *pre_path,const char *relative_path,w_uint16_t isdir);
char * host_filepath_copy(char *path);
w_bool_t host_filepath_isdir(char *path);
w_err_t host_filepath_release(char *path);

char * host_filepath_get_current(void);
w_err_t host_filepath_set_current(char *path);


w_int32_t host_filepath_split(char *path,char **layers,w_int32_t layercnt);
w_err_t host_filepath_check_valid(char *path);
char* host_filepath_get_parent(char *path);
char* host_filepath_get_filename(char *path);
char* host_filepath_to_directory(char *path);


#endif
#endif
#endif

