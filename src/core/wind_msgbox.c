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
#if WIND_MESSAGE_SUPPORT
extern void wind_thread_dispatch(void);
WIND_MPOOL(msgboxpool,WIND_MBOX_MAX_NUM,sizeof(msgbox_s));

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

w_err_t wind_msgbox_init(void)
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


//�������䣬ֻ�����߳��д������������ж��к��߳�����֮ǰ
msgbox_s *wind_msgbox_create(const char *name,thread_s *owner)
{
    msgbox_s *pmsgbox;
    wind_notice("create msgbox:%s",name);
    pmsgbox = msgbox_malloc();
    WIND_ASSERT_RETURN(pmsgbox != NULL,NULL);

    pmsgbox->name = name;
    DNODE_INIT(pmsgbox->msgboxnode);
    DLIST_INIT(pmsgbox->msglist);
    pmsgbox->num = 0;
    pmsgbox->magic = WIND_MSGBOX_MAGIC;
    pmsgbox->owner = owner;
    wind_close_interrupt();
    dlist_insert_tail(&g_core.msgboxlist,&pmsgbox->msgboxnode);
    wind_open_interrupt();
    return pmsgbox;
}

w_err_t wind_msgbox_destroy(msgbox_s *pmsgbox)
{
    dnode_s *dnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(pmsgbox != NULL,ERR_NULL_POINTER);
    wind_notice("destroy msgbox:%s",pmsgbox->name);
    pthread = pmsgbox->owner;
    if((pmsgbox->owner->runstat == THREAD_STATUS_SLEEP) 
       && (pmsgbox->owner->cause == CAUSE_MSG))
    {
        pthread->runstat = THREAD_STATUS_READY;
    }

    wind_close_interrupt();
    dlist_remove_tail(&g_core.msgboxlist);
    pmsgbox->magic = 0;
    pmsgbox->owner = NULL;
    pmsgbox->name = NULL;
    dnode = dlist_head(&pmsgbox->msglist);
    if(dnode != NULL)
    {
        wind_warn("msgbox:%s is NOT empty while destroying it.",pmsgbox->name);
    }
    msgbox_free(pmsgbox);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_msgbox_post(msgbox_s *msgbox,msg_s *pmsg)
{
    thread_s *pthread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_COMMAN);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_COMMAN);
    //����Ϣ��������
    wind_close_interrupt();
    dlist_insert_tail(&msgbox->msglist,&pmsg->msgnode);
    msgbox->num ++;

    //����������߳�
    pthread = msgbox->owner;
    if((pthread->runstat != THREAD_STATUS_SLEEP) 
       || (pthread->cause != CAUSE_MSG))
    {
        wind_open_interrupt();
        return ERR_OK;
    }
    msgbox->owner->runstat = THREAD_STATUS_READY;
    wind_open_interrupt();
    wind_thread_dispatch();//�л��߳�
    return ERR_OK;
}


w_err_t wind_msgbox_fetch(msgbox_s *msgbox,msg_s **pmsg,w_uint32_t timeout)
{
    w_err_t err;
    w_uint32_t ticks;
    dnode_s *dnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(msgbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(msgbox->magic == WIND_MSGBOX_MAGIC,ERR_COMMAN);
    WIND_ASSERT_RETURN(msgbox->owner,ERR_COMMAN);
    
    //�������������Ϣ����ֱ�ӷ�����Ϣ
    wind_close_interrupt();
    if(msgbox->num > 0)
    {
        msgbox->num --;
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        wind_open_interrupt();
        return ERR_OK;
    }

    //�����̼߳���˯�߶���
    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;
    pthread = msgbox->owner;
    pthread->runstat = THREAD_STATUS_SLEEP;
    pthread->cause = CAUSE_MSG;
    dlist_insert_tail(&g_core.sleeplist,&pthread->sleepthr.node);
    wind_open_interrupt();
    
    wind_thread_dispatch();
    if(pthread->cause == CAUSE_MSG)
    {
        if(msgbox->num <= 0)
        {
            pthread->runstat = THREAD_STATUS_READY;
            return ERR_NULL_POINTER;
        }
        dnode = dlist_remove_head(&msgbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        wind_open_interrupt();
        err = ERR_OK;
    }
    else if(pthread->cause == CAUSE_SLEEP)
    {
        err = ERR_TIMEOUT;
    }
    else
        err = ERR_COMMAN;
    
    return err;
}


#endif  //WIND_MESSAGE_SUPPORT

