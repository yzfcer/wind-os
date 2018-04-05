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
WIND_MPOOL(mboxpool,WIND_MBOX_MAX_NUM,sizeof(mbox_s));

static mbox_s *mbox_malloc(void)
{
    return wind_pool_alloc(mboxpool);
}

static w_err_t mbox_free(mbox_s *mbox)
{
    return wind_pool_free(mboxpool,mbox);
}


//********************************************internal functions******************************

//********************************************internal functions******************************


//**********************************************extern functions******************************

w_err_t wind_mbox_init(void)
{
    w_err_t err;
    err = wind_pool_create("mbox",mboxpool,sizeof(mboxpool),sizeof(mbox_s));
    return err;
}

//�������䣬ֻ�����߳��д������������ж��к��߳�����֮ǰ
mbox_s *wind_mbox_create(const char *name,thread_s *owner)
{
    mbox_s *pmbox;
    pmbox = mbox_malloc();
    WIND_ASSERT_RETURN(pmbox != NULL,NULL);

    pmbox->name = name;
    DNODE_INIT(pmbox->mboxnode);
    DLIST_INIT(pmbox->msglist);
    pmbox->num = 0;
    pmbox->valid = B_TRUE;
    pmbox->owner = owner;
    wind_close_interrupt();
    dlist_insert_tail(&g_core.mboxlist,&pmbox->mboxnode);
    wind_open_interrupt();
    return pmbox;
}

w_err_t wind_mbox_destroy(mbox_s *pmbox)
{
    w_err_t err;
    dnode_s *dnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(pmbox != NULL,ERR_NULL_POINTER);
    pthread = pmbox->owner;
    if((pmbox->owner->runstat == THREAD_STATUS_SLEEP) 
       && (pmbox->owner->cause == CAUSE_MSG))
    {
        pthread->runstat = THREAD_STATUS_READY;
    }

    wind_close_interrupt();
    dlist_remove_tail(&g_core.mboxlist);
    pmbox->valid = B_FALSE;
    pmbox->owner = NULL;
    pmbox->name = NULL;
    dnode = dlist_head(&pmbox->msglist);
    if(dnode != NULL)
    {
        wind_warn("msgbox:%s is NOT empty while destroying it.",pmbox->name);
    }
    mbox_free(pmbox);
    wind_open_interrupt();
    return ERR_OK;
}

w_err_t wind_mbox_post(mbox_s *mbox,msg_s *pmsg)
{
    thread_s *pthread;
    WIND_ASSERT_RETURN(mbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(mbox->valid,ERR_COMMAN);
    WIND_ASSERT_RETURN(mbox->owner,ERR_COMMAN);
    //����Ϣ��������
    wind_close_interrupt();
    dlist_insert_tail(&mbox->msglist,&pmsg->msgnode);
    mbox->num ++;

    //����������߳�
    pthread = mbox->owner;
    if((pthread->runstat != THREAD_STATUS_SLEEP) 
       || (pthread->cause != CAUSE_MSG))
    {
        wind_open_interrupt();
        return ERR_OK;
    }
    mbox->owner->runstat = THREAD_STATUS_READY;
    wind_open_interrupt();
    wind_thread_dispatch();//�л��߳�
    return ERR_OK;
}


w_err_t wind_mbox_fetch(mbox_s *mbox,msg_s **pmsg,w_uint32_t timeout)
{
    w_err_t err;
    w_uint32_t ticks;
    dnode_s *dnode;
    thread_s *pthread;
    WIND_ASSERT_RETURN(mbox != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(pmsg != NULL,ERR_NULL_POINTER);
    WIND_ASSERT_RETURN(mbox->valid,ERR_COMMAN);
    WIND_ASSERT_RETURN(mbox->owner,ERR_COMMAN);
    
    //�������������Ϣ����ֱ�ӷ�����Ϣ
    wind_close_interrupt();
    if(mbox->num > 0)
    {
        dnode = dlist_remove_head(&mbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        wind_open_interrupt();
        return ERR_OK;
    }

    //�����̼߳���˯�߶���
    ticks = timeout *WIND_TICK_PER_SEC / 1000;
    if(ticks == 0)
        ticks = 1;
    pthread = mbox->owner;
    pthread->runstat = THREAD_STATUS_SLEEP;
    pthread->cause = CAUSE_MSG;
    dlist_insert_tail(&g_core.sleeplist,&pthread->sleepthr.node);
    wind_open_interrupt();
    
    wind_thread_dispatch();
    if(pthread->cause == CAUSE_MSG)
    {
        if(mbox->num <= 0)
        {
            pthread->runstat = THREAD_STATUS_READY;
            return ERR_NULL_POINTER;
        }
        dnode = dlist_remove_head(&mbox->msglist);
        *pmsg = DLIST_OBJ(dnode,msg_s,msgnode);
        wind_open_interrupt();
        err = ERR_OK;
    }
    else if(pthread->cause == CAUSE_SLEEP)
    {
        err = ERR_OK;
    }
    
    return err;
}


#endif  //WIND_MESSAGE_SUPPORT

