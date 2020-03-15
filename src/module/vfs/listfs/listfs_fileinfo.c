/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: listfs_fileinfo.c
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
#include "listfs_fileinfo.h"
#include "listfs.h"
#include "wind_conv.h"
#include "wind_debug.h"
#include "wind_string.h"
#if WIND_LISTFS_SUPPORT


void fileinfo_be2le(lfile_info_s *info)
{
    if(wind_endian() == ENDIAN_BIG)
    {
        SELF_BE2LE_4(info->magic);
        SELF_BE2LE_4(info->filesize);
        SELF_BE2LE_4(info->spacesize);
        SELF_BE2LE_4(info->parent_addr);
        SELF_BE2LE_4(info->self_addr);
        SELF_BE2LE_4(info->last_addr);
        SELF_BE2LE_4(info->prevfile_addr);
        SELF_BE2LE_4(info->nextfile_addr);
        SELF_BE2LE_4(info->children_cnt);
        SELF_BE2LE_4(info->headchild_addr);
        SELF_BE2LE_4(info->tailchild_addr);
    }
}

static w_err_t check_fileinfo(lfile_info_s *info)
{
    if((info->self_addr == info->parent_addr)&&(0 != info->parent_addr))
        return W_ERR_INVALID;
    if((info->self_addr == info->prevfile_addr)&&(0 != info->prevfile_addr))
        return W_ERR_INVALID;
    if((info->self_addr == info->nextfile_addr)&&(0 != info->nextfile_addr))
        return W_ERR_INVALID;
    if((info->prevfile_addr == info->nextfile_addr)&&(0 != info->nextfile_addr))
        return W_ERR_INVALID;
    return W_ERR_OK;
}

w_err_t fileinfo_init(lfile_info_s *info,char *name,
    w_uint32_t self_addr,w_uint32_t parent_addr,w_uint32_t prev_addr,w_uint8_t attr)
{
    w_int32_t namelen;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    namelen = wind_strlen(name);
    WIND_ASSERT_RETURN(namelen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(namelen < LFILE_NAME_LEN,W_ERR_INVALID);
    wind_memset(info,0,sizeof(lfile_info_s));
    info->magic = LISTFILE_MAGIC;
    wind_strcpy(info->name,name);
    info->parent_addr = parent_addr;
    info->self_addr = self_addr;
    info->prevfile_addr = prev_addr;
    info->attr = attr;
    return W_ERR_OK;
}


w_err_t fileinfo_read(lfile_info_s *info,w_blkdev_s *blkdev,w_uint32_t addr)
{
    w_err_t err;
    w_int32_t cnt;
    
    w_uint8_t *blk = W_NULL;
    lfile_info_s *tmpinfo;
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(addr != 0,W_ERR_INVALID);

    do
    {
        err = W_ERR_OK;
        blk = listfs_mem_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        cnt = wind_blkdev_read(blkdev,addr,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"read blkdata failed");
        tmpinfo = (lfile_info_s*)blk;
        fileinfo_be2le(tmpinfo);
        if(tmpinfo->magic != LISTFILE_MAGIC)
        {
            err = W_ERR_INVALID;
            break;
        }
        wind_memcpy(info,tmpinfo,sizeof(lfile_info_s));
        if(W_ERR_OK != check_fileinfo(info))
            wind_error("fileinfo addr error");
    }while(0);
    if(blk != W_NULL)
        listfs_mem_free(blk);
    return err;
}

w_err_t fileinfo_write(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    w_int32_t cnt;
    w_uint8_t *blk = W_NULL;
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->self_addr != 0,W_ERR_INVALID);
    if(W_ERR_OK != check_fileinfo(info))
        wind_error("fileinfo addr error");
    do
    {
        err = W_ERR_OK;
        blk = listfs_mem_malloc(blkdev->blksize);
        WIND_ASSERT_BREAK(blk != W_NULL,W_ERR_MEM,"malloc blk failed");
        cnt = wind_blkdev_read(blkdev,info->self_addr,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"read blkdata failed");
        fileinfo_be2le(info);
        wind_memcpy(blk,info,sizeof(lfile_info_s));
        cnt = wind_blkdev_write(blkdev,info->self_addr,blk,1);
        WIND_ASSERT_BREAK(cnt > 0,W_ERR_HARDFAULT,"read blkdata failed");
    }while(0);
    if(blk != W_NULL)
        listfs_mem_free(blk);
    return err;
}

w_err_t fileinfo_get_parent(lfile_info_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    wind_trace("fileinfo_get_parent");
    return fileinfo_read(info,blkdev,info->parent_addr);
}

w_err_t fileinfo_get_next(lfile_info_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    wind_trace("fileinfo_get_next");
    return fileinfo_read(info,blkdev,info->nextfile_addr);
}

w_err_t fileinfo_get_prev(lfile_info_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    wind_trace("fileinfo_get_prev");
    return fileinfo_read(info,blkdev,info->prevfile_addr);
}

w_err_t fileinfo_get_headchild(lfile_info_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    wind_trace("fileinfo_get_headchild");
    return fileinfo_read(info,blkdev,info->headchild_addr);
}

w_err_t fileinfo_get_tailchild(lfile_info_s *info,w_blkdev_s *blkdev)
{
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->magic == LISTFILE_MAGIC,W_ERR_INVALID);
    wind_trace("fileinfo_get_tailchild");
    return fileinfo_read(info,blkdev,info->tailchild_addr);
}


w_err_t fileinfo_add_update_parent(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;

    w_uint32_t self_addr;
    lfile_info_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->parent_addr != 0,W_ERR_INVALID);
    wind_trace("fileinfo_add_update_parent");
    do
    {
        err = W_ERR_OK;
        tmpinfo = listfs_mem_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc tmpinfo failed");
        self_addr = info->self_addr;
        err = fileinfo_read(tmpinfo,blkdev,info->parent_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"read parent info failed");

        if(tmpinfo->headchild_addr == 0)
            tmpinfo->headchild_addr = self_addr;
        tmpinfo->tailchild_addr = self_addr;
        tmpinfo->children_cnt ++;
        err = fileinfo_write(tmpinfo,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"write fileinfo failed");

    }while(0);
    if(tmpinfo != W_NULL)
        listfs_mem_free(tmpinfo);
    return err;
}

w_err_t fileinfo_add_update_prev(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;

    w_uint32_t self_addr;
    lfile_info_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->prevfile_addr != 0,W_ERR_INVALID);
    
    wind_trace("fileinfo_add_update_prev");
    do
    {
        err = W_ERR_OK;
        tmpinfo = listfs_mem_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc tmpinfo failed");
        self_addr = info->self_addr;
        err = fileinfo_read(tmpinfo,blkdev,info->prevfile_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blkibfo failed");
        tmpinfo->nextfile_addr = self_addr;
        err = fileinfo_write(tmpinfo,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"write fileinfo failed");

    }while(0);
    if(tmpinfo != W_NULL)
        listfs_mem_free(tmpinfo);
    return err;
}

w_err_t fileinfo_rm_update_parent(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    lfile_info_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->parent_addr != 0,W_ERR_INVALID);
    wind_trace("fileinfo_rm_update_parent");

    do
    {
        err = W_ERR_OK;
        tmpinfo = listfs_mem_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc tmpinfo failed");
        err = fileinfo_read(tmpinfo,blkdev,info->parent_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,W_ERR_FAIL,"read parent info failed");

        if(tmpinfo->headchild_addr == info->self_addr)
            tmpinfo->headchild_addr = info->nextfile_addr;
        if(tmpinfo->tailchild_addr == info->self_addr)
            tmpinfo->tailchild_addr = info->prevfile_addr;
        tmpinfo->children_cnt --;
        err = fileinfo_write(tmpinfo,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"write fileinfo failed");

    }while(0);
    if(tmpinfo != W_NULL)
        listfs_mem_free(tmpinfo);
    return err;
}

w_err_t fileinfo_rm_update_prev(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    lfile_info_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->prevfile_addr != 0,W_ERR_INVALID);
    
    wind_trace("fileinfo_rm_update_prev");
    do
    {
        err = W_ERR_OK;
        tmpinfo = listfs_mem_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc tmpinfo failed");
        err = fileinfo_read(tmpinfo,blkdev,info->prevfile_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blkibfo failed");
        tmpinfo->nextfile_addr = info->nextfile_addr;
        err = fileinfo_write(tmpinfo,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"write fileinfo failed");

    }while(0);
    if(tmpinfo != W_NULL)
        listfs_mem_free(tmpinfo);
    return err;
}


w_err_t fileinfo_rm_update_next(lfile_info_s *info,w_blkdev_s *blkdev)
{
    w_err_t err;
    lfile_info_s *tmpinfo = W_NULL;
    
    WIND_ASSERT_RETURN(info != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(info->nextfile_addr != 0,W_ERR_INVALID);
    
    wind_trace("fileinfo_rm_update_next");
    do
    {
        err = W_ERR_OK;
        tmpinfo = listfs_mem_malloc(sizeof(lfile_info_s));
        WIND_ASSERT_BREAK(tmpinfo != W_NULL,W_ERR_MEM,"malloc tmpinfo failed");
        err = fileinfo_read(tmpinfo,blkdev,info->nextfile_addr);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"read blkibfo failed");
        tmpinfo->prevfile_addr = info->prevfile_addr;
        err = fileinfo_write(tmpinfo,blkdev);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"write fileinfo failed");

    }while(0);
    if(tmpinfo != W_NULL)
        listfs_mem_free(tmpinfo);
    return err;
}

#endif


