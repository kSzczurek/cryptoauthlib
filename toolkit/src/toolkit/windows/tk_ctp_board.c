/// @file tk_ctp_board.c
///
/// 2023

#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <cfgmgr32.h>
#include <initguid.h>
#include <devpkey.h>
#include <propkey.h>

#include "hidapi.h"
#include "toolkit/tk_ctp_board.h"
#include "toolkit/tk_api_log_int.h"

#ifdef __cplusplus
extern "C"
{
#endif


static wchar_t* tk_ctp_board_int_str_to_wstr(const char* str);
static void* tk_ctp_board_int_hid_get_device_interface_property(const char* interface_path, const DEVPROPKEY* property_key, DEVPROPTYPE expected_property_type);
static void* tk_ctp_board_int_hid_get_devnode_property(DEVINST dev_node, const DEVPROPKEY* property_key, DEVPROPTYPE expected_property_type);
static bool tk_ctp_board_int_hid_locate_dev_node(DEVINST* dev_node, const char* interface_path);
static bool tk_ctp_board_int_hid_locate_parent_dev_node(DEVINST* parent_dev_node, DEVINST dev_node, unsigned short vendor_id, unsigned short product_id);
static bool tk_ctp_board_int_hid_locate_parent_dev_node_by_dev_path(DEVINST* parent_dev_node, const char* interface_path, unsigned short vendor_id, unsigned short product_id);


struct tk_ctp_board_info TK_API_EXPORT* TK_API_CALL tk_ctp_board_enumerate(void)
{
    tk_api_log_int_clear();

    struct tk_ctp_board_info* boards = NULL;
    struct tk_ctp_board_info** boards_ite = &boards;
    struct hid_device_info* nedbg_devs = hid_enumerate(TK_CTP_BOARD_NEDBG_VID, TK_CTP_BOARD_NEDBG_PID);
    struct hid_device_info* host_mcu_devs = hid_enumerate(TK_CTP_BOARD_HOST_MCU_VID, TK_CTP_BOARD_HOST_MCU_PID);

    for (struct hid_device_info* nedbg_ite = nedbg_devs; nedbg_ite != NULL; nedbg_ite = nedbg_ite->next)
    {
        if (nedbg_ite->path == NULL)
        {
            tk_api_log_int_error("tk_ctp_board_enumerate: nEDBG path missing");
            continue;
        }

        if (nedbg_ite->serial_number == NULL)
        {
            tk_api_log_int_error("tk_ctp_board_enumerate: serial number missing for nEDBG: %s", nedbg_ite->path);
            continue;
        }

        DEVINST nedbg_parent_node;
        bool result = tk_ctp_board_int_hid_locate_parent_dev_node_by_dev_path(&nedbg_parent_node, nedbg_ite->path, TK_CTP_BOARD_HUB_VID, TK_CTP_BOARD_HUB_PID);

        if (!result)
        {
            continue;
        }

        struct hid_device_info* host_mcu_ite = host_mcu_devs;

        for (; host_mcu_ite != NULL; host_mcu_ite = host_mcu_ite->next)
        {
            if (host_mcu_ite->path == NULL)
            {
                // Board already used or failed to get parent.
                continue;
            }

            DEVINST host_mcu_parent_node;
            result = tk_ctp_board_int_hid_locate_parent_dev_node_by_dev_path(&host_mcu_parent_node, host_mcu_ite->path, TK_CTP_BOARD_HUB_VID, TK_CTP_BOARD_HUB_PID);

            if (!result)
            {
                // Do not try again.
                free(host_mcu_ite->path);
                host_mcu_ite->path = NULL;
            }
            else if (nedbg_parent_node == host_mcu_parent_node)
            {
                // Match! :)
                break;
            }
            else
            {
            }
        }

        if (host_mcu_ite == NULL)
        {
            tk_api_log_int_error("tk_ctp_board_enumerate: could not match Host MCU for nEDBG: %s", nedbg_ite->path);
            continue;
        }

        if (host_mcu_ite->path == NULL)
        {
            tk_api_log_int_error("tk_ctp_board_enumerate: host mcu without path, how did we get match with nEDBG: %s ?", nedbg_ite->path);
            continue;
        }

        struct tk_ctp_board_info* new_board = (struct tk_ctp_board_info*)malloc(sizeof(struct tk_ctp_board_info));

        if (new_board == NULL)
        {
            tk_api_log_int_error("tk_ctp_board_enumerate: board allocation failed");
            break;
        }

        new_board->nedbg_serial_number = nedbg_ite->serial_number;
        nedbg_ite->serial_number = NULL;

        new_board->nedbg_path = nedbg_ite->path;
        nedbg_ite->path = NULL;

        new_board->host_mcu_path = host_mcu_ite->path;
        host_mcu_ite->path = NULL;

        new_board->next = NULL;

        *boards_ite = new_board;
        boards_ite = &new_board->next;
    }

    hid_free_enumeration(host_mcu_devs);
    hid_free_enumeration(nedbg_devs);

    if (boards == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_enumerate: No CryptoAuth Trust Platform boards found");
    }

    return boards;
}

struct tk_ctp_board_info TK_API_EXPORT * TK_API_CALL tk_ctp_board_next_board(struct tk_ctp_board_info* boards)
{
    tk_api_log_int_clear();

    if (boards == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_next_board: null board");
        return NULL;
    }

    return boards->next;
}

void TK_API_EXPORT TK_API_CALL tk_ctp_board_free_enumeration(struct tk_ctp_board_info* boards)
{
    tk_api_log_int_clear();

    struct tk_ctp_board_info* b = boards;
    while (b)
    {
        free(b->nedbg_serial_number);
        free(b->nedbg_path);
        free(b->host_mcu_path);
        b = b->next;
    }
}

const wchar_t TK_API_EXPORT * TK_API_CALL tk_ctp_board_get_nedbg_serial_number(struct tk_ctp_board_info* board)
{
    tk_api_log_int_clear();

    if (board == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_get_nedbg_serial_number: null board");
        return NULL;
    }

    return board->nedbg_serial_number;
}

const char TK_API_EXPORT * TK_API_CALL tk_ctp_board_get_nedbg_path(struct tk_ctp_board_info* board)
{
    tk_api_log_int_clear();

    if (board == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_get_nedbg_path: null board");
        return NULL;
    }

    return board->nedbg_path;
}

const char TK_API_EXPORT * TK_API_CALL tk_ctp_board_get_host_mcu_path(struct tk_ctp_board_info* board)
{
    tk_api_log_int_clear();

    if (board == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_get_host_mcu_path: null board");
        return NULL;
    }

    return board->host_mcu_path;
}

static wchar_t* tk_ctp_board_int_str_to_wstr(const char* str)
{
    if (str == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_int_str_to_wstr: invalid argument");
        return NULL;
    }

    size_t wstr_len = mbstowcs(NULL, str, INT_MAX);
    wchar_t* wstr = (wchar_t*)calloc(wstr_len + 1U, sizeof(wchar_t));

    if (wstr == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_int_str_to_wstr: calloc failed");
        return NULL;
    }

    size_t chars_converted = mbstowcs(wstr, str, wstr_len);

    if (chars_converted > wstr_len)
    {
        tk_api_log_int_error("tk_ctp_board_int_str_to_wstr: buffer too small for %d chars", chars_converted);
        free(wstr);
        return NULL;
    }

    wstr[chars_converted] = L'\0';

    return wstr;
}

static void* tk_ctp_board_int_hid_get_device_interface_property(const char* interface_path, const DEVPROPKEY* property_key, DEVPROPTYPE expected_property_type)
{
    if (interface_path == NULL || property_key == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_get_device_interface_property: invalid arguments");
        return NULL;
    }

    wchar_t* wpath = tk_ctp_board_int_str_to_wstr(interface_path);

    if (wpath == NULL)
    {
        return NULL;
    }

    PBYTE property_value = NULL;
    ULONG len = 0;
    DEVPROPTYPE property_type;
    CONFIGRET cr = CM_Get_Device_Interface_PropertyW(wpath, property_key, &property_type, NULL, &len, 0);

    if (cr != CR_BUFFER_SMALL || property_type != expected_property_type)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_get_device_interface_property: failed with cr: %d, prop: %d, expected prop: %d for %s", cr, property_type, expected_property_type, interface_path);
        goto error;
    }

    property_value = (PBYTE)calloc(len, sizeof(BYTE));

    if (property_value == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_get_device_interface_property: calloc failed");
        goto error;
    }

    cr = CM_Get_Device_Interface_PropertyW(wpath, property_key, &property_type, property_value, &len, 0);

    if (cr != CR_SUCCESS)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_get_device_interface_property: failed with cr: %d for %s", cr, interface_path);
        goto error;
    }

    return property_value;

error:
    free(property_value);
    free(wpath);
    return NULL;
}

static void* tk_ctp_board_int_hid_get_devnode_property(DEVINST dev_node, const DEVPROPKEY* property_key, DEVPROPTYPE expected_property_type)
{
    if (property_key == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_get_devnode_property: invalid argument");
        return NULL;
    }

    ULONG len = 0;
    DEVPROPTYPE property_type;
    CONFIGRET cr = CM_Get_DevNode_PropertyW(dev_node, property_key, &property_type, NULL, &len, 0);

    if (cr != CR_BUFFER_SMALL || property_type != expected_property_type)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_get_devnode_property: failed with cr: %d, prop: %d, expected prop: %d for %d", cr, property_type, expected_property_type, dev_node);
        return NULL;
    }

    PBYTE property_value = (PBYTE)calloc(len, sizeof(BYTE));

    if (property_value == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_get_devnode_property: calloc failed");
        return NULL;
    }

    cr = CM_Get_DevNode_PropertyW(dev_node, property_key, &property_type, property_value, &len, 0);

    if (cr != CR_SUCCESS)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_get_devnode_property: failed with cr: %d for %d", cr, dev_node);
        free(property_value);
        property_value = NULL;
    }

    return property_value;
}

static bool tk_ctp_board_int_hid_locate_dev_node(DEVINST* dev_node, const char* interface_path)
{
    if (dev_node == NULL || interface_path == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_locate_dev_node: invalid arguments");
        return false;
    }

    bool result = false;
    wchar_t* device_id = (wchar_t*)tk_ctp_board_int_hid_get_device_interface_property(interface_path, &DEVPKEY_Device_InstanceId, DEVPROP_TYPE_STRING);

    if (device_id != NULL)
    {
            CONFIGRET cr = CM_Locate_DevNodeW(dev_node, (DEVINSTID_W)device_id, CM_LOCATE_DEVNODE_NORMAL);

            if (cr != CR_SUCCESS)
            {
                tk_api_log_int_error("tk_ctp_board_int_hid_locate_dev_node: failed to get device node id, cr: %d for %s, ", cr, interface_path);
            }
            else
            {
                result = true;
            }
    }

    free(device_id);

    return result;
}

static bool tk_ctp_board_int_hid_locate_parent_dev_node(DEVINST* parent_dev_node, DEVINST dev_node, unsigned short vendor_id, unsigned short product_id)
{
    if (parent_dev_node == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_locate_parent_dev_node: invalid argument");
        return false;
    }

    // See: https://learn.microsoft.com/en-us/windows-hardware/drivers/install/standard-usb-identifiers#single-interface-usb-devices
    wchar_t hardware_id_subpart[] = L"USB\\VID_xxxx&PID_xxxx";
    (void)_snwprintf(hardware_id_subpart, sizeof(hardware_id_subpart) / sizeof(hardware_id_subpart[0]), L"USB\\VID_%04x&PID_%04x", vendor_id, product_id);
    hardware_id_subpart[sizeof(hardware_id_subpart) / sizeof(hardware_id_subpart[0]) - 1U] = L'\0';

    DEVINST parent_node = dev_node;
    bool result = false;

    while (!result)
    {
        CONFIGRET cr = CM_Get_Parent(&parent_node, parent_node, 0);

        if (cr != CR_SUCCESS)
        {
            tk_api_log_int_error("tk_ctp_board_int_hid_locate_parent_dev_node: failed with cr: %d for %d", cr, dev_node);
            break;
        }

        wchar_t* hardware_ids = (wchar_t*)tk_ctp_board_int_hid_get_devnode_property(parent_node, &DEVPKEY_Device_HardwareIds, DEVPROP_TYPE_STRING_LIST);

        if (hardware_ids == NULL)
        {
            // Not found, error should be already logged.
            break;
        }

        if (wcsstr(hardware_ids, hardware_id_subpart) != NULL)
        {
            // Got it! :)
            result = true;
            *parent_dev_node = parent_node;
        }

        free(hardware_ids);
    }

    return result;
}

static bool tk_ctp_board_int_hid_locate_parent_dev_node_by_dev_path(DEVINST* parent_dev_node, const char* interface_path, unsigned short vendor_id, unsigned short product_id)
{
    if (parent_dev_node == NULL || interface_path == NULL)
    {
        tk_api_log_int_error("tk_ctp_board_int_hid_locate_parent_dev_node_by_dev_path: invalid arguments");
        return false;
    }

    DEVINST dev_node;
    bool result = tk_ctp_board_int_hid_locate_dev_node(&dev_node, interface_path);

    if (result)
    {
        DEVINST parent_node;
        result = tk_ctp_board_int_hid_locate_parent_dev_node(&parent_node, dev_node, vendor_id, product_id);

        if (result)
        {
            *parent_dev_node = parent_node;
        }
    }

    return result;
}


#ifdef __cplusplus
}
#endif
