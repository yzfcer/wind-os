/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_filepath.h
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: �ļ�·��������غ���
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
#ifndef WIND_FILEPATH_H__
#define WIND_FILEPATH_H__
#include "wind_config.h"
#include "wind_type.h"
#if WIND_MODULE_VFS_SUPPORT
char *wind_filepath_generate(char *pre_path,char *relative_path,w_uint16_t isdir);
char *wind_filepath_remove_tail(const char *path);
char * wind_filepath_copy(const char *path);
w_bool_t wind_filepath_isdir(const char *path);
w_err_t wind_filepath_release(char *path);
char * wind_filepath_get_current(void);
w_err_t wind_filepath_set_current(const char *path);
w_int32_t wind_filepath_split(char *path,char **layers,w_int32_t layercnt);
w_err_t wind_filepath_check_valid(const char *path);
char* wind_filepath_get_parent(const char *path);
char* wind_filepath_get_filename(const char *path);
char* wind_filepath_to_directory(const char *path);
char* wind_filepath_suffix(const char *path);


#endif
#endif

