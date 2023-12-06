/// @file tk_atca_hid_device_factory.h
///
/// 2023

#ifndef CRYPTOAUTHLIB_TOOLKIT_ATCA_HID_DEVICE_FACTORY_H
#define CRYPTOAUTHLIB_TOOLKIT_ATCA_HID_DEVICE_FACTORY_H

#include "toolkit/tk_api_export.h"
#include "atca_device.h"

#ifdef __cplusplus
extern "C"
{
#endif


/// Create ATCA device instance based on device path.
ATCADevice TK_API_EXPORT TK_API_CALL tk_atca_hid_device_factory_create(const char* path, const char* device_type);

/// Destroy previously created device.
void TK_API_EXPORT TK_API_CALL tk_atca_hid_device_factory_destroy(ATCADevice dev);


#ifdef __cplusplus
}
#endif

#endif
