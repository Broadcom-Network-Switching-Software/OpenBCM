/**
 * \file src/bcm/dnx/tsn/tsn_common.c
 * 
 *
 * Time Sensitive Network common functions
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
/** see header file */
shr_error_e
dnx_tsn_common_port_to_ps_mode_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_egq_ps_mode_e * ps_mode)
{
    int nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));

    switch (nof_priorities)
    {
        case (1):
            *ps_mode = DBAL_ENUM_FVAL_EGQ_PS_MODE_EGQ_PS_MODE_ONE_PRIORITY;
            break;
        case (2):
            *ps_mode = DBAL_ENUM_FVAL_EGQ_PS_MODE_EGQ_PS_MODE_TWO_PRIORITY;
            break;
        case (4):
            *ps_mode = DBAL_ENUM_FVAL_EGQ_PS_MODE_EGQ_PS_MODE_FOUR_PRIORITY;
            break;
        case (8):
            *ps_mode = DBAL_ENUM_FVAL_EGQ_PS_MODE_EGQ_PS_MODE_EIGHT_PRIORITY;
            break;
        default:
            *ps_mode = 0;
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_tsn_common_egr_tsn_interface_get(
    int unit,
    bcm_port_t port,
    int *egr_tsn_if)
{
    int egq_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));

    *egr_tsn_if = egq_if - dnx_data_egr_queuing.params.egr_if_nif_alloc_start_get(unit);

exit:
    SHR_FUNC_EXIT;
}
