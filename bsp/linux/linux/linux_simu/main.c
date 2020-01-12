#include <wind_string.h>
#include <stdlib.h>

char *argv_bak = NULL;
extern void wind_os_prelaunch();
static void save_argv(int argc,char *argv[])
{
    int len;
    if(argc != 1)
        return;
    len = wind_strlen(argv[0]);
    argv_bak = (char *)malloc(len+1);
    if(argv_bak != NULL)
        wind_strcpy(argv_bak,argv[0]);
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
