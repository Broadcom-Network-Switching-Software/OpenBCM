/** \file tunnel_init_li.c
 *  LI TUNNEL encapsulation functionality for DNX
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

#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include "tunnel_init.h"

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
 * Indicate whether tunnel type LI
 */
void
dnx_tunnel_initiator_is_li(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_li)
{

    if (tunnel_type == bcmTunnelTypeLawfulInterception)
    {
        *is_li = TRUE;
    }
    else
    {
        *is_li = FALSE;
    }
}

static shr_error_e
dnx_tunnel_initiator_allocate_local_lif_and_li_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    dbal_tables_e dbal_table_id,
    uint32 global_outlif,
    uint32 arp_out_lif,
    uint32 *local_outlif)
{
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

    sal_memset(&lif_table_mngr_outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    /** don't allocate lif if replace  */
    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }

    /** set outlif phase */
    lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2;

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    lif_table_mngr_outlif_info.global_lif = global_outlif;

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LI_CONTENT_ID, INST_SINGLE, tunnel->aux_data);
    /** LI content ID always 4ytes */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA_SIZE, INST_SINGLE, 4);
    if (arp_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, arp_out_lif);
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, (int *) local_outlif, &lif_table_mngr_outlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_li_table_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 local_out_lif,
    uint32 *next_local_outlif,
    bcm_tunnel_initiator_t * tunnel)
{
    uint32 entry_handle_id, arp_out_lif = 0;
    lif_table_mngr_outlif_info_t outlif_info;
    uint8 is_field_valid;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, local_out_lif, entry_handle_id, &outlif_info));

    if (tunnel != NULL)
    {
        /** Can be passed from API */
        tunnel->type = bcmTunnelTypeLawfulInterception;

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_LI_CONTENT_ID, &is_field_valid));

        if (is_field_valid)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_LI_CONTENT_ID, INST_SINGLE, &tunnel->aux_data));
        }
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &is_field_valid, &arp_out_lif));

    if (tunnel != NULL)
    {
        if (is_field_valid && (arp_out_lif != 0))
        {
            /** map local to global LIF and set */
            bcm_gport_t next_gport = BCM_GPORT_INVALID;
            /** In case local lif is not found, return next pointer 0 */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                        arp_out_lif, &next_gport));
            if (next_gport != BCM_GPORT_INVALID)
            {
                BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel->l3_intf_id, next_gport);
            }
        }
    }

    if (next_local_outlif != NULL)
    {
        *next_local_outlif = arp_out_lif;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_li_initiator_create(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t tunnel_gport,
    int global_outlif,
    uint32 next_local_outlif)
{
    dbal_tables_e dbal_table_id;
    uint32 local_outlif;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate new tunnel out-LIF - REPLACE flag is not set */
    if (!_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {

        SHR_IF_ERR_EXIT(dnx_tunnel_init_get_table_id(unit, tunnel->type, &dbal_table_id));

        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_allocate_local_lif_and_li_table_set
                        (unit, tunnel, dbal_table_id, (uint32) global_outlif, next_local_outlif, &local_outlif));
    }
    else
    {
        uint32 lif_get_flags;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

        /** get GPort HW resources (global and local tunnel outlif) */
        lif_get_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, tunnel_gport, lif_get_flags, &gport_hw_resources));

        /** Check that eep is an index of a Tunnel OutLIF (in SW DB) */
        if (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_DATA_ENTRY)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal tunnel, not related to LI Tunnel");
        }

        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_allocate_local_lif_and_li_table_set
                        (unit, tunnel, gport_hw_resources.outlif_dbal_table_id, (uint32) global_outlif,
                         next_local_outlif, (uint32 *) &gport_hw_resources.local_out_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_li_clear(
    int unit,
    int global_outlif,
    int local_outlif)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, local_outlif, 0));

    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, global_outlif));

exit:
    SHR_FUNC_EXIT;

}
