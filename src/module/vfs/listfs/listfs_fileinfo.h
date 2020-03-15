/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: listfs_fileinfo.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.04.05
**��        ��: �ļ�ϵͳ�Կ��豸�Ĳ����ӿ�
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
#ifndef LISTFS_FILEINFO_H__
#define LISTFS_FILEINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_blkdev.h"
#if WIND_LISTFS_SUPPORT

#define LISTFILE_MAGIC 0x7D5349AC

#define LFILE_NAME_LEN 64    //�ļ�������

//��fileinfo���ݿ���ȡ��blkinfo
#define FILEINFO_BLKINFO(blk) (lfile_blkinfo_s*)&blk[sizeof(lfile_info_s)]

//#define BLKINFO_HAS_OFFSET(info,ofst,blksize) ((ofst >= info->offset)&&(ofst < info->offset + info->blkused * blksize))




//�̻��ļ�ͷ����Ϣ
typedef struct __lfile_info_s
{
    w_uint32_t magic;                //ħ����
    char       name[LFILE_NAME_LEN]; //�ļ���
    w_int32_t  filesize;             //�ļ���С
    w_int32_t  spacesize;            //�ļ��ռ��С
    w_uint32_t   parent_addr;          //����ַ
    w_uint32_t   self_addr;            //��ǰ��ַ
    w_uint32_t   last_addr;            //���һ������Ϣ��ַ
    w_uint32_t   prevfile_addr;        //��һ���ļ���ַ
    w_uint32_t   nextfile_addr;        //��һ���ļ���ַ
    w_int32_t  children_cnt;         //���ļ�������
    w_uint32_t   headchild_addr;       //��һ�����ļ���ַ
    w_uint32_t   tailchild_addr;       //���һ���ļ���ַ
    w_uint8_t  attr;                 //�Ƿ�Ŀ¼���ɶ�����д�����أ�У��
}lfile_info_s;

void fileinfo_be2le(lfile_info_s *info);
w_err_t fileinfo_init(lfile_info_s *info,char *name,
    w_uint32_t self_addr,w_uint32_t parent_addr,w_uint32_t prev_addr,w_uint8_t attr);

w_err_t fileinfo_read(lfile_info_s *info,w_blkdev_s *blkdev,w_uint32_t addr);

w_err_t fileinfo_write(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_next(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_prev(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_headchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_tailchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_add_update_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_add_update_prev(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_rm_update_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_rm_update_prev(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_rm_update_next(lfile_info_s *info,w_blkdev_s *blkdev);

#endif
#endif
