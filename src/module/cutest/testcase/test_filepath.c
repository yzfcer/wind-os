/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_filepath.c
** Author      : Jason Zhou
** Last Date   : 2017/10/22 16:29:55
** Description : Filepath function test suite
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

#include "wind_cut.h"
#include "wind_string.h"
#include "wind_heap.h"
#include "wind_filepath.h"
#if  TEST_FILEPATH_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
static char *bak_path = (char *)W_NULL;














CASE_SETUP(current)
{
    char *path;
    if(bak_path != W_NULL)
    {
        wind_free(bak_path);
        bak_path = (char *)W_NULL;
    }
    path = wind_filepath_get_current();
    WIND_ASSERT_RETURN(path != W_NULL,W_ERR_FAIL);
    bak_path = wind_salloc(path,HP_ALLOCID_VFS);
    WIND_ASSERT_RETURN(bak_path != W_NULL,W_ERR_FAIL);
    return W_ERR_OK;
}

CASE_TEARDOWN(current)
{
    w_err_t err;
    WIND_ASSERT_RETURN(bak_path != W_NULL,W_ERR_FAIL);
    err = wind_filepath_set_current(bak_path);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    wind_free(bak_path);
    bak_path = (char*)W_NULL;
    return W_ERR_OK;
}

CASE_FUNC(current)
{
    char *path;
    w_err_t err;
    err = wind_filepath_set_current(W_NULL);
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_set_current((char *)W_NULL);
    EXPECT_NE(err,W_ERR_OK);
    err = wind_filepath_set_current("tet");
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

CASE_SETUP(valid) FUNC_EMPTY
CASE_TEARDOWN(valid) FUNC_EMPTY
CASE_FUNC(valid)
{
    w_err_t err;
    err = wind_filepath_check_valid((char *)W_NULL);
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

CASE_SETUP(parent) FUNC_EMPTY
CASE_TEARDOWN(parent) FUNC_EMPTY
CASE_FUNC(parent)
{
    char *newpath;
    newpath = wind_filepath_get_parent(W_NULL);
    EXPECT_EQ(newpath,W_NULL);
    newpath = wind_filepath_get_parent("");
    EXPECT_EQ(newpath,W_NULL);
    newpath = wind_filepath_get_parent("/");
    EXPECT_EQ(newpath,W_NULL);
    newpath = wind_filepath_get_parent("/test");
    EXPECT_STR_EQ(newpath,"/");
    wind_free(newpath);
    newpath = wind_filepath_get_parent("/test/");
    EXPECT_STR_EQ(newpath,"/");
    wind_free(newpath);
    newpath = wind_filepath_get_parent("/test/test1");
    EXPECT_STR_EQ(newpath,"/test/");
    wind_free(newpath);
    newpath = wind_filepath_get_parent("/test/test1/");
    EXPECT_STR_EQ(newpath,"/test/");
    wind_free(newpath);
    
}


CASE_SETUP(filename) FUNC_EMPTY
CASE_TEARDOWN(filename) FUNC_EMPTY
CASE_FUNC(filename)
{
    char *newpath;
    newpath = wind_filepath_get_filename(W_NULL);
    EXPECT_EQ(newpath,W_NULL);
    newpath = wind_filepath_get_filename("");
    EXPECT_EQ(newpath,W_NULL);
    
    newpath = wind_filepath_get_filename("/");
    EXPECT_NE(newpath,W_NULL);
    EXPECT_STR_EQ(newpath,"");
    wind_free(newpath);

    newpath = wind_filepath_get_filename("/test");
    EXPECT_STR_EQ(newpath,"test");
    wind_free(newpath);
    
    newpath = wind_filepath_get_filename("/test/");
    EXPECT_STR_EQ(newpath,"test");
    wind_free(newpath);
    
    newpath = wind_filepath_get_filename("/test/test1");
    EXPECT_STR_EQ(newpath,"test1");
    wind_free(newpath);
    
    newpath = wind_filepath_get_filename("/test/test1/");
    EXPECT_STR_EQ(newpath,"test1");
    wind_free(newpath);
    
}

CASE_SETUP(isdir) FUNC_EMPTY
CASE_TEARDOWN(isdir) FUNC_EMPTY
CASE_FUNC(isdir)
{
    w_bool_t isdir;
    isdir = wind_filepath_isdir(W_NULL);
    EXPECT_EQ(isdir,W_FALSE);
    isdir = wind_filepath_isdir("");
    EXPECT_EQ(isdir,W_FALSE);
    isdir = wind_filepath_isdir("test");
    EXPECT_EQ(isdir,W_FALSE);
    isdir = wind_filepath_isdir("/test");
    EXPECT_EQ(isdir,W_FALSE);
    isdir = wind_filepath_isdir("/");
    EXPECT_EQ(isdir,W_TRUE);
    isdir = wind_filepath_isdir("/test/");
    EXPECT_EQ(isdir,W_TRUE);
    isdir = wind_filepath_isdir("/test/test1/");
    EXPECT_EQ(isdir,W_TRUE);
}


SUITE_SETUP(filepath) FUNC_EMPTY
SUITE_TEARDOWN(filepath) FUNC_EMPTY
CASE_SETUP(split) FUNC_EMPTY
CASE_TEARDOWN(split) FUNC_EMPTY

CASE_FUNC(split)
{
    w_int32_t cnt;
    char *path;
    char *layer[5];
    cnt = wind_filepath_split(W_NULL,layer,5);
    EXPECT_SMALLER(cnt,0);
    cnt = wind_filepath_split("/test",W_NULL,5);
    EXPECT_SMALLER(cnt,0);
    cnt = wind_filepath_split("/test",layer,1);
    EXPECT_SMALLER(cnt,0);

    path = wind_salloc("/",HP_ALLOCID_VFS);
    EXPECT_NE(path,W_NULL);
    cnt = wind_filepath_split(path,layer,5);
    EXPECT_EQ(cnt,1);
    EXPECT_STR_EQ(layer[0],"");
    wind_free(path);
    
    path = wind_salloc("/test",HP_ALLOCID_VFS);
    EXPECT_NE(path,W_NULL);
    cnt = wind_filepath_split(path,layer,5);
    EXPECT_EQ(cnt,2);
    EXPECT_STR_EQ(layer[0],"");
    EXPECT_STR_EQ(layer[1],"test");
    wind_free(path);

    path = wind_salloc("/test/",HP_ALLOCID_VFS);
    EXPECT_NE(path,W_NULL);
    cnt = wind_filepath_split(path,layer,5);
    EXPECT_EQ(cnt,2);
    EXPECT_STR_EQ(layer[0],"");
    EXPECT_STR_EQ(layer[1],"test");
    wind_free(path);
    
    path = wind_salloc("/test/test1",HP_ALLOCID_VFS);
    EXPECT_NE(path,W_NULL);
    cnt = wind_filepath_split(path,layer,5);
    EXPECT_EQ(cnt,3);
    EXPECT_STR_EQ(layer[0],"");
    EXPECT_STR_EQ(layer[1],"test");
    EXPECT_STR_EQ(layer[2],"test1");
    wind_free(path);
    
    path = wind_salloc("/test/test1/",HP_ALLOCID_VFS);
    EXPECT_NE(path,W_NULL);
    cnt = wind_filepath_split(path,layer,5);
    EXPECT_EQ(cnt,3);
    EXPECT_STR_EQ(layer[0],"");
    EXPECT_STR_EQ(layer[1],"test");
    EXPECT_STR_EQ(layer[2],"test1");
    wind_free(path);

    path = wind_salloc("/test/test1/test2",HP_ALLOCID_VFS);
    EXPECT_NE(path,W_NULL);
    cnt = wind_filepath_split(path,layer,5);
    EXPECT_EQ(cnt,4);
    EXPECT_STR_EQ(layer[0],"");
    EXPECT_STR_EQ(layer[1],"test");
    EXPECT_STR_EQ(layer[2],"test1");
    EXPECT_STR_EQ(layer[3],"test2");
    wind_free(path);

    path = wind_salloc("/test/test1/test2/test3",HP_ALLOCID_VFS);
    EXPECT_NE(path,W_NULL);
    cnt = wind_filepath_split(path,layer,5);
    EXPECT_EQ(cnt,5);
    EXPECT_STR_EQ(layer[0],"");
    EXPECT_STR_EQ(layer[1],"test");
    EXPECT_STR_EQ(layer[2],"test1");
    EXPECT_STR_EQ(layer[3],"test2");
    EXPECT_STR_EQ(layer[4],"test3");
    wind_free(path);
    
}


TEST_CASES_START(filepath)
TEST_CASE(current)
TEST_CASE(valid)
TEST_CASE(parent)
TEST_CASE(filename)
TEST_CASE(isdir)
TEST_CASE(split)
TEST_CASES_END
TEST_SUITE(filepath)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
