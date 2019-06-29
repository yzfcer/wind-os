/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_module.c
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: wind-osģ����
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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_module.h"
#include "wind_mutex.h"
#include "wind_core.h"
#include "wind_string.h"
#if WIND_MODULE_MAGIC
typedef struct 
{
    char *modname;
    w_uint32_t version;
}mod_depend_s;

#define NODE_TO_module(node) (w_module_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_module_s*)0)->obj.objnode)))
static w_dlist_s modulelist;
w_err_t _wind_module_mod_init(void)
{
    DLIST_INIT(modulelist);
    _wind_register_modules();
    return W_ERR_OK;
}

static w_int32_t module_parse_depend(char *dependstr,mod_depend_s **dmod)
{
    wind_error("TODO parse depend here");
    return 0;
}

static w_err_t module_check_depend(w_module_s *module)
{
    int i,cnt;
    w_err_t err;
    w_module_s *dmod;
    mod_depend_s *depend;
    cnt = module_parse_depend(module->depend,&depend);
    if(cnt <= 0)
        return W_ERR_OK;
    err = W_ERR_OK;
    for(i = 0;i < cnt;i ++)
    {
        dmod = wind_module_get(depend[i].modname);
        WIND_ASSERT_BREAK(dmod != dmod, W_ERR_NOT_SUPPORT, "depend module is NOT ready");
        WIND_ASSERT_BREAK(dmod->version >= depend[i].version, W_ERR_VERSION, "depend module version error");
    }
    return err;
}

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
    err = module_check_depend(module);
    WIND_ASSERT_RETURN(err == W_ERR_OK, err);
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

