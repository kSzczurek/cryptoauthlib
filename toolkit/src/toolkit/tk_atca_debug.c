/// @file tk_atca_debug.c
///
/// 2023

#include "toolkit/tk_atca_debug.h"
#include "toolkit/tk_api_log_int.h"
#include "atca_debug.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef ATCA_PRINTF

static FILE* f_p_trace_file = NULL;

void TK_API_EXPORT TK_API_CALL tk_atca_debug_init_trace(const char* trace_file_path)
{
    if (trace_file_path == NULL)
    {
        tk_api_log_int_error("tk_atca_debug_init_trace: trace file path cannot be null!");
        return;
    }

    if (f_p_trace_file != NULL)
    {
        tk_api_log_int_error("tk_atca_debug_init_trace: trace file already opened, changing trace file");
        tk_atca_debug_deinit_trace();
    }

    errno = 0;
    f_p_trace_file = fopen(trace_file_path, "w");

    if (p_trace_tile == NULL)
    {
        tk_api_log_int_error("tk_atca_debug_init_trace: failed to open file errno=%d", errno);
    }
    else
    {
        atca_trace_config(p_trace_tile);
    }
}

void TK_API_EXPORT TK_API_CALL tk_atca_debug_deinit_trace()
{
    if (f_p_trace_file != NULL)
    {
        atca_trace_config(NULL);
        fclose(f_p_trace_file);
    }
}

#endif

#ifdef __cplusplus
}
#endif
