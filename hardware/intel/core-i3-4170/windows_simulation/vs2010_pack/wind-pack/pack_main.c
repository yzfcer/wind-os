#include "boot_pack.h"
#include "wind_type.h"

w_err_t thread_console(w_int32_t argc,char **argv);

int main(int argc,char **argv)
{
    return pack_main(argc,argv);
}