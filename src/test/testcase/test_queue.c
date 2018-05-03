/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_queue.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2018/01/09 16:29:55
** ��        ��: 
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2017/10/22 16:29:55
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: ��Ԫ���������Ĳ���ģ��
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************ͷ�ļ�����***********************************************/
#include "wind_cut.h"
#include "wind_queue.h"
#include "wind_string.h"
#include "wind_queue.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/********************************************�ڲ���������**********************************************/

w_uint8_t queuebuf[128];


/********************************************�ڲ���������*********************************************/



/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/


CASE_SETUP(queueinfo)
{

}

CASE_TEARDOWN(queueinfo)
{

}

CASE_FUNC(queueinfo)
{
    w_err_t err;
    queue_s *q = (queue_s *)queuebuf;
    err = wind_queue_create(queuebuf,sizeof(queuebuf),sizeof(w_int32_t));
    EXPECT_NE(err,ERR_OK);
    EXPECT_EQ(q->magic,WIND_QUEUE_MAGIC);
    EXPECT_EQ(q->rd,q->buf);
    EXPECT_EQ(q->wr,q->buf);
    EXPECT_EQ(q->itemsize,sizeof(w_int32_t));
    EXPECT_EQ(q->count,0);
    EXPECT_EQ(q->capacity,(sizeof(queuebuf) - (w_uint32_t)(((queue_s *)0)->buf)) / q->itemsize);
    EXPECT_EQ(q->end,q->buf + q->capacity * q->itemsize);
    err = wind_queue_destory(q);
    EXPECT_EQ(ERR_OK,err);

}

CASE_SETUP(queuefunc)
{

}

CASE_TEARDOWN(queuefunc)
{

}

CASE_FUNC(queuefunc)
{
    w_err_t err;
    w_int32_t i;
    w_int32_t res;
    w_int32_t va = 100;
    queue_s *queue;
    err = wind_queue_create(queuebuf,sizeof(queuebuf),sizeof(w_int32_t));
    EXPECT_NE(err,ERR_OK);
    for(i = 0;i < 50;i ++)
    {
        va = 100 + i;
        res = wind_queue_write(queue,&va,sizeof(w_int32_t));
        EXPECT_EQ(res,sizeof(w_int32_t));
        
    }
    
    for(i = 0;i < 50;i ++)
    {
        res = wind_queue_read(queue,&va,sizeof(w_int32_t));
        EXPECT_EQ(res,sizeof(w_int32_t));
        EXPECT_EQ(va,100 + i);
    }
    err = wind_queue_destory(queue);
    EXPECT_EQ(ERR_OK,err);

}





SUITE_SETUP(test_queue)
{

}

SUITE_TEARDOWN(test_queue)
{

}


TEST_CASES_START(test_queue)
TEST_CASE(queueinfo)
TEST_CASE(queuefunc)
TEST_CASES_END
TEST_SUITE(test_queue)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
