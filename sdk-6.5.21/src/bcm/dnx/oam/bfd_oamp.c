/** \file bfd_oamp.c
 * 
 *
 * BFD OAMP table access procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BFD

 /*
  * Include files.
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
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
#ifdef BCM_DNX2_SUPPORT
#include <src/bcm/dnx/oam/oamp_v1/oamp_v1_pe_infra.h>
#endif
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
            *bfd_dbal_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS;
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
 * See prototype definition in bfd_oamp.h for function description
 */
void
dnx_bfd_endpoint_db_build_param_for_oamp_rmep_db(
    int unit,
    const bcm_bfd_endpoint_info_t * endpoint_info,
    const bfd_temp_oamp_rmep_db_data_t * rmep_hw_write_data,
    dnx_oam_oamp_rmep_db_entry_t * entry_values)
{
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
    entry_values->rmep_state.bfd_state.diag_profile = endpoint_info->remote_diag;
    entry_values->rmep_state.bfd_state.flags_profile = BFD_FLAGS_VAL_TO_PROFILE(endpoint_info->remote_flags);
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
    endpoint_info->remote_flags = BFD_FLAGS_PROFILE_TO_VAL(entry_values->rmep_state.bfd_state.flags_profile);
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
    return BCM_OAMP_MEP_DB_SCAN_RATE_TO_SW_ENTRY_PERIOD(tx_rate);
}
