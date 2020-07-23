/** \file tunnel_init_bfd.c
 *  BFD TUNNEL encapsulation functionality for DNX
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

/*#include <bcm/types.h>
#include <bcm/tunnel.h>
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

/* Indicate whether tunnel type bfd sip */
void
dnx_tunnel_initiator_is_bfd_extra_sip(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    bcm_ip_t dip,
    bcm_ip_t sip,
    bcm_if_t arp_out_lif,
    uint32 *is_bfd_extra_sip)
{

    if ((tunnel_type == bcmTunnelTypeIpAnyIn4) && (dip == 0) && (sip != 0) &&
        (arp_out_lif != 0) && (arp_out_lif != BCM_IF_INVALID))
    {
        *is_bfd_extra_sip = TRUE;
    }
    else
    {
        *is_bfd_extra_sip = FALSE;
    }
}

static shr_error_e
dnx_tunnel_initiator_allocate_local_lif_and_bfd_table_set(
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
    lif_table_mngr_outlif_info.table_specific_flags = DNX_ALGO_LIF_TABLE_SPECIFIC_BFD_EXTRA_SIP;

    /** set outlif phase */
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_encap_access_to_outlif_phase
                    (unit, tunnel->encap_access, &lif_table_mngr_outlif_info.outlif_phase));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    lif_table_mngr_outlif_info.global_lif = global_outlif;

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, tunnel->sip);
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
dnx_tunnel_initiator_bfd_extra_sip_table_get(
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

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, local_out_lif, entry_handle_id, &outlif_info));

    if (tunnel != NULL)
    {

        tunnel->type = bcmTunnelTypeIpAnyIn4;

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_RAW_DATA, &is_field_valid, &tunnel->sip));

        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_outlif_phase_to_encap_access
                        (unit, outlif_info.outlif_phase, &(tunnel->encap_access)));
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
        else if (!is_field_valid)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "ARP Out Lif not found");
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
dnx_tunnel_bfd_initiator_create(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t tunnel_gport,
    int global_outlif,
    uint32 arp_out_lif)
{
    dbal_tables_e dbal_table_id;
    uint32 local_outlif;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate new tunnel out-LIF - REPLACE flag is not set */
    if (!_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {

        SHR_IF_ERR_EXIT(dnx_tunnel_init_get_table_id(unit, tunnel->type, &dbal_table_id));

        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_allocate_local_lif_and_bfd_table_set
                        (unit, tunnel, dbal_table_id, (uint32) global_outlif, arp_out_lif, &local_outlif));
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
        if ((gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_IPV4_TUNNEL) ||
            (gport_hw_resources.outlif_dbal_result_type != DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_BFD_IPV4_EXTRA_SIP))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal tunnel, not related to BFD Source IP");
        }

        /** Get entry information from IPv4 Tunnel BFD SIP table */
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_bfd_extra_sip_table_get
                        (unit, gport_hw_resources.outlif_dbal_table_id, gport_hw_resources.local_out_lif,
                         &arp_out_lif, NULL));
        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_allocate_local_lif_and_bfd_table_set
                        (unit, tunnel, gport_hw_resources.outlif_dbal_table_id, (uint32) global_outlif,
                         arp_out_lif, (uint32 *) &gport_hw_resources.local_out_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_bfd_extra_sip_clear(
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
