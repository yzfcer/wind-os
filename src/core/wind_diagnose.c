/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_diagnose.c
**��   ��   ��: Jason Zhou
**����޸�����: 2019.02.24
**��        ��:ϵͳ������ϴ���ģ��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.02.24
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.02.24
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_diagnose.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_mutex.h"
#include "wind_board_port.h"



#if WIND_DIAGNOSE_SUPPORT
static w_dlist_s diagnoselist;


w_err_t _wind_diagnose_mod_init(void)
{
    DLIST_INIT(diagnoselist);
    return W_ERR_OK;
}

w_diagnose_s *wind_diagnose_get(const char *name)
{
    return (w_diagnose_s*)wind_obj_get(name,&diagnoselist);
}

w_err_t wind_diagnose_register(w_diagnose_s *diagnose)
{
    w_diagnose_s *diag;    
    wind_notice("register diagnose:%s", diagnose->obj.name != W_NULL?diagnose->obj.name:"null");
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
    wind_notice("unregister diagnose:%s", diagnose->obj.name != W_NULL?diagnose->obj.name:"null");
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
    diag = DLIST_OBJ(dnode,w_diagnose_s,obj.objnode);
    return W_ERR_OK;
}

static w_err_t diagnose_check(w_diagnose_s *diagnose)
{
    
    WIND_ASSERT_RETURN(diagnose->obj.magic == WIND_DIAGNOSE_MAGIC,DIAG_RES_DIAG_MAGIC_ERROR);
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
    wind_notice("diagnose check start");
    foreach_node(dnode,&diagnoselist)
    {
        diagnose = (w_diagnose_s*)DLIST_OBJ(dnode,w_diagnose_s,obj.objnode);
        wind_notice("check %s",diagnose->obj.name);
        diagnose->result = diagnose_check(diagnose);
    }
    wind_printf("--------------------------------------------------------\r\n");
    wind_printf("diagnose check result:\r\n");
    foreach_node(dnode,&diagnoselist)
    {
        diagnose = (w_diagnose_s*)DLIST_OBJ(dnode,w_diagnose_s,obj.objnode);
        
        if(diagnose->result != DIAG_RES_OK)
            err = W_ERR_FAIL;
        wind_printf("%-12s status %-6s errorcode %d\r\n",diagnose->obj.name,
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
        diagnose = (w_diagnose_s *)DLIST_OBJ(dnode,w_diagnose_s,obj.objnode);
        wind_printf("%-12s ",diagnose->obj.name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}

#endif

