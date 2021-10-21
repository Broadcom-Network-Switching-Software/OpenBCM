/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#include "dnx_port_nif_ofr_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_enum_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

shr_error_e
dnx_port_ofr_info_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}
