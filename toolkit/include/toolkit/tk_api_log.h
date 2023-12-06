/// @file tk_api_log.h
///
/// 2023

#ifndef CRYPTOAUTHLIB_TOOLKIT_API_LOG_H
#define CRYPTOAUTHLIB_TOOLKIT_API_LOG_H

#include "toolkit/tk_api_export.h"

#ifdef __cplusplus
extern "C"
{
#endif


/// Get last error that was encountered by Toolkit.
const char TK_API_EXPORT * TK_API_CALL tk_api_log_get_last_error(void);


#ifdef __cplusplus
}
#endif

#endif
