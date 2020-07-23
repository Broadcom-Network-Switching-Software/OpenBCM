/** \file tunnel_init_ipv6.c
 *  IPV6 TUNNEL encapsulation functionality for DNX
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>

#include <bcm/types.h>
#include <bcm/tunnel.h>

#include <bcm_int/common/debug.h>

#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include "tunnel_init.h"
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

/*
 * }
 */
/*
 * MACROs and ENUMs
 * {
 */
/*
 * End of MACROs
 * }
 */
/*
 * Verify functions
 * {
 */

/*
 * return weather tunnel type is ipv6
 */
void
dnx_tunnel_init_ipv6_is_ipv6_tunnel(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_ipv6)
{
    switch (tunnel_type)
    {
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeVxlan6:
        case bcmTunnelTypeVxlan6Gpe:
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeSR6:
        case bcmTunnelTypeEthIn6:
            *is_ipv6 = TRUE;
            break;
        default:
            *is_ipv6 = FALSE;
            break;
    }
}

shr_error_e
dnx_tunnel_initiator_ipv6_data_entry_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 next_local_outlif,
    uint32 *local_outlif)
{
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

    sal_memset(&lif_table_mngr_outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    /** dip eedb data entry has no global lif  */
    lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;

    /** don't allocate lif if replace  */
    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }

    /** set outlif phase */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_encap_access_to_outlif_phase
                    (unit, tunnel->encap_access + 1, &lif_table_mngr_outlif_info.outlif_phase));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /** Set DATA fields */
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, tunnel->dip6);
    if (next_local_outlif != DNX_ALGO_GPM_LIF_INVALID)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     next_local_outlif);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_ENTRY_RESERVED, INST_SINGLE,
                                 DBAL_DEFINE_DATA_ENTRY_RESERVED_ETPS_TYPE_MSBS);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, (int *) local_outlif, &lif_table_mngr_outlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_ipv6_data_entry_table_get(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 local_outlif,
    uint32 *local_next_outlif)
{
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t outlif_info;
    uint8 is_field_valid;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

   /** read DATA_ENTRY table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, local_outlif, entry_handle_id, &outlif_info));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_DIP6, &is_field_valid));
    if (is_field_valid)
    {
        dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_DIP6, INST_SINGLE, tunnel->dip6);
    }
    else
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Illegal result type for table EEDB_DATA_ENTRY: entry with field DIP6 was expected");
    }

    /** Get next outlif pointer - an ARP entry if exist */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &is_field_valid, local_next_outlif));
    if (!is_field_valid)
    {
        /** If field not valid, next outlif should be -1 */
        *local_next_outlif = DNX_ALGO_GPM_LIF_INVALID;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
