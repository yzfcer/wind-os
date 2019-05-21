#include <string.h>
#include <Windows.h>
char *argv_bak = NULL;
extern void wind_os_prelaunch();
int main(int argc,char *argv[])
{
    int len;
    HANDLE hproc;
    //memset(argv_bak,0,sizeof(argv_bak));
    hproc = GetModuleHandle(NULL);
    SetPriorityClass(hproc,REALTIME_PRIORITY_CLASS);
    if(argc == 1)
    {
        len = strlen(argv[0]);
        argv_bak = malloc(len+1);
        if(argv_bak != NULL)
        {
            //memset(argv_bak,0,sizeof(argv_bak));
            strcpy(argv_bak,argv[0]);
        }
            
    }
	wind_os_prelaunch();
}
