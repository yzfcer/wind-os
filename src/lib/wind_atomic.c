/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: wind_atomic.h / wind_atomic.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2014/10/26 2:20:23
** ��        ��: ����ԭ�Ӳ�����������
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2014/10/26 2:20:23
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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_core.h"
#include "wind_atomic.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus



/********************************************�ڲ���������**********************************************/



/********************************************�ڲ���������*********************************************/



/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
void atomic_inc(w_atomic_t atom)
{
    wind_disable_interrupt();
    atom.value ++;
    wind_enable_interrupt();
}

void atomic_dec(w_atomic_t atom)
{
    wind_disable_interrupt();
    atom.value --;
    wind_enable_interrupt();
}

void atomic_add(w_atomic_t atom,w_uint32_t va)
{
    wind_disable_interrupt();
    atom.value += va;
    wind_enable_interrupt();
}

void atomic_minus(w_atomic_t atom,w_uint32_t value)
{
    wind_disable_interrupt();
    atom.value -= value;
    wind_enable_interrupt();
}

void atomic_set(w_atomic_t atom,w_uint32_t value)
{
    wind_disable_interrupt();
    atom.value -= value;
    wind_enable_interrupt();

}

void atomic_get(w_atomic_t atom,w_uint32_t *value)
{
    wind_disable_interrupt();
    *value = atom.value;
    wind_enable_interrupt();   
}

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

