/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_fsops.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 文件系统标准化操作集API接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_fs.h"
#include "wind_file.h"
#include "wind_pool.h"
#include "wind_debug.h"
#include "wind_mutex.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_core.h"
#include "wind_dlist.h"
#include "wind_dbgpoint.h"
#include "wind_board_port.h"
#include "treefs.h"
#include "listfs.h"

#if WIND_MODULE_VFS_SUPPORT
#define NODE_TO_FSOPS(dnode) (w_fsops_s*)(((w_uint8_t*)(dnode))-((w_addr_t)&(((w_fsops_s*)0)->obj.objnode)))

static w_dlist_s fs_ops_list;

w_fsops_s *wind_fsops_get(const char *name)
{
    return (w_fsops_s*)wind_obj_get(name,&fs_ops_list);
}

char *wind_fsops_checktype(w_blkdev_s *blkdev,char *type)
{
    w_dnode_s *dnode;
    w_err_t err;
    w_fsops_s *ops = (w_fsops_s *)W_NULL;
    wind_disable_switch();
    foreach_node(dnode,&fs_ops_list)
    {
        err = W_ERR_FAIL;
        ops = NODE_TO_FSOPS(dnode);
        if(ops->matchfs)
            err = ops->matchfs((char*)wind_obj_name(&blkdev->obj));
        if(err == W_ERR_OK)
            break;
    }
    wind_disable_switch();
    if(ops)
        return (char*)wind_obj_name(&ops->obj);
    return (char*)W_NULL;
}

w_err_t wind_fsops_register(w_fsops_s *ops)
{
    w_err_t err = W_ERR_OK;
    w_fsops_s *tmpops;
    WIND_ASSERT_RETURN(ops != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(ops->obj.magic == WIND_FSTYPE_MAGIC,W_ERR_INVALID);
    wind_notice("register vfs ops:%s",wind_obj_name(&ops->obj));
    tmpops = wind_fsops_get(ops->obj.name);
    if(tmpops != W_NULL)
    {
        wind_notice("vfs ops has been registered.\r\n");
        return W_ERR_OK;
    }
    if(ops->opsinit != W_NULL)
    {
        err = ops->opsinit();
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    }
    wind_obj_init(&ops->obj,WIND_FSTYPE_MAGIC,ops->obj.name,&fs_ops_list);
    return W_ERR_OK;
}

w_err_t wind_fsops_unregister(w_fsops_s *ops)
{
    w_err_t err;
    WIND_ASSERT_RETURN(ops != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(ops->obj.magic == WIND_FSTYPE_MAGIC,W_ERR_INVALID);
    wind_notice("unregister vfs ops:%s",wind_obj_name(&ops->obj));
    err = wind_obj_deinit(&ops->obj,WIND_FSTYPE_MAGIC,&fs_ops_list);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    return err;
}

#if WIND_TREEFS_SUPPORT
extern w_fsops_s treefs_ops;
#endif
#if WIND_LISTFS_SUPPORT
extern w_fsops_s listfs_ops;
#endif
#if WIND_HOSTFS_SUPPORT
extern w_fsops_s hostfs_ops;
#endif

w_err_t wind_fsops_init(void)
{
    DLIST_INIT(fs_ops_list);
#if WIND_TREEFS_SUPPORT
    wind_fsops_register(&treefs_ops);
#endif
#if WIND_LISTFS_SUPPORT
    wind_fsops_register(&listfs_ops);
#endif
#if WIND_HOSTFS_SUPPORT
    wind_fsops_register(&hostfs_ops);
#endif
    return W_ERR_OK;
}



#endif
