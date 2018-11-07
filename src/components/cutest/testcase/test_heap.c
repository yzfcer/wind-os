/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_heap.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2017/10/22 16:29:55
** ��        ��: �ѵ�Ԫ����
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2017/10/22 16:29:55
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************ͷ�ļ�����***********************************************/
#include "wind_cut.h"
#include "wind_heap.h"
#include "wind_string.h"
#include "wind_var.h"

#if (WIND_CUTEST_SUPPORT && TEST_HEAP_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/********************************************�ڲ���������**********************************************/




/********************************************�ڲ���������*********************************************/


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/

CASE_SETUP(heapfunc)
{

}

CASE_TEARDOWN(heapfunc)
{

}

CASE_FUNC(heapfunc)
{
    w_int32_t res;
    w_err_t err;
    w_uint8_t *buff;
    buff = (w_uint8_t*)wind_malloc(64);
    EXPECT_NE(buff,W_NULL);
    wind_memset(buff,0,64);
    wind_strcpy((char*)buff,"heap test start.");
    res = wind_strcmp((char*)buff,"heap test start.");
    EXPECT_EQ(res,0);
    err = wind_free(buff);
    EXPECT_EQ(err,W_ERR_OK);
}

CASE_SETUP(heapmulti)
{
    
}

CASE_TEARDOWN(heapmulti)
{

}

static char *buff[12];
CASE_FUNC(heapmulti)
{
    w_int32_t i;
    w_int32_t res;
    w_err_t err;
    w_int32_t len = 8;
    for(i = 0;i < 12;i ++)
    {
        len += 8;
        buff[i] = (char*)wind_malloc(len);
        EXPECT_NE(buff[i],W_NULL);
        wind_memset(buff[i],0,len);
        wind_strcpy((char*)buff[i],"heap test start.");
        res = wind_strcmp((char*)buff[i],"heap test start.");
        EXPECT_EQ(res,0);
    }
    for(i = 0;i < 12;i ++)
    {
        err = wind_free(buff[i]);
        EXPECT_EQ(err,W_ERR_OK);
        wind_heapitem_print();
    }

}


SUITE_SETUP(test_heap)
{

}

SUITE_TEARDOWN(test_heap)
{

}


TEST_CASES_START(test_heap)
TEST_CASE(heapfunc)
TEST_CASE(heapmulti)
TEST_CASES_END
TEST_SUITE(test_heap)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
