/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_treefs.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2017/10/22 16:29:55
** ��        ��: treefs�ļ�ϵͳ��Ԫ����
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
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cut.h"
#include "wind_string.h"
#include "treefs.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if (WIND_CONSOLE_SUPPORT && WIND_FS_SUPPORT)

CASE_SETUP(treefs_create)
{

}


CASE_TEARDOWN(treefs_create)
{

}

CASE_FUNC(treefs_create)
{
    w_err_t err;
    treefile_s *file;
    file = treefile_open("/test.txt",FMODE_CRT);
    EXPECT_NE(file,NULL);
    err = treefile_close(file);
    EXPECT_EQ(err,ERR_OK);
    err = treefile_rm(file);
    EXPECT_EQ(err,ERR_OK);
    file = treefile_open("/test1.txt",FMODE_CRT);
    EXPECT_NE(file,NULL);
    err = treefile_close(file);
    EXPECT_EQ(err,ERR_OK);
    err = treefile_rm(file);
    EXPECT_EQ(err,ERR_OK);
    file = treefile_open("/test2.txt",FMODE_CRT);
    EXPECT_NE(file,NULL);
    err = treefile_close(file);
    EXPECT_EQ(err,ERR_OK);
    err = treefile_rm(file);
    EXPECT_EQ(err,ERR_OK);
}


CASE_SETUP(treefs_readwrite)
{
}

CASE_TEARDOWN(treefs_readwrite)
{
    
}

static w_uint8_t buff[32];
CASE_FUNC(treefs_readwrite)
{
    w_int32_t len;
    w_err_t err;
    treefile_s *file;
    char *str = "this is a file test string.";
    file = treefile_open("/test.txt",FMODE_CRT | FMODE_W);
    EXPECT_NE(file,NULL);
    len = treefile_write(file,(w_uint8_t*)str,wind_strlen(str));
    EXPECT_EQ(len,wind_strlen(str));
    err = treefile_close(file);
    EXPECT_EQ(err,ERR_OK);

    wind_memset(buff,0,32);
    file = treefile_open("/test.txt",FMODE_R);
    EXPECT_NE(file,NULL);
    len = treefile_read(file,buff,32);
    EXPECT_EQ(len,wind_strlen(str));
    len = wind_strcmp(str,(char*)buff);
    EXPECT_EQ(len,0);
    err = treefile_close(file);
    EXPECT_EQ(err,ERR_OK);
    err = treefile_rm(file);
    EXPECT_EQ(err,ERR_OK);
    
}

SUITE_SETUP(test_treefs)
{
    //treefile_s *file;
    treefs_format();
    //file = treefile_open("/",FMODE_CRT);
    //treefile_close(file);
}

SUITE_TEARDOWN(test_treefs)
{

}



TEST_CASES_START(test_treefs)
TEST_CASE(treefs_create)
TEST_CASE(treefs_readwrite)
TEST_CASES_END
TEST_SUITE(test_treefs)

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
