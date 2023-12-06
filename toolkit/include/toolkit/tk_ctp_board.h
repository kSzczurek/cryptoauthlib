/// @file tk_ctp_board.h
///
/// 2023

#ifndef CRYPTOAUTHLIB_TOOLKIT_CTP_BOARD_H
#define CRYPTOAUTHLIB_TOOLKIT_CTP_BOARD_H

#include <stddef.h>

#include "toolkit/tk_api_export.h"

#ifdef __cplusplus
extern "C"
{
#endif


/// Vendor ID of nEDBG embedded on CryptoAuth Trust Platform board.
#define TK_CTP_BOARD_NEDBG_VID 0x03EB

/// Product ID of nEDBG embedded on CryptoAuth Trust Platform board.
#define TK_CTP_BOARD_NEDBG_PID 0x2175

/// Vendor ID of Host MCU embedded on CryptoAuth Trust Platform board.
#define TK_CTP_BOARD_HOST_MCU_VID 0x03EB

/// Product ID of Host MCU embedded on CryptoAuth Trust Platform board.
#define TK_CTP_BOARD_HOST_MCU_PID 0x2312

/// Vendor ID of USB HUB embedded on CryptoAuth Trust Platform board.
#define TK_CTP_BOARD_HUB_VID 0x0424

/// Product ID of USB HUB embedded on CryptoAuth Trust Platform board.
#define TK_CTP_BOARD_HUB_PID 0x2422


/// CryptoAuth Trust Platform board info.
struct tk_ctp_board_info
{
    // Serial Number of nEDBG embedded on board.
    wchar_t* nedbg_serial_number;

    // Path to nEDBG embedded on board.
    char* nedbg_path;

    // Path to Host MCU embedded on board.
    char* host_mcu_path;

    // Pointer to the next board info.
    struct tk_ctp_board_info* next;
};


/// Enumerate CryptoAuth Trust Platform boards attached to the host PC via USB.
struct tk_ctp_board_info TK_API_EXPORT * TK_API_CALL tk_ctp_board_enumerate(void);

/// Get next board info.
struct tk_ctp_board_info TK_API_EXPORT * TK_API_CALL tk_ctp_board_next_board(struct tk_ctp_board_info* boards);

/// Free resources allocated for boards enumeration.
void TK_API_EXPORT TK_API_CALL tk_ctp_board_free_enumeration(struct tk_ctp_board_info* boards);

/// Get Serial Number of nEDBG from board info.
const wchar_t TK_API_EXPORT * TK_API_CALL tk_ctp_board_get_nedbg_serial_number(struct tk_ctp_board_info* board);

/// Get Path of nEDBG from board info.
const char TK_API_EXPORT * TK_API_CALL tk_ctp_board_get_nedbg_path(struct tk_ctp_board_info* board);

/// Get Path of Host MCU from board info.
const char TK_API_EXPORT * TK_API_CALL tk_ctp_board_get_host_mcu_path(struct tk_ctp_board_info* board);


#ifdef __cplusplus
}
#endif

#endif
