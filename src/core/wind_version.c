/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_version.h / wind_version.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os�ĺ��Ĵ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_version.h"
#include "wind_debug.h"
const char* versioninfo = \
"E-mail:yzfcer@163.com.\r\n";

w_uint8_t *g_wind_logo[4] = 
{
    " _    __    _",
    " \\\\  //\\\\  // ",
    "  \\\\//  \\\\//  ",
    "   \\/    \\/   "   
};

const w_int8_t *wind_get_core_version_info(void)
{
    return versioninfo;
}

const w_int8_t *wind_get_core_version(void)
{
    return WIND_CORE_VERSION;
}
static void output_logo(void)
{
    int i;
    for(i = 0;i < 4;i++)
    {
        wind_printf("%s\r\n",g_wind_logo[i]);
    }
}

void _wind_print_os_info(void)
{
    const char *str;
    wind_printf("\r\n\r\nwind_os version:%s\r\n",wind_get_core_version());
    output_logo();
    wind_printf("core created on:%s @ chongqing,china.\r\n",__DATE__);
    str = wind_get_core_version_info();
    wind_printf(str);
    wind_printf("core is built at %s %s\r\n\r\n",__TIME__,__DATE__);
}

