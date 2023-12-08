/// @file tk_atca_hid_device_factory.c
///
/// 2023

#include "toolkit/tk_atca_hid_device_factory.h"
#include "toolkit/tk_api_log_int.h"
#include "atca_basic.h"
#include "atca_iface.h"
#include "atca_cfgs.h"

#ifdef __cplusplus
extern "C"
{
#endif


ATCADevice TK_API_EXPORT TK_API_CALL tk_atca_hid_device_factory_create(const char* path, const char* device_type)
{
    tk_api_log_int_clear();

    char* path_copy = NULL;
    ATCAIfaceCfg* iface_cfg = NULL;
    ATCADevice dev = NULL;
    ATCADeviceType dev_type = iface_get_device_type_by_name(device_type);

    if (dev_type == ATCA_DEV_INVALID || dev_type == ATCA_DEV_UNKNOWN)
    {
        tk_api_log_int_error("tk_atca_hid_device_factory_create: Unknown dev type: %s", device_type == NULL ? "Null" : device_type);
        goto error;
    }

    size_t path_len = strlen(path);
    path_copy = (char*)hal_malloc(path_len + 1U);

    if (path_copy == NULL)
    {
        tk_api_log_int_error("tk_atca_hid_device_factory_create: Calloc of path buffer failed, path len: %d", path_len);
        goto error;
    }

    iface_cfg = (ATCAIfaceCfg*)hal_malloc(sizeof(ATCAIfaceCfg));

    if (iface_cfg == NULL)
    {
        tk_api_log_int_error("tk_atca_hid_device_factory_create: Malloc of iface cfg failed");
        goto error;
    }

    (void)memcpy(path_copy, path, path_len);
    path_copy[path_len] = '\0';

    // Default values from the example.
    // See: cfg_atsha20xa_kithid_default
    iface_cfg->iface_type            = ATCA_HID_IFACE;
    iface_cfg->devtype               = dev_type;
    iface_cfg->rx_retries            = 1;
    iface_cfg->atcahid.dev_interface = ATCA_KIT_AUTO_IFACE;
    iface_cfg->atcahid.dev_identity  = 0;
    iface_cfg->atcahid.idx           = 0;
    iface_cfg->atcahid.vid           = 0; // vid and
    iface_cfg->atcahid.pid           = 0; // pid are not needed as we use exact path
    iface_cfg->atcahid.packetsize    = 64;
    iface_cfg->atcahid.path          = path_copy;

    ATCA_STATUS init_status = atcab_init_ext(&dev, iface_cfg);

    if (init_status != ATCA_SUCCESS)
    {
        tk_api_log_int_error("tk_atca_hid_device_factory_create: init failed with %d", init_status);
        goto error;
    }

    return dev;

error:
    hal_free(path_copy);
    hal_free(iface_cfg);
    atcab_release_ext(&dev);
    return NULL;
}

void TK_API_EXPORT TK_API_CALL tk_atca_hid_device_factory_destroy(ATCADevice dev)
{
    if (dev != NULL && dev->mIface.mIfaceCFG != NULL)
    {
        if (dev->mIface.mIfaceCFG->atcahid.path != NULL)
        {
            hal_free((char*)dev->mIface.mIfaceCFG->atcahid.path);
        }

        hal_free(dev->mIface.mIfaceCFG);
    }

    atcab_release_ext(&dev);
}


#ifdef __cplusplus
}
#endif
