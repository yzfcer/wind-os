/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_heap.c
** Author      : Jason Zhou
** Last Date   : 2017/10/22 16:29:55
** Description : Heap interface unit test suite
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
#include "wind_heap.h"
#include "wind_string.h"

#if (WIND_MODULE_CUTEST_SUPPORT && TEST_HEAP_SUPPORT)
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

static char *buff[12];
CASE_SETUP(multi) FUNC_EMPTY
CASE_TEARDOWN(multi) FUNC_EMPTY
CASE_FUNC(multi)
{
    w_int32_t i;
    w_int32_t res;
    w_err_t err;
    w_int32_t len = 16;
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
    }

}


SUITE_SETUP(heap) FUNC_EMPTY
SUITE_TEARDOWN(heap) FUNC_EMPTY

TEST_CASES_START(heap)
TEST_CASE(func)
TEST_CASE(multi)
TEST_CASES_END
TEST_SUITE(heap)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
