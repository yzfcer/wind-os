/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: listfs_bitmap.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.04.05
**��        ��: �ļ�ϵͳ�Կ�λͼ�Ĳ����ӿ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.04.05
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.04.05
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_LISTFS_BITMAP_H__
#define WIND_LISTFS_BITMAP_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "listfs.h"
#include "wind_blkdev.h"
#if WIND_FS_SUPPORT
#define BITMAP_USED (0x01 << 0)
#define BITMAP_BAD  (0x01 << 1)


w_err_t listfs_bitmap_init(listfs_s *listfs);

w_err_t listfs_bitmap_set(listfs_s *listfs,w_int32_t unit_idx,w_uint8_t bitflag);

w_err_t listfs_bitmap_find_free(listfs_s *listfs,w_int32_t *freeidx);

w_err_t listfs_bitmap_clear(listfs_s *listfs);

#endif
#endif
