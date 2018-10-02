/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_user.h
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: ϵͳ���û�������صĶ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_USER_H__
#define WIND_USER_H__
#include "wind_config.h"
#include "wind_type.h"
#if WIND_CONSOLE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif

#define WIND_USER_NAME_LEN 20
#define WIND_USER_PWD_LEN 20

typedef enum __usrprio_e
{
    USRPRIO_SUPPER,//��������Ա
    USRPRIO_MGR,//һ�����Ա
    USRPRIO_COMMAN//��ͨ�û�
}usrprio_e;

typedef struct __user_s
{
    //struct __user_s *next;
    w_int8_t name[WIND_USER_NAME_LEN];
    w_int8_t pwd[WIND_USER_PWD_LEN];
    usrprio_e prio;
} user_s;


user_s *wind_user_create(w_int8_t *name,w_int8_t *pwd,usrprio_e prio);
w_err_t wind_user_insert(user_s *usr);
w_err_t wind_user_remove(user_s *usr);





#ifdef __cplusplus
}
#endif
#endif
#endif
