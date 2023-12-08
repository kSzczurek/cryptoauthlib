/// @file tk_api_log_int.d
///
/// 2023

#include <string.h>
#include <stdarg.h>

#include "tk_api_log_int.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define TK_API_LOG_INT_LAST_ERROR_MAX_LEN 1024U
static char f_tk_api_log_int_last_error_buffer[TK_API_LOG_INT_LAST_ERROR_MAX_LEN + 1U];

const char TK_API_EXPORT * TK_API_CALL tk_api_log_get_last_error(void)
{
    return f_tk_api_log_int_last_error_buffer;
}

void tk_api_log_int_clear(void)
{
    (void)memset(f_tk_api_log_int_last_error_buffer, 0, sizeof(f_tk_api_log_int_last_error_buffer));
}

void tk_api_log_int_error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    (void)vsnprintf(f_tk_api_log_int_last_error_buffer, TK_API_LOG_INT_LAST_ERROR_MAX_LEN, fmt, args);
    f_tk_api_log_int_last_error_buffer[TK_API_LOG_INT_LAST_ERROR_MAX_LEN] = '\0';

    va_end(args);
}


#ifdef __cplusplus
}
#endif
