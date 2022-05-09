/** \file bfd_oamp.c
 * 
 *
 * BFD OAMP table access procedures for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BFD

 /*
  * Include files.
  * {
  */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/bfd.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <include/bcm_int/dnx/algo/oam/algo_oam.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/bfd/algo_bfd.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/oam_oamp_crc.h>
#include <src/bcm/dnx/oam/oamp_v1/oamp_v1_pe_infra.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_bfd_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** Bit offset for TC field in API field */
#define TC_IN_API_OFFSET 2
/** Size of TC field in bits */
#define TC_IN_API_NOF_BITS 3

/** All flags relevant to punt profile */
#define PUNT_PROFILE_FLAGS (\
BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE | \
BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE | \
BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE | \
BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE | \
BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE | \
BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE | \
BCM_BFD_ENDPOINT_RDI_ON_LOC | \
BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR)

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * Global and Static
 * {
 */

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_mep_type_parsing(
    int unit,
    bcm_bfd_tunnel_type_t bfd_api_mep_type,
    uint32 in_flags,
    uint32 flags2,
    uint32 label_ttl,
    bcm_ip_t dst_ip_addr,
    dbal_enum_value_field_oamp_mep_type_e * bfd_dbal_mep_type,
    uint32 *out_flags,
    uint8 *ach_sel)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (bfd_api_mep_type)
    {
        case bcmBFDTunnelTypeUdp:
            if (_SHR_IS_FLAG_SET(in_flags, BCM_BFD_ENDPOINT_MULTIHOP) ||
                _SHR_IS_FLAG_SET(in_flags, BCM_BFD_ECHO) ||
                _SHR_IS_FLAG_SET(flags2, BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR))
            {
                *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP;
            }
            else
            {
                *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP;
            }
            break;

        case bcmBFDTunnelTypeMpls:
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS;
            break;

        case bcmBFDTunnelTypeMplsTpCcCv:
            if (dst_ip_addr != 0)
            {
                *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP;
            }
            else
            {
                *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS;
            }
            break;

        case bcmBFDTunnelTypePweControlWord:
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
            *out_flags |= DNX_OAMP_BFD_MEP_ACH;
            *ach_sel = 0;
            break;

        case bcmBFDTunnelTypePweRouterAlert:
            *out_flags |= DNX_OAMP_BFD_MEP_ROUTER_ALERT;
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
            *ach_sel = 0;
            if (_SHR_IS_FLAG_SET(in_flags, BCM_BFD_ENDPOINT_PWE_ACH))
            {
                *out_flags |= DNX_OAMP_BFD_MEP_ACH;
            }
            break;

        case bcmBFDTunnelTypePweTtl:
            if (label_ttl)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for type bcmBFDTunnelTypePweTtl, TTL must be 1.\n");
            }
            if (_SHR_IS_FLAG_SET(in_flags, BCM_BFD_ENDPOINT_PWE_ACH))
            {
                *out_flags |= DNX_OAMP_BFD_MEP_ACH;
                *ach_sel = 0;
            }
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
            break;

        case bcmBFDTunnelTypeMplsTpCc:
        case bcmBFDTunnelTypePweGal:
            *out_flags |= DNX_OAMP_BFD_MEP_ACH;
            *out_flags |= DNX_OAMP_BFD_MEP_GAL;
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;
            *ach_sel = 1;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: tunnel type %d not supported\n", bfd_api_mep_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Map Diagnostic Code(5 bits) to diag-remote(4 bits).Profile 9,10 are mapped to values 27,28 respectively,the rest are mapped 1 to 1.
 *
 */

int
dnx_bfd_endpoint_map_diag(
    int diag)
{
    /*
     * map 27,28 to rmep 9,10
     */
    return diag == 27 ? 9 : diag == 28 ? 10 : diag;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
void
dnx_bfd_endpoint_db_build_param_for_oamp_rmep_db(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data,
    dnx_oam_oamp_rmep_db_entry_t * entry_values)
{
    int entry_value = dnx_bfd_endpoint_map_diag(endpoint_info->remote_diag);
    entry_values->oam_id = rmep_hw_write_data->endpoint_id;

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME))
    {
        /**
         * Explicit detection time flag is set: get detection time
         * value from "bfd_detection time" field.
         */
        entry_values->period = endpoint_info->bfd_detection_time;
    }
    else
    {
        /**
         * Explicit detection time flag is clear: get detection time
         * value by multiplying local_min_rx by remote_detect_mult.
         */
        entry_values->period = endpoint_info->local_min_rx * endpoint_info->remote_detect_mult;
    }
    entry_values->loc_clear_threshold = endpoint_info->loc_clear_threshold;
    entry_values->punt_profile = rmep_hw_write_data->punt_profile_index;
    entry_values->punt_good_profile = rmep_hw_write_data->punt_good_profile.new_punt_good_profile;
    entry_values->punt_next_good_packet =
        ((endpoint_info->flags2 & BCM_BFD_ENDPOINT_FLAGS2_PUNT_NEXT_GOOD_PACKET) ? 1 : 0);

    entry_values->rmep_state.bfd_state.detect_multiplier = endpoint_info->remote_detect_mult;
    entry_values->rmep_state.bfd_state.diag_profile = entry_value;

    if (DNX_OAMP_IS_V1(unit))
    {
        /** For Jericho2 use flags profile */
        entry_values->rmep_state.bfd_state.flags = BFD_FLAGS_VAL_TO_PROFILE(endpoint_info->remote_flags);
    }
    else
    {
        entry_values->rmep_state.bfd_state.flags = endpoint_info->remote_flags;
    }
    entry_values->rmep_state.bfd_state.state = endpoint_info->remote_state;

    entry_values->last_ccm_lifetime_valid_on_create =
        _SHR_IS_FLAG_SET(endpoint_info->faults, BCM_BFD_ENDPOINT_REMOTE_LOC);
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_endpoint_db_write_tx_rate_to_oamp_hw(
    int unit,
    uint32 entry_index,
    uint32 tx_period_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_TX_RATES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_TX_RATE_PROFILE, entry_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RATE_VALUE, INST_SINGLE, tx_period_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
void
dnx_bfd_endpoint_db_analyze_param_from_oamp_rmep_db(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_rmep_db_data_t * rmep_sw_read_data,
    const dnx_oam_oamp_rmep_db_entry_t * entry_values)
{
    endpoint_info->loc_clear_threshold = entry_values->loc_clear_threshold;
    rmep_sw_read_data->punt_profile_index = entry_values->punt_profile;
    rmep_sw_read_data->punt_good_profile.old_punt_good_profile = entry_values->punt_good_profile;

    endpoint_info->remote_detect_mult = entry_values->rmep_state.bfd_state.detect_multiplier;
    endpoint_info->remote_diag = entry_values->rmep_state.bfd_state.diag_profile;
    if (DNX_OAMP_IS_V1(unit))
    {
        /** For Jericho2 use flags profile */
        endpoint_info->remote_flags = BFD_FLAGS_PROFILE_TO_VAL(entry_values->rmep_state.bfd_state.flags);
    }
    else
    {
        endpoint_info->remote_flags = entry_values->rmep_state.bfd_state.flags;
    }
    endpoint_info->remote_state = entry_values->rmep_state.bfd_state.state;

    endpoint_info->bfd_detection_time = entry_values->period;
    endpoint_info->faults |= entry_values->loc ? BCM_BFD_ENDPOINT_REMOTE_LOC : 0;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
int
dnx_bfd_get_bfd_period(
    int unit,
    uint16 endpoint_id,
    uint16 tx_rate,
    uint32 ccm_count)
{
    uint32 full_entry_threshold;

    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);

    if (tx_rate == 0)
    {
        /**
         * When this value is configured using bcm_bfd_endpoint_create,
         * it means that no BFD packets should be transmitted for this
         * endpoint, so CCM (BFD) count is set to 1.  However, if the
         * user writes this value directly to the register and doesn't
         * set CCM count, Tx packets are transmitted at maximum rate.
         */
        if ((ccm_count >= 2) || ((endpoint_id >= full_entry_threshold) && (ccm_count == 1)))
        {
            /** In this case, there will be no packets transmitted */
            return 0;
        }
    }

    /** If it's not the "no transmission" case, use this conversion */
    return BCM_OAMP_MEP_DB_SCAN_RATE_TO_SW_ENTRY_PERIOD(unit, tx_rate);
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_itmh_pph_priority_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    uint32 itmh_pph_priority_index)
{
    int dummy;
    dnx_oam_itmh_priority_t itmh_priority_data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_itmh_pph_priority_profile.profile_data_get
                    (unit, itmh_pph_priority_index, &dummy, &itmh_priority_data));
    endpoint_info->int_pri = itmh_priority_data.dp | (itmh_priority_data.tc << TC_IN_API_OFFSET);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_itmh_pph_priority_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    uint32 *itmh_pph_priority_write,
    uint32 *itmh_pph_priority_index,
    uint32 *tc,
    uint32 *dp,
    uint32 *qos)
{
    shr_error_e rv;
    int itmh_priority_index;
    dnx_oam_itmh_priority_t itmh_priority_data;
    uint8 write_hw;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&itmh_priority_data, 0, sizeof(dnx_oam_itmh_priority_t));

    itmh_priority_data.tc =
        UTILEX_GET_BITS_RANGE(endpoint_info->int_pri, TC_IN_API_OFFSET + TC_IN_API_NOF_BITS - 1, TC_IN_API_OFFSET);
    itmh_priority_data.dp = UTILEX_GET_BITS_RANGE(endpoint_info->int_pri, TC_IN_API_OFFSET - 1, 0);
    rv = algo_oam_db.oamp_itmh_pph_priority_profile.allocate_single
        (unit, NO_FLAGS, &itmh_priority_data, NULL, &itmh_priority_index, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free TC/DP profile entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    *itmh_pph_priority_write = write_hw;
    *itmh_pph_priority_index = itmh_priority_index;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        *tc = itmh_priority_data.tc;
        *dp = itmh_priority_data.dp;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_mep_type_classification_set(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{

    uint32 lif_id_nof_bits, lif_id = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    SHR_FUNC_INIT_VARS(unit);

    lif_id_nof_bits = dnx_data_bfd.general.nof_bits_your_discr_to_lif_get(unit);
    /** Classification is the same for accelerated and non-accelerated endpoints */
    if (BFD_MEP_TYPE_CLASSIFIED_BY_DISCR(endpoint_info->type))
    {
        /** Only these bits are used as lookup key - do not use all available bits */
        lif_id = UTILEX_GET_BITS_RANGE(endpoint_info->local_discr, lif_id_nof_bits - 1, 0);
        if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_WITH_ID))
        {
            endpoint_info->id = lif_id;
            endpoint_info->id |= (SAL_BIT(BFD_MEP_ID_DISCR_CLASS) | SAL_BIT(BFD_MEP_ID_NON_ACC_BIT));
        }
        if (_SHR_IS_FLAG_SET
            (hw_write_data->bfd_sw_state_flags, DBAL_DEFINE_BFD_ENDPOINT_SW_STATE_FLAGS_BFD_SERVER_CLIENT))
        {
            if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_WITH_ID))
            {
                /** For server-client, bits 17 and 16 need to be in the id */
                endpoint_info->id = lif_id;
            }
        }
        hw_write_data->gathered_data.lif_key_prefix = DBAL_ENUM_FVAL_OAM_KEY_PREFIX_BFD;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, endpoint_info->gport,
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF |
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT,
                                                           &gport_hw_resources));
        lif_id = gport_hw_resources.global_in_lif;
        if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_BFD_ENDPOINT_WITH_ID))
        {
            endpoint_info->id = lif_id;
            endpoint_info->id |= SAL_BIT(BFD_MEP_ID_NON_ACC_BIT);
        }
        hw_write_data->gathered_data.lif_key_prefix = DBAL_ENUM_FVAL_OAM_KEY_PREFIX_HIGH;
    }

    hw_write_data->endpoint_id = lif_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the OAMP BFD local
 *          minimum echo value from the template
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_bfd_oamp_local_min_echo_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info)
{
    uint32 local_min_echo;
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_local_min_echo.profile_data_get(unit, 0, &dummy, &local_min_echo));

    endpoint_info->local_min_echo = local_min_echo;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_local_min_echo_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    uint32 *hw_data_oamp_local_min_echo_write,
    uint32 *hw_data_oamp_local_min_echo)
{
    shr_error_e rv;
    int dummy;
    uint8 write_hw;
    uint32 oamp_local_min_echo, new_oamp_local_min_echo;
    SHR_FUNC_INIT_VARS(unit);

    /**
     *  For all accelerated BFD endpoints, local min echo in
     *  generated Tx packets is read from one HW register
     */
    new_oamp_local_min_echo = endpoint_info->local_min_echo;

    /**
     *  The BFD OAMP local min echo "resource" is treated as
     *  a "template"
     */
    rv = algo_bfd_db.bfd_local_min_echo.allocate_single
        (unit, NO_FLAGS, &new_oamp_local_min_echo, NULL, &dummy, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_IF_ERR_EXIT(algo_bfd_db.bfd_local_min_echo.profile_data_get
                        (unit, DISCR_PROF, &dummy, &oamp_local_min_echo));
        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                     "Error: the BFD OAMP local min echo has been set.\n "
                     "The value is 0x%04X.\nThe local min echo "
                     "provided was 0x%08X.", oamp_local_min_echo, new_oamp_local_min_echo);
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    *hw_data_oamp_local_min_echo_write = write_hw;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        *hw_data_oamp_local_min_echo = new_oamp_local_min_echo;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_seamless_udp_sport_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_endpoint_data_t * hw_write_data)
{
    int dummy;
    uint16 port;
    uint8 write_hw;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    port = endpoint_info->udp_src_port;
    rv = algo_bfd_db.bfd_seamless_udp_sport.allocate_single(unit, NO_FLAGS, &port, NULL, &dummy, &write_hw);
    if (rv == _SHR_E_FULL)
    {
        /** Full means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: value provided for seamless BFD SRC does not match existing value.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    hw_write_data->gathered_data.sbfd_udp_src_port_write = write_hw;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        hw_write_data->gathered_data.sbfd_udp_src_port = port;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_endpoint_rmep_profiles_delete(
    int unit,
    const dnx_oam_oamp_rmep_db_entry_t * rmep_entry_values,
    uint8 punt_profile_delete,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (punt_profile_delete)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_PUNT_PROFILE_TBL, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, rmep_entry_values->punt_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    /*
     * Clear punt good profile data from HW
     */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_good_profile_hw_configure(unit, punt_good_hw_write_data));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_delete_rmep_data_from_templates(
    int unit,
    const dnx_oam_oamp_rmep_db_entry_t * rmep_entry_values,
    uint8 *punt_profile_delete,
    dnx_oam_oamp_punt_good_profile_temp_data_t * punt_good_hw_write_data)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.free_single
                    (unit, rmep_entry_values->punt_profile, punt_profile_delete));

    /*
     * Free punt good profile from SW
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_free
                    (unit, rmep_entry_values->punt_good_profile, punt_good_hw_write_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function prepares to add an OAMP punt profile
 *        entry to the punt profile hardware table, or if that
 *        entry already exists, increases the relevant counter.
 *        This is achieved by using the template "OAM punt
 *        profile."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] rmep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP RMEP DB.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_bfd_oamp_punt_profile_sw_update(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data)
{
    shr_error_e rv;
    dnx_oam_oamp_punt_event_profile_t punt_data;
    int punt_profile_index;
    uint8 write_hw;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Since 'punt_data' is eventually used as key (MULTI_SET_KEY) and
     * since there are 'alignment holes' in 'dnx_oam_oamp_punt_event_profile_t',
     * it must be, first, be fully initialized (each single byte).
     */
    sal_memset(&punt_data, 0, sizeof(punt_data));
    punt_data.punt_rate = (endpoint_info->sampling_ratio > 0) ? endpoint_info->sampling_ratio - 1 : 0;
    punt_data.punt_enable = (endpoint_info->sampling_ratio > 0) ? 1 : 0;
    punt_data.rx_state_update_en = (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
        (endpoint_info->flags & BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE) ? 1 :
        (endpoint_info->flags & BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE) ? 3 : 2;
    punt_data.profile_scan_state_update_en = (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
        (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE) ? 1 :
        (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE) ? 3 : 2;
    punt_data.mep_rdi_update_loc_en = (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_ON_LOC);
    punt_data.mep_rdi_update_loc_clear_en = (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR);
    punt_data.mep_rdi_update_rx_en = FALSE;

    rv = algo_oam_db.oamp_punt_event_prof_template.allocate_single
        (unit, NO_FLAGS, &punt_data, NULL, &punt_profile_index, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free punt profile entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    rmep_hw_write_data->punt_profile_write = write_hw;
    rmep_hw_write_data->punt_profile_index = punt_profile_index;

    if (write_hw == TRUE)
    {
        /** Save data for writing to HW table at the end   */
        sal_memcpy(&rmep_hw_write_data->punt_profile, &punt_data, sizeof(dnx_oam_oamp_punt_event_profile_t));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_modify_rmep_data_in_templates(
    int unit,
    const bcm_bfd_endpoint_info_t * new_endpoint_info,
    const bcm_bfd_endpoint_info_t * existing_endpoint_info,
    const dnx_oam_oamp_rmep_db_entry_t * rmep_entry_values,
    uint8 *punt_profile_delete,
    bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** First, get the existing endpoint data */
    rmep_hw_write_data->punt_profile_index = rmep_entry_values->punt_profile;

    if ((new_endpoint_info->sampling_ratio != existing_endpoint_info->sampling_ratio) ||
        ((new_endpoint_info->flags & PUNT_PROFILE_FLAGS) != (existing_endpoint_info->flags & PUNT_PROFILE_FLAGS)))
    {
                /** A different punt profile is needed */
        SHR_IF_ERR_EXIT(dnx_bfd_oamp_punt_profile_sw_update(unit, new_endpoint_info, rmep_hw_write_data));

        SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.free_single
                        (unit, rmep_entry_values->punt_profile, punt_profile_delete));
    }

    if ((new_endpoint_info->punt_good_packet_period != existing_endpoint_info->punt_good_packet_period))
    {
        /** A different punt good profile is needed */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_alloc
                        (unit, 1 /* Exchange */ , new_endpoint_info->punt_good_packet_period,
                         rmep_entry_values->punt_good_profile, &rmep_hw_write_data->punt_good_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_endpoint_rmep_db_write_to_hw(
    int unit,
    const bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data)
{

    SHR_FUNC_INIT_VARS(unit);

    if (rmep_hw_write_data->punt_profile_write)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_profile_tbl_set
                        (unit, rmep_hw_write_data->punt_profile_index, &rmep_hw_write_data->punt_profile));
    }

    /*
     * Configure punt good profile data in HW
     */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_good_profile_hw_configure
                    (unit, &rmep_hw_write_data->punt_good_profile));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_write_rmep_data_to_templates(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Punt profile - used for reception only */
    SHR_IF_ERR_EXIT(dnx_bfd_oamp_punt_profile_sw_update(unit, endpoint_info, rmep_hw_write_data));

    /** Punt good profile */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_alloc(unit, 0 /* Allocate */ , endpoint_info->punt_good_packet_period, 0     /* Not
                                                                                                                                 * used 
                                                                                                                                 */ , &rmep_hw_write_data->punt_good_profile));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads an OAMP punt profile
 *        entry from the template "OAM punt profile."
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - see definition of struct for
 *        description of fields
 * \param [in] rmep_sw_read_data - Pointer to struct where
 *        profile ID is stored.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_oamp_punt_profile_sw_read(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_rmep_db_data_t * rmep_sw_read_data)
{
    dnx_oam_oamp_punt_event_profile_t punt_data;
    int dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.profile_data_get
                    (unit, rmep_sw_read_data->punt_profile_index, &dummy, &punt_data));

    endpoint_info->sampling_ratio = (punt_data.punt_enable ? punt_data.punt_rate + 1 : 0);

    switch (punt_data.rx_state_update_en)
    {
        case 0:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE;
            break;

        case 1:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE;
            break;

        case 3:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE;
            break;

        default:
            /*
             * Case 2 - no flags
             */
            break;
    }
    switch (punt_data.profile_scan_state_update_en)
    {
        case 0:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE;
            break;

        case 1:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE;
            break;

        case 3:
            endpoint_info->flags |= BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE;
            break;

        default:
            /*
             * Case 2 - no flags
             */
            break;
    }

    if (punt_data.mep_rdi_update_loc_en)
    {
        endpoint_info->flags |= BCM_BFD_ENDPOINT_RDI_ON_LOC;
    }

    if (punt_data.mep_rdi_update_loc_clear_en)
    {
        endpoint_info->flags |= BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_read_rmep_data_from_templates(
    int unit,
    bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_rmep_db_data_t * rmep_sw_read_data)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_bfd_oamp_punt_profile_sw_read(unit, endpoint_info, rmep_sw_read_data));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_get
                    (unit, rmep_sw_read_data->punt_good_profile.old_punt_good_profile,
                     &endpoint_info->punt_good_packet_period));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition in bfd_oamp.h for function description
 */
shr_error_e
dnx_bfd_oamp_vccv_channel_type_set(
    int unit)
{
    uint32 entry_handle_id;
    int vccv_channel_entry_id;
    int max_entries = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set channel types and format with values in list */
    SHR_IF_ERR_EXIT(dnx_oam_channel_type_table_get_number_of_entries(unit, &max_entries));
    vccv_channel_entry_id = max_entries - 1;

    SHR_IF_ERR_EXIT(dnx_oam_channel_type_rx_set_entry(unit, vccv_channel_entry_id, DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_BFD_RAW, 0, 0, DNX_MPLS_TP_VCCV_BFD_CHANNEL_TYPE_VALUE, 1, 1, 0       /* pemla_oam_channel_type_index 
                                                                                                                                                                                         */ ));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERIN1_MPLS_SPECULATIVE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GACH_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
