
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

#include <soc/dnx/mdb.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/switch/switch_svtag.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm/error.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/port/port_match.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>

#define SWITCH_SVTAG_EGRESS_SUPPORTED_FLAGS         (BCM_SWITCH_SVTAG_EGRESS_REPLACE | BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY)

#define SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32                  2

#define SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES                    2

#define SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE               1

#define SWITCH_SVTAG_SUPPORTED_LIF_TYPES(gport)               (BCM_GPORT_IS_TUNNEL(gport) || BCM_GPORT_IS_VLAN_PORT(gport))

#define SWITCH_SVTAG_IS_GPORT_SUPPORT_ACCUMULATION(gport)     (BCM_GPORT_IS_TUNNEL(gport))

#define SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag,field,position) svtag |= (field << position)

#define SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,pos,size) ((svtag >> pos) & (UTILEX_BITS_MASK(size-1,0)))

shr_error_e
dnx_switch_svtag_dabl_table_field_set(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info,
    uint32 entry_handle_id)
{

    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_core_t core;
    uint32 out_tm_port;
    dbal_tables_e table_id;
    uint8 is_key;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (svtag_lookup_info->tunnel)
    {

        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        table_id = DBAL_TABLE_ESEM_TUNNEL_SVTAG;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, svtag_lookup_info->tunnel, lif_flags, &gport_hw_resources));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_ESEM_TUNNEL_SVTAG_ETPS_SVTAG);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_LOCAL_OUT_LIF,
                                   gport_hw_resources.local_out_lif);
    }
    else
    {
        table_id = DBAL_TABLE_ESEM_PORT_SVTAG;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_ESEM_PORT_SVTAG_ETPS_SVTAG);
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, table_id, DBAL_FIELD_OUT_TM_PORT, &is_key));
    if (is_key)
    {

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, svtag_lookup_info->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_get(unit, gport_info.local_port, &core, &out_tm_port));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, out_tm_port);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

void
dnx_switch_svtag_info_from_svtag_hw_entry_get(
    int unit,
    uint32 *svtag_hw_entry,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    uint32 accumulation = 0;
    uint32 svtag = 0;

    SHR_BITCOPY_RANGE(&svtag, 0, svtag_hw_entry, dnx_data_switch.svtag.egress_svtag_hw_field_position_bits_get(unit),
                      dnx_data_switch.svtag.svtag_label_size_bits_get(unit));
    SHR_BITCOPY_RANGE(&accumulation, 0, svtag_hw_entry,
                      dnx_data_switch.svtag.egress_svtag_accumulation_indication_hw_field_position_bits_get(unit),
                      SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);

    svtag_info->accumulation = accumulation & UTILEX_BITS_MASK(SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE - 1, 0);

    svtag_info->sci =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag, dnx_data_switch.svtag.egress_svtag_sci_position_bits_get(unit),
                                              dnx_data_switch.svtag.egress_svtag_sci_size_bits_get(unit));
    svtag_info->pkt_type =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,
                                              dnx_data_switch.svtag.egress_svtag_pkt_type_position_bits_get(unit),
                                              dnx_data_switch.svtag.egress_svtag_pkt_type_size_bits_get(unit));
    svtag_info->ipv6_indication =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,
                                              dnx_data_switch.
                                              svtag.egress_svtag_ipv6_indication_position_bits_get(unit),
                                              SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);
    svtag_info->offset_addr =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,
                                              dnx_data_switch.svtag.egress_svtag_offset_addr_position_bits_get(unit),
                                              dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit));
    svtag_info->signature =
        SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,
                                              dnx_data_switch.svtag.egress_svtag_signature_position_bits_get(unit),
                                              dnx_data_switch.svtag.egress_svtag_signature_size_bits_get(unit));
}

shr_error_e
dnx_switch_svtag_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_INGRESS_GENERAL_CONFIGURATION, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_SWAP_TPID_OFFSET, INST_SINGLE,
                                 dnx_data_switch.svtag.ingress_svtag_position_bytes_get(unit));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_SWAP_TPID_SIZE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_SIT_TAG_SWAP_SIZE_4_BYTES);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static void
dnx_switch_svtag_egress_entry_fill_lookup_struct(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info)
{
    svtag_lookup_info->flags = flags;

    if (SWITCH_SVTAG_SUPPORTED_LIF_TYPES(gport))
    {
        svtag_lookup_info->tunnel = gport;
    }
    else
    {
        svtag_lookup_info->port = gport;
    }

}

static shr_error_e
dnx_switch_svtag_egress_entry_common_verify(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info)
{
    uint8 is_physical_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(svtag_lookup_info, _SHR_E_PARAM, "bcm_switch_svtag_encapsulation_lookup_info_t");

    if (!dnx_data_switch.svtag.supported_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "SVTAG isn't supported in this device.");
    }

    if (_SHR_IS_FLAG_SET(svtag_lookup_info->flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        if (svtag_lookup_info->port != 0 || svtag_lookup_info->tunnel != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "If the default entry flag BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY was used the gport value must be 0.");
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, svtag_lookup_info->port, &is_physical_port));

        if (BCM_GPORT_IS_TRUNK(svtag_lookup_info->port)
            || (!SWITCH_SVTAG_SUPPORTED_LIF_TYPES(svtag_lookup_info->tunnel) && !is_physical_port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport type.");
        }
        if (!dnx_data_switch.svtag.feature_get(unit, dnx_data_switch_svtag_egress_svtag_legacy_api_support))
        {
            if (svtag_lookup_info->tunnel && !svtag_lookup_info->port)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "svtag entry for tunnel has to have port as well in this device.");
            }
        }
        else
        {
            if ((svtag_lookup_info->port) != 0 && (svtag_lookup_info->tunnel != 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "configuration of both tunnel and port is not supported in this device.");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_svtag_egress_entry_get_verify(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, svtag_lookup_info));

    SHR_NULL_CHECK(svtag_info, _SHR_E_PARAM, "bcm_switch_svtag_egress_info_t");

    if (_SHR_IS_FLAG_SET(svtag_lookup_info->flags, ~BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The only supported flag for this API is the %d flag.",
                     BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_svtag_egress_entry_traverse_verify(
    int unit,
    uint32 flags,
    switch_svtag_traverse_cb_t * trav_fn)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flags, ~BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The only supported flag for this API is the %d flag.",
                     BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL)
        && (trav_fn->egress_traverse_cb != NULL || trav_fn->encapsulation_traverse_cb != NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "If the BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL flag is set the callback function must be null.");
    }

    if (!_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL))
    {
        if (trav_fn->encapsulation_traverse_cb == NULL)
        {
            if ((!dnx_data_switch.svtag.feature_get(unit, dnx_data_switch_svtag_egress_svtag_legacy_api_support))
                || (trav_fn->egress_traverse_cb == NULL))
            {
                SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "bcm_switch_svtag_egress_traverse_cb");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_svtag_egress_entry_delete_verify(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, svtag_lookup_info));

    if (_SHR_IS_FLAG_SET(svtag_lookup_info->flags, ~BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The only supported flag for this API is the %d flag.",
                     BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_svtag_egress_entry_add_verify(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    int table_capacity = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, svtag_lookup_info));

    SHR_IF_ERR_EXIT(mdb_db_infos.
                    capacity.get(unit, dnx_data_switch.svtag.svtag_physical_db_get(unit), &table_capacity));

    if (table_capacity == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "SVTAG table is not supported for the current MDB profile.");
    }
    SHR_NULL_CHECK(svtag_info, _SHR_E_PARAM, "bcm_switch_svtag_egress_info_t");

    if (svtag_info->sci >= (1 << dnx_data_switch.svtag.egress_svtag_sci_size_bits_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SCI field %d exceeds the MAX size of %d.", svtag_info->sci,
                     (1 << dnx_data_switch.svtag.egress_svtag_sci_size_bits_get(unit)) - 1);
    }

    if (svtag_info->signature >= (1 << dnx_data_switch.svtag.egress_svtag_signature_size_bits_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "signature field %d exceeds the MAX size of %d.", svtag_info->signature,
                     (1 << dnx_data_switch.svtag.egress_svtag_signature_size_bits_get(unit)) - 1);
    }

    if (svtag_info->offset_addr >= (1 << dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "offset addr field %d exceeds the MAX size of %d.", svtag_info->offset_addr,
                     (1 << dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit)) - 1);
    }

    if ((svtag_info->accumulation) &&
        (svtag_info->offset_addr > ((1 << dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit))
                                    - dnx_data_macsec.general.sectag_offset_min_value_get(unit))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "offset addr (%d) must be set to be smaller than the the complement of the SECTAG minimum value when the accumulation is set. (value smaller than %d)",
                     svtag_info->offset_addr, ((1 << dnx_data_switch.svtag.egress_svtag_offset_addr_size_bits_get(unit))
                                               - dnx_data_macsec.general.sectag_offset_min_value_get(unit)));
    }

    if ((svtag_info->pkt_type > bcmSvtagPktTypeKayMgmt) || (svtag_info->pkt_type < bcmSvtagPktTypeNonMacsec))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported option for SVTAG packet type %d.", svtag_info->pkt_type);
    }

    if (_SHR_IS_FLAG_SET(svtag_lookup_info->flags, ~SWITCH_SVTAG_EGRESS_SUPPORTED_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     svtag_lookup_info->flags, SWITCH_SVTAG_EGRESS_SUPPORTED_FLAGS);
    }

    if (_SHR_IS_FLAG_SET(svtag_lookup_info->flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY)
        && svtag_info->accumulation != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The accumulation field must be set to 0 in case the BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY is used");
    }

    if (!SWITCH_SVTAG_IS_GPORT_SUPPORT_ACCUMULATION(svtag_lookup_info->tunnel) && svtag_info->accumulation)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The accumulation mode field can be set to 1 only for the case where the key is a tunnel.");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_svtag_egress_entry_add_internal(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    uint32 entry_handle_id;
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];
    uint32 svtag = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_add_verify(unit, svtag_lookup_info, svtag_info));

    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->sci,
                                    dnx_data_switch.svtag.egress_svtag_sci_position_bits_get(unit));
    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->pkt_type,
                                    dnx_data_switch.svtag.egress_svtag_pkt_type_position_bits_get(unit));
    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->ipv6_indication,
                                    dnx_data_switch.svtag.egress_svtag_ipv6_indication_position_bits_get(unit));
    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->offset_addr,
                                    dnx_data_switch.svtag.egress_svtag_offset_addr_position_bits_get(unit));
    SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag, svtag_info->signature,
                                    dnx_data_switch.svtag.egress_svtag_signature_position_bits_get(unit));

    if (_SHR_IS_FLAG_SET(svtag_lookup_info->flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SVTAG_DEFAULT, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_DEFAULT_VALUE, INST_SINGLE, svtag);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        dbal_entry_action_flags_e dbal_action_flag = DBAL_COMMIT_UPDATE;
        uint32 accumulation = svtag_info->accumulation;

        sal_memset(svtag_hw_entry, 0, sizeof(uint32) * SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32);

        SHR_BITCOPY_RANGE(svtag_hw_entry, dnx_data_switch.svtag.egress_svtag_hw_field_position_bits_get(unit), &svtag,
                          0, dnx_data_switch.svtag.svtag_label_size_bits_get(unit));

        SHR_BITCOPY_RANGE(svtag_hw_entry,
                          dnx_data_switch.svtag.egress_svtag_accumulation_indication_hw_field_position_bits_get(unit),
                          &accumulation, 0, SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_switch_svtag_dabl_table_field_set(unit, svtag_lookup_info, entry_handle_id));

        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_DESTINATION, INST_SINGLE,
                                         svtag_hw_entry);
        if (!_SHR_IS_FLAG_SET(svtag_lookup_info->flags, BCM_SWITCH_SVTAG_EGRESS_REPLACE))
        {
            dbal_action_flag = DBAL_COMMIT;

            if (!SWITCH_SVTAG_SUPPORTED_LIF_TYPES(svtag_lookup_info->tunnel))
            {
                SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, svtag_lookup_info->port, TRUE));
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_action_flag));

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

int
bcm_dnx_switch_svtag_egress_entry_add(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_egress_info_t * svtag_info)
{

    bcm_switch_svtag_encapsulation_lookup_info_t svtag_lookup_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_data_switch.svtag.feature_get(unit, dnx_data_switch_svtag_egress_svtag_legacy_api_support))
    {
        SHR_IF_ERR_EXIT(BCM_E_UNAVAIL);
    }
    sal_memset(&svtag_lookup_info, 0, sizeof(bcm_switch_svtag_encapsulation_lookup_info_t));
    dnx_switch_svtag_egress_entry_fill_lookup_struct(unit, flags, gport, &svtag_lookup_info);
    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_add_internal(unit, &svtag_lookup_info, svtag_info));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_svtag_egress_entry_get_internal(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    uint32 entry_handle_id;
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_get_verify(unit, svtag_lookup_info, svtag_info));

    sal_memset(svtag_hw_entry, 0, sizeof(uint32) * SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32);

    if (_SHR_IS_FLAG_SET(svtag_lookup_info->flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        uint32 svtag = 0;
        uint32 accumulation = 0;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SVTAG_DEFAULT, &entry_handle_id));

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SVTAG_DEFAULT_VALUE, INST_SINGLE, &svtag);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        SHR_BITCOPY_RANGE(svtag_hw_entry, dnx_data_switch.svtag.egress_svtag_hw_field_position_bits_get(unit), &svtag,
                          0, dnx_data_switch.svtag.svtag_label_size_bits_get(unit));

        SHR_BITCOPY_RANGE(svtag_hw_entry,
                          dnx_data_switch.svtag.egress_svtag_accumulation_indication_hw_field_position_bits_get(unit),
                          &accumulation, 0, SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);
    }
    else
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_switch_svtag_dabl_table_field_set(unit, svtag_lookup_info, entry_handle_id));

        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_ENCAP_DESTINATION, INST_SINGLE,
                                       svtag_hw_entry);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    dnx_switch_svtag_info_from_svtag_hw_entry_get(unit, svtag_hw_entry, svtag_info);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_switch_svtag_egress_entry_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    bcm_switch_svtag_encapsulation_lookup_info_t svtag_lookup_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_data_switch.svtag.feature_get(unit, dnx_data_switch_svtag_egress_svtag_legacy_api_support))
    {
        SHR_IF_ERR_EXIT(BCM_E_UNAVAIL);
    }
    sal_memset(&svtag_lookup_info, 0, sizeof(bcm_switch_svtag_encapsulation_lookup_info_t));
    dnx_switch_svtag_egress_entry_fill_lookup_struct(unit, flags, gport, &svtag_lookup_info);
    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_get_internal(unit, &svtag_lookup_info, svtag_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_svtag_egress_entry_delete_internal(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_delete_verify(unit, svtag_lookup_info));

    if (_SHR_IS_FLAG_SET(svtag_lookup_info->flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SVTAG_DEFAULT, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_DEFAULT_VALUE, INST_SINGLE, 0);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_switch_svtag_dabl_table_field_set(unit, svtag_lookup_info, entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, (DBAL_COMMIT)));

        if (!BCM_GPORT_IS_TUNNEL(svtag_lookup_info->tunnel))
        {
            SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, svtag_lookup_info->port, FALSE));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_switch_svtag_egress_entry_delete(
    int unit,
    uint32 flags,
    bcm_gport_t gport)
{

    bcm_switch_svtag_encapsulation_lookup_info_t svtag_lookup_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_data_switch.svtag.feature_get(unit, dnx_data_switch_svtag_egress_svtag_legacy_api_support))
    {
        SHR_IF_ERR_EXIT(BCM_E_UNAVAIL);
    }
    sal_memset(&svtag_lookup_info, 0, sizeof(bcm_switch_svtag_encapsulation_lookup_info_t));
    dnx_switch_svtag_egress_entry_fill_lookup_struct(unit, flags, gport, &svtag_lookup_info);
    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_delete_internal(unit, &svtag_lookup_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_switch_svtag_egress_entry_traverse_invoke_cb_function(
    int unit,
    uint32 flags,
    uint8 is_legacy,
    bcm_gport_t gport_port,
    bcm_gport_t gport_tunnel,
    bcm_switch_svtag_egress_info_t * svtag_info,
    switch_svtag_traverse_cb_t * trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (is_legacy)
    {
        bcm_gport_t gport = gport_tunnel ? gport_tunnel : gport_port;
        SHR_IF_ERR_EXIT((trav_fn->egress_traverse_cb) (unit, 0, gport, svtag_info, user_data));
    }
    else
    {
        bcm_switch_svtag_encapsulation_lookup_info_t svtag_lookup_info;
        svtag_lookup_info.flags = flags;
        svtag_lookup_info.port = gport_port;
        svtag_lookup_info.tunnel = gport_tunnel;
        SHR_IF_ERR_EXIT((trav_fn->encapsulation_traverse_cb) (unit, &svtag_lookup_info, svtag_info, user_data));
    }
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_switch_svtag_egress_entry_traverse_internal(
    int unit,
    uint32 flags,
    uint8 is_legacy,
    switch_svtag_traverse_cb_t * trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int table_iter;
    int is_end;
    dbal_tables_e svtag_tables[SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES] =
        { DBAL_TABLE_ESEM_PORT_SVTAG, DBAL_TABLE_ESEM_TUNNEL_SVTAG };
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];
    bcm_switch_svtag_egress_info_t svtag_info;
    bcm_port_t logical_port;
    int out_tm_valid;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_traverse_verify(unit, flags, trav_fn));

    for (table_iter = 0; table_iter < SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES; table_iter++)
    {
        if ((_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL)))
        {

            if (svtag_tables[table_iter] == DBAL_TABLE_ESEM_PORT_SVTAG)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));
                SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                while (!is_end)
                {
                    uint32 port;

                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, &port));

                    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_ref_count_get(unit, core_idx, port, &out_tm_valid));
                        if (out_tm_valid)
                        {

                            SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_to_logical_get(unit, core_idx, port, &logical_port));
                            break;
                        }
                    }

                    SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, logical_port, FALSE));
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                }
            }
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, svtag_tables[table_iter]));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, svtag_tables[table_iter], &entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            while (!is_end)
            {
                uint32 svtag_key;
                uint8 is_key;
                bcm_gport_t gport_tunnel = 0;
                bcm_gport_t gport_port = 0;

                sal_memset(&svtag_info, 0, sizeof(bcm_switch_svtag_egress_info_t));

                SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get
                                (unit, svtag_tables[table_iter], DBAL_FIELD_OUT_TM_PORT, &is_key));
                if (is_key)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, &svtag_key));

                    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_ref_count_get
                                        (unit, core_idx, svtag_key, &out_tm_valid));
                        if (out_tm_valid)
                        {

                            SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_to_logical_get
                                            (unit, core_idx, svtag_key, &logical_port));
                            break;
                        }
                    }

                    BCM_GPORT_LOCAL_SET(gport_port, logical_port);
                }
                SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get
                                (unit, svtag_tables[table_iter], DBAL_FIELD_ESEM_LOCAL_OUT_LIF, &is_key));

                if (is_key)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_ESEM_LOCAL_OUT_LIF, &svtag_key));

                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL,
                                     svtag_key, &gport_tunnel));
                }
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_ENCAP_DESTINATION, INST_SINGLE, svtag_hw_entry));

                dnx_switch_svtag_info_from_svtag_hw_entry_get(unit, svtag_hw_entry, &svtag_info);

                SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_traverse_invoke_cb_function
                                (unit, 0, is_legacy, gport_port, gport_tunnel, &svtag_info, trav_fn, user_data));

                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

int
bcm_dnx_switch_svtag_egress_entry_traverse(
    int unit,
    uint32 flags,
    bcm_switch_svtag_egress_traverse_cb trav_fn,
    void *user_data)
{
    switch_svtag_traverse_cb_t egress_svtag_cb;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    egress_svtag_cb.egress_traverse_cb = trav_fn;

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_traverse_internal(unit, flags, TRUE, &egress_svtag_cb, user_data));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_switch_svtag_encapsulation_add(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_add_internal(unit, svtag_lookup_info, svtag_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_switch_svtag_encapsulation_get(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_get_internal(unit, svtag_lookup_info, svtag_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_switch_svtag_encapsulation_delete(
    int unit,
    bcm_switch_svtag_encapsulation_lookup_info_t * svtag_lookup_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_delete_internal(unit, svtag_lookup_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_switch_svtag_encapsulation_traverse(
    int unit,
    uint32 flags,
    bcm_switch_svtag_encapsulation_traverse_cb trav_fn,
    void *user_data)
{
    switch_svtag_traverse_cb_t egress_svtag_cb;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    egress_svtag_cb.encapsulation_traverse_cb = trav_fn;
    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_traverse_internal(unit, flags, FALSE, &egress_svtag_cb, user_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_svtag_is_present_in_esem_cmd(
    int unit,
    uint32 esem_cmd_profile,
    int *app_db_id,
    uint8 *found)
{
    dnx_esem_access_if_t esem_if;
    dbal_enum_value_field_esem_offset_e esem_entry_offset;
    dnx_esem_cmd_data_t esem_cmd_data;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, esem_cmd_profile, &esem_cmd_data, NULL));

    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_info_get
                    (unit, 0, ESEM_ACCESS_TYPE_SVTAG, 1, &esem_if, &esem_entry_offset, NULL));

    if (esem_cmd_data.esem[esem_if].valid
        && ((esem_cmd_data.esem[esem_if].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_LIF)
            || (esem_cmd_data.esem[esem_if].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_TM_PORT))
        && (esem_cmd_data.esem[esem_if].designated_offset == esem_entry_offset))
    {
        *found = TRUE;
        *app_db_id = esem_cmd_data.esem[esem_if].app_db_id;
    }
    else
    {
        *found = FALSE;
        *app_db_id = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_svtag_swap_set(
    int unit,
    bcm_port_t local_port,
    uint32 ptc,
    int core_id,
    uint8 enable)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC, ptc);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_ENABLE, INST_SINGLE, enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_OP_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_GLOBAL_TRIGGER);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_svtag_local_port_enable(
    int unit,
    bcm_port_t local_port,
    uint8 enable)
{
    uint32 ptc;
    int core_id;
    uint32 entry_handle_id, nof_bytes_to_skip_first_header;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PTC, ptc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT2, INST_SINGLE,
                                 enable ? DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_IPSEC_MACSEC :
                                 DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT3, INST_SINGLE,
                                 enable ? DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_IPSEC_MACSEC :
                                 DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_DEFAULT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT4, INST_SINGLE,
                                 enable ? DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT4_IPSEC_MACSEC :
                                 DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT4_DEFAULT);
    if (dnx_data_pp.stages.is_vtt5_supported_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC_CS_PROFILE_VTT5, INST_SINGLE,
                                     enable ? DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT5_IPSEC_MACSEC :
                                     DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT5_DEFAULT);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_switch_svtag_swap_set(unit, local_port, ptc, core_id, enable));

    nof_bytes_to_skip_first_header =
        (enable) ? UTILEX_TRANSLATE_BITS_TO_BYTES(dnx_data_switch.svtag.svtag_label_size_bits_get(unit)) : 0;

    SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_set(unit, local_port, nof_bytes_to_skip_first_header));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_svtag_port_set_verify(
    int unit,
    bcm_gport_t gport,
    uint8 enable)
{
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_SET(gport))
    {
        if (dnx_algo_port_valid_verify(unit, gport) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid local port (0x%x).\n", gport);
        }
    }
    else if (!BCM_GPORT_IS_TRUNK(gport) && !BCM_GPORT_IS_LOCAL(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport (0x%x) type.\n", gport);
    }

    if (!BCM_GPORT_IS_TRUNK(gport))
    {
        if (BCM_GPORT_IS_LOCAL(gport))
        {
            logical_port = BCM_GPORT_LOCAL_GET(gport);
        }
        else
        {
            logical_port = gport;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        if (!DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expected ingress TM port (%d)\n", logical_port);

        }
        if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expected egress TM port (%d)\n", logical_port);
        }
    }

    SHR_BOOL_VERIFY(enable, _SHR_E_PARAM, "\nenable");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_svtag_macsec_set(
    int unit,
    bcm_gport_t gport,
    int enable)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    {
        uint32 pp_port_index;

        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            dbal_enum_value_field_etpp_forward_port_cs_var_e forward_cs;
            dbal_enum_value_field_etpp_enc1_port_cs_var_e enc_1_cpp;
            dbal_enum_value_field_etpp_enc5_port_cs_var_e enc_5_cpp;
            if (enable)
            {
                forward_cs = DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_MACSEC;
                enc_1_cpp = DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_MACSEC;
                enc_5_cpp = DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_MACSEC;
            }
            else
            {
                forward_cs = DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_ETHERNET_TYPICAL;
                enc_1_cpp = DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_ETHERNET_TYPICAL;
                enc_5_cpp = DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_ETHERNET_TYPICAL;
            }

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                       gport_info.internal_port_pp_info.pp_port[pp_port_index]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_info.internal_port_pp_info.core_id[pp_port_index]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                         forward_cs);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENC_1_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                         enc_1_cpp);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENC_5_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                         enc_5_cpp);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_svtag_macsec_get(
    int unit,
    bcm_gport_t gport,
    uint8 *enable)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *enable = FALSE;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    {
        dbal_enum_value_field_etpp_forward_port_cs_var_e forward_cs;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[0]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[0]);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FORWARDING_CONTEXT_PORT_PROFILE, INST_SINGLE, &forward_cs));

        if (forward_cs == DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_MACSEC)
        {
            *enable = TRUE;
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_svtag_port_set(
    int unit,
    bcm_gport_t gport,
    uint8 enable)
{
    bcm_port_t local_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_port_set_verify(unit, gport, enable));

    SHR_IF_ERR_EXIT(dnx_switch_svtag_macsec_set(unit, gport, enable));

    if (BCM_GPORT_IS_TRUNK(gport))
    {

        int core;
        int trunk_id = BCM_GPORT_TRUNK_GET(gport);
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            bcm_pbmp_t local_port_pbmp_per_core;
            BCM_PBMP_CLEAR(local_port_pbmp_per_core);
            SHR_IF_ERR_EXIT(dnx_trunk_local_port_bmp_get(unit, trunk_id, core, &local_port_pbmp_per_core));
            BCM_PBMP_ITER(local_port_pbmp_per_core, local_port)
            {
                SHR_IF_ERR_EXIT(dnx_switch_svtag_local_port_enable(unit, local_port, enable));

            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_switch_svtag_local_port_enable
                        (unit, (BCM_GPORT_IS_LOCAL(gport) ? BCM_GPORT_LOCAL_GET(gport) : gport), enable));
    }

    SHR_IF_ERR_EXIT(dnx_port_pp_estimate_bta_set(unit, gport,
                                                 ((enable ==
                                                   TRUE) ? BITS2BYTES(dnx_data_switch.
                                                                      svtag.svtag_label_size_bits_get(unit)) : 0)));
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_switch_svtag_port_get(
    int unit,
    bcm_gport_t gport,
    uint8 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_port_set_verify(unit, gport, *enable));

    SHR_IF_ERR_EXIT(dnx_switch_svtag_macsec_get(unit, gport, enable));

exit:
    SHR_FUNC_EXIT;

}
