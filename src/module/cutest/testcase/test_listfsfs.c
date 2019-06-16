/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_listfs.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: listfs文件系统单元测试
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
#include "wind_cut.h"
#include "wind_string.h"
#if (WIND_CUTEST_SUPPORT && TEST_LISTFS_SUPPORT)
#include "listfs.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

listfs_s g_lfs;
CASE_SETUP(create)
{

}


CASE_TEARDOWN(create)
{

}

CASE_FUNC(create)
{
    w_err_t err;
    listfile_s *file;
    file = listfile_open(&g_lfs,"/test.txt",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = listfile_remove(&g_lfs,"/test.txt");
    EXPECT_EQ(err,W_ERR_OK);
    file = listfile_open(&g_lfs,"/test.txt",LFMODE_R);
    EXPECT_EQ(file,W_NULL);

    file = listfile_open(&g_lfs,"/test1.txt",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = listfile_open(&g_lfs,"/test2.txt",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    err = listfile_remove(&g_lfs,"/test1.txt");
    EXPECT_EQ(err,W_ERR_OK);
    err = listfile_remove(&g_lfs,"/test2.txt");
    EXPECT_EQ(err,W_ERR_OK);
}


CASE_SETUP(readwrite)
{
}

CASE_TEARDOWN(readwrite)
{
    
}

static w_uint8_t buff[32];
CASE_FUNC(readwrite)
{
    w_int32_t len;
    w_err_t err;
    listfile_s *file;
    char *str = "this is a file test string.";
    file = listfile_open(&g_lfs,"/test.txt",LFMODE_CRT | LFMODE_W);
    EXPECT_NE(file,W_NULL);
    len = listfile_write(file,(w_uint8_t*)str,wind_strlen(str));
    EXPECT_EQ(len,wind_strlen(str));
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);

    wind_memset(buff,0,32);
    file = listfile_open(&g_lfs,"/test.txt",LFMODE_R);
    EXPECT_NE(file,W_NULL);
    len = listfile_read(file,buff,32);
    EXPECT_EQ(len,wind_strlen(str));
    len = wind_strcmp(str,(char*)buff);
    EXPECT_EQ(len,0);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = listfile_remove(&g_lfs,"/test.txt");
    EXPECT_EQ(err,W_ERR_OK);
    
}


CASE_SETUP(format)
{
}

CASE_TEARDOWN(format)
{
    
}

CASE_FUNC(format)
{
    w_err_t err;
    w_blkdev_s *blkdev;
    blkdev = wind_blkdev_get("memblk");
    if(blkdev == W_NULL)
    {
        wind_memset(&g_lfs,0,sizeof(listfs_s));
        return;
    }
        
    wind_blkdev_open(blkdev);
    err = listfs_init(&g_lfs,blkdev);
    if(err != W_ERR_OK)
    {
        err = listfs_format(&g_lfs,blkdev);
        EXPECT_EQ(err,W_ERR_OK);
    }
}

SUITE_SETUP(listfs)
{
    w_err_t err;
    w_blkdev_s *blkdev;
    blkdev = wind_blkdev_get("memblk");
    if(blkdev == W_NULL)
    {
        wind_error("get blkdev failed");
        wind_memset(&g_lfs,0,sizeof(listfs_s));
        return;
    }
        
    wind_blkdev_open(blkdev);
    err = listfs_init(&g_lfs,blkdev);
    if(err != W_ERR_OK)
    {
        err = listfs_format(&g_lfs,blkdev);
        EXPECT_EQ(err,W_ERR_OK);
    }

    //wind_blkdev_close(blkdev);
}

SUITE_TEARDOWN(listfs)
{

}



TEST_CASES_START(listfs)
TEST_CASE(format)
TEST_CASE(create)
//TEST_CASE(readwrite)
TEST_CASES_END
TEST_SUITE(listfs)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
