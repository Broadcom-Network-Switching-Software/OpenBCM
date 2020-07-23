
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/switch/switch_svtag.h>
#include <shared/shrextend/shrextend_error.h>
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



#define SWITCH_SVTAG_EGRESS_SUPPORTED_FLAGS         (BCM_SWITCH_SVTAG_EGRESS_REPLACE | BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY)


#define SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32                  2

#define SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES                    2

#define SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE               1




#define SWITCH_SVTAG_UPDATE_SVTAG_FIELD(svtag,field,position) svtag |= (field << position)

#define SWITCH_SVTAG_EXTRACT_FIELD_FROM_SVTAG(svtag,pos,size) ((svtag >> pos) & (UTILEX_BITS_MASK(size-1,0)))






shr_error_e
dnx_switch_svtag_dabl_table_field_set(
    int unit,
    bcm_gport_t gport,
    uint32 entry_handle_id)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_TUNNEL(gport))
    {
        
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        uint32 lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESEM_TUNNEL_SVTAG, entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_ESEM_TUNNEL_SVTAG_ETPS_SVTAG);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_LOCAL_OUT_LIF,
                                   gport_hw_resources.local_out_lif);
    }
    else
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESEM_PORT_SVTAG, entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_ESEM_PORT_SVTAG_ETPS_SVTAG);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, gport);
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


shr_error_e
dnx_switch_svtag_egress_entry_common_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport)
{
    uint8 is_physical_port;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_switch.svtag.supported_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "SVTAG isn't supported in this device.");
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        if (gport != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "If the default entry flag BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY was used the gport value must be 0.");
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));

        if (!BCM_GPORT_IS_TUNNEL(gport) && !is_physical_port)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport type.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_switch_svtag_egress_entry_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, flags, gport));

    SHR_NULL_CHECK(svtag_info, _SHR_E_PARAM, "bcm_switch_svtag_egress_info_t");

    if (_SHR_IS_FLAG_SET(flags, ~BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
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
    bcm_switch_svtag_egress_traverse_cb trav_fn)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flags, ~BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The only supported flag for this API is the %d flag.",
                     BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL) && (trav_fn != NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "If the BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL flag is set the callback function must be null.");
    }

    if (!_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_TRAVERSE_DELETE_ALL))
    {
        SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "bcm_switch_svtag_egress_traverse_cb");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_svtag_egress_entry_delete_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, flags, gport));

    if (_SHR_IS_FLAG_SET(flags, ~BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
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
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    uint8 is_physical_port;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_common_verify(unit, flags, gport));

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

    if ((svtag_info->offset_addr != 0) && (svtag_info->accumulation))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "offset addr (%d) must be set to 0 when the accumulation is set.",
                     svtag_info->offset_addr);
    }

    if ((svtag_info->pkt_type > bcmSvtagPktTypeKayMgmt) || (svtag_info->pkt_type < bcmSvtagPktTypeNonMacsec))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported option for SVTAG packet type %d.", svtag_info->pkt_type);
    }

    if (_SHR_IS_FLAG_SET(flags, ~SWITCH_SVTAG_EGRESS_SUPPORTED_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     flags, SWITCH_SVTAG_EGRESS_SUPPORTED_FLAGS);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY) && svtag_info->accumulation != FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The accumulation field must be set to 0 in case the BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY is used");
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, gport, &is_physical_port));

    if (is_physical_port && svtag_info->accumulation != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The accumulation field must be set to 0 in case a physical port is used as a key.");
    }

exit:
    SHR_FUNC_EXIT;
}


int
bcm_dnx_switch_svtag_egress_entry_add(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_switch_svtag_egress_info_t * svtag_info)
{
    uint32 entry_handle_id;
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];
    uint32 svtag = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_add_verify(unit, flags, gport, svtag_info));

    
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

    
    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SVTAG_DEFAULT, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_DEFAULT_VALUE, INST_SINGLE, svtag);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        sal_memset(svtag_hw_entry, 0, sizeof(uint32) * SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32);

        SHR_BITCOPY_RANGE(svtag_hw_entry, dnx_data_switch.svtag.egress_svtag_hw_field_position_bits_get(unit), &svtag,
                          0, dnx_data_switch.svtag.svtag_label_size_bits_get(unit));

        SHR_BITCOPY_RANGE(svtag_hw_entry,
                          dnx_data_switch.svtag.egress_svtag_accumulation_indication_hw_field_position_bits_get(unit),
                          (uint32 *) &(svtag_info->accumulation), 0, SWITCH_SVTAG_BOOL_INDICATION_FIELD_SIZE);

        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_switch_svtag_dabl_table_field_set(unit, gport, entry_handle_id));

        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_DESTINATION, INST_SINGLE,
                                         svtag_hw_entry);
        if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_REPLACE))
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            
            if (!BCM_GPORT_IS_TUNNEL(gport))
            {
                SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, gport, TRUE));
            }
        }
    }
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
    uint32 entry_handle_id;
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_get_verify(unit, flags, gport, svtag_info));

    sal_memset(svtag_hw_entry, 0, sizeof(uint32) * SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32);
    
    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
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

        SHR_IF_ERR_EXIT(dnx_switch_svtag_dabl_table_field_set(unit, gport, entry_handle_id));

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
bcm_dnx_switch_svtag_egress_entry_delete(
    int unit,
    uint32 flags,
    bcm_gport_t gport)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_svtag_egress_entry_delete_verify(unit, flags, gport));
    
    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_SVTAG_EGRESS_DEFAULT_ENTRY))
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SVTAG_DEFAULT, &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SVTAG_DEFAULT_VALUE, INST_SINGLE, 0);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_SVTAG, &entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_switch_svtag_dabl_table_field_set(unit, gport, entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, (DBAL_COMMIT)));
        
        if (!BCM_GPORT_IS_TUNNEL(gport))
        {
            SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, gport, FALSE));
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
    uint32 entry_handle_id;
    int table_iter;
    int is_end;
    dbal_tables_e svtag_tables[SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES] =
        { DBAL_TABLE_ESEM_PORT_SVTAG, DBAL_TABLE_ESEM_TUNNEL_SVTAG };
    dbal_fields_e svtag_keys[SWITCH_SVTAG_NOF_SVTAG_DBAL_TABLES] =
        { DBAL_FIELD_TM_PORT, DBAL_FIELD_ESEM_LOCAL_OUT_LIF };
    uint32 svtag_hw_entry[SWITCH_SVTAG_HW_FIELD_SIZE_IN_UINT32];
    bcm_switch_svtag_egress_info_t svtag_info;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
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
                                    (unit, entry_handle_id, DBAL_FIELD_TM_PORT, &port));
                    
                    SHR_IF_ERR_EXIT(dnx_port_match_svtag_esem_cmd_set(unit, port, FALSE));
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

                sal_memset(&svtag_info, 0, sizeof(bcm_switch_svtag_egress_info_t));

                
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, svtag_keys[table_iter], &svtag_key));

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_ENCAP_DESTINATION, INST_SINGLE, svtag_hw_entry));

                dnx_switch_svtag_info_from_svtag_hw_entry_get(unit, svtag_hw_entry, &svtag_info);

                if (svtag_keys[table_iter] == DBAL_FIELD_ESEM_LOCAL_OUT_LIF)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL,
                                     svtag_key, &gport));
                }
                else
                {
                    gport = svtag_key;
                }

                
                SHR_IF_ERR_EXIT((*trav_fn) (unit, 0, gport, &svtag_info, user_data));

                
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_svtag_is_present_in_esem_cmd(
    int unit,
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint8 *found)
{
    dnx_esem_access_if_t esem_if;
    dbal_enum_value_field_esem_offset_e esem_entry_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_info_get
                    (unit, 0, ESEM_ACCESS_TYPE_SVTAG, &esem_if, &esem_entry_offset, NULL));

    if (esem_cmd_data->esem[esem_if].valid
        && ((esem_cmd_data->esem[esem_if].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_LIF)
            || (esem_cmd_data->esem[esem_if].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_OUT_TM_PORT))
        && (esem_cmd_data->esem[esem_if].designated_offset == esem_entry_offset))
    {
        *found = TRUE;
    }
    else
    {
        *found = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_switch_svtag_swap_set(
    int unit,
    uint32 ptc,
    int core_id,
    uint8 enable)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC, ptc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_OP_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_GLOBAL_TRIGGER);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
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
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_TRUNK(gport) && !BCM_GPORT_IS_LOCAL(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport (0x%x) type. \n", gport);
    }

    if (BCM_GPORT_IS_LOCAL(gport))
    {
        
        SHR_MAX_VERIFY(BCM_GPORT_LOCAL_GET(gport), dnx_data_port.general.nof_tm_ports_get(unit), _SHR_E_PARAM,
                       "local port %d, is out of range: %d", BCM_GPORT_LOCAL_GET(gport),
                       dnx_data_port.general.nof_tm_ports_get(unit));
    }
    
    SHR_BOOL_VERIFY(enable, _SHR_E_PARAM, "\nenable");

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_switch_svtag_port_set(
    int unit,
    bcm_gport_t gport,
    uint8 enable)
{
    bcm_port_t local_port;
    uint32 pp_port_index;
    uint32 ptc;
    uint32 nof_bytes_to_skip_first_header;
    int core_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!BCM_GPORT_IS_TRUNK(gport))
    {
        BCM_GPORT_LOCAL_SET(gport, gport);
    }
    
    SHR_IF_ERR_EXIT(dnx_switch_svtag_port_set_verify(unit, gport, enable));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_port_pp_macsec_set(unit, gport_info.internal_port_pp_info.core_id[pp_port_index],
                                               gport_info.internal_port_pp_info.pp_port[pp_port_index], enable));

    }

    
    if (BCM_GPORT_IS_TRUNK(gport))
    {
        
        int core;
        int trunk_id = BCM_GPORT_TRUNK_GET(gport);
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); ++core)
        {
            bcm_pbmp_t local_port_pbmp_per_core;

            BCM_PBMP_CLEAR(local_port_pbmp_per_core);
            SHR_IF_ERR_EXIT(dnx_trunk_local_port_bmp_get(unit, trunk_id, core, &local_port_pbmp_per_core));
            BCM_PBMP_ITER(local_port_pbmp_per_core, local_port)
            {
                SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core_id));
                SHR_IF_ERR_EXIT(dnx_switch_svtag_swap_set(unit, ptc, core_id, enable));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, BCM_GPORT_LOCAL_GET(gport), &ptc, &core_id));
        SHR_IF_ERR_EXIT(dnx_switch_svtag_swap_set(unit, ptc, core_id, enable));
    }

    
    nof_bytes_to_skip_first_header =
        (enable) ? UTILEX_TRANSLATE_BITS_TO_BYTES(dnx_data_switch.svtag.svtag_label_size_bits_get(unit)) : 0;
    
    SHR_IF_ERR_EXIT(dnx_port_first_header_size_to_skip_set
                    (unit, BCM_GPORT_IS_TRUNK(gport) ? gport : BCM_GPORT_LOCAL_GET(gport),
                     nof_bytes_to_skip_first_header));

exit:
    SHR_FUNC_EXIT;

}

