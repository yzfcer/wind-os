/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: RBTree.h / RBTree.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2015/2/6 21:16:45
** ��        ��: 
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2015/2/6 21:16:45
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: �������ͨ��ʵ�ַ���
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
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
static w_rbt_node_s *nil = W_NULL; //Ϊ�˱������۽��ı߽����������һ��nil���������е�NULL 

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

w_rbt_node_s* rbt_parent(w_rbt_node_s *z) //����ĳ���ĸ�ĸ 
{ 
    return z->parent; 
} 
static w_rbt_node_s* rbt_left(w_rbt_node_s *z) //���������� 
{ 
    return z->left; 
} 
static w_rbt_node_s *rbt_right(w_rbt_node_s *z) //���������� 
{ 
    return z->right; 
} 
static void left_rotate(w_rbt_node_s **root, w_rbt_node_s *x) //����ת�����xԭ����������y��ת��Ϊx�ĸ�ĸ 
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

static void right_rotate(w_rbt_node_s **root, w_rbt_node_s *x) //����ת�����xԭ����������y��ת��Ϊx�ĸ�ĸ 
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

static void insert_fixup(w_rbt_node_s **root, w_rbt_node_s *z) //�������, Ҫά�ֺ�����������ʵĲ����� 
{ 
    w_rbt_node_s *y; 
    if(root == W_NULL || z == W_NULL) 
        return;
    while( rbt_parent(z)->color == TREE_RED ) //��Ϊ����Ľ���Ǻ�ɫ�ģ�����ֻ����Υ������3,�����縸���Ҳ�Ǻ�ɫ�ģ�Ҫ������ 
    { 
        if( rbt_parent(rbt_parent(z))->left == rbt_parent(z) ) //���Ҫ����Ľ��z���丸���������� 
        { 
            y=rbt_parent(rbt_parent(z))->right; // y����Ϊz���常��� 
            if( y->color == TREE_RED ) //case 1: ���y����ɫΪ��ɫ����ô��y��z�ĸ���ͬʱ��Ϊ��ɫ��Ȼ���z�� 
            { //�游��Ϊ��ɫ��������z���游������Υ������3,��z���Ƴ�z���游��� 
                y->color=TREE_BLACK; 
                z->parent->color=TREE_BLACK; 
                z->parent->parent->color=TREE_RED; 
                z=z->parent->parent; 
            } 
            else 
            { 
                if( z == z->parent->right ) //case 2: ���y����ɫΪ��ɫ������z��z�ĸ�ĸ���ҽ�㣬��z����ת�����ҽ�z��Ϊԭ��z��parent. 
                { 
                    z=z->parent; 
                    left_rotate(root, z); 
                } 
                z->parent->color=TREE_BLACK; //case 3: ���y����ɫΪ��ɫ������z��z�ĸ�ĸ�����㣬��ô��z�� 
                z->parent->parent->color=TREE_RED; //���׵���ɫ��Ϊ�ڣ���z���游����ɫ��Ϊ�죬Ȼ����תz���游 
                right_rotate(root,z->parent->parent); 
            } 
        } 
        else //��ǰһ������Գƣ�Ҫ����Ľ��z���丸����������,ע����ȥ 
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
    (*root)->color=TREE_BLACK; //����������Ϊroot�ĸ��Ļ�����root����ɫ����Ϊ��ɫ 
} 

w_rbt_node_s* successor(w_rbt_s *tree, w_rbt_node_s *x) //Ѱ�ҽ��x�������� 
{ 
    w_rbt_node_s *q; 
    w_rbt_node_s *p; 
    w_rbt_node_s *y; 
    if( x->right != nil ) //���x����������Ϊ�գ���ôΪ������������ߵĽ�� 
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
    else //���x��������Ϊ�գ���ôx�ĺ��Ϊx������������Ϊ������������ 
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
            brother = parent->right; /* Case 1: �ֵܽ��Ϊ��ɫ:  ��parentΪ֧��, �������� */          
            if(rbt_is_red(brother))          
            {               
                rbt_set_red(parent);           
                rbt_set_black(brother);           
                rbt_left_rotate(root, parent);  /* ���ս��node����, �ֵܽ���Ϊparent->right */                
                brother = parent->right;  /* ע��: ��ʱ����û�н���������Ҫ�������ĵ������� */           
            }            /* Case 2: �ֵܽ��Ϊ��ɫ(Ĭ��), ���ֵܽ���2���ӽ�㶼Ϊ��ɫ */        
            if(rbt_is_black(brother->left) && rbt_is_black(brother->right))          
            {                
                rbt_set_red(brother);              
                node = parent;           
            }           
            else            
            {                
                /* Case 3: �ֵܽ��Ϊ��ɫ(Ĭ��),�ֵܽڵ�����ӽ��Ϊ��ɫ, ���ӽ��Ϊ��ɫ:  ��brotherΪ֧��, �������� */               
                if(rbt_is_black(brother->right))              
                {                   
                    rbt_set_black(brother->left);                   
                    rbt_set_red(brother);                   
                    rbt_right_rotate(root, brother);                    

                    /* ���ս��node���� */                    
                    brother = parent->right;                
                }                               
                /* Case 4: �ֵܽ��Ϊ��ɫ(Ĭ��), �ֵܽ���Һ��ӽ��Ϊ��ɫ:  ��parentΪ֧��, �������� */               
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
            /* Case 1: �ֵܽ��Ϊ��ɫ:  ��parentΪ֧��, �������� */         
            if(rbt_is_red(brother))        
            {              
                rbt_set_red(parent);         
                rbt_set_black(brother);        
                rbt_right_rotate(root, parent);                
                /* ���ս��node���� */        
                brother = parent->left;                               
                /* ע��: ��ʱ����û�н���������Ҫ�������ĵ������� */            
            }            
            /* Case 2: �ֵܽ��Ϊ��ɫ(Ĭ��), ���ֵܽ���2���ӽ�㶼Ϊ��ɫ */      
            if(rbt_is_black(brother->left) && rbt_is_black(brother->right))         
            {              
                rbt_set_red(brother);             
                node = parent;           
            }          
            else           
            {                
                /* Case 3: �ֵܽ��Ϊ��ɫ(Ĭ��),�ֵܽڵ�����ӽ��Ϊ��ɫ, ���ӽ��Ϊ��ɫ:  ��brotherΪ֧��, �������� */             
                if(rbt_is_black(brother->left))         
                {                 
                    rbt_set_red(brother);                
                    rbt_set_black(brother->right);               
                    rbt_left_rotate(root, brother);                    

                    /* ���ս��node���� */             
                    brother = parent->left;              
                }                            

                /* Case 4: �ֵܽ��Ϊ��ɫ(Ĭ��), �ֵܽ�����ӽ��Ϊ��ɫ: ��parentΪ֧��, �������� */               
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
    w_rbt_node_s *next = W_NULL, *refer = W_NULL;    /* ����dnode�ĺ�̽��next */    

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
    }    /* ���������̽��Ľ��refer(�ο����) */   
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
    return rbt_delete_fixup(root, refer); /* �޸���������� */
}

/*
* �����ض��Ľڵ㣬��Ҫ�ⲿ����
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
* �����ض��Ľڵ㣬��Ҫ�ⲿ����
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







w_int32_t rbt_insert(w_rbt_s *tree, w_rbt_node_s *z) //������ 
{ 
    w_rbt_node_s *x; //��x���浱ǰ����ĸ�ĸ��㣬��p���浱ǰ�Ľ�� 
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
        nil->color=TREE_BLACK; //nil����ɫ����Ϊ�� 
        (*root)->left=nil; 
        (*root)->right=nil; 
        (*root)->parent=nil; 
        (*root)->color=TREE_BLACK; //Ϊ����������2,������ɫ����Ϊ��ɫ 
    }
    else //��������Ѿ���Ϊ�գ���ô�Ӹ���ʼ���������²��Ҳ���� 
    { 
        x=*root; //��x���浱ǰ����ĸ�ĸ��㣬��p���浱ǰ�Ľ�� 
        p=nil; 
        while(x != nil) //���z->keyС�ڵ�ǰ����valueֵ����������ȥ��������ұ���ȥ 
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
        x->color=TREE_RED; //�²���Ľ����ɫ����Ϊ��ɫ 
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
        insert_fixup(root, x); //�����������е��� 
    }
    RB_TREE_UNLOCK(tree->treeLock);
    return 0;
} 



w_int32_t rbt_delete(w_rbt_s *tree, w_rbt_node_s *z) //�ں����root��ɾ�����z 
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
            return _rb_delete(root, node); /* ɾ����� */       
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
