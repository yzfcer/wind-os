/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: module_cutest.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2015/1/24 20:24:37
** ��        ��: cutestģ�����
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2015/1/24 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/

#if (WIND_MODULE_CUTEST_SUPPORT)

/********************************************�ڲ���������**********************************************/




/********************************************�ڲ���������*********************************************/



/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
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




MODULE_INIT(cutest)
{
    return wind_cmd_register(COMMAND(cutest),1);
}

MODULE_EXIT(cutest)
{
    return wind_cmd_unregister(COMMAND(cutest));
}

MODULE_DEF(cutest, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
