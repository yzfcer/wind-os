/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_dict.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 参数字典功能测试
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2017/10/22 16:29:55
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************头文件定义***********************************************/
#include "wind_cut.h"
#include "wind_dict.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_DICT_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/********************************************内部变量定义**********************************************/






/********************************************内部函数定义*********************************************/


/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/



CASE_SETUP(func) FUNC_EMPTY
CASE_TEARDOWN(func) FUNC_EMPTY
CASE_FUNC(func)
{
    w_err_t err;
    w_dictset_s *dictset;
    w_dict_s *dict;
    dictset = wind_dictset_create("dicttest");
    EXPECT_NE(dictset,W_NULL);
    dictset = wind_dictset_get("dicttest");
    EXPECT_NE(dictset,W_NULL);
    
    
    dict = wind_dict_create("name1","value1");
    EXPECT_NE(dict,W_NULL);
    err = wind_dictset_insert(dictset,dict);
    EXPECT_EQ(err,W_ERR_OK);

    dict = wind_dict_create("name2","value2");
    EXPECT_NE(dict,W_NULL);
    err = wind_dictset_insert(dictset,dict);
    EXPECT_EQ(err,W_ERR_OK);

    dict = wind_dict_create("name3","value3");
    EXPECT_NE(dict,W_NULL);
    err = wind_dictset_insert(dictset,dict);
    EXPECT_EQ(err,W_ERR_OK);
    wind_dictset_print_all();

    dict = wind_dict_get(dictset,"name1");
    EXPECT_NE(dict,W_NULL);
    EXPECT_STR_EQ(dict->value,"value1");

    err = wind_dictset_remove(dictset,dict);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_dict_destroy(dict);
    EXPECT_EQ(err,W_ERR_OK);
    dict = wind_dict_get(dictset,"name1");
    EXPECT_EQ(dict,W_NULL);
    
    wind_dictset_print_all();
    dict = wind_dict_get(dictset,"name2");
    EXPECT_NE(dict,W_NULL);
    EXPECT_STR_EQ(dict->value,"value2");

    dict = wind_dict_get(dictset,"name3");
    EXPECT_NE(dict,W_NULL);
    EXPECT_STR_EQ(dict->value,"value3");


    err = wind_dictset_destroy(dictset);
    EXPECT_EQ(err,W_ERR_OK);
}



SUITE_SETUP(dict) FUNC_EMPTY
SUITE_TEARDOWN(dict) FUNC_EMPTY

TEST_CASES_START(dict)
TEST_CASE(func)
TEST_CASES_END
TEST_SUITE(dict)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
