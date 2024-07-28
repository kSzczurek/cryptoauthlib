/// @file tk_atca_debug.h
///
/// 2024

#ifndef CRYPTOAUTHLIB_TOOLKIT_TK_ATCA_DEBUG_H
#define CRYPTOAUTHLIB_TOOLKIT_TK_ATCA_DEBUG_H

#include "toolkit/tk_api_export.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef ATCA_PRINTF

void TK_API_EXPORT TK_API_CALL tk_atca_debug_init_trace(const char* trace_file_path);

void TK_API_EXPORT TK_API_CALL tk_atca_debug_deinit_trace();

#endif

#ifdef __cplusplus
}
#endif

#endif
