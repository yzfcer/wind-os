/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: boot_imghead.h
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: wind-boot��image�ļ�ͷ��������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2018.11.10
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_PACK_H__
#define WIND_PACK_H__

#include "wind_config.h"
#include "wind_type.h"

#define IMG_MAGIC 0x4528AB6D
#define IMG_HEAD_VER 0x01
#define IMG_HEAD_LEN 512
typedef struct __img_head_s img_head_s;
struct __img_head_s
{
    w_uint32_t magic;
    w_uint32_t img_len;
    w_uint32_t head_len;
    w_uint32_t head_ver;
    w_uint32_t hard_ver;
    w_uint32_t soft_ver;
    w_uint32_t bin_crc;
    w_uint32_t bin_offset;
    w_uint32_t encrypt_type;
    char       img_name[64];
    char       arch_name[32];
    char       cpu_name[32];
    char       board_name[32];
};

void boot_img_head_print(img_head_s *head);
w_err_t boot_img_head_get(img_head_s *head,w_uint8_t *buff);
w_err_t boot_img_head_set(img_head_s *head,w_uint8_t *buff);




#endif


