/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: cmd_cutest.c
**��   ��   ��: Jason Zhou
**����޸�����: 2013.11.06
**��        ��: ϵͳ�Ŀ���̨����test���������ṩһЩϵͳ�ں˲��Ե��������Ӧ����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.11.06
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
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
#include "wind_cut.h"

#include "wind_cmd.h"
#include "test_framework.h"

#if (WIND_CONSOLE_SUPPORT && CMD_CUTEST_SUPPORT)

COMMAND_DISC(cutest)
{
    wind_printf("to test some test cases.\r\n");
}

COMMAND_USAGE(cutest)
{
    wind_printf("cutest list:--to show all test suites and cases list.\r\n");
    wind_printf("cutest <suitename> <casename>:--to test some appointed test cases.\r\n");
    wind_printf("       suitename:--use *to test all test suite.\r\n");
    wind_printf("       casename:--use *to test all test suite.\r\n");
}

COMMAND_MAIN(cutest,argc,argv)
{
    return cutest_main(argc,argv);
}

COMMAND_DEF(cutest);


#endif
