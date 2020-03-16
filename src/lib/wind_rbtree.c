/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: RBTree.h / RBTree.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/2/6 21:16:45
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2015/2/6 21:16:45
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 红黑树的通用实现方法
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************header file***********************************************/
#include "wind_rbtree.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#ifndef W_NULL 
#define W_NULL (void*)0
#endif

/********************************************internal variables**********************************************/
static w_rbt_node_s __nil;
static w_rbt_node_s *nil = W_NULL; //为了避免讨论结点的边界情况，定义一个nil结点代替所有的NULL 

#define rbt_set_black(node) (node)->color=TREE_BLACK
#define rbt_set_red(node) (node)->color=TREE_RED
#define rbt_is_black(node) ((node)->color==TREE_BLACK)
#define rbt_is_red(node) ((node)->color==TREE_RED)
#define rbt_left_rotate(root,node) left_rotate(root,node)
#define rbt_right_rotate(root,node) right_rotate(root,node)
#define rbt_copy_color(n1,n2) (n1)->color = (n2)->color
#define RBT_SUCCESS 0
#define RBT_FAIL -1


/********************************************internal functions**********************************************/

w_rbt_node_s* rbt_parent(w_rbt_node_s *z) //返回某结点的父母 
{ 
    return z->parent; 
} 
static w_rbt_node_s* rbt_left(w_rbt_node_s *z) //返回左子树 
{ 
    return z->left; 
} 
static w_rbt_node_s *rbt_right(w_rbt_node_s *z) //返回右子树 
{ 
    return z->right; 
} 
static void left_rotate(w_rbt_node_s **root, w_rbt_node_s *x) //左旋转：结点x原来的右子树y旋转成为x的父母 
{ 
    w_rbt_node_s *y;

    if(root == W_NULL || x == W_NULL) 
        return;
    if( x-> right == nil ) 
        return;
    y = rbt_right(x); 
    x->right=y->left; 
    if(y->left != nil) 
    { 
        y->left->parent=x; 
    } 
    y->parent=x->parent; 
    if( x->parent == nil ) 
    { 
        (*root)=y; 
    } 
    else 
    { 
        if( x == rbt_left(rbt_parent(x)) ) 
        { 
            x->parent->left=y; 
        } 
        else 
        { 
            x->parent->right=y; 
        } 
    } 
    y->left=x; 
    x->parent=y; 
} 

static void right_rotate(w_rbt_node_s **root, w_rbt_node_s *x) //右旋转：结点x原来的左子树y旋转成为x的父母 
{ 
    w_rbt_node_s *y;

    if(root == W_NULL || x == W_NULL) 
    { 
        return;
    } 
    if( x->left == nil ) 
        return;

    y=rbt_left(x); 
    x->left=y->right; 
    if( y->right != nil ) 
    { 
        y->right->parent=x; 
    } 
    y->parent=x->parent; 
    if( x->parent == nil ) 
    { 
        (*root)=y; 
    } 
    else 
    { 
        if(x == rbt_left(rbt_parent(x)) ) 
        { 
            x->parent->left=y; 
        } 
        else 
        { 
            x->parent->right=y; 
        } 
    } 
    y->right=x; 
    x->parent=y; 
}

static void insert_fixup(w_rbt_node_s **root, w_rbt_node_s *z) //插入结点后, 要维持红黑树四条性质的不变性 
{ 
    w_rbt_node_s *y; 
    if(root == W_NULL || z == W_NULL) 
        return;
    while( rbt_parent(z)->color == TREE_RED ) //因为插入的结点是红色的，所以只可能违背性质3,即假如父结点也是红色的，要做调整 
    { 
        if( rbt_parent(rbt_parent(z))->left == rbt_parent(z) ) //如果要插入的结点z是其父结点的左子树 
        { 
            y=rbt_parent(rbt_parent(z))->right; // y设置为z的叔父结点 
            if( y->color == TREE_RED ) //case 1: 如果y的颜色为红色，那么将y与z的父亲同时着为黑色，然后把z的 
            { //祖父变为红色，这样子z的祖父结点可能违背性质3,将z上移成z的祖父结点 
                y->color=TREE_BLACK; 
                z->parent->color=TREE_BLACK; 
                z->parent->parent->color=TREE_RED; 
                z=z->parent->parent; 
            } 
            else 
            { 
                if( z == z->parent->right ) //case 2: 如果y的颜色为黑色，并且z是z的父母的右结点，则z左旋转，并且将z变为原来z的parent. 
                { 
                    z=z->parent; 
                    left_rotate(root, z); 
                } 
                z->parent->color=TREE_BLACK; //case 3: 如果y的颜色为黑色，并且z是z的父母的左结点，那么将z的 
                z->parent->parent->color=TREE_RED; //父亲的颜色变为黑，将z的祖父的颜色变为红，然后旋转z的祖父 
                right_rotate(root,z->parent->parent); 
            } 
        } 
        else //与前一种情况对称，要插入的结点z是其父结点的右子树,注释略去 
        { 
            y=rbt_parent(rbt_parent(z))->left; 
            if( y->color == TREE_RED) 
            { 
                z->parent->color=TREE_BLACK; 
                y->color=TREE_BLACK; 
                z->parent->parent->color=TREE_RED; 
                z=z->parent->parent; 
            } 
            else 
            { 
                if( z == z->parent->left ) 
                { 
                    z=z->parent; 
                    right_rotate(root,z); 
                } 
                z->parent->color=TREE_BLACK; 
                z->parent->parent->color=TREE_RED; 
                left_rotate(root,z->parent->parent); 
            } 
        } 
    } 
    (*root)->color=TREE_BLACK; //最后如果上升为root的根的话，把root的颜色设置为黑色 
} 

w_rbt_node_s* successor(w_rbt_s *tree, w_rbt_node_s *x) //寻找结点x的中序后继 
{ 
    w_rbt_node_s *q; 
    w_rbt_node_s *p; 
    w_rbt_node_s *y; 
    if( x->right != nil ) //如果x的右子树不为空，那么为右子树中最左边的结点 
    { 
        q=nil; 
        p=x->right; 
        while( p->left != nil ) 
        { 
            q=p; 
            p=p->left; 
        } 
        return q; 
    } 
    else //如果x的右子树为空，那么x的后继为x的所有祖先中为左子树的祖先 
    { 
        y=x->parent; 
        while( y != nil && x == y->right ) 
        { 
            x=y; 
            y=y->parent; 
        } 
        return y; 
    } 
} 

w_int32_t rbt_delete_fixup(w_rbt_node_s **root, w_rbt_node_s *node)
{ 
    w_rbt_node_s *parent = W_NULL, *brother = W_NULL;  

    while(rbt_is_black(node) && ((*root) != node))  
    {           /* Set parent and brother */     
        parent = node->parent;              
        if(node == parent->left)      
        {         
            brother = parent->right; /* Case 1: 兄弟结点为红色:  以parent为支点, 左旋处理 */          
            if(rbt_is_red(brother))          
            {               
                rbt_set_red(parent);           
                rbt_set_black(brother);           
                rbt_left_rotate(root, parent);  /* 参照结点node不变, 兄弟结点改为parent->right */                
                brother = parent->right;  /* 注意: 此时处理还没有结束，还需要做后续的调整处理 */           
            }            /* Case 2: 兄弟结点为黑色(默认), 且兄弟结点的2个子结点都为黑色 */        
            if(rbt_is_black(brother->left) && rbt_is_black(brother->right))          
            {                
                rbt_set_red(brother);              
                node = parent;           
            }           
            else            
            {                
                /* Case 3: 兄弟结点为黑色(默认),兄弟节点的左子结点为红色, 右子结点为黑色:  以brother为支点, 右旋处理 */               
                if(rbt_is_black(brother->right))              
                {                   
                    rbt_set_black(brother->left);                   
                    rbt_set_red(brother);                   
                    rbt_right_rotate(root, brother);                    

                    /* 参照结点node不变 */                    
                    brother = parent->right;                
                }                               
                /* Case 4: 兄弟结点为黑色(默认), 兄弟结点右孩子结点为红色:  以parent为支点, 左旋处理 */               
                rbt_copy_color(brother, parent);               
                rbt_set_black(brother->right);              
                rbt_set_black(parent);                
                rbt_left_rotate(root, parent);                              
                node = (*root);            
            }        
        }      
        else      
        {
            brother = parent->left;           
            /* Case 1: 兄弟结点为红色:  以parent为支点, 右旋处理 */         
            if(rbt_is_red(brother))        
            {              
                rbt_set_red(parent);         
                rbt_set_black(brother);        
                rbt_right_rotate(root, parent);                
                /* 参照结点node不变 */        
                brother = parent->left;                               
                /* 注意: 此时处理还没有结束，还需要做后续的调整处理 */            
            }            
            /* Case 2: 兄弟结点为黑色(默认), 且兄弟结点的2个子结点都为黑色 */      
            if(rbt_is_black(brother->left) && rbt_is_black(brother->right))         
            {              
                rbt_set_red(brother);             
                node = parent;           
            }          
            else           
            {                
                /* Case 3: 兄弟结点为黑色(默认),兄弟节点的右子结点为红色, 左子结点为黑色:  以brother为支点, 左旋处理 */             
                if(rbt_is_black(brother->left))         
                {                 
                    rbt_set_red(brother);                
                    rbt_set_black(brother->right);               
                    rbt_left_rotate(root, brother);                    

                    /* 参照结点node不变 */             
                    brother = parent->left;              
                }                            

                /* Case 4: 兄弟结点为黑色(默认), 兄弟结点左孩子结点为红色: 以parent为支点, 右旋处理 */               
                rbt_copy_color(brother, parent);            
                rbt_set_black(brother->left);             
                rbt_set_black(parent);              
                rbt_right_rotate(root, parent);                     
                node = (*root);        
            }      
        }   
    }    
    rbt_set_black(node);  
    return RBT_SUCCESS;
}


w_int32_t _rb_delete(w_rbt_node_s **root, w_rbt_node_s *dnode)
{   
    w_rbt_node_s *next = W_NULL, *refer = W_NULL;    /* 查找dnode的后继结点next */    

    if((nil == dnode->left) || (nil == dnode->right))   
    {   
        next = dnode;   
    }   
    else    
    {      
        next = dnode->right;      
        while(nil != next->left)  
        {          
            next = next->left;    
        }   
    }    /* 设置替代后继结点的结点refer(参考结点) */   
    if(nil != next->left)   
    {      
        refer = next->left;   
    }    
    else    
    {      
        refer = next->right;  
    }    
    refer->parent = next->parent;   
    if(nil == next->parent)   
    {      
        *root = refer;
    }  
    else   
    {      
        if(next == next->parent->left)     
        {         
            next->parent->left = refer;  
        }     
        else      
        {         
            next->parent->right = refer;    
        }   
    }   
    if(next != dnode)   
    {     
        dnode->key = next->key;        /* Copy next's satellite data into dnode */   
    }    
    if(rbt_is_red(next)) /* Not black */    
    {      
        //free(next);      
        return RBT_SUCCESS;   
    }   
    //free(next);   
    return rbt_delete_fixup(root, refer); /* 修复红黑树性质 */
}

/*
* 查找特定的节点，需要外部加锁
*/
static void __rbt_middle_tranverse(w_rbt_node_s **root,rbt_access_fn access,void *arg)
{ 

    if(root == W_NULL)
        return;
    if( (*root) != W_NULL && (*root) != nil ) 
    { 
        __rbt_middle_tranverse(&(*root)->left,access,arg); 
        if(access != (void*)0)
            access(*root,arg);
        __rbt_middle_tranverse(&(*root)->right,access,arg); 
    } 
} 

/*
* 查找特定的节点，需要外部加锁
*/
w_rbt_node_s* __rbt_search(w_rbt_node_s **root,w_rbt_node_s *z,rbt_access_fn access,void *arg)
{ 
    //w_rbt_node_s **root = &tree->root;

    if(root == W_NULL)
        return W_NULL;
    if( (*root) != nil ) 
    { 
        if( z->key < (*root)->key ) 
        { 
            return __rbt_search(&(*root)->left,z,access,arg); 
        } 
        else if ( z->key > (*root)->key ) 
        { 
            return __rbt_search(&(*root)->right,z,access,arg); 
        } 
        else 
        { 
            if(access)
                access(*root,arg);
            return (*root); 
        } 
    } 
    return W_NULL;
} 
/********************************************global variables**********************************************/



/********************************************global functions**********************************************/







w_int32_t rbt_insert(w_rbt_s *tree, w_rbt_node_s *z) //插入结点 
{ 
    w_rbt_node_s *x; //用x保存当前顶点的父母结点，用p保存当前的结点 
    w_rbt_node_s *p; 
    w_rbt_node_s **root = &tree->root;

    if(root == W_NULL || z == W_NULL) 
    { 
        return -1;
    } 

    RB_TREE_LOCK(tree->treeLock);
    if((*root) == W_NULL)
    {
        *root = z;
        nil = &__nil;
        nil->color=TREE_BLACK; //nil的颜色设置为黑 
        (*root)->left=nil; 
        (*root)->right=nil; 
        (*root)->parent=nil; 
        (*root)->color=TREE_BLACK; //为了满足性质2,根的颜色设置为黑色 
    }
    else //如果此树已经不为空，那么从根开始，从上往下查找插入点 
    { 
        x=*root; //用x保存当前顶点的父母结点，用p保存当前的结点 
        p=nil; 
        while(x != nil) //如果z->key小于当前结点的value值，则从左边下去，否则从右边下去 
        { 
            p=x; 
            if(z->key < x->key ) 
            { 
                x=x->left; 
            } 
            else if(z->key > x->key) 
            { 
                x=x->right; 
            } 
            else 
            { 
                RB_TREE_UNLOCK(tree->treeLock);
                return -1; 
            } 

        } 
        x = z;
        x->color=TREE_RED; //新插入的结点颜色设置为红色 
        x->left=nil; 
        x->right=nil; 
        x->parent=p; 
        x->key=z->key; 
        if( z->key < p->key ) 
        { 
            p->left = x; 
        } 
        else 
        { 
            p->right = x; 
        } 
        insert_fixup(root, x); //插入后对树进行调整 
    }
    RB_TREE_UNLOCK(tree->treeLock);
    return 0;
} 



w_int32_t rbt_delete(w_rbt_s *tree, w_rbt_node_s *z) //在红黑树root中删除结点z 
{    
    w_rbt_node_s *node;// = *root;
    w_rbt_node_s **root = &tree->root;
    node = *root;

    if(nil == node)  
    {     
        return RBT_SUCCESS;
    }
    RB_TREE_LOCK(tree->treeLock);
    while(nil != node)
    {     
        if(z->key == node->key)
        {           
            return _rb_delete(root, node); /* 删除结点 */       
        }       
        else if(z->key< node->key)
        {
            node = node->left;  
        }       
        else       
        {          
            node = node->right;  
        }   
    }   
    RB_TREE_UNLOCK(tree->treeLock);
    return RBT_SUCCESS;
}





w_rbt_node_s* rbt_search(w_rbt_s *tree,w_rbt_node_s *z,rbt_access_fn access,void *arg)
{
    w_rbt_node_s *node = W_NULL;
    w_rbt_node_s **root = &tree->root;

    RB_TREE_LOCK(tree->treeLock);
    node = __rbt_search(root,z,access,arg);
    RB_TREE_UNLOCK(tree->treeLock);
    return node;
}


void rbt_middle_tranverse(w_rbt_s *tree,rbt_access_fn access,void *arg)
{
    w_rbt_node_s **root = &tree->root;

    RB_TREE_LOCK(tree->treeLock);
    __rbt_middle_tranverse(root,access,arg);
    RB_TREE_UNLOCK(tree->treeLock);
}

void access(w_rbt_node_s *node,void *arg)
{
    wind_printf("%d ",node->key);
}

w_rbt_s tree;

void rbt_test(void)
{
    //w_rbt_node_s *tree = W_NULL;
    w_rbt_node_s tnode[20];
    w_int32_t i;

    for(i = 0;i < 20;i ++)
    {
        tnode[i].key = i+1;
        rbt_insert(&tree,&tnode[i]);
    }
    wind_printf("#################################--0---\r\n"); 
    rbt_middle_tranverse(&tree,access,0);

    wind_printf("#################################--1---\r\n"); 
    rbt_delete(&tree,&tnode[3]);
    rbt_middle_tranverse(&tree,access,0);

    wind_printf("#################################--2---\r\n"); 
    rbt_delete(&tree,&tnode[7]);
    rbt_middle_tranverse(&tree,access,0);

    wind_printf("#################################--3---\r\n"); 
    rbt_delete(&tree,&tnode[8]);
    rbt_middle_tranverse(&tree,access,0);

    wind_printf("#################################--4---\r\n"); 
    rbt_delete(&tree,&tnode[15]);
    rbt_middle_tranverse(&tree,access,0);    
}


#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
