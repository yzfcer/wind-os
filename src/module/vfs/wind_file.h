/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_file.h
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: �ļ�ϵͳ��׼��API�ӿ�
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
#ifndef WIND_FS_H__
#define WIND_FS_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
#include "wind_filepath.h"
#include "fs_def.h"

#if WIND_FS_SUPPORT

//w_file_s *file_malloc(void);
//w_err_t file_free(w_file_s *file);
w_err_t _wind_file_mod_init(void);


w_bool_t wind_file_check(const char *path);
w_file_s *wind_file_get(w_fs_s *fs,const char *path);
w_file_s *wind_file_get_bypath(const char *path);

w_file_s* wind_fopen(const char *path,w_uint16_t fmode);
w_err_t wind_fclose(w_file_s *file);
w_err_t wind_fremove(const char *path);
char* wind_fchild(w_file_s *dir,w_int32_t index);

w_err_t wind_fseek(w_file_s *file,w_int32_t offset);
w_int32_t wind_ftell(w_file_s *file);
w_int32_t wind_fread(w_file_s *file,w_uint8_t *buff, w_int32_t size);
w_int32_t wind_fwrite(w_file_s *file,w_uint8_t *buff, w_int32_t size);
w_err_t wind_fgets(w_file_s *file,char *buff, w_int32_t maxlen);
w_err_t wind_fputs(w_file_s *file,char *buff);
w_err_t wind_frename(w_file_s *file,char *newname);
#endif
#endif
