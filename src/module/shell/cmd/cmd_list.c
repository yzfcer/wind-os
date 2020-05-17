/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: cmd_list.c
**��   ��   ��: Jason Zhou
**����޸�����: 2013.10.19
**��        ��: ϵͳ���ں˶����ӡ����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.10.19
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2013.10.19
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "wind_sem.h"
#include "wind_daemon.h"
#include "wind_event.h"
#include "wind_mutex.h"
#include "wind_pool.h"
#include "wind_msgbox.h"
#include "wind_heap.h"
#include "wind_chdev.h"
#include "wind_blkdev.h"
#include "wind_pipe.h"
#include "wind_watchdog.h"
#include "wind_timer.h"
#include "wind_fs.h"
#include "wind_conv.h"
#include "wind_module.h"
#include "wind_fsm.h"

#if (CMD_LIST_SUPPORT)

COMMAND_DISC(list)
{
    wind_printf("to show core objects infomation.\r\n");
}

COMMAND_USAGE(list)
{
    wind_printf("list thread:--show thread infomation.\r\n");
    wind_printf("list pool:--show all core object pools.\r\n");
    
#if WIND_MUTEX_SUPPORT
    wind_printf("list mutex:--show all mutex status.\r\n");
#endif
#if WIND_SEM_SUPPORT
        wind_printf("list sem:--show sem infomation.\r\n");
#endif
#if WIND_EVENT_SUPPORT
    wind_printf("list event:--show event infomation.\r\n");
#endif
#if WIND_PIPE_SUPPORT
    wind_printf("list pipe:--show all pipe infomation.\r\n");
#endif
#if WIND_MSGBOX_SUPPORT
    wind_printf("list msgbox:--show all message box infomation.\r\n");
#endif
#if WIND_HEAP_SUPPORT
    wind_printf("list heap:--show all heap map infomation.\r\n");
#endif
#if WIND_TIMER_SUPPORT
    wind_printf("list timer:--show all timer infomation.\r\n");
#endif

#if WIND_CHDEV_SUPPORT
    wind_printf("list chdev:--show all character devices list.\r\n");
#endif
#if WIND_BLKDEV_SUPPORT
    wind_printf("list blkdev:--show all block devices list.\r\n");
#endif
#if WIND_WATCHDOG_SUPPORT
    wind_printf("list watchdog:--show all thread watchdog.\r\n");
#endif
#if WIND_DAEMON_SUPPORT
    wind_printf("list daemon:--show all thread that under daemon.\r\n");
#endif
#if WIND_MODULE_VFS_SUPPORT
    wind_printf("list vfs:--show vfs mount infomation.\r\n");
#endif
#if WIND_MODULE_SUPPORT
    wind_printf("list module:--show wind-os modules infomation.\r\n");
#endif
#if WIND_FSM_SUPPORT
    wind_printf("list fsm:--show fsm list infomation.\r\n");
    wind_printf("list fsm_model:--show fsm_model list.\r\n");
#endif
}

COMMAND_MAIN(list,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(0 == wind_strcmp(argv[1],"thread"))
    {
        wind_thread_print_list();
        return W_ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"pool"))
    {
        wind_pool_print_list();
        return W_ERR_OK;
    }
#if WIND_MUTEX_SUPPORT
    else if(0 == wind_strcmp(argv[1],"mutex"))
    {
        wind_mutex_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_SEM_SUPPORT
    else if(0 == wind_strcmp(argv[1],"sem"))
    {
        wind_sem_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_EVENT_SUPPORT
    else if(0 == wind_strcmp(argv[1],"event"))
    {
        wind_event_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_PIPE_SUPPORT
    else if(0 == wind_strcmp(argv[1],"pipe"))
    {
        wind_pipe_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_MSGBOX_SUPPORT
    else if(0 == wind_strcmp(argv[1],"msgbox"))
    {
        wind_msgbox_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_HEAP_SUPPORT
    else if(0 == wind_strcmp(argv[1],"heap"))
    {
        wind_heap_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_TIMER_SUPPORT
    else if(0 == wind_strcmp(argv[1],"timer"))
    {
        wind_timer_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_CHDEV_SUPPORT
    else if(0 == wind_strcmp(argv[1],"chdev"))
    {
        wind_chdev_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_BLKDEV_SUPPORT
    else if(0 == wind_strcmp(argv[1],"blkdev"))
    {
        wind_blkdev_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_WATCHDOG_SUPPORT
    else if(0 == wind_strcmp(argv[1],"watchdog"))
    {
        wind_watchdog_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_DAEMON_SUPPORT
    else if(0 == wind_strcmp(argv[1],"daemon"))
    {
        wind_daemon_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_MODULE_VFS_SUPPORT
    else if(0 == wind_strcmp(argv[1],"vfs"))
    {
        wind_vfs_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_MODULE_SUPPORT
    else if(0 == wind_strcmp(argv[1],"module"))
    {
        wind_module_print_list();
        return W_ERR_OK;
    }
#endif
#if WIND_FSM_SUPPORT
    else if(0 == wind_strcmp(argv[1],"fsm"))
    {
        wind_fsm_print_list();
        return W_ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"fsm_model"))
    {
        wind_fsm_model_print_list();
        return W_ERR_OK;
    }
#endif

    else
        wind_error("command is NOT support on device.");
    return W_ERR_FAIL;
}

COMMAND_DEF(list);

#endif
