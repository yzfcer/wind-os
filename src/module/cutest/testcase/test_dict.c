/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_dict.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2017/10/22 16:29:55
** ��        ��: �����ֵ书�ܲ���
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
#include "wind_dict.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_DICT_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/********************************************�ڲ���������**********************************************/






/********************************************�ڲ���������*********************************************/


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/



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
