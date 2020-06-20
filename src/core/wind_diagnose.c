/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_diagnose.c
** Author      : Jason Zhou
** Last Date   : 2019.02.24
** Description :System diagnosis processing module
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.02.24
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.02.24
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_diagnose.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_pool.h"
#include "wind_string.h"
#include "wind_mutex.h"
#include "wind_board_port.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_DIAGNOSE_SUPPORT
#define NODE_TO_DIAGNOSENOSE(node) NODEPTR_TO_ITEMPTR(node,w_diagnose_s,obj.objnode)
static w_dlist_s diagnoselist;


w_err_t _wind_diagnose_mod_init(void)
{
    DLIST_INIT(diagnoselist);
    _wind_pool_diagnose_init();
    return W_ERR_OK;
}

w_diagnose_s *wind_diagnose_get(const char *name)
{
    return (w_diagnose_s*)wind_obj_get(name,&diagnoselist);
}

w_err_t wind_diagnose_register(w_diagnose_s *diagnose)
{
    w_diagnose_s *diag;    
    wind_notice("register diagnose:%s", wind_obj_name(&diagnose->obj));
    wind_debug("diagnose magic:%-08x,%-08x", diagnose->obj.magic,(~WIND_DIAGNOSE_MAGIC));
    WIND_ASSERT_RETURN(diagnose != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(diagnose->obj.magic == (~WIND_DIAGNOSE_MAGIC),W_ERR_INVALID);
    diag = wind_diagnose_get(diagnose->obj.name);
    if(diag != W_NULL)
    {
        wind_notice("diagnose has been registered.\r\n");
        return W_ERR_OK;
    }
    
    diagnose->obj.magic = WIND_DIAGNOSE_MAGIC;
    wind_disable_switch();
    dlist_insert_tail(&diagnoselist,&diagnose->obj.objnode);
    wind_enable_switch();
    return W_ERR_OK;
}

w_err_t wind_diagnose_unregister(w_diagnose_s *diagnose)
{
    w_dnode_s *dnode;
    w_diagnose_s *diag;
    WIND_ASSERT_RETURN(diagnose != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(diagnose->obj.magic == WIND_DIAGNOSE_MAGIC,W_ERR_INVALID);
    wind_notice("unregister diagnose:%s", wind_obj_name(&diagnose->obj));
    diag = wind_diagnose_get(diagnose->obj.name);
    if(diag == W_NULL)
    {
        wind_error("diagnose has NOT been registered.\r\n");
        return W_ERR_FAIL;
    }
    wind_disable_switch();
    dnode = dlist_remove(&diagnoselist,&diagnose->obj.objnode);
    wind_enable_switch();
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    diag = NODE_TO_DIAGNOSENOSE(dnode);
    return W_ERR_OK;
}

static w_err_t diagnose_check(w_diagnose_s *diagnose)
{
    
    WIND_ASSERT_RETURN(diagnose->obj.magic == WIND_DIAGNOSE_MAGIC,DIAG_RES_DIAG_MAGIC_ERROR);
    WIND_ASSERT_RETURN(diagnose->diagnose_func != W_NULL,DIAG_RES_FUNC_NULL);
    diagnose->result = diagnose->diagnose_func();
    return diagnose->result;
}
static w_err_t wind_diagnose_print_result(void)
{
    w_dnode_s *dnode;
    w_diagnose_s *diagnose;
    wind_printf("diagnose check result:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-10s\r\n","diagnose","status","errcode");
    wind_print_space(5);
    foreach_node(dnode,&diagnoselist)
    {
        diagnose = NODE_TO_DIAGNOSENOSE(dnode);
        wind_printf("%-16s %-8s %-10d\r\n",
            diagnose->obj.name,(diagnose->result == 0)?"OK":"ERROR",diagnose->result);
    }
    wind_print_space(5);
    return W_ERR_OK;
}
w_err_t wind_diagnose_check(void)
{
    w_err_t err = W_ERR_OK;
    w_dnode_s *dnode;
    w_diagnose_s *diagnose;
    wind_disable_switch();
    wind_notice("diagnose check start");
    foreach_node(dnode,&diagnoselist)
    {
        diagnose = NODE_TO_DIAGNOSENOSE(dnode);
        wind_notice(" ========= check %s",diagnose->obj.name);
        diagnose->result = diagnose_check(diagnose);
        if(diagnose->result != DIAG_RES_OK)
            err = W_ERR_FAIL;
    }
    wind_diagnose_print_result();

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
        diagnose = NODE_TO_DIAGNOSENOSE(dnode);
        wind_printf("%-12s ",diagnose->obj.name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}

#endif //#if WIND_DIAGNOSE_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
