/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_test.c
**��   ��   ��: �ܽ���
**����޸�����: 2013.11.06
**��        ��: ϵͳ�Ŀ���̨����test���������ṩһЩϵͳ�ں˲��Ե��������Ӧ����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2013.11.06
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2013.11.06
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "wind_softirq.h"
#include "wind_heap.h"
#include "cut.h"

#include "wind_cmd.h"
#include "test_framework.h"

#if WIND_CONSOLE_SUPPORT

w_err_t cmd_testheap_main(w_int32_t argc,char **argv)
{
#if WIND_HEAP_SUPPORT
    if(0 == wind_strcmp(argv[0],"heap"))
    {
        wind_printf("heaptest not support yet\r\n");
        return ERR_OK;
    }
#endif
    return ERR_COMMAN;
}

COMMAND_DISC(test)
{
    wind_printf("to test some test cases.\r\n");
}

COMMAND_USAGE(test)
{
    wind_printf("test show:to show all test suites and cases list.\r\n");
    wind_printf("test <suitename> <casename>:to test some appointed test cases.\r\n");
    wind_printf("suitename:use *to test all test suite.\r\n");
    wind_printf("casename:use *to test all test suite.\r\n");
}

COMMAND_MAIN(test,argc,argv)
{
    if(0 == wind_strcmp(argv[1],"heap"))
    {
        return cmd_testheap_main(argc,argv);
    }
    else if(0 == wind_strcmp(argv[1],"show"))
    {
        show_test_suites();
    }
    else if(argc >= 3)
    {
        cut_test_start(argv[1],argv[2]);
    }
    return  ERR_COMMAN;
}

COMMAND_DEF(test);

void wind_cmd_register_cmd_test(console_s *ctrl)
{
    wind_cmd_register(COMMAND(test),1);
}


#endif
