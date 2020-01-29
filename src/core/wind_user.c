/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_user.c
**创   建   人: Jason Zhou
**最后修改日期: 2019.01.28
**描        述: 系统用户管理模块
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.01.28
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.01.28
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_user.h"
#include "wind_debug.h"
#include "wind_core.h"
#include "wind_string.h"
#include "wind_pool.h"
#include "wind_macro.h"
//#include "wind_mutex.h"
//#include "wind_board_port.h"

#if WIND_USER_SUPPORT
#define NODE_TO_USER(node) (w_user_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_user_s*)0)->usernode)))
static w_dlist_s userlist;
WIND_POOL(userpool,WIND_USER_MAX_NUM,sizeof(w_user_s));

static __INLINE__ w_user_s *user_malloc(void)
{
    return (w_user_s*)wind_pool_malloc(userpool);
}

static __INLINE__ w_err_t user_free(void *user)
{
    return wind_pool_free(userpool,user);
}

static w_err_t check_user_name_format(const char *username)
{
    w_int32_t i;
    w_int32_t namelen;
    WIND_ASSERT_RETURN(username != W_NULL,W_ERR_PTR_NULL);
    namelen = wind_strlen(username);
    WIND_ASSERT_RETURN(namelen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(namelen < USER_NAME_MAXLEN,W_ERR_INVALID);

    WIND_ASSERT_RETURN(IS_ALPHABET(username[0]),W_ERR_INVALID);
    for(i = 1;i < USER_NAME_MAXLEN;i ++)
    {
        if(username[i] == 0)
            return W_ERR_OK;
        WIND_ASSERT_RETURN(IS_ALPHABET(username[i])||IS_DECIMAL(username[i]),W_ERR_INVALID);
    }
    return W_ERR_FAIL;
}

static w_err_t check_passwd_format(const char *passwd)
{
    w_int32_t i;
    w_int32_t pwdlen;
    WIND_ASSERT_RETURN(passwd != W_NULL,W_ERR_PTR_NULL);
    pwdlen = wind_strlen(passwd);
    WIND_ASSERT_RETURN(pwdlen >= 6,W_ERR_INVALID);
    WIND_ASSERT_RETURN(pwdlen < PASSWD_MAXLEN,W_ERR_INVALID);
    
    for(i = 1;i < USER_NAME_MAXLEN;i ++)
    {
        if(passwd[i] == 0)
            return W_ERR_OK;
        WIND_ASSERT_RETURN(passwd[i] >= 0x20,W_ERR_INVALID);
        WIND_ASSERT_RETURN(passwd[i] < 0x7f,W_ERR_INVALID);
    }
    return W_ERR_FAIL;
}


w_err_t _wind_user_mod_init(void)
{
    w_err_t err;
    w_user_s *user;
    DLIST_INIT(userlist);
    err = wind_pool_create("user",userpool,sizeof(userpool),sizeof(w_user_s));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    user = wind_user_create(USER_SUPER,"root","123456");
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_FAIL);
    user = wind_user_create(USER_ADMIN,"wind","123456");
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_FAIL);
    return err;
}

w_user_s *wind_user_get(const char *name)
{
    w_user_s *user;
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    wind_disable_switch();
    foreach_node(dnode,&userlist)
    {
        user = NODE_TO_USER(dnode);
        if(user->name && (wind_strcmp(name,user->name) == 0))
        {
            wind_enable_switch();
            return user;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_user_init(w_user_s *user,w_user_e usertype,const char *username,const char *passwd)
{
    wind_notice("init user:%s",username != W_NULL?username:"null");
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(check_user_name_format(username) == W_ERR_OK,W_ERR_INVALID);
    WIND_ASSERT_RETURN(check_passwd_format(passwd) == W_ERR_OK,W_ERR_INVALID);

    user->magic = WIND_USER_MAGIC;
    wind_strcpy(user->name,username);
    wind_strcpy(user->passwd,passwd);
    user->flag = 0;
    CLR_F_USER_POOL(user);
    user->usertype = usertype;
    DNODE_INIT(user->usernode);
    
    wind_disable_switch();
    dlist_insert_tail(&userlist,&user->usernode);
    wind_enable_switch();
    return W_ERR_OK;
}



w_user_s *wind_user_create(w_user_e usertype,const char *username,const char *passwd)
{
    w_err_t err;
    w_user_s *user;
    user = user_malloc();
    WIND_ASSERT_RETURN(user != W_NULL,W_NULL);
    err = wind_user_init(user,usertype,username,passwd);
    if(err == W_ERR_OK)
    {
        SET_F_USER_POOL(user);
        return user;
    }
    user_free(user);
    return W_NULL; 
}

w_err_t wind_user_destroy(w_user_s *user)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(user->magic == WIND_USER_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(user->usertype != USER_SUPER,W_ERR_FAIL);
    wind_notice("destroy user:%s",user->name != W_NULL?user->name:"null");
    wind_disable_switch();
    dnode = dlist_remove(&userlist,&user->usernode);
    wind_enable_switch();
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    user->magic = (~WIND_USER_MAGIC);
    if(IS_F_USER_POOL(user))
        user_free(user);
    return W_ERR_OK;  
}

w_err_t wind_user_modify_passwd(w_user_s *user,const char *newpasswd)
{
    w_int32_t pwdlen;
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(user->magic == WIND_USER_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(check_passwd_format(newpasswd) == W_ERR_OK,W_ERR_INVALID);
    
    pwdlen = wind_strlen(newpasswd);
    WIND_ASSERT_RETURN(pwdlen >= 6,W_ERR_INVALID);
    WIND_ASSERT_RETURN(pwdlen < PASSWD_MAXLEN-1,W_ERR_INVALID);
    wind_disable_switch();
    wind_strcpy(user->passwd,newpasswd);
    wind_enable_switch();
    return W_ERR_OK;
}



w_err_t wind_user_print(void)
{
    w_dnode_s *dnode;
    w_user_s *user;
    w_dlist_s *list = &userlist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\n\r\nuser list:\r\n");
    wind_disable_switch();
    wind_print_space(3);
    wind_printf("%-12s %-4s\r\n","user","group");
    wind_print_space(3);
    foreach_node(dnode,list)
    {
        user = NODE_TO_USER(dnode);
        wind_printf("%-12s %-4d\r\n",user->name,user->usertype);
    }
    wind_print_space(3);
    wind_enable_switch();
    return W_ERR_OK;
}

#endif

