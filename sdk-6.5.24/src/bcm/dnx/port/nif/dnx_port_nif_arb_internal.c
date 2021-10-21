/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#include "dnx_port_nif_arb_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/port/imb/imb_ethu.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_enum_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Explicitly importing 2 cdu functions.
 */
int imb_cdu_port_mac_mode_calc(
    int unit,
    int current_mac_mode,
    int first_lane_local,
    int nof_phys,
    int *mac_mode);

int imb_cdu_mac_mode_config_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * mac_ports);

/* Static functions */
/* General */

/* dbal */
static shr_error_e dnx_port_arb_mac_mode_config_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *mac_mode);
shr_error_e
dnx_port_arb_tx_context_get(
    int unit,
    int logical_port,
    int *arb_context)
{
    dnx_algo_port_info_s port_info;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));

    /*
     * Check arbiter context type 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE))
    {
        *arb_context = ethu_info.lane_in_core;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, arb_context));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "logical port %d is invalid (not an eth nor otn).", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_port_mac_mode_config_set(
    int unit,
    bcm_port_t logical_port,
    uint32 mac_mode)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_MAC_MODE_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID, ethu_info.ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG_ID, ethu_info.mac_mode_config_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG, INST_SINGLE, mac_mode);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_mac_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *mac_mode)
{

    bcm_pbmp_t port_phys;
    int nof_phys, first_phy, first_lane_local;
    uint32 current_mac_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get the needed parameters of the port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &port_phys));
    BCM_PBMP_COUNT(port_phys, nof_phys);
    _SHR_PBMP_FIRST(port_phys, first_phy);
    first_lane_local = first_phy % dnx_data_nif.eth.mac_mode_config_lanes_nof_get(unit);

    SHR_IF_ERR_EXIT(dnx_port_arb_mac_mode_config_get(unit, logical_port, &current_mac_mode));

    SHR_IF_ERR_EXIT(imb_cdu_port_mac_mode_calc(unit, current_mac_mode, first_lane_local, nof_phys, mac_mode));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_mac_mode_config_nof_ports_get(
    int unit,
    bcm_port_t logical_port,
    int *mac_mode_config_nof_ports)
{
    bcm_pbmp_t mac_ports;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_cdu_mac_mode_config_ports_get(unit, logical_port, &mac_ports));
    BCM_PBMP_COUNT(mac_ports, *mac_mode_config_nof_ports);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_arb_link_list_sections_required_get(
    int unit,
    int speed,
    int *nof_sections)
{
    SHR_FUNC_INIT_VARS(unit);

    if (speed == 0)
    {
        *nof_sections = 0;
    }
    else if (speed <= dnx_data_nif.arb.min_port_speed_for_link_list_section_get(unit))
    {
        *nof_sections = 1;
    }
    else
    {
        *nof_sections = UTILEX_DIV_ROUND_UP(speed, dnx_data_nif.arb.link_list_sections_denominator_get(unit)) * 2;
    }

    SHR_FUNC_EXIT;
}

/*
 * General static functions
 */

/*
 * DBAL static functions
 */

static shr_error_e
dnx_port_arb_mac_mode_config_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *mac_mode)
{
    uint32 entry_handle_id;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_MAC_MODE_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU_ID, ethu_info.ethu_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG_ID, ethu_info.mac_mode_config_id);

    /*
     * set value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAC_MODE_CONFIG, INST_SINGLE, mac_mode);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
