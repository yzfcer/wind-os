#include "stdio.h"
#include "wind_std.h"
#include "shell_framework.h"
#include "wind_string.h"
#include "lua.h"
#define LUA_MAXINPUT 512
typedef int FILEHANDLE;
#ifndef NULL
#define NULL (void*)0
#endif

extern w_shell_ctx_s g_shell_ctx[WIND_SHELL_CTX_COUNT];
extern w_int32_t shell_read_line(w_shell_ctx_s *ctx,w_int32_t len);

int lua_readline(lua_State *L,char *buff,const char *prmt)
{
    int len;
    wind_std_output((w_uint8_t*)prmt,wind_strlen(prmt));
    len = shell_read_line(&g_shell_ctx[0],LUA_MAXINPUT);
    wind_strcpy(buff,g_shell_ctx[0].buf);
    buff[len] = '\n';
    buff[len+1] = '\0';
    return 1;
}


