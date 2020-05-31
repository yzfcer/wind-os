/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : wind_rbtree.c
** Author      : Jason Zhou
** Last Date   : 2015/2/6 21:16:45
** Description : The general implementation method of red black tree
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2015/2/6 21:16:45
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
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
static w_rbt_node_s *nil = W_NULL; 

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
//Return the parent of a node 
w_rbt_node_s* rbt_parent(w_rbt_node_s *rbnode)
{ 
    return rbnode->parent; 
}

//Return to left subtree
static w_rbt_node_s* rbt_left(w_rbt_node_s *rbnode)
{ 
    return rbnode->left; 
}

//Return to right subtree
static w_rbt_node_s *rbt_right(w_rbt_node_s *rbnode)
{ 
    return rbnode->right; 
}

//Left rotation: node x turns the original right subtree y and becomes the parent of X
static void left_rotate(w_rbt_node_s **root, w_rbt_node_s *rbnode)
{ 
    w_rbt_node_s *y;

    if(root == W_NULL || rbnode == W_NULL)
        return;
    if( rbnode-> right == nil )
        return;
    y = rbt_right(rbnode); 
    rbnode->right=y->left; 
    if(y->left != nil)
    { 
        y->left->parent=rbnode; 
    }
    y->parent=rbnode->parent; 
    if( rbnode->parent == nil )
    { 
        (*root)=y; 
    }
    else 
    { 
        if( rbnode == rbt_left(rbt_parent(rbnode)))
        { 
            rbnode->parent->left=y; 
        }
        else 
        { 
            rbnode->parent->right=y; 
        }
    }
    y->left=rbnode; 
    rbnode->parent=y; 
}

//Right rotation: node x turns the original left subtree y and becomes the parent of X
static void right_rotate(w_rbt_node_s **root, w_rbt_node_s *rbnode)
{ 
    w_rbt_node_s *y;

    if(root == W_NULL || rbnode == W_NULL)
    { 
        return;
    }
    if( rbnode->left == nil )
        return;

    y=rbt_left(rbnode); 
    rbnode->left=y->right; 
    if( y->right != nil )
    { 
        y->right->parent=rbnode; 
    }
    y->parent=rbnode->parent; 
    if( rbnode->parent == nil )
    { 
        (*root)=y; 
    }
    else 
    { 
        if(rbnode == rbt_left(rbt_parent(rbnode)))
        { 
            rbnode->parent->left=y; 
        }
        else 
        { 
            rbnode->parent->right=y; 
        }
    }
    y->right=rbnode; 
    rbnode->parent=y; 
}

//After inserting a node, the invariance of four properties of the red black tree should be maintained 
static void insert_fixup(w_rbt_node_s **root, w_rbt_node_s *rbnode)
{ 
    w_rbt_node_s *y; 
    if(root == W_NULL || rbnode == W_NULL)
        return;
    while( rbt_parent(rbnode)->color == TREE_RED )
    { 
        if( rbt_parent(rbt_parent(rbnode))->left == rbt_parent(rbnode))
        { 
            y=rbt_parent(rbt_parent(rbnode))->right; 
            if( y->color == TREE_RED )
            { 
                y->color=TREE_BLACK; 
                rbnode->parent->color=TREE_BLACK; 
                rbnode->parent->parent->color=TREE_RED; 
                rbnode=rbnode->parent->parent; 
            }
            else 
            { 
                if( rbnode == rbnode->parent->right )
                { 
                    rbnode=rbnode->parent; 
                    left_rotate(root, rbnode); 
                }
                rbnode->parent->color=TREE_BLACK; 
                rbnode->parent->parent->color=TREE_RED; 
                right_rotate(root,rbnode->parent->parent); 
            }
        }
        else 
        { 
            y=rbt_parent(rbt_parent(rbnode))->left; 
            if( y->color == TREE_RED)
            { 
                rbnode->parent->color=TREE_BLACK; 
                y->color=TREE_BLACK; 
                rbnode->parent->parent->color=TREE_RED; 
                rbnode=rbnode->parent->parent; 
            }
            else 
            { 
                if( rbnode == rbnode->parent->left )
                { 
                    rbnode=rbnode->parent; 
                    right_rotate(root,rbnode); 
                }
                rbnode->parent->color=TREE_BLACK; 
                rbnode->parent->parent->color=TREE_RED; 
                left_rotate(root,rbnode->parent->parent); 
            }
        }
    }
    (*root)->color=TREE_BLACK; //Set the color of root to black
}

//Search for the middle order successor of node x 
w_rbt_node_s* successor(w_rbtree_s *tree, w_rbt_node_s *rbnode)
{ 
    w_rbt_node_s *q; 
    w_rbt_node_s *p; 
    w_rbt_node_s *y; 
    if( rbnode->right != nil ) 
    { 
        q=nil; 
        p=rbnode->right; 
        while( p->left != nil )
        { 
            q=p; 
            p=p->left; 
        }
        return q; 
    }
    else  
    { 
        y=rbnode->parent; 
        while( y != nil && rbnode == y->right )
        { 
            rbnode=y; 
            y=y->parent; 
        }
        return y; 
    }
}

//After deleting the nodes, the invariance of four properties of the red black tree should be maintained
w_int32_t rbt_delete_fixup(w_rbt_node_s **root, w_rbt_node_s *rbnode)
{ 
    w_rbt_node_s *parent = W_NULL, *brother = W_NULL;  

    while(rbt_is_black(rbnode)&& ((*root)!= rbnode))
    {           /* Set parent and brother */     
        parent = rbnode->parent;        
        if(rbnode == parent->left) 
        {         
            brother = parent->right;         
            if(rbt_is_red(brother)) 
            {               
                rbt_set_red(parent);     
                rbt_set_black(brother);     
                rbt_left_rotate(root, parent);                
                brother = parent->right;        
            }      
            if(rbt_is_black(brother->left)&& rbt_is_black(brother->right)) 
            {                
                rbt_set_red(brother);        
                rbnode = parent;     
            }
            else            
            {                
                             
                if(rbt_is_black(brother->right)) 
                {                   
                    rbt_set_black(brother->left);             
                    rbt_set_red(brother);             
                    rbt_right_rotate(root, brother);              
                    brother = parent->right;          
                }
                rbt_copy_color(brother, parent);         
                rbt_set_black(brother->right);        
                rbt_set_black(parent);          
                rbt_left_rotate(root, parent);                        
                rbnode = (*root);      
            }
        }
        else      
        {
            brother = parent->left;     
            if(rbt_is_red(brother)) 
            {              
                rbt_set_red(parent);   
                rbt_set_black(brother);  
                rbt_right_rotate(root, parent);          
                brother = parent->left;                         
             }
            if(rbt_is_black(brother->left)&& rbt_is_black(brother->right))
            {              
                rbt_set_red(brother);       
                rbnode = parent;     
            }
            else           
            {                
                 if(rbt_is_black(brother->left))
                {                 
                    rbt_set_red(brother);          
                    rbt_set_black(brother->right);         
                    rbt_left_rotate(root, brother);              
                    brother = parent->left;        
                }

                rbt_copy_color(brother, parent);      
                rbt_set_black(brother->left);       
                rbt_set_black(parent);        
                rbt_right_rotate(root, parent);               
                rbnode = (*root);  
            }
        }
    }
    rbt_set_black(rbnode);  
    return RBT_SUCCESS;
}


w_int32_t _rb_delete(w_rbt_node_s **root, w_rbt_node_s *rbnode)
{   
    w_rbt_node_s *next = W_NULL, *refer = W_NULL; 

    if((nil == rbnode->left)|| (nil == rbnode->right))
    {   
        next = rbnode;
    }
    else    
    {      
        next = rbnode->right;
        while(nil != next->left)
        {          
            next = next->left;    
        }
    }
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
    if(next != rbnode)
    {     
        rbnode->key = next->key;  
    }
    if(rbt_is_red(next))
    {      
        //free(next);
        return RBT_SUCCESS;   
    }
    //free(next);   
    return rbt_delete_fixup(root, refer); 
}


static void __rbt_middle_tranverse(w_rbt_node_s **root,rbt_access_fn access,void *arg)
{ 

    if(root == W_NULL)
        return;
    if( (*root)!= W_NULL && (*root)!= nil )
    { 
        __rbt_middle_tranverse(&(*root)->left,access,arg); 
        if(access != (void*)0)
            access(*root,arg);
        __rbt_middle_tranverse(&(*root)->right,access,arg); 
    }
}

//To find a specific node, external locking is required
w_rbt_node_s* __rbt_search(w_rbt_node_s **root,w_rbt_node_s *rbnode,rbt_access_fn access,void *arg)
{ 
    //w_rbt_node_s **root = &tree->root;
    if(root == W_NULL)
        return W_NULL;
    if( (*root)!= nil )
    { 
        if( rbnode->key < (*root)->key )
        { 
            return __rbt_search(&(*root)->left,rbnode,access,arg); 
        }
        else if ( rbnode->key > (*root)->key )
        { 
            return __rbt_search(&(*root)->right,rbnode,access,arg); 
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







w_int32_t wind_rbtree_insert(w_rbtree_s *tree, w_rbt_node_s *rbnode)//插入结点 
{ 
    w_rbt_node_s *x; 
    w_rbt_node_s *p; 
    w_rbt_node_s **root = &tree->root;

    if(root == W_NULL || rbnode == W_NULL)
    { 
        return -1;
    }

    RB_TREE_LOCK(tree->lock);
    if((*root)== W_NULL)
    {
        *root = rbnode;
        nil = &__nil;
        nil->color=TREE_BLACK; 
        (*root)->left=nil; 
        (*root)->right=nil; 
        (*root)->parent=nil; 
        (*root)->color=TREE_BLACK; 
    }
    else 
    { 
        x=*root; 
        p=nil; 
        while(x != nil)
        { 
            p=x; 
            if(rbnode->key < x->key )
            { 
                x=x->left; 
            }
            else if(rbnode->key > x->key)
            { 
                x=x->right; 
            }
            else 
            { 
                RB_TREE_UNLOCK(tree->lock);
                return -1; 
            }

        }
        x = rbnode;
        x->color=TREE_RED; 
        x->left=nil; 
        x->right=nil; 
        x->parent=p; 
        x->key=rbnode->key; 
        if( rbnode->key < p->key )
        { 
            p->left = x; 
        }
        else 
        { 
            p->right = x; 
        }
        insert_fixup(root, x); 
    }
    RB_TREE_UNLOCK(tree->lock);
    return 0;
}



w_int32_t rbt_delete(w_rbtree_s *tree, w_rbt_node_s *rbnode)
{    
    w_rbt_node_s *node;
    w_rbt_node_s **root = &tree->root;
    node = *root;

    if(nil == node)
    {     
        return RBT_SUCCESS;
    }
    RB_TREE_LOCK(tree->lock);
    while(nil != node)
    {     
        if(rbnode->key == node->key)
        {           
            return _rb_delete(root, node); 
        }
        else if(rbnode->key< node->key)
        {
            node = node->left;  
        }
        else       
        {          
            node = node->right;  
        }
    }
    RB_TREE_UNLOCK(tree->lock);
    return RBT_SUCCESS;
}





w_rbt_node_s* wind_rbtree_search(w_rbtree_s *tree,w_rbt_node_s *rbnode,rbt_access_fn access,void *arg)
{
    w_rbt_node_s *node = W_NULL;
    w_rbt_node_s **root = &tree->root;

    RB_TREE_LOCK(tree->lock);
    node = __rbt_search(root,rbnode,access,arg);
    RB_TREE_UNLOCK(tree->lock);
    return node;
}


void wind_rbtree_middle_tranverse(w_rbtree_s *tree,rbt_access_fn access,void *arg)
{
    w_rbt_node_s **root = &tree->root;

    RB_TREE_LOCK(tree->lock);
    __rbt_middle_tranverse(root,access,arg);
    RB_TREE_UNLOCK(tree->lock);
}

void access(w_rbt_node_s *node,void *arg)
{
    wind_printf("%d ",node->key);
}

#if 0
w_rbtree_s tree;
void rbt_test(void)
{
    //w_rbt_node_s *tree = W_NULL;
    w_rbt_node_s tnode[20];
    w_int32_t i;

    for(i = 0;i < 20;i ++)
    {
        tnode[i].key = i+1;
        wind_rbtree_insert(&tree,&tnode[i]);
    }
    wind_printf("#################################--0---\r\n"); 
    wind_rbtree_middle_tranverse(&tree,access,0);

    wind_printf("#################################--1---\r\n"); 
    rbt_delete(&tree,&tnode[3]);
    wind_rbtree_middle_tranverse(&tree,access,0);

    wind_printf("#################################--2---\r\n"); 
    rbt_delete(&tree,&tnode[7]);
    wind_rbtree_middle_tranverse(&tree,access,0);

    wind_printf("#################################--3---\r\n"); 
    rbt_delete(&tree,&tnode[8]);
    wind_rbtree_middle_tranverse(&tree,access,0);

    wind_printf("#################################--4---\r\n"); 
    rbt_delete(&tree,&tnode[15]);
    wind_rbtree_middle_tranverse(&tree,access,0);    
}
#endif

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
