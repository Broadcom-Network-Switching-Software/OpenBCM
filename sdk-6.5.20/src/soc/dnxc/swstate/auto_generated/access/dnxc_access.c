
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnxc/swstate/auto_generated/access/dnxc_access.h>



const char *
soc_dnxc_isolation_status_t_get_name(soc_dnxc_isolation_status_t value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("soc_dnxc_isolation_status_active", value, soc_dnxc_isolation_status_active);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("soc_dnxc_isolation_status_isolated", value, soc_dnxc_isolation_status_isolated);

    return NULL;
}



#undef BSL_LOG_MODULE
