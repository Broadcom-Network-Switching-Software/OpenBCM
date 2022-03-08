/** \file wide_data_flow.c
 *  * wide data APIs for flows APIs conversion.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm/flow.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include "src/bcm/dnx/flow/porting/wide_data_flow.h"

shr_error_e
dnx_wide_data_set_flow(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    uint64 data)
{
    bcm_flow_terminator_info_t flow_term_info = { 0 };
    bcm_flow_handle_info_t flow_handle_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport, _SHR_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                    (unit, flow_handle_info.flow_id, FLOW_APP_TYPE_TERM, &flow_handle_info.flow_handle));
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;

    flow_term_info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID;
    COMPILER_64_COPY(flow_term_info.additional_data, data);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_create(unit, &flow_handle_info, &flow_term_info, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_wide_data_get_flow(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    uint64 *data)
{
    bcm_flow_terminator_info_t flow_term_info = { 0 };
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_special_fields_t special_fields = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO((*data));

    SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                    (unit, gport, _SHR_GPORT_TYPE_FLOW_LIF, DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR,
                     &flow_handle_info.flow_id));
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_gport_get
                    (unit, flow_handle_info.flow_id, FLOW_APP_TYPE_TERM, &flow_handle_info.flow_handle));

    SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_get(unit, &flow_handle_info, &flow_term_info, &special_fields));

    if (_SHR_IS_FLAG_SET(flow_term_info.valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID))
    {
        COMPILER_64_COPY(*data, flow_term_info.additional_data);
    }

exit:
    SHR_FUNC_EXIT;
}
