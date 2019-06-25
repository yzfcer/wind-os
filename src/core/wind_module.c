/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_module.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: wind-os模块框架
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
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_module.h"
#include "wind_mutex.h"
#include "wind_core.h"
#include "wind_string.h"
#if WIND_MODULE_MAGIC
#define NODE_TO_module(node) (w_module_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_module_s*)0)->obj.objnode)))
static w_dlist_s modulelist;

w_err_t wind_module_register(w_module_s *module)
{
    w_module_s *devi;    
    w_err_t err;
    WIND_ASSERT_RETURN(module != W_NULL,W_ERR_PTR_NULL);

    WIND_ASSERT_RETURN(module->obj.magic == (~WIND_MODULE_MAGIC),W_ERR_INVALID);
    WIND_ASSERT_RETURN(module->obj.name != W_NULL,W_ERR_PTR_NULL);
    wind_notice("register module:%s",wind_obj_name(&module->obj));
    devi = wind_module_get(module->obj.name);
    if(devi != W_NULL)
    {
        wind_notice("device has been registered.\r\n");
        return W_ERR_OK;
    }
    if(module->init)
    {
        err = module->init();
        if(err != W_ERR_OK)
        {
            wind_error("blkdev:%s init failed:%d.",module->obj.name,err);
            return err;
        }
    }
    //module->mutex = wind_mutex_create(module->obj.name);
    wind_obj_init(&module->obj,WIND_MODULE_MAGIC,module->obj.name,&modulelist);
    
    return W_ERR_OK;
}

w_err_t wind_module_unregister(w_module_s *module)
{
    w_err_t err;
    WIND_ASSERT_RETURN(module != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(module->obj.magic == WIND_MODULE_MAGIC,W_ERR_INVALID);
    wind_notice("unregister module:%s",module->obj.name);
    err = wind_obj_deinit(&module->obj,WIND_MODULE_MAGIC,&modulelist);
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    if(module->exit)
        module->exit();
    //wind_mutex_destroy(module->mutex);
    //module->mutex = W_NULL;
    return W_ERR_OK;
}


w_err_t _wind_module_mod_init(void)
{
    DLIST_INIT(modulelist);
    _wind_register_modules();
    return W_ERR_OK;
}

w_module_s *wind_module_get(const char *name)
{
    return (w_module_s*)wind_obj_get(name,&modulelist);
}


w_err_t wind_module_print(void)
{
    w_dnode_s *dnode;
    w_module_s *module;
    int cnt = 0;
    w_dlist_s *list = &modulelist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\nmodule list:\r\n");
    
    foreach_node(dnode,list)
    {
        module = NODE_TO_module(dnode);
        wind_printf("%-12s ",module->obj.name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}


#endif


