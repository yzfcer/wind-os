/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_tree.c
** 创   建   人: 周江村
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: 多叉树单元测试
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
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
        tt = DLIST_OBJ(tree,test_tree_s,tree);
        tt->layer = layer;
    }
    else
    {
        tt = DLIST_OBJ(tree,test_tree_s,tree);
        tp = DLIST_OBJ(tree->parent,test_tree_s,tree);
        tt->layer = tp->layer + 1;
    }
        
}
void print_layer(w_tree_s *tree)
{
    w_int32_t i;
    test_tree_s *tt;
    tt = DLIST_OBJ(tree,test_tree_s,tree);
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
        tt->name[4] = i+0x30; 
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


SUITE_SETUP(test_tree){}
SUITE_TEARDOWN(test_tree){}



TEST_CASES_START(test_tree)
TEST_CASE(tree_case)
TEST_CASES_END
TEST_SUITE(test_tree)


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
