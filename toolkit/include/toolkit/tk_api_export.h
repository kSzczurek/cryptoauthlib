/// @file tk_api_export.h
///
/// 2023

#ifndef CRYPTOAUTHLIB_TOOLKIT_API_EXPORT_H
#define CRYPTOAUTHLIB_TOOLKIT_API_EXPORT_H

#ifdef _WIN32
      #define TK_API_EXPORT __declspec(dllexport)
      #define TK_API_CALL
#else
      #define TK_API_EXPORT /**< API export macro */
      #define TK_API_CALL /**< API call macro */
#endif

#endif
