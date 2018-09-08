/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_message.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os����Ϣ��صĴ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_msgbox.h"
#include "wind_core.h"
#include "wind_var.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_pool.h"

#if WIND_MSGBOX_SUPPORT
extern void _wind_thread_dispatch(void);
WIND_POOL(msgboxpool,WIND_MBOX_MAX_NUM,sizeof(msgbox_s));

static msgbox_s *msgbox_malloc(void)
{
    return wind_pool_malloc(msgboxpool);
}

static w_err_t msgbox_free(msgbox_s *msgbox)
{
    return wind_pool_free(msgboxpool,msgbox);
}


//********************************************internal functions******************************

//********************************************internal functions******************************


//**********************************************extern functions******************************

w_err_t _wind_msgbox_mod_init(void)
{
    w_err_t err;
    err = wind_pool_create("msgbox",msgboxpool,sizeof(msgboxpool),sizeof(msgbox_s));
    return err;
}

void wind_msg_init(msg_s *msg,w_uint16_t msg_id,w_uint16_t msg_len,void *msg_arg)
{
    msg->msg_id = msg_id;
    msg->msg_len = msg_len;
    msg->msg_arg = msg_arg;
    DNODE_INIT(msg->msgnode);
}

msgbox_s *wind_msgbox_get(const char *name)
{
    msgbox_s *msgbox;
    dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.msgboxlist)
    {
        msgbox = DLIST_OBJ(dnode,msgbox_s,msgboxnode);
        if(wind_strcmp(name,msgbox->name) == 0)
        {
            wind_enable_switch();
            return msgbox;
        }
    }
    wind_enable_switch();
    return NULL;
}

//�������䣬����������Ǹ��̲߳��ܴ��ж�ȡ��Ϣ
msgbox_s *wind_msgbox_create(const char *name)
{
    msgbox_s *msgbox;
    wind_notice("create msgbox:%s",name);
    msgbox = msgbox_malloc();
    WIND_ASSERT_RETURN(msgbox != NULL,NULL);

    msgbox->name = name;
    DNODE_INIT(msgbox->msgboxnode);
    DLIST_INIT(msgbox->msglist);
    msgbox->msgnum = 0;
    msgbox->magic = WIND_MSGBOX_MAGIC;
    msgbox->owner = wind_thread_current();
    wind_disable_interrupt();
    dlist_insert_tail(&g_core.msgboxlist,&msgbox->msgboxnode);
    wind_enable_interrupt();
    return msgbox;
}


w_err_t wind_msgbox_trydestroy(msgbox_s *msgbox)
{
    dnode_s *pdnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_INVALID_PARAM);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,ERR_FAIL);
    wind_disable_interrupt();
    pdnode = dlist_head(&msgbox->msglist);
    if(pdnode != NULL)
    {
        wind_enable_interrupt();
        return ERR_FAIL;
    }
    wind_enable_interrupt();
    return wind_msgbox_destroy(msgbox);
}

w_err_t wind_msgbox_destroy(msgbox_s *msgbox)
{
    dnode_s *dnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_INVALID_PARAM);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,ERR_FAIL);
    wind_notice("destroy msgbox:%s",msgbox->name);
    wind_disable_interrupt();
    dlist_remove_tail(&g_core.msgboxlist);
    wind_enable_interrupt();
    thread = msgbox->owner;
    if((msgbox->owner->runstat == THREAD_STATUS_SLEEP) 
       && (msgbox->owner->cause == CAUSE_MSG))
    {
        thread->runstat = THREAD_STATUS_READY;
    }

    msgbox->magic = 0;
    dnode = dlist_head(&msgbox->msglist);
    if(dnode != NULL)
    {
        wind_warn("msgbox:%s is NOT empty while destroying it.",msgbox->name);
    }
    msgbox_free(msgbox);
    return ERR_OK;
}

w_err_t wind_msgbox_post(msgbox_s *msgbox,msg_s *pmsg)
{
    thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_FAIL);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_FAIL);
    //����Ϣ��������
    wind_disable_interrupt();
    dlist_insert_tail(&msgbox->msglist,&pmsg->msgnode);
    msgbox->msgnum ++;

    //����������߳�
    thread = msgbox->owner;
    if((thread->runstat != THREAD_STATUS_SLEEP) 
       || (thread->cause != CAUSE_MSG))
    {
        wind_enable_interrupt();
        return ERR_OK;
    }
    msgbox->owner->runstat = THREAD_STATUS_READY;
    wind_enable_interrupt();
    _wind_thread_dispatch();//�л��߳�
    return ERR_OK;
}


w_err_t wind_msgbox_wait(msgbox_s *msgbox,msg_s **pmsg,w_uint32_t timeout)
{
    w_err_t err;
    w_uint32_t ticks;
    dnode_s *dnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_FAIL);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_FAIL);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,ERR_FAIL);
    //�������������Ϣ����ֱ�ӷ�����Ϣ
    wind_disable_interrupt();
    if(msgbox->msgnum > 0)
    {
        msgbox->msgnum --;
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        wind_enable_interrupt();
        return ERR_OK;
    }

    //�����̼߳���˯�߶���
    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;
    thread = msgbox->owner;
    thread->runstat = THREAD_STATUS_SLEEP;
    thread->cause = CAUSE_MSG;
    dlist_insert_tail(&g_core.sleeplist,&thread->sleepnode.dnode);
    wind_enable_interrupt();
    
    _wind_thread_dispatch();
    if(thread->cause == CAUSE_MSG)
    {
        if(msgbox->msgnum <= 0)
        {
            thread->runstat = THREAD_STATUS_READY;
            return ERR_NULL_POINTER;
        }
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        wind_enable_interrupt();
        err = ERR_OK;
    }
    else if(thread->cause == CAUSE_SLEEP)
    {
        err = ERR_TIMEOUT;
    }
    else
        err = ERR_FAIL;
    
    return err;
}

w_err_t wind_msgbox_trywait(msgbox_s *msgbox,msg_s **pmsg)
{
    w_err_t err;
    dnode_s *dnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_FAIL);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_FAIL);
    thread = wind_thread_current();
    WIND_ASSERT_RETURN(msgbox->owner == thread,ERR_FAIL);
    //�������������Ϣ����ֱ�ӷ�����Ϣ
    wind_disable_interrupt();
    if(msgbox->msgnum > 0)
    {
        msgbox->msgnum --;
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        err = ERR_OK;
    }
    else
    {
        *pmsg = NULL;
        err = ERR_FAIL;
    }
    wind_enable_interrupt();
    return err;
}

w_err_t wind_msgbox_print(dlist_s *list)
{
    dnode_s *dnode;
    msgbox_s *msgbox;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nmsgbox list as following:\r\n");
    wind_print_space(5);
    wind_printf("%-16s %-8s %-16s\r\n","msgbox","msg_num","owner");
    wind_print_space(5);

    foreach_node(dnode,list)
    {
        msgbox = (msgbox_s *)DLIST_OBJ(dnode,msgbox_s,msgboxnode);
        wind_printf("%-16s %-8d %-16s\r\n",
            msgbox->name,msgbox->msgnum,msgbox->owner->name);
    }
    wind_print_space(5);
    return ERR_OK;
}

#endif  //WIND_MSGBOX_SUPPORT

