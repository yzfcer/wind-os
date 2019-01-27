#include "shell_framework.h"
#include "wind_type.h"

w_err_t thread_shell(w_int32_t argc,char **argv);

int main(int argc,char **argv)
{
    return thread_shell(argc,argv);
}