/** \file algo_ptp.c
 * 
 *
 * Resource and templates needed for the ptp feature.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <bcm/types.h>
#include <bcm/stat.h>
#include <bcm_int/dnx/algo/ptp/algo_ptp.h>
#include <bcm_int/dnx/ptp/ptp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ptp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_ptp_access.h>

/** See .h file */
void
dnx_algo_ptp_tm_port_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_ptp_port_profile_info_t *profile_info = (dnx_ptp_port_profile_info_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "ptp (1588) port profile info:", NULL,
                                   NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "ptp_enabled", profile_info->ptp_enabled, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "flags", profile_info->flags, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "pkt_drop", profile_info->pkt_drop, NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "pkt_tocpu", profile_info->pkt_tocpu, NULL,
                                   "0x%x");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/**
 * \brief
 *   Initialize PTP port profile allocation manager.
 *
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_ptp_port_profile_tm_init(
    int unit)
{
    dnx_ptp_port_profile_info_t profile_info;
    dnx_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /** clear profile info */
    sal_memset(&profile_info, 0x0, sizeof(dnx_ptp_port_profile_info_t));

    /** Add all entities to template init id with given data */
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE | DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE;
    data.first_profile = 0;
    data.nof_profiles = dnx_data_ptp.general.nof_port_profiles_get(unit);
    data.max_references = dnx_data_port.general.nof_pp_ports_get(unit) * dnx_data_device.general.nof_cores_get(unit);
    data.default_profile = 0;
    data.data_size = sizeof(dnx_ptp_port_profile_info_t);
    data.default_data = &profile_info;
    sal_strncpy(data.name, DNX_PTP_PORT_TEMPLATE_NAME, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    /** create template manager */
    SHR_IF_ERR_EXIT(algo_ptp_db.ptp_port_profile.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_algo_ptp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_ptp_db.init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_ptp_port_profile_tm_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
