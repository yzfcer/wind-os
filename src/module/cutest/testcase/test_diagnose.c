/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_diagnose.c
** Author      : Jason Zhou
** Last Date   : 2017/10/22 16:29:55
** Description : 诊断功能测试
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2017/10/22 16:29:55
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************header file***********************************************/
#include "wind_cut.h"
#include "wind_diagnose.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_DIAGNOSE_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/********************************************internal variables**********************************************/






/********************************************internal functions**********************************************/


/********************************************global variables**********************************************/



/********************************************global functions**********************************************/



CASE_SETUP(func) FUNC_EMPTY
CASE_TEARDOWN(func) FUNC_EMPTY
CASE_FUNC(func)
{
    w_err_t err;
    err = wind_diagnose_check();
    EXPECT_EQ(err,W_ERR_OK);
}



SUITE_SETUP(diagnose) FUNC_EMPTY
SUITE_TEARDOWN(diagnose) FUNC_EMPTY

TEST_CASES_START(diagnose)
TEST_CASE(func)
TEST_CASES_END
TEST_SUITE(diagnose)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
