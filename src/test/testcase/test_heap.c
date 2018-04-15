/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_heap.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2017/10/22 16:29:55
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
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************ͷ�ļ�����***********************************************/
#include "cut.h"
#include "wind_heap.h"
#include "wind_string.h"
/********************************************�ڲ���������**********************************************/




/********************************************�ڲ���������*********************************************/


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/


CASE_SETUP(heapinfo)
{

}

CASE_TEARDOWN(heapinfo)
{

}

CASE_FUNC(heapinfo)
{
    w_int32_t res;
    w_err_t err;
    w_uint8_t *buff;
    buff = (w_uint8_t*)wind_heap_alloc_default(64);
    EXPECT_NE(buff,NULL);
    wind_memset(buff,0,64);
    wind_strcpy((char*)buff,"heap test start.");
    res = wind_strcmp((char*)buff,"heap test start.");
    EXPECT_EQ(res,0);
    err = wind_heap_free(buff);
    EXPECT_EQ(err,ERR_OK);
}

CASE_SETUP(heapfunc)
{

}

CASE_TEARDOWN(heapfunc)
{

}

CASE_FUNC(heapfunc)
{
}

CASE_SETUP(heapmulti)
{
    
}

CASE_TEARDOWN(heapmulti)
{

}

CASE_FUNC(heapmulti)
{
}


SUITE_SETUP(test_heap)
{

}

SUITE_TEARDOWN(test_heap)
{

}


TEST_CASES_START(test_heap)
TEST_CASE(heapinfo)
TEST_CASE(heapfunc)
TEST_CASE(heapmulti)
TEST_CASES_END
TEST_SUITE(test_heap)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
