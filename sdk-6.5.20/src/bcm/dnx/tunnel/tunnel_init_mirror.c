/** \file tunnel_init_mirror.c
 *  IPV4 TUNNEL encapsulation functionality for DNX
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
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/port/port_esem.h>
#include "tunnel_init.h"
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

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

/* Indicate weather tunnel type is erspan or rspan */
void
dnx_tunnel_initiator_is_mirror(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_erspan_or_rpsan)
{

    if ((tunnel_type == bcmTunnelTypeErspan) || (tunnel_type == bcmTunnelTypeRspanAdvanced))
    {
        *is_erspan_or_rpsan = TRUE;
    }
    else
    {
        *is_erspan_or_rpsan = FALSE;
    }
}

shr_error_e
dnx_tunnel_initiator_allocate_local_lif_and_erspan_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    dbal_tables_e dbal_table_id,
    uint32 global_outlif,
    uint32 next_local_outlif,
    uint32 *local_outlif)
{
    int system_headers_mode;
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
    SHR_IF_ERR_EXIT(dnx_tunnel_initiator_encap_access_to_outlif_phase
                    (unit, tunnel->encap_access, &lif_table_mngr_outlif_info.outlif_phase));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    lif_table_mngr_outlif_info.global_lif = global_outlif;

    /** Set DATA fields */
    if (tunnel->flags & BCM_TUNNEL_INIT_ERSPAN_TYPE3)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWITCH_ID, INST_SINGLE, tunnel->switch_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HW_ID, INST_SINGLE, tunnel->hw_id);
    }
    else
    {
        system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            uint64 erspan_raw_header;

            /** verison=1, Session ID = span_id, other fields = 0 */
            COMPILER_64_SET(erspan_raw_header, (0x1 << 28 | tunnel->span_id), 0);
            dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, erspan_raw_header);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERSPANV2_HEADER_INDEX_17_19, INST_SINGLE,
                                         tunnel->aux_data);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE,
                                         _SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_ERSPAN_WITH_SN));
        }
    }
    if (next_local_outlif != DNX_ALGO_GPM_LIF_INVALID)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     next_local_outlif);
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, (int *) local_outlif, &lif_table_mngr_outlif_info));

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_ERSPAN_WITH_SN))
    {
        /*
         * Add OAM_LIF_DB entry
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, entry_handle_id));

        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ZERO_PADDING, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, *local_outlif);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE, tunnel->stat_cmd);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                     tunnel->counter_command_id);

        if ((tunnel->flags & BCM_TUNNEL_REPLACE) == BCM_TUNNEL_REPLACE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_allocate_local_lif_an_rspan_advanced_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    dbal_tables_e dbal_table_id,
    bcm_gport_t tunnel_gport,
    uint32 global_outlif,
    uint32 *local_outlif)
{
    uint32 old_esem_cmd_profile;
    uint32 new_esem_cmd_profile;
    uint8 remove_old_esem_cmd_profile = 0;
    uint8 set_new_esem_cmd_profile = 0;
    dnx_esem_cmd_data_t esem_cmd_data;
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info;
    dnx_port_esem_cmd_info_t esem_cmd_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    esem_cmd_info.nof_accesses = 0;

    sal_memset(&lif_table_mngr_outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    /** don't allocate lif if replace  */
    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }

    /** set outlif phase */
    lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RSPAN;
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_add
                    (unit, 0, ESEM_ACCESS_TYPE_PORT_SA, DBAL_ENUM_FVAL_ESEM_APP_DB_ID_SSP_NAMESPACE,
                     DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID, &esem_cmd_info));

    /** Allocate an esem access cmd and configure the cmd table. */
    old_esem_cmd_profile = dnx_data_esem.access_cmd.no_action_get(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, 0, DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_MAX, 0,
                                               &esem_cmd_info,
                                               (int) old_esem_cmd_profile,
                                               (int *) &new_esem_cmd_profile, &esem_cmd_data,
                                               &set_new_esem_cmd_profile, &remove_old_esem_cmd_profile));

    if (set_new_esem_cmd_profile == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, new_esem_cmd_profile, esem_cmd_data));
    }

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    lif_table_mngr_outlif_info.global_lif = global_outlif;

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, new_esem_cmd_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE, tunnel->class_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, INST_SINGLE, 0);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, (int *) local_outlif, &lif_table_mngr_outlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_erspan_table_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 local_out_lif,
    uint32 *next_local_outlif,
    bcm_tunnel_initiator_t * tunnel)
{
    uint32 entry_handle_id, ip_out_lif = 0;
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

        tunnel->type = bcmTunnelTypeErspan;
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_SWITCH_ID, &is_field_valid));

        if (is_field_valid)
        {
            /** identify erspan type3   */
            tunnel->flags |= BCM_TUNNEL_INIT_ERSPAN_TYPE3;

            dbal_entry_handle_value_field16_get(unit, entry_handle_id, DBAL_FIELD_SWITCH_ID, INST_SINGLE,
                                                &tunnel->switch_id);
        }

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_HW_ID, &is_field_valid));
        if (is_field_valid)
        {
            dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_HW_ID, INST_SINGLE, &tunnel->hw_id);
        }

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_ERSPANV2_HEADER_INDEX_17_19, &is_field_valid,
                         &tunnel->aux_data));

        SHR_IF_ERR_EXIT(dnx_tunnel_initiator_outlif_phase_to_encap_access
                        (unit, outlif_info.outlif_phase, &(tunnel->encap_access)));
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &is_field_valid, &ip_out_lif));

    if (tunnel != NULL)
    {
        if (is_field_valid && (ip_out_lif != 0))
        {
            /** map local to global LIF and set */
            bcm_gport_t next_gport = BCM_GPORT_INVALID;
            /** In case local lif is not found, return next pointer 0 */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                        ip_out_lif, &next_gport));
            if (next_gport != BCM_GPORT_INVALID)
            {
                BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel->l3_intf_id, next_gport);
            }
        }
        else if (!is_field_valid)
        {
            /** If next pointer field does not exist, l3 interface field is set in INVALID */
            tunnel->l3_intf_id = BCM_IF_INVALID;
            ip_out_lif = DNX_ALGO_GPM_LIF_INVALID;
        }
    }

    if (next_local_outlif != NULL)
    {
        *next_local_outlif = ip_out_lif;
    }

    if (tunnel != NULL)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, entry_handle_id));

        /*
         * key construction
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ZERO_PADDING, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, 0);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, local_out_lif);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

        /*
         * read table value
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE,
                                   (uint32 *) &tunnel->stat_cmd);
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE,
                                  &tunnel->counter_command_id);

        if (dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT) == _SHR_E_NONE)
        {
            tunnel->flags |= BCM_TUNNEL_INIT_ERSPAN_WITH_SN;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_rspan_advanced_table_get(
    int unit,
    uint32 local_outlif,
    bcm_tunnel_initiator_t * tunnel)
{
    uint32 entry_handle_id, class_id = 0;
    lif_table_mngr_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RSPAN, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, local_outlif, entry_handle_id, &outlif_info));

    /** Get DATA fields */
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE, &class_id);

    tunnel->type = bcmTunnelTypeRspanAdvanced;
    tunnel->class_id = (uint16) class_id;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_mirror_tunnel_initiator_create(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t tunnel_gport,
    int global_outlif,
    uint32 arp_out_lif)
{

    uint32 local_outlif;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate new tunnel out-LIF - REPLACE flag is not set */
    if (!_SHR_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_REPLACE))
    {
        dbal_tables_e dbal_table_id;

        SHR_IF_ERR_EXIT(dnx_tunnel_init_get_table_id(unit, tunnel->type, &dbal_table_id));

        if (tunnel->type == bcmTunnelTypeErspan)
        {
            /** Add entry to ERSPAN EEDB table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_allocate_local_lif_and_erspan_table_set
                            (unit, tunnel, dbal_table_id, (uint32) global_outlif, arp_out_lif, &local_outlif));
        }
        else if (tunnel->type == bcmTunnelTypeRspanAdvanced)
        {
            /** Add entry to RSPAN EEDB table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_allocate_local_lif_an_rspan_advanced_table_set
                            (unit, tunnel, dbal_table_id, tunnel_gport, (uint32) global_outlif, &local_outlif));
        }
    }
    /** Replace existing out-LIF */
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
        if (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_ERSPAN)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal to replace non IP-Tunnel OutLIF to IP Tunnel OutLIF");
        }

        if (tunnel->type == bcmTunnelTypeErspan)
        {
            /** Get entry information from ERSPAN EEDB table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_erspan_table_get
                            (unit, gport_hw_resources.outlif_dbal_table_id, gport_hw_resources.local_out_lif,
                             &arp_out_lif, NULL));
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_allocate_local_lif_and_erspan_table_set
                            (unit, tunnel, gport_hw_resources.outlif_dbal_table_id, (uint32) global_outlif,
                             arp_out_lif, (uint32 *) &gport_hw_resources.local_out_lif));
        }
        else if (tunnel->type == bcmTunnelTypeRspanAdvanced)
        {
            /** Update entry to RSPAN EEDB table */
            SHR_IF_ERR_EXIT(dnx_tunnel_initiator_allocate_local_lif_an_rspan_advanced_table_set
                            (unit, tunnel, gport_hw_resources.outlif_dbal_table_id, tunnel_gport,
                             (uint32) global_outlif, (uint32 *) &gport_hw_resources.local_out_lif));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tunnel_initiator_erspan_clear(
    int unit,
    int global_outlif,
    int local_outlif)
{
    uint32 entry_handle_id;
    uint8 counter_command_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_LIF_DB, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ZERO_PADDING, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, local_outlif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, &counter_command_id);

    if (dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT) == _SHR_E_NONE)
    {
        /*
         * Clear all entries in OAM_LIF_DB if exists
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, local_outlif, 0));

    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, global_outlif));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_tunnel_initiator_rspan_advanced_clear(
    int unit,
    int global_outlif,
    int local_outlif)
{
    uint32 esem_cmd_profile;
    uint8 remove_esem_cmd_profile;
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    remove_esem_cmd_profile = 0;

    /** Read EEDB entry from DBAL */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_RSPAN, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, local_outlif, entry_handle_id, &outlif_info));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, &esem_cmd_profile));

    /** remove ESEM cmd profile from template manager */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_free(unit, esem_cmd_profile, &remove_esem_cmd_profile));

    if (remove_esem_cmd_profile == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_clear(unit, esem_cmd_profile));
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, local_outlif, 0));

    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, global_outlif));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
