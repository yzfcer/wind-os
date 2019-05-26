/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_filepath.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 文件路径函数单元测试
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
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_filepath.h"
#if (WIND_CUTEST_SUPPORT && TEST_FILEPATH_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
static char *bak_path = W_NULL;

/********************************************内部变量定义**********************************************/



/********************************************内部函数定义*********************************************/


/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

CASE_SETUP(current_path)
{
    char *path;
    w_int32_t len;
    if(bak_path != W_NULL)
        wind_free(bak_path);
    path = wind_filepath_get_current();
    WIND_ASSERT_RETURN_VOID(path != W_NULL);
    len = wind_strlen(path);
    bak_path = (char*)wind_malloc(len+1);
    wind_strcpy(bak_path,path);
}

CASE_TEARDOWN(current_path)
{
    w_err_t err;
    WIND_ASSERT_RETURN_VOID(bak_path != W_NULL);
    err = wind_filepath_get_parent(bak_path);
    WIND_ASSERT_RETURN_VOID(err == W_ERR_OK);
    wind_free(bak_path);
    bak_path = (char*)W_NULL;
}

CASE_FUNC(current_path)
{
    char *path;
    w_err_t err;
    err = wind_filepath_set_current("tet");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_set_current(W_NULL);
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_set_current("");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_set_current("/mnt/");
    EXPECT_EQ(err,W_ERR_OK);
    path = wind_filepath_get_current();
    EXPECT_NE(path,W_NULL);
    EXPECT_STR_EQ(path,"/mnt/");
    
    err = wind_filepath_set_current("/usr");
    EXPECT_EQ(err,W_ERR_OK);
    path = wind_filepath_get_current();
    EXPECT_NE(path,W_NULL);
    EXPECT_STR_EQ(path,"/usr/");   
    
}

CASE_SETUP(path_valid)
{
}

CASE_TEARDOWN(path_valid)
{
}

CASE_FUNC(path_valid)
{
    w_err_t err;
    err = wind_filepath_check_valid(W_NULL);
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_check_valid("");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_check_valid("/");
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_filepath_check_valid("/mnt");
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_filepath_check_valid("/*");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_check_valid("/!");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_check_valid("/~");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_check_valid("/@");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_check_valid("/#");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_check_valid("/$");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_check_valid("/%%");
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_check_valid("/^");
    EXPECT_NE(err,W_ERR_OK);

}

SUITE_SETUP(filepath)
{

}

SUITE_TEARDOWN(filepath)
{

}


TEST_CASES_START(filepath)
TEST_CASE(current_path)
TEST_CASE(path_valid)
TEST_CASES_END
TEST_SUITE(filepath)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
