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

#include "wind_config.h"
#include "wind_type.h"
#include "wind_os_hwif.h"
#include "wind_msgbox.h"
#include "wind_core.h"
#include "wind_stati.h"
#include "wind_var.h"
#include "wind_debug.h"
#include "wind_string.h"

#if WIND_MSGBOX_SUPPORT
extern void _wind_thread_dispatch(void);
WIND_POOL(msgboxpool,WIND_MBOX_MAX_NUM,sizeof(msgbox_s));

static msgbox_s *msgbox_malloc(void)
{
    return wind_pool_alloc(msgboxpool);
}

static w_err_t msgbox_free(msgbox_s *msgbox)
{
    return wind_pool_free(msgboxpool,msgbox);
}


//********************************************internal functions******************************

//********************************************internal functions******************************


//**********************************************extern functions******************************

w_err_t _wind_msgbox_init(void)
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

//�������䣬ֻ�����߳��д������������ж��к��߳�����֮ǰ
msgbox_s *wind_msgbox_create(const char *name,thread_s *owner)
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
    msgbox->owner = owner;
    wind_close_interrupt();
    dlist_insert_tail(&g_core.msgboxlist,&msgbox->msgboxnode);
    wind_open_interrupt();
    return msgbox;
}

w_err_t wind_msgbox_destroy(msgbox_s *msgbox)
{
    dnode_s *dnode;
    thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    wind_notice("destroy msgbox:%s",msgbox->name);
    thread = msgbox->owner;
    if((msgbox->owner->runstat == THREAD_STATUS_SLEEP) 
       && (msgbox->owner->cause == CAUSE_MSG))
    {
        thread->runstat = THREAD_STATUS_READY;
    }

    wind_close_interrupt();
    dlist_remove_tail(&g_core.msgboxlist);
    msgbox->magic = 0;
    msgbox->owner = NULL;
    msgbox->name = NULL;
    dnode = dlist_head(&msgbox->msglist);
    if(dnode != NULL)
    {
        wind_warn("msgbox:%s is NOT empty while destroying it.",msgbox->name);
    }
    msgbox_free(msgbox);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_msgbox_post(msgbox_s *msgbox,msg_s *pmsg)
{
    thread_s *thread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_COMMAN);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_COMMAN);
    //����Ϣ��������
    wind_close_interrupt();
    dlist_insert_tail(&msgbox->msglist,&pmsg->msgnode);
    msgbox->msgnum ++;

    //����������߳�
    thread = msgbox->owner;
    if((thread->runstat != THREAD_STATUS_SLEEP) 
       || (thread->cause != CAUSE_MSG))
    {
        wind_open_interrupt();
        return ERR_OK;
    }
    msgbox->owner->runstat = THREAD_STATUS_READY;
    wind_open_interrupt();
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
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_COMMAN);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_COMMAN);
    
    //�������������Ϣ����ֱ�ӷ�����Ϣ
    wind_close_interrupt();
    if(msgbox->msgnum > 0)
    {
        msgbox->msgnum --;
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        wind_open_interrupt();
        return ERR_OK;
    }

    //�����̼߳���˯�߶���
    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;
    thread = msgbox->owner;
    thread->runstat = THREAD_STATUS_SLEEP;
    thread->cause = CAUSE_MSG;
    dlist_insert_tail(&g_core.sleeplist,&thread->sleepthr.node);
    wind_open_interrupt();
    
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
        wind_open_interrupt();
        err = ERR_OK;
    }
    else if(thread->cause == CAUSE_SLEEP)
    {
        err = ERR_TIMEOUT;
    }
    else
        err = ERR_COMMAN;
    
    return err;
}

w_err_t wind_msgbox_trywait(msgbox_s *msgbox,msg_s **pmsg)
{
    w_err_t err;
    dnode_s *dnode;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_COMMAN);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_COMMAN);
    
    //�������������Ϣ����ֱ�ӷ�����Ϣ
    wind_close_interrupt();
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
        err = ERR_COMMAN;
    }
    wind_open_interrupt();
    return err;
}

w_err_t wind_msgbox_print(dlist_s *list)
{
    dnode_s *dnode;
    msgbox_s *msgbox;
    WIND_ASSERT_RETURN(list != NULL,ERR_NULL_POINTER);
    wind_printf("\r\n\r\nmsgbox list as following:\r\n");
    wind_printf("----------------------------------------------\r\n");
    wind_printf("%-16s %-8s\r\n","msgbox","msg_num");
    wind_printf("----------------------------------------------\r\n");

    foreach_node(dnode,list)
    {
        msgbox = (msgbox_s *)DLIST_OBJ(dnode,msgbox_s,msgboxnode);
        wind_printf("%-16s %-8d \r\n",
            msgbox->name,msgbox->msgnum);
    }
    wind_printf("----------------------------------------------\r\n");
    return ERR_OK;
}

#endif  //WIND_MSGBOX_SUPPORT

