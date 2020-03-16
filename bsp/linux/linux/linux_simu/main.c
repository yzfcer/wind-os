#include <wind_string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>



void show_call_stack(void)
{
    int i;
    void *buffer[1024];
    int n = backtrace(buffer, 1024);
    char **symbols = backtrace_symbols(buffer, n);
    for (i = 0; i < n; i++) 
	{
        printf("%s\n", symbols[i]);
    }
}

void segv_handle(int signo) 
{
    if (signo == SIGSEGV) 
	{
        printf("\r\n\r\n\r\n");
        printf("-----------------------------------------------\n");
        printf("Receive SIGSEGV signal\n");
        printf("-----call stack-----\n");
        show_call_stack();
        exit(-1);
    }
	else 
	{
        printf("this is sig %d", signo);
    }
}

void reg_sig_segv() 
{
    signal(SIGSEGV, segv_handle);
}

char *argv_bak = NULL;
extern void wind_os_prelaunch();
static void save_argv(int argc,char *argv[])
{
    int len;
    if(argc != 1)
	{
		return;
	}
        
	reg_sig_segv();
    len = wind_strlen(argv[0]);
    argv_bak = (char *)malloc(len+1);
    if(argv_bak != NULL)
	{
        wind_strcpy(argv_bak,argv[0]);	
	}
	printf("argv_bak:%s\n",argv_bak);
}

int main(int argc,char *argv[])
{
    //HANDLE hproc;
    //system("mode con cols=100 lines=50");//设置窗口大小
    save_argv(argc,argv);
    //hproc = GetModuleHandle(NULL);
    //SetPriorityClass(hproc,REALTIME_PRIORITY_CLASS);
    wind_os_prelaunch();
}
