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
#include "wind_mutex.h"
#include "wind_os_hwif.h"

#if WIND_USER_SUPPORT
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


w_err_t _wind_user_mod_init(void)
{
    w_err_t err;
    w_user_s *user;
    DLIST_INIT(userlist);
    err = wind_pool_create("user",userpool,sizeof(userpool),sizeof(w_user_s));
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    user = wind_user_create(USER_SUPER,"root","123456");
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_FAIL);
    user = wind_user_create(USER_SUPER,"wind","123456");
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
        user = DLIST_OBJ(dnode,w_user_s,usernode);
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
    w_int32_t namelen,pwdlen;
    WIND_ASSERT_RETURN(username != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(passwd != W_NULL,W_ERR_PTR_NULL);
    namelen = wind_strlen(username);
    WIND_ASSERT_RETURN(namelen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(namelen < USER_NAME_LEN-1,W_ERR_INVALID);
    
    pwdlen = wind_strlen(passwd);
    WIND_ASSERT_RETURN(pwdlen >= 6,W_ERR_INVALID);
    WIND_ASSERT_RETURN(pwdlen < PASSWD_LEN-1,W_ERR_INVALID);

    user->magic = WIND_USER_MAGIC;
    wind_strcpy(user->name,username);
    wind_strcpy(user->passwd,passwd);
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
        user->flag_pool = 1;
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
    wind_disable_switch();
    dnode = dlist_remove(&userlist,&user->usernode);
    wind_enable_switch();
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    user->magic = 0;
    user_free(user);

    return W_ERR_OK;  
}

w_err_t wind_user_modify_passwd(w_user_s *user,const char *newpasswd)
{
    w_int32_t pwdlen;
    WIND_ASSERT_RETURN(user != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(user->magic == WIND_USER_MAGIC,W_ERR_INVALID);
    WIND_ASSERT_RETURN(newpasswd != W_NULL,W_ERR_PTR_NULL);
    
    pwdlen = wind_strlen(newpasswd);
    WIND_ASSERT_RETURN(pwdlen >= 6,W_ERR_INVALID);
    WIND_ASSERT_RETURN(pwdlen < PASSWD_LEN-1,W_ERR_INVALID);
    wind_strcpy(user->passwd,newpasswd);
    return W_ERR_OK;
}



w_err_t wind_user_print(void)
{
    w_dnode_s *dnode;
    w_user_s *user;
    int cnt = 0;
    w_dlist_s *list = &userlist;
    WIND_ASSERT_RETURN(list != W_NULL,W_ERR_PTR_NULL);
    wind_printf("\r\ndev list as following:\r\n");
    foreach_node(dnode,list)
    {
        user = (w_user_s *)DLIST_OBJ(dnode,w_user_s,usernode);
        wind_printf("%-12s ",user->name);
        cnt ++;
        if((cnt & 0x03) == 0)
            wind_printf("\r\n");
    }
    return W_ERR_OK;
}

#endif

