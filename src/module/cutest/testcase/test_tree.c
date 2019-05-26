/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: test_tree.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2017/10/22 16:29:55
** ��        ��: �������Ԫ����
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
#include "wind_cut.h"
#include "wind_tree.h"
#include "wind_pool.h"
#include "wind_string.h"
#if (WIND_CUTEST_SUPPORT && TEST_TREE_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define TREENUM 10
typedef struct 
{
    char name[12];
    w_int32_t layer;
    w_tree_s tree;
}test_tree_s;
#define TREE_TO_TEST_TREE(node) (test_tree_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((test_tree_s*)0)->tree)))

WIND_POOL(test_tree,TREENUM,sizeof(test_tree_s));
test_tree_s *trees[TREENUM+1];




CASE_SETUP(tree_case)
{
    int i;
    wind_pool_create("test_tree",test_tree,
                     sizeof(test_tree),sizeof(test_tree_s));
    for(i = 0;i < TREENUM+1;i ++)
        trees[i] = 0;
}


CASE_TEARDOWN(tree_case)
{
    int i;
    wind_pool_destroy(test_tree);
    for(i = 0;i < TREENUM+1;i ++)
        trees[i] = 0;
}

void set_layer(w_tree_s *tree)
{
    w_int32_t layer = 0;
    test_tree_s *tt,*tp;
    if(tree->parent == W_NULL)
    {
        tt = TREE_TO_TEST_TREE(tree);
        tt->layer = layer;
    }
    else
    {
        tt = TREE_TO_TEST_TREE(tree);
        tp = TREE_TO_TEST_TREE(tree->parent);
        tt->layer = tp->layer + 1;
    }
        
}
void print_layer(w_tree_s *tree)
{
    w_int32_t i;
    test_tree_s *tt;
    tt = TREE_TO_TEST_TREE(tree);
    for(i = 0;i< tt->layer;i ++)
        test_printf("   ");
    test_printf("|---%s:%d\r\n",tt->name,tt->layer);
}



CASE_FUNC(tree_case)
{
    w_int32_t i;
    test_tree_s *tt;
    for(i = 0;i < TREENUM;i ++)
    {
        trees[i] = wind_pool_malloc(test_tree);
        tt = trees[i];
        tt->layer = 0;
        wind_tree_init(&tt->tree);
        wind_memset(tt->name,0,12);
        wind_strcpy(tt->name,"tree");
        tt->name[4] = (char)(i+0x30); 
    }
    
    wind_tree_insert_child(&trees[0]->tree,&trees[1]->tree);
    wind_tree_insert_child(&trees[0]->tree,&trees[2]->tree);
    wind_tree_insert_child(&trees[0]->tree,&trees[3]->tree);
    wind_tree_insert_child(&trees[1]->tree,&trees[4]->tree);
    wind_tree_insert_brother(&trees[2]->tree,&trees[5]->tree);
    wind_tree_insert_brother(&trees[3]->tree,&trees[6]->tree);
    wind_tree_insert_brother(&trees[3]->tree,&trees[7]->tree);
    wind_tree_insert_brother(&trees[4]->tree,&trees[8]->tree);
    wind_tree_insert_child(&trees[8]->tree,&trees[9]->tree);
    wind_tree_visit(&trees[0]->tree,set_layer);
    wind_tree_visit(&trees[0]->tree,print_layer);
}


SUITE_SETUP(tree){}
SUITE_TEARDOWN(tree){}



TEST_CASES_START(tree)
TEST_CASE(tree_case)
TEST_CASES_END
TEST_SUITE(tree)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
