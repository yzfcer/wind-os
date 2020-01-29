/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_board_port.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os��CPU��ϵ��صĴ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_config.h"
#include "wind_type.h"
#include "wind_board_port.h"
#include "wind_debug.h"
#include <Windows.h>                //��Ҫ������ͷ�ļ�


/*
 * �豸������߳�ģʽ�����ĳ�ʼ������Ĺ��Ӻ�����Ϊ�˱�֤tick
 * �жϵ��ȶ���һ������ϵͳtick�жϻ�ŵ�ϵͳ�����߳�״̬ʱ��ʼ
 * ִ�У��ڽ����߳�֮ǰ������tick�����жϼ��߳��л���
 */
void wind_enter_thread_hook(void)
{
}


/*
 * ����CPU�豸����,һ���MCU����������������Ĺ��ܣ����������ʵ�ּ��ɣ�
 * ������ܲ��Ǳ����,���ޱ�Ҫ���Բ���ʵ�֣�ֱ�ӿ��ü��ɡ�
 */
void wind_system_reset(void)
{
    extern char *argv_bak;
    if(argv_bak != W_NULL)
        system(argv_bak);
    ExitProcess(0);
}



#if WIND_HEAP_SUPPORT
/*
 * ��ϵͳ��Ҫ֧���ڴ�ѹ���ʱ����Ҫ�ڴ�ѵ���ʼ��ַ�ͶԵĴ�С��
 * wind-os����֧�ִ���������������ڴ�ѣ����ҿ������ڴ����
 * ����һ��ռ䴴��һ��Ƕ�׵��ڴ�ѣ�����ĳЩ�ض���Ŀ��
 */ 
#include "wind_heap.h"
static w_uint8_t buff1[0x10000];
static w_uint8_t buff2[0x100000];
void _wind_heaps_create(void)
{
    wind_heap_create("heap1",(w_addr_t)(buff1),sizeof(buff1),0);
    wind_heap_create("heap2",(w_addr_t)(buff2),sizeof(buff2),0);
}
#endif

#if WIND_MODULE_VFS_SUPPORT
/*
 * ��ϵͳ��Ҫ֧���ļ�ϵͳ����ʱ����Ҫ�������ʼ��mount�Ĺ���
 * �ڲ���Ҫ�ļ�ϵͳʱ�����Բ�ʵ��
 */ 
#include "wind_fs.h"
void _wind_fs_mount_init(void)
{
    wind_vfs_mount("fs0","listfs","disk0","/");
    wind_vfs_mount("fs1","treefs","null0","/var");
    wind_vfs_mount("fs2","treefs","null1","/mnt/");
    wind_vfs_mount("fs3","hostfs","D:/hostfs/","/host");
}
#endif

//��ջ��ѹ��һ��uintֵ
static void push_stack(w_stack_t ** Stackpp, w_stack_t v)
{
    *Stackpp -= 1;//esp - 4
    **Stackpp = v;//
    return;
}

/*
 * ��ʼ���߳�ջ�������̳߳����л�ʱ����ջ����ȡ����ʼ������
 * ���������˳����Բο���Ӧ��CPU�߳̽���ջ��˳��
 */ 

w_stack_t *_wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_stack_t *pstkbt,w_int32_t stk_depth)
{
	w_stack_t *stk_bottom = pstkbt + stk_depth - 1;
    push_stack(&stk_bottom, (w_stack_t)pdata);        //ͨ�����ָ�����ҵ��̺߳������̲߳���
    push_stack(&stk_bottom, (w_stack_t)0);            //ƽ���ջ��(���ù�)
    push_stack(&stk_bottom, (w_stack_t)pfunc);        //�߳���ں��� ���������������̺߳���
    push_stack(&stk_bottom, (w_stack_t)5);                //push ebp
    push_stack(&stk_bottom, (w_stack_t)7);                //push edi
    push_stack(&stk_bottom, (w_stack_t)6);                //push esi
    push_stack(&stk_bottom, (w_stack_t)3);                //push ebx
    push_stack(&stk_bottom, (w_stack_t)2);                //push ecx
    push_stack(&stk_bottom, (w_stack_t)1);                //push edx
    push_stack(&stk_bottom, (w_stack_t)0);                //push eax
    return stk_bottom;
}




