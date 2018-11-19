/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: boot_imghead.h
**创   建   人: 周江村
**最后修改日期: 
**描        述: wind-boot的image文件头部处理函数
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2018.11.10
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
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


