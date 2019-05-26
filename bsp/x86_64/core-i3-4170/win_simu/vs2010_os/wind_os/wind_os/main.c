#include <string.h>
#include <Windows.h>
char *argv_bak = NULL;
extern void wind_os_prelaunch();
static save_argv(int argc,char *argv[])
{
    int len;
    if(argc != 1)
        return;
    len = strlen(argv[0]);
    argv_bak = malloc(len+1);
    if(argv_bak != NULL)
        strcpy(argv_bak,argv[0]);
}

int main(int argc,char *argv[])
{
    HANDLE hproc;
    save_argv(argc,argv);
    hproc = GetModuleHandle(NULL);
    SetPriorityClass(hproc,REALTIME_PRIORITY_CLASS);
	wind_os_prelaunch();
}
