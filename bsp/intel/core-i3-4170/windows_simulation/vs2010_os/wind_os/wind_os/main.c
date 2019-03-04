#include <Windows.h>

extern void wind_os_prelaunch();
void main(void)
{
    HANDLE hproc;
    hproc = GetModuleHandle(NULL);
    SetPriorityClass(hproc,REALTIME_PRIORITY_CLASS);
	wind_os_prelaunch();
}