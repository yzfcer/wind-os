/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_hostfs.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: hostfs主机文件系统单元测试
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
#include "hostfs.h"
#include "wind_memblk.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_HOSTFS_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


//=================================================================
w_hostfs_s g_hfs;

static w_err_t hfs_init(void)
{
    w_err_t err = W_ERR_OK;
    if(g_hfs.magic == HOSTFS_MAGIC)
    {
        err = hostfs_deinit(&g_hfs);
        EXPECT_EQ(err,W_ERR_OK);
    }

    wind_memset(&g_hfs,0,sizeof(g_hfs));
    err = hostfs_init(&g_hfs,"D:/hostfs");
    return err;
}

static w_err_t hfs_deinit(void)
{
    w_err_t err = W_ERR_OK;
    if(g_hfs.magic == HOSTFS_MAGIC)
    {
        err = hostfs_deinit(&g_hfs);
        EXPECT_EQ(err,W_ERR_OK);
        g_hfs.magic = 0;
    }
    return err;
}

CASE_SETUP(create)
{
    w_err_t err;
    err = hfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}


CASE_TEARDOWN(create)
{
    w_err_t err;
    err = hfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_FUNC(create)
{
    w_err_t err;
    w_hostfile_s *file;
    file = hostfile_open(&g_hfs,"/test.txt",HFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = hostfile_remove(&g_hfs,"/test.txt");
    EXPECT_EQ(err,W_ERR_OK);
    file = hostfile_open(&g_hfs,"/test.txt",HFMODE_R);
    EXPECT_EQ(file,W_NULL);
    
    file = hostfile_open(&g_hfs,"/test1.txt",HFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = hostfile_open(&g_hfs,"/test2.txt",HFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);

    file = hostfile_open(&g_hfs,"/test1.txt",HFMODE_R);
    EXPECT_NE(file,W_NULL);
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);

    file = hostfile_open(&g_hfs,"/test2.txt",HFMODE_R);
    EXPECT_NE(file,W_NULL);
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    
    err = hostfile_remove(&g_hfs,"/test1.txt");
    EXPECT_EQ(err,W_ERR_OK);
    
    err = hostfile_remove(&g_hfs,"/test2.txt");
    EXPECT_EQ(err,W_ERR_OK);
}

CASE_SETUP(readdir)
{
    w_err_t err;
    err = hfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}


CASE_TEARDOWN(readdir)
{
    w_err_t err;
    err = hostfile_remove(&g_hfs,"/readdir_test/");
    EXPECT_EQ(err,W_ERR_OK);

    err = hfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_FUNC(readdir)
{
    w_err_t err;
    w_hostfile_s *file;
    w_hostfile_s *sub = (w_hostfile_s *)W_NULL;

    file = hostfile_open(&g_hfs,"/readdir_test/",HFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = hostfile_open(&g_hfs,"/readdir_test/test1",HFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = hostfile_open(&g_hfs,"/readdir_test/test2",HFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = hostfile_open(&g_hfs,"/readdir_test/testdir/",HFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = hostfile_close(file);
    EXPECT_NE(file,W_NULL);

	file = hostfile_open(&g_hfs,"/readdir_test/",HFMODE_R);
    EXPECT_NE(file,W_NULL);
    while(1)
    {
        err = hostfile_readdir(file,&sub);
        if(err != W_ERR_OK)
            break;
        if((wind_strcmp(sub->name,".") != 0) &&
            (wind_strcmp(sub->name,"..") != 0))
            break;
    }
    
    EXPECT_EQ(err,W_ERR_OK);
    err = hostfile_readdir(file,&sub);
    EXPECT_EQ(err,W_ERR_OK);
    err = hostfile_readdir(file,&sub);
    EXPECT_EQ(err,W_ERR_OK);
    err = hostfile_readdir(file,&sub);
    EXPECT_NE(err,W_ERR_OK);
 
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = hostfile_remove(&g_hfs,"/readdir_test/");
    EXPECT_EQ(err,W_ERR_OK);
	file = hostfile_open(&g_hfs,"/readdir_test/",HFMODE_R);
    EXPECT_EQ(file,W_NULL);

}


CASE_SETUP(readwrite)
{
    w_err_t err;
    err = hfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_TEARDOWN(readwrite)
{
    w_err_t err;
    err = hfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

static w_uint8_t buff[32];
CASE_FUNC(readwrite)
{
    w_int32_t len;
    w_err_t err;
    w_hostfile_s *file;
    char *str = "this is a file test string.";

	file = hostfile_open(&g_hfs,"/test.txt",HFMODE_CRT | HFMODE_W);
    EXPECT_NE(file,W_NULL);
    len = hostfile_write(file,(w_uint8_t*)str,wind_strlen(str));
    EXPECT_EQ(len,wind_strlen(str));
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);

    wind_memset(buff,0,32);
    file = hostfile_open(&g_hfs,"/test.txt",HFMODE_R);
    EXPECT_NE(file,W_NULL);
    len = hostfile_read(file,buff,32);
    EXPECT_EQ(len,wind_strlen(str));
    len = wind_strcmp(str,(char*)buff);
    EXPECT_EQ(len,0);
    err = hostfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = hostfile_remove(&g_hfs,"/test.txt");
    EXPECT_EQ(err,W_ERR_OK);
    
}


CASE_SETUP(format)
{
    w_err_t err;
    err = hfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_TEARDOWN(format)
{
    w_err_t err;
    err = hfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_FUNC(format)
{

}

SUITE_SETUP(hostfs)
{
    w_err_t err;
    err = hfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

SUITE_TEARDOWN(hostfs)
{
    w_err_t err;
    err = hfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}



TEST_CASES_START(hostfs)
TEST_CASE(format)
TEST_CASE(create)
TEST_CASE(readdir)
TEST_CASE(readwrite)
TEST_CASES_END
TEST_SUITE(hostfs)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
