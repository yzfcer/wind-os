/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_diagnose.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.02.24
**描        述:系统故障诊断处理模块
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.02.24
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.02.24
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_diagnose.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_mutex.h"
#include "wind_os_hwif.h"



#if WIND_DIAGNOSE_SUPPORT
static w_dlist_s diagnoselist;


w_err_t _wind_diagnose_mod_init(void)
{
    DLIST_INIT(diagnoselist);
    return W_ERR_OK;
}

w_diagnose_s *wind_diagnose_get(const char *name)
{
    w_diagnose_s *diagnose;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&diagnoselist)
    {
        diagnose = DLIST_OBJ(dnode,w_diagnose_s,diagnosenode);
        if(diagnose->name && (wind_strcmp(name,diagnose->name) == 0))
        {
            wind_enable_switch();
            return diagnose;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_diagnose_register(w_diagnose_s *diagnose)
{
    w_diagnose_s *diag;    
    WIND_ASSERT_RETURN(diagnose != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(diagnose->magic == (~WIND_DIAGNOSE_MAGIC),W_ERR_INVALID);
    wind_notice("register diagnose:%s",diagnose->name);
    diag = wind_diagnose_get(diagnose->name);
    if(diag != W_NULL)
    {
        wind_notice("diagnose has been registered.\r\n");
        return W_ERR_OK;
    }
    
    diagnose->magic = WIND_DIAGNOSE_MAGIC;
    wind_disable_switch();
    dlist_insert_tail(&diagnoselist,&diagnose->diagnosenode);
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_diagnose_unregister(w_diagnose_s *diagnose)
{
    w_dnode_s *dnode;
    w_diagnose_s *diag;
    WIND_ASSERT_RETURN(diagnose != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(diagnose->magic == WIND_DIAGNOSE_MAGIC,W_ERR_INVALID);
    diag = wind_diagnose_get(diagnose->name);
    if(diag == W_NULL)
    {
        wind_error("diagnose has NOT been registered.\r\n");
        return W_ERR_FAIL;
    }
    wind_disable_switch();
    dnode = dlist_remove(&diagnoselist,&diagnose->diagnosenode);
    wind_enable_switch();
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    diag = DLIST_OBJ(dnode,w_diagnose_s,diagnosenode);
    return W_ERR_OK;
}

static w_err_t diagnose_check(w_diagnose_s *diagnose)
{
    
    WIND_ASSERT_RETURN(diagnose->magic == WIND_DIAGNOSE_MAGIC,DIAG_RES_DIAG_MAGIC_ERROR);
    WIND_ASSERT_RETURN(diagnose->diagnose_func != W_NULL,DIAG_RES_FUNC_NULL);
    diagnose->result = diagnose->diagnose_func();
    return diagnose->result;
}

w_err_t wind_diagnose_check(void)
{
    w_err_t err = W_ERR_OK;
    w_dnode_s *dnode;
    w_diagnose_s *diagnose;
    wind_disable_switch();
    wind_notice("system diagnose start");
    foreach_node(dnode,&diagnoselist)
    {
        diagnose = (w_diagnose_s*)DLIST_OBJ(dnode,w_diagnose_s,diagnosenode);
        diagnose->result = diagnose_check(diagnose);
    }
    wind_printf("--------------------------------------------------------\r\n");
    wind_printf("\r\ndiagnose check result:\r\n");
    foreach_node(dnode,&diagnoselist)
    {
        diagnose = (w_diagnose_s*)DLIST_OBJ(dnode,w_diagnose_s,diagnosenode);
        if(diagnose->result != DIAG_RES_OK)
            err = W_ERR_FAIL;
        wind_printf("%-12s status %-6s errorcode %d\r\n",diagnose->name,
            diagnose->result==0?"OK":"ERROR",diagnose->result);
    }
    wind_printf("--------------------------------------------------------\r\n");
    wind_enable_switch();
    return err;
}

w_err_t wind_diagnose_print(void)
{
    w_dnode_s *dnode;
    w_diagnose_s *diagnose;
    int cnt = 0;
    w_dlist_s *list = &diagnoselist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\ndiagnose list:\r\n");
    foreach_node(dnode,list)
    {
        diagnose = (w_diagnose_s *)DLIST_OBJ(dnode,w_diagnose_s,diagnosenode);
        wind_printf("%-12s ",diagnose->name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}

#endif

