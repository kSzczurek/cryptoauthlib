/// @file tk_api_log_int.h
///
/// 2023

#ifndef CRYPTOAUTHLIB_TOOLKIT_API_LOG_INT_H
#define CRYPTOAUTHLIB_TOOLKIT_API_LOG_INT_H

#include "toolkit/tk_api_log.h"

#ifdef __cplusplus
extern "C"
{
#endif


/// Get last error that was encountered by Toolkit.
void tk_api_log_int_clear(void);

/// Log error.
void tk_api_log_int_error(const char* fmt, ...);


#ifdef __cplusplus
}
#endif

#endif
