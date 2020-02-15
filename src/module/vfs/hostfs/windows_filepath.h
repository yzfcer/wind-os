/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: windows_filepath.h
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: windows�ļ�·��������غ���
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
#ifndef WINDOWS_FILEPATH_H__
#define WINDOWS_FILEPATH_H__
#include "wind_config.h"
#include "wind_type.h"
#if (HOST_OS_TYPE == HOST_OS_WINDOWS)
hfileattr_e hostfs_file_type(char *path);
char *windows_filepath_remove_tail(char *path);
char *windows_filepath_generate(char *pre_path,char *relative_path,w_uint16_t isdir);
char * windows_filepath_copy(char *path);
w_bool_t windows_filepath_isdir(char *path);
w_err_t windows_filepath_release(char *path);

char * windows_filepath_get_current(void);
w_err_t windows_filepath_set_current(char *path);


w_int32_t windows_filepath_split(char *path,char **layers,w_int32_t layercnt);
w_err_t windows_filepath_check_valid(char *path);
char* windows_filepath_get_parent(char *path);
char* windows_filepath_get_filename(char *path);
char* windows_filepath_to_directory(char *path);


#endif
#endif

