/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: console_framework.h / console_framework.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2015/1/24 20:08:03
** ��        ��: �ܹ�֧�ֱ�׼��������Ŀ���̨�Ŀ��
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2015/1/24 20:08:03
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ���������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __CONSOLE_FRAMEWORK_H__
#define __CONSOLE_FRAMEWORK_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_key.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "cmd_history.h"
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************ͷ�ļ�����***********************************************/

#if WIND_CONSOLE_SUPPORT

/***********************************************�궨��*************************************************/
#define WIND_CMD_MAX_LEN 512//һ���������󳤶�
#define WIND_CMD_NAME_LEN 12//һ�������ʾ����󳤶�
#define WIND_CTL_USRNAME_LEN 20//�û����ĳ���
#define WIND_CTL_PWD_LEN 20//�������󳤶�
#define WIND_CONSOLE_COUNT 1//֧�ֵĿ������ն˵�����
#define CMD_PARAM_CNT 10

#define USER_AUTHENTICATION_EN 0
    


/**********************************************ö�ٶ���************************************************/





/*********************************************�ṹ�嶨��***********************************************/

typedef enum __w_cslstat_e
{
    CSLSTAT_USER,//��Ҫ�����û���
    CSLSTAT_PWD,//��Ҫ��������
    CSLSTAT_CMD,//��������ģʽ
    CSLSTAT_APP //������Ӧ�ó���ģʽ,��ʱ����
} w_ctlstat_e;

typedef struct __w_cmd_s
{
    //struct __cmd_s *next;
    w_dnode_s cmdnode;
    char* name;//���������
    void (*showdisc)(void);//��Ҫ����˵��
    void (*showusage)(void);//��ϸ�İ���˵��
    w_err_t (*execute)(w_int32_t argc,char **argv);//�������ں���
}w_cmd_s;


//�õ��ֽ��Ĳ����б�
typedef struct __cmd_param_s
{
    w_uint32_t argc;
    char *argv[CMD_PARAM_CNT];
}w_cmd_param_s;


typedef struct __console_s
{
    w_ctlstat_e stat;//��ǰ�Ľ���״̬
    w_int32_t index;//�������һ���ַ��±�
    w_int8_t key_evt_f;
    w_int8_t key_evt_len;
    w_uint32_t key_evt_id;
    char buf[WIND_CMD_MAX_LEN];//���յ����ݻ�����
    char user[WIND_CTL_USRNAME_LEN];//�û���
    char pwd[WIND_CTL_PWD_LEN];//�����ֵ
    
    w_cmd_his_s his;
    w_cmd_param_s param;
    w_dlist_s cmd_list;
}w_console_s;


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/

//��������б�
void wind_cmd_init(w_console_s *ctrl);
w_err_t _create_console_thread(void);
w_cmd_s *wind_cmd_get(const char *name);
w_err_t wind_cmd_register(w_cmd_s *cmd,int cnt);
w_err_t wind_cmd_print(void);
void _wind_register_all_cmd(w_console_s *ctrl);
extern void wind_cmd_register_cmd_test(w_console_s *ctrl);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef __CONSOLE_FRAMEWORK_H__