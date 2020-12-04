
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/dnxc/swstate/sw_state_features.h>



#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#elif BCM_DNXF_SUPPORT
#include <include/soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#endif

#include <shared/bsl.h>
#include <soc/drv.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

typedef struct dnxc_sw_state_utils_s {
    uint8 is_warmboot_supported;
    uint8 alloc_during_test;
    uint8 sw_state_skip_alloc;
} dnxc_sw_state_utils_t;

dnxc_sw_state_utils_t sw_state_utils_db[SOC_MAX_NUM_DEVICES];




uint8
sw_state_is_flag_on(
    uint32 flags,
    uint32 flag)
{
    return ((flags & flag) ? 1 : 0);
}

int
sw_state_is_warm_boot(int unit)
{
    return SOC_WARM_BOOT(unit);
}

int
sw_state_is_detaching(int unit)
{
    return SOC_IS_DETACHING(unit);
}

int
sw_state_is_done_init(int unit)
{
    return SOC_IS_DONE_INIT(unit);
}

uint8
dnxc_sw_state_alloc_during_test_set(
    int unit,
    uint8 flag)
{
    sw_state_utils_db[unit].alloc_during_test = flag;
    return 0;
}

uint8
dnxc_sw_state_alloc_during_test_get(
    int unit)
{
    return sw_state_utils_db[unit].alloc_during_test;
}

int dnxc_sw_state_skip_alloc_mode_set(int unit, uint8 on_off)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sw_state_utils_db[unit].sw_state_skip_alloc = on_off;

    DNXC_SW_STATE_FUNC_RETURN;
}

int dnxc_sw_state_skip_alloc_mode_get(int unit, uint8 *on_off)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    *on_off = sw_state_utils_db[unit].sw_state_skip_alloc;

    DNXC_SW_STATE_FUNC_RETURN;
}

uint8 is_prefix(const char *pre, const char *str)
{
    uint32 pre_len = sal_strlen(pre);
    uint32 str_len = sal_strlen(str);
    if (str_len < pre_len) {
        return 0;
    }
    return (sal_strncmp(pre, str, pre_len) == 0);
}

void dnxc_sw_state_is_warmboot_supported_set(int unit, uint8 value) {
    sw_state_utils_db[unit].is_warmboot_supported = value;
}

uint8 dnxc_sw_state_is_warmboot_supported_get(int unit) {
    return sw_state_utils_db[unit].is_warmboot_supported;
}

#undef _ERR_MSG_MODULE_NAME

