/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_softirq.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2017/10/22 16:29:55
** ��        ��: ���жϵ�Ԫ����
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
/*********************************************header file***********************************************/
#include "wind_cut.h"
#include "wind_softirq.h"
#include "wind_core.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_SOFTIRQ_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/********************************************internal variables**********************************************/
//softirq_s *softirqs[4];





/********************************************internal functions**********************************************/
static w_uint32_t test_v = 0;
static void softirq_test1(void)
{
    test_v += 20;
}
static void softirq_test2(void)
{
    test_v += 10;
}

/********************************************global variables**********************************************/



/********************************************global functions**********************************************/



CASE_SETUP(func) FUNC_EMPTY
CASE_TEARDOWN(func) FUNC_EMPTY
CASE_FUNC(func)
{
    wind_softirq_reg(WIND_SOFTINT_MAX_NUM-1,softirq_test1);
    wind_softirq_reg(WIND_SOFTINT_MAX_NUM-2,softirq_test2);
    test_v = 20;
    wind_softirq_trig(WIND_SOFTINT_MAX_NUM-1);
    EXPECT_EQ(test_v,40);
    wind_softirq_trig(WIND_SOFTINT_MAX_NUM-2);
    EXPECT_EQ(test_v,50);
    wind_softirq_unreg(WIND_SOFTINT_MAX_NUM-1);
    wind_softirq_trig(WIND_SOFTINT_MAX_NUM-1);
    EXPECT_EQ(test_v,50);
    wind_softirq_trig(WIND_SOFTINT_MAX_NUM-2);
    EXPECT_EQ(test_v,60);
    wind_softirq_unreg(WIND_SOFTINT_MAX_NUM-2);
    wind_softirq_trig(WIND_SOFTINT_MAX_NUM-2);
    EXPECT_EQ(test_v,60);
}



SUITE_SETUP(softirq) FUNC_EMPTY
SUITE_TEARDOWN(softirq) FUNC_EMPTY

TEST_CASES_START(softirq)
TEST_CASE(func)
TEST_CASES_END
TEST_SUITE(softirq)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
