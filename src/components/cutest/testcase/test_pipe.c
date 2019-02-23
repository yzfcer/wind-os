/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_pipe.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2018/01/09 16:29:55
** ��        ��: �ܵ���Ԫ����
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
/*********************************************ͷ�ļ�����***********************************************/
#include "wind_cut.h"
#include "wind_pipe.h"
#include "wind_string.h"
#include "wind_queue.h"

#if (WIND_CUTEST_SUPPORT && TEST_PIPE_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/********************************************�ڲ���������**********************************************/

w_uint8_t pipebuf[128];
w_pipe_s test_pp;


/********************************************�ڲ���������*********************************************/



/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
CASE_SETUP(pipeinit)
{

}

CASE_TEARDOWN(pipeinit)
{

}

CASE_FUNC(pipeinit)
{
    w_err_t err;
    w_pipe_s *pipe;
    err = wind_pipe_init(&test_pp,"test",pipebuf,sizeof(pipebuf));
    EXPECT_EQ(err,W_ERR_OK);
    pipe = wind_pipe_get("test");
    EXPECT_NE(pipe,W_NULL);
    EXPECT_EQ(pipe->magic,WIND_PIPE_MAGIC);
    EXPECT_STR_EQ(pipe->name,"test");
    EXPECT_FALSE(IS_F_PIPE_POOL(pipe));
    EXPECT_EQ(pipe->buff,pipebuf);
    EXPECT_EQ(pipe->buflen,sizeof(pipebuf));
    err = wind_pipe_destroy(pipe);
    EXPECT_EQ(W_ERR_OK,err);

    err = wind_pipe_init(&test_pp,W_NULL,pipebuf,sizeof(pipebuf));
    EXPECT_EQ(err,W_ERR_OK);
    pipe = wind_pipe_get(W_NULL);
    EXPECT_EQ(pipe,W_NULL);
    err = wind_pipe_destroy(&test_pp);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(test_pp.magic,0);
}

CASE_SETUP(pipeinfo)
{

}

CASE_TEARDOWN(pipeinfo)
{

}

CASE_FUNC(pipeinfo)
{
    w_err_t err;
    w_pipe_s *pipe;
    pipe = wind_pipe_create("test",pipebuf,sizeof(pipebuf));
    EXPECT_NE(pipe,W_NULL);
    EXPECT_EQ(pipe->magic,WIND_PIPE_MAGIC);
    EXPECT_STR_EQ(pipe->name,"test");
    EXPECT_TRUE(IS_F_PIPE_POOL(pipe));
    EXPECT_EQ(pipe->buff,pipebuf);
    EXPECT_EQ(pipe->buflen,sizeof(pipebuf));
    err = wind_pipe_destroy(pipe);
    EXPECT_EQ(W_ERR_OK,err);
}




CASE_SETUP(pipefunc)
{

}

CASE_TEARDOWN(pipefunc)
{

}

CASE_FUNC(pipefunc)
{
    w_err_t err;
    w_int32_t i;
    w_int32_t res;
    w_pipe_s *pipe;
    w_int8_t buf[14];
    pipe = wind_pipe_create("test",pipebuf,sizeof(pipebuf));
    EXPECT_NE(pipe,W_NULL);
    for(i = 0;i < 10;i ++)
    {
        wind_memset(buf,0,14);
        res = wind_pipe_write(pipe,"test123456789",13);
        EXPECT_EQ(res,13);
        res = wind_pipe_read(pipe,buf,14);
        EXPECT_EQ(res,13);
        res = wind_memcmp(buf,"test123456789",13);
        EXPECT_EQ(res,0);
    }
    err = wind_pipe_destroy(pipe);
    EXPECT_EQ(W_ERR_OK,err);

}





SUITE_SETUP(test_pipe)
{

}

SUITE_TEARDOWN(test_pipe)
{

}


TEST_CASES_START(test_pipe)
TEST_CASE(pipeinit)
TEST_CASE(pipeinfo)
TEST_CASE(pipefunc)
TEST_CASES_END
TEST_SUITE(test_pipe)

#ifdef __cplusplus
}
#endif
#endif 
