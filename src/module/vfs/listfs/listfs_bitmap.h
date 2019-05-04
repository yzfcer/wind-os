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
typedef struct
{
    w_addr_t addr1;         //��λͼ����ʼ��ַ
    w_addr_t addr2;         //����λͼ����ʼ��ַ
    w_int32_t free_blkidx;   //����λͼ��λ��
    w_int32_t addr_cnt;      //λͼ������
    w_int32_t free_byteidx;  //����λͼ�ֽ�λ��
    w_blkdev_s *blkdev;
}lfs_bitmap_s;

w_err_t listfs_bitmap_init(lfs_bitmap_s *bp,w_addr_t start_addr,w_int32_t count,w_blkdev_s *blkdev);

w_err_t listfs_bitmap_update(lfs_bitmap_s *bp);

w_err_t listfs_bitmap_set(lfs_bitmap_s *bp,w_addr_t addr,w_uint8_t bitflag);

w_err_t listfs_bitmap_alloc_blk(lfs_bitmap_s *bp,w_addr_t *addr,w_int32_t cnt);

w_err_t listfs_bitmap_free_blk(lfs_bitmap_s *bp,w_addr_t *addr,w_int32_t count);

w_err_t listfs_bitmap_clear(lfs_bitmap_s *bp);

w_uint32_t listfs_bitmap_get_usedblk(lfs_bitmap_s *bp);


#endif
#endif
