
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM
#ifdef BCM_DNX2_SUPPORT

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/bfd.h>

#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
#include <src/bcm/dnx/oam/oamp_v1/oamp_v1_pe.h>
#include <src/bcm/dnx/oam/oam_oamp_crc.h>
#include <src/bcm/dnx/oam/oamp_v1/oamp_v1_pe_infra.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_oam.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <include/bcm/vlan.h>
#include <include/bcm_int/dnx/switch/switch_tpid.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/algo/bfd/algo_bfd.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_bfd_access.h>
#include <include/bcm/switch.h>
#include <include/bcm_int/dnx_dispatch.h>
#include <include/soc/dnx/intr/auto_generated/jr2/jr2_intr.h>
#include <soc/dnx/mdb.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <src/bcm/dnx/oam/oamp_v1/bfd_oamp_v1.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <src/bcm/dnx/oam/oamp_v1/oam_oamp_v1.h>

#define DNX_OAM_SET_RDI_GEN_METHOD_FIELD_ON_MEP_PROFILE(rdi_method, rx_disable, scanner_disable) \
               rdi_method = ( (rx_disable)==0) <<1 | ((scanner_disable)==0)

#define IS_OAM_CLIENT(x) (_SHR_IS_FLAG_SET(x->flags2, BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET))

#define DNX_OAM_IS_CCM_PHASE_ODD(ccm_transmit_phase_is_odd, is_short_entry_type, endpoint_id) \
    ccm_transmit_phase_is_odd = is_short_entry_type ? 0 : (endpoint_id >> 2) & 1

#define DNX_OAM_OAMP_CRC_BIT 64
#define DNX_OAM_OAMP_CRC_BYTE 120

typedef struct
{

    uint32 oamp_mep_pe_profile;
    uint32 oamp_mep_pe_profile_mask;

    uint32 opcode_or_gach_channel;
    uint32 opcode_or_gach_channel_mask;

    uint32 opcode_or_gach_select;
    uint32 opcode_or_gach_select_mask;

    uint8 crc;

    uint8 crc_mask_index;

    uint8 valid;

} dnx_oam_oamp_flex_crc_tcam_t;

typedef struct
{

    uint8 flex_crc_per_bit_mask[BITS2BYTES(DNX_OAM_OAMP_CRC_BIT)];

    uint8 flex_crc_per_byte_mask[BITS2BYTES(DNX_OAM_OAMP_CRC_BYTE)];

} dnx_oam_oamp_flex_crc_pdu_mask_t;

shr_error_e
dnx_oam_oamp_v1_mep_db_extra_data_length_get(
    int unit,
    int index,
    uint32 *length)
{
    uint32 entry_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    entry_id = MEP_DB_ENTRY_TO_OAM_ID(index);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXT_DATA_LENGTH, INST_SINGLE, length));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mep_db_48b_maid_set(
    int unit,
    bcm_oam_group_info_t * group_info,
    uint16 calculated_crc)
{
    uint8 *extra_header_data = NULL;
    uint32 nof_data_bits = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(extra_header_data, DNX_OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE))
    {
        nof_data_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY;
    }
    else if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE))
    {
        nof_data_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY + DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY;
    }
    else if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
    {
        nof_data_bits = DNX_OAM_OAMP_NOF_MAID_BITS;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags indicate no extra data for MAID storation");
    }

    sal_memcpy(extra_header_data, group_info->name, BCM_OAM_GROUP_NAME_LENGTH);

    SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_extra_data_set
                    (unit, OAM_ID_TO_MEP_DB_ENTRY(group_info->group_name_index),
                     nof_data_bits, extra_header_data, DBAL_DEFINE_OAM_OPCODE_CCM,
                     calculated_crc, 0, _SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_REPLACE)));
exit:
    SHR_FREE(extra_header_data);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mac_da_msb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t msb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_msb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_msb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_msb_profiles_get(unit);

    if (profile >= nof_da_mac_msb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_MSB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_MSB_PROFILE, profile);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DA_MSB, INST_SINGLE, msb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mac_da_msb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t msb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_msb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_msb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_msb_profiles_get(unit);

    if (profile >= nof_da_mac_msb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_MSB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_MSB_PROFILE, profile);

    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_DA_MSB, INST_SINGLE, msb);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mac_da_lsb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t lsb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_lsb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_lsb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_lsb_profiles_get(unit);
    if (profile >= nof_da_mac_lsb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_LSB_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, profile);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DA_LSB, INST_SINGLE, lsb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mac_da_lsb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t lsb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_lsb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_lsb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_lsb_profiles_get(unit);

    if (profile >= nof_da_mac_lsb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_LSB_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, profile);

    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_DA_LSB, INST_SINGLE, lsb);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_get(
    int unit,
    uint8 profile,
    uint8 *exp,
    uint8 *ttl)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, profile);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TTL, INST_SINGLE, ttl);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_EXP, INST_SINGLE, exp);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mep_profile_clear(
    int unit,
    uint8 profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE_ID, profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mep_profile_set(
    int unit,
    uint8 profile,
    uint8 is_rfc_6374,
    const dnx_oam_mep_profile_t * mep_profile)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_ccm_interval_e dmm_interval, lmm_interval, opcode_0_interval, opcode_1_interval;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE_ID, profile);

    if (is_rfc_6374)
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMM_RATE, INST_SINGLE, mep_profile->dmm_rate);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMM_RATE, INST_SINGLE, mep_profile->lmm_rate);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile->dmm_rate, &dmm_interval));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMM_RATE, INST_SINGLE, dmm_interval);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile->lmm_rate, &lmm_interval));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMM_RATE, INST_SINGLE, lmm_interval);
    }

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_DUAL_ENDED_LM, INST_SINGLE,
                                mep_profile->piggy_back_lm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_SLM, INST_SINGLE, mep_profile->slm_lm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_LM, INST_SINGLE,
                                mep_profile->report_mode_lm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_DM, INST_SINGLE,
                                mep_profile->report_mode_dm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RDI_GEN_METHOD, INST_SINGLE,
                                mep_profile->rdi_gen_method);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMM_OFFSET, INST_SINGLE, mep_profile->dmm_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMR_OFFSET, INST_SINGLE, mep_profile->dmr_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMM_OFFSET, INST_SINGLE, mep_profile->lmm_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMR_OFFSET, INST_SINGLE, mep_profile->lmr_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_BIT_MAP, INST_SINGLE,
                                mep_profile->opcode_bit_map);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DM_MEASUREMENT_TYPE, INST_SINGLE,
                                mep_profile->dm_measurement_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_COUNT, INST_SINGLE, mep_profile->ccm_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DMM_COUNT, INST_SINGLE, mep_profile->dmm_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_COUNT, INST_SINGLE, mep_profile->lmm_count);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile->opcode_0_rate, &opcode_0_interval));
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 0, opcode_0_interval);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile->opcode_1_rate, &opcode_1_interval));
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 1, opcode_1_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 0, mep_profile->opcode_0_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 1, mep_profile->opcode_1_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_MAID_CHECK, INST_SINGLE,
                                 mep_profile->maid_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_MDL_CHECK, INST_SINGLE,
                                 mep_profile->mdl_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_SRC_IP_CHECK, INST_SINGLE,
                                 mep_profile->src_ip_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_UDP_SRC_PORT_CHECK, INST_SINGLE,
                                 mep_profile->udp_src_port_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_YOUR_DISCR_CHECK, INST_SINGLE,
                                 mep_profile->your_disc_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_MSB_PROFILE, INST_SINGLE,
                                 mep_profile->oamp_mac_da_oui_prof);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_COUNTER_ENABLE, INST_SINGLE,
                                mep_profile->opcode_tx_statistics_enable);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RX_COUNTER_ENABLE, INST_SINGLE,
                                mep_profile->opcode_rx_statistics_enable);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_MSK, INST_SINGLE, mep_profile->opcode_mask);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID_SHIFT, INST_SINGLE, mep_profile->mep_id_shift);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MEASUREMENT_PERIOD_PROFILE, INST_SINGLE,
                                mep_profile->slm_measurement_period_index);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mep_profile_get(
    int unit,
    uint8 profile,
    dnx_oam_mep_profile_t * mep_profile)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_ccm_interval_e dmm_interval = 0, lmm_interval = 0, opcode_0_interval = 0, opcode_1_interval =
        0;

    int dmm_rate_temp, lmm_rate_temp, opcode_0_rate_temp, opcode_1_rate_temp;
    uint32 src_ip_chk_dis_temp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE_ID, profile);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DMM_RATE, INST_SINGLE, (uint8 *) &dmm_interval);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LMM_RATE, INST_SINGLE, (uint8 *) &lmm_interval);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_DUAL_ENDED_LM, INST_SINGLE,
                              &mep_profile->piggy_back_lm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_SLM, INST_SINGLE, &mep_profile->slm_lm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_LM, INST_SINGLE,
                              &mep_profile->report_mode_lm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_DM, INST_SINGLE,
                              &mep_profile->report_mode_dm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_RDI_GEN_METHOD, INST_SINGLE,
                              &mep_profile->rdi_gen_method);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DMM_OFFSET, INST_SINGLE, &mep_profile->dmm_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DMR_OFFSET, INST_SINGLE, &mep_profile->dmr_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LMM_OFFSET, INST_SINGLE, &mep_profile->lmm_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LMR_OFFSET, INST_SINGLE, &mep_profile->lmr_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_BIT_MAP, INST_SINGLE,
                              &mep_profile->opcode_bit_map);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DM_MEASUREMENT_TYPE, INST_SINGLE,
                              &mep_profile->dm_measurement_type);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DISABLE_MDL_CHECK, INST_SINGLE,
                              &mep_profile->mdl_check_dis);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CCM_COUNT, INST_SINGLE, &mep_profile->ccm_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DMM_COUNT, INST_SINGLE, &mep_profile->dmm_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LMM_COUNT, INST_SINGLE, &mep_profile->lmm_count);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 0, (uint8 *) &opcode_0_interval);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 1, (uint8 *) &opcode_1_interval);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 0, &mep_profile->opcode_0_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 1, &mep_profile->opcode_1_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DISABLE_SRC_IP_CHECK, INST_SINGLE,
                               &src_ip_chk_dis_temp);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_MEASUREMENT_PERIOD_PROFILE, INST_SINGLE,
                              &mep_profile->slm_measurement_period_index);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period(unit, dmm_interval, &(dmm_rate_temp)));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period(unit, lmm_interval, &(lmm_rate_temp)));
    mep_profile->dmm_rate = dmm_rate_temp;
    mep_profile->lmm_rate = lmm_rate_temp;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period(unit, opcode_0_interval, &(opcode_0_rate_temp)));
    mep_profile->opcode_0_rate = opcode_0_rate_temp;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period(unit, opcode_1_interval, &(opcode_1_rate_temp)));
    mep_profile->opcode_1_rate = opcode_1_rate_temp;
    mep_profile->src_ip_check_dis = src_ip_chk_dis_temp;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_set(
    int unit,
    uint8 profile,
    uint8 ttl,
    uint8 exp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL, INST_SINGLE, ttl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, INST_SINGLE, exp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_clear(
    int unit,
    uint8 profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_ccm_endpoint_mpls_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    uint8 last_reference;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.free_single
                    (unit, (int) mep_db_entry->push_profile, &last_reference));

    if (last_reference)
    {

        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG;

        mep_hw_profiles_write_data->mpls_pwe_exp_ttl_profile = mep_db_entry->push_profile;
        mep_hw_profiles_write_data->ttl_exp.exp = 0;
        mep_hw_profiles_write_data->ttl_exp.ttl = 0;
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_oam_oamp_v1_ccm_endpoint_mpls_oam_get(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mep_db_entry->label == MPLS_LABEL_GAL)
    {
        if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
        {
            endpoint_info->type = bcmOAMEndpointTypeMplsLmDmSection;
        }
        else
        {
            endpoint_info->type = bcmOAMEndpointTypeBhhSection;
        }
    }
    else
    {
        endpoint_info->egress_label.label = mep_db_entry->label;
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_get(unit,
                                                                 mep_db_entry->push_profile,
                                                                 &endpoint_info->egress_label.exp,
                                                                 &endpoint_info->egress_label.ttl));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_ccm_endpoint_mpls_oam_init(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    int profile_id;
    uint8 first_reference;
    dnx_oam_ttl_exp_profile_t mpls_profile;

    SHR_FUNC_INIT_VARS(unit);

    mep_db_entry->label = (endpoint_info->type == bcmOAMEndpointTypeBhhSection
                           || endpoint_info->type ==
                           bcmOAMEndpointTypeMplsLmDmSection) ? MPLS_LABEL_GAL : endpoint_info->egress_label.label;

    mpls_profile.ttl = endpoint_info->egress_label.ttl;
    mpls_profile.exp = endpoint_info->egress_label.exp;

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.allocate_single
                    (unit, 0, &mpls_profile, NULL, &profile_id, &first_reference));

    if (first_reference)
    {

        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG;

        mep_hw_profiles_write_data->mpls_pwe_exp_ttl_profile = profile_id;
        mep_hw_profiles_write_data->ttl_exp.exp = mpls_profile.exp;
        mep_hw_profiles_write_data->ttl_exp.ttl = mpls_profile.ttl;
    }

    mep_db_entry->push_profile = (uint8) profile_id;

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_oam_oamp_v1_ccm_endpoint_eth_oam_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    uint8 last_reference;
    bcm_mac_t sa_mac;

    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_pp_port_2_sys_port.free_single
                        (unit, mep_db_entry->unified_port_access.port_profile, &last_reference));

        if (last_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_LOCL_PORT_TO_SYSTEM_PORT_FLAG;
            mep_hw_profiles_write_data->pp_port_profile = mep_db_entry->unified_port_access.port_profile;
            mep_hw_profiles_write_data->system_port = 0;
        }
    }
    if (mep_db_entry->nof_vlan_tags != 0)
    {
        SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.free_single
                        (unit, (int) mep_db_entry->inner_tpid_index, &last_reference));

        if (last_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_INNER_TPID_FLAG;

            mep_hw_profiles_write_data->inner_tpid_index = mep_db_entry->inner_tpid_index;
            mep_hw_profiles_write_data->inner_tpid = 0;

        }
        if (mep_db_entry->nof_vlan_tags == 2)
        {

            SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.free_single
                            (unit, (int) mep_db_entry->outer_tpid_index, &last_reference));
            if (last_reference)
            {
                mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_OUTER_TPID_FLAG;

                mep_hw_profiles_write_data->outer_tpid_index = mep_db_entry->outer_tpid_index;
                mep_hw_profiles_write_data->outer_tpid = 0;
            }
        }

        SHR_IF_ERR_EXIT(algo_oam_db.oam_sa_mac_msb_profile_template.free_single
                        (unit, (int) mep_db_entry->sa_gen_msb_profile, &last_reference));

        if (last_reference)
        {
            sal_memset(sa_mac, 0, sizeof(bcm_mac_t));

            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_SA_MAC_MSB_FLAG;

            mep_hw_profiles_write_data->sa_mac_msb_profile = mep_db_entry->sa_gen_msb_profile;
            sal_memcpy(mep_hw_profiles_write_data->sa_mac_msb, sa_mac, DNX_OAM_MAC_SIZE);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_ccm_endpoint_mep_db_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    uint8 last_reference;

    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {

        SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.free_single
                        (unit, (int) mep_db_entry->itmh_tc_dp_profile, &last_reference));

        if (last_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_ITMH_PRIORITY_FLAG;

            mep_hw_profiles_write_data->itmh_profile = mep_db_entry->itmh_tc_dp_profile;
            mep_hw_profiles_write_data->itmh_priority.dp = 0;
            mep_hw_profiles_write_data->itmh_priority.tc = 0;
        }
    }

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.free_single(unit, (int) mep_db_entry->mep_profile, &last_reference));

    if (last_reference)
    {
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG;
        mep_hw_profiles_write_data->mep_profile_index = mep_db_entry->mep_profile;
        sal_memset(&mep_hw_profiles_write_data->mep_profile, 0, sizeof(dnx_oam_mep_profile_t));
    }

    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_ccm_endpoint_eth_oam_profiles_free
                        (unit, mep_db_entry, mep_hw_profiles_write_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_ccm_endpoint_mpls_profiles_free
                        (unit, mep_db_entry, mep_hw_profiles_write_data));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mep_db_48b_maid_clear(
    int unit,
    uint32 header_entry_index)
{
    int i;
    uint32 next_bank_offset, current_entry, extra_data_len;
    SHR_FUNC_INIT_VARS(unit);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_extra_data_length_get
                    (unit, OAM_ID_TO_MEP_DB_ENTRY(header_entry_index), &extra_data_len));

    current_entry = header_entry_index;
    for (i = 0; i < extra_data_len; i++)
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, current_entry));

        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, current_entry));

        current_entry += next_bank_offset;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_mep_db_ccm_endpoint_get(
    int unit,
    uint16 oam_id,
    uint8 is_down_mep_egress_injection,
    dnx_oam_oamp_ccm_endpoint_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    dnx_oam_oamp_mep_db_args_t *oamp_params_rfc_6374 = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(oamp_params_rfc_6374, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for rfc_6347 entry",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
    if (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
    {

        oamp_params->extra_data_ptr = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get
                        (unit, MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr), oamp_params_rfc_6374));
        entry_values->mep_type = oamp_params_rfc_6374->mep_type;
    }
    else
    {
        entry_values->mep_type = oamp_params->mep_type;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_Q_ENTRY_ACCESS, oamp_params));
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
    }

    entry_values->flags = 0;

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_Q_ENTRY;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_ENABLE)
        && UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_STAT_ENABLE))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_OFFLOADED_LM_ENABLE;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_ENABLE)
        && UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_STAT_ENABLE))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_OFFLOADED_DM_ENABLE;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_IS_UPMEP))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_IS_UPMEP;

        entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port =
            oamp_params->unified_port_access.ccm_eth_up_mep_port.pp_port;
        entry_values->unified_port_access.ccm_eth_up_mep_port.port_core =
            oamp_params->unified_port_access.ccm_eth_up_mep_port.port_core;
    }
    else
    {
        entry_values->unified_port_access.port_profile = oamp_params->unified_port_access.port_profile;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_RDI_FROM_SCANNER))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_RDI_FROM_SCANNER;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_RDI_FROM_PACKET))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_RDI_FROM_PACKET;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_PORT_TLV_EN))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_PORT_TLV_VAL))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_ACH))
    {
        entry_values->flags |= DNX_OAMP_OAM_RF6374_ACH;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_GAL))
    {
        entry_values->flags |= DNX_OAMP_OAM_RF6374_GAL;
    }

    if (oamp_params->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {

        if (UTILEX_GET_BIT(oamp_params->fec_id_or_glob_out_lif, FIELD_IS_FEC_ID_FLAG) != 0)
        {
            entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC;
            entry_values->fec_id_or_glob_out_lif.fec_id = oamp_params->fec_id_or_glob_out_lif;

            UTILEX_SET_BIT(entry_values->fec_id_or_glob_out_lif.fec_id, 0, FIELD_IS_FEC_ID_FLAG);
        }
        else
        {
            entry_values->fec_id_or_glob_out_lif.glob_out_lif = oamp_params->fec_id_or_glob_out_lif;
        }
    }

    if (is_down_mep_egress_injection)
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION;
        entry_values->vsi = oamp_params->exclusive.oam_only.vsi;
    }

    entry_values->label = oamp_params->mpls_pwe.label;
    entry_values->counter_ptr = oamp_params->counter_ptr;
    entry_values->dest_sys_port_agr = oamp_params->dest_sys_port_agr;
    entry_values->extra_data_header = oamp_params->extra_data_ptr;
    entry_values->itmh_tc_dp_profile = oamp_params->itmh_tc_dp_profile;
    entry_values->mep_profile = oamp_params->mep_profile;
    entry_values->mep_pe_profile = oamp_params->mep_pe_profile;
    entry_values->push_profile = oamp_params->mpls_pwe.push_profile;
    entry_values->counter_interface = oamp_params->counter_interface;
    entry_values->crps_core_select = oamp_params->crps_core_select;
    entry_values->flex_lm_dm_entry = oamp_params->flex_lm_dm_ptr;
    if (entry_values->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374)
    {
        entry_values->session_id = oamp_params_rfc_6374->exclusive.rfc_6374_only.session_id;
        entry_values->timestamp_format = oamp_params_rfc_6374->exclusive.rfc_6374_only.timestamp_format;
    }
    else
    {
        entry_values->mep_id = oamp_params->exclusive.oam_only.oam_mep_id;
        entry_values->maid = oamp_params->exclusive.oam_only.oam_maid;
        entry_values->inner_vid = oamp_params->exclusive.oam_only.oam_inner_vid;
        entry_values->outer_vid = oamp_params->exclusive.oam_only.oam_outer_vid;
        entry_values->ccm_interval = oamp_params->exclusive.oam_only.oam_ccm_interval;
        entry_values->sa_gen_lsb = oamp_params->exclusive.oam_only.oam_sa_mac_lsb;
        entry_values->sa_gen_msb_profile = oamp_params->exclusive.oam_only.oam_sa_mac_msb_profile;
        entry_values->icc_index = oamp_params->exclusive.oam_only.oam_icc_index;
        entry_values->mdl = oamp_params->exclusive.oam_only.oam_mdl;
        entry_values->nof_vlan_tags = oamp_params->exclusive.oam_only.oam_nof_vlan_tags;
        entry_values->inner_tpid_index = oamp_params->exclusive.oam_only.oam_inner_tpid_index;
        entry_values->inner_pcp_dei = oamp_params->exclusive.oam_only.oam_inner_dei_pcp;
        entry_values->outer_tpid_index = oamp_params->exclusive.oam_only.oam_outer_tpid_index;
        entry_values->outer_pcp_dei = oamp_params->exclusive.oam_only.oam_outer_dei_pcp;
        entry_values->interface_status_tlv_code = oamp_params->exclusive.oam_only.oam_interface_status_tlv_code;
        entry_values->server_destination = oamp_params->exclusive.oam_only.server_destination;
        entry_values->signal_degradation_tx = oamp_params->exclusive.oam_only.tx_signal;
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FREE(oamp_params_rfc_6374);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_rmep_index_db_add(
    int unit,
    uint16 oam_id,
    uint16 mep_id,
    uint32 rmep_idx)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_INDEX_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, mep_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, INST_SINGLE, rmep_idx);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_rmep_index_db_get(
    int unit,
    uint16 oam_id,
    uint16 mep_id,
    uint32 *rmep_idx)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_INDEX_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, mep_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, INST_SINGLE, rmep_idx);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_rmep_index_db_remove(
    int unit,
    uint16 oam_id,
    uint16 mep_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_INDEX_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, mep_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_ccm_endpoint_mep_db_create_system_headers(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const bcm_oam_group_info_t * group_info,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    uint8 slm_enabled)
{
    dnx_oam_itmh_priority_t itmh_priority;
    int profile_id;
    int system_headers_mode;
    uint32 udh_header_size;
    uint8 first_reference;
    uint32 mep_pe_profile;
    uint32 sys_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_vlan_port_t vlan_port;
    dnx_algo_gpm_gport_phy_info_t *tx_gport_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&vlan_port);

    SHR_ALLOC_SET_ZERO(tx_gport_data, sizeof(dnx_algo_gpm_gport_phy_info_t),
                       "Structure for reading data about tx_gport", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

    mep_db_entry->server_destination = 0;
    if (!DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
    {

        itmh_priority.tc = (endpoint_info->int_pri & 0x1F) >> 2;
        itmh_priority.dp = (endpoint_info->int_pri & 0x3);

        SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.allocate_single
                        (unit, 0, &itmh_priority, NULL, &profile_id, &first_reference));
        mep_db_entry->itmh_tc_dp_profile = profile_id;
        if (first_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_ITMH_PRIORITY_FLAG;

            mep_hw_profiles_write_data->itmh_profile = profile_id;
            mep_hw_profiles_write_data->itmh_priority.dp = itmh_priority.dp;
            mep_hw_profiles_write_data->itmh_priority.tc = itmh_priority.tc;
        }

        if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
        {

            SHR_IF_ERR_EXIT(dnx_oam_system_port_profile_sw_update(unit, endpoint_info, mep_hw_profiles_write_data));
            mep_db_entry->unified_port_access.port_profile = mep_hw_profiles_write_data->pp_port_profile;

            if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) && udh_header_size != 0)
            {
                if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48, &mep_pe_profile));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP, &mep_pe_profile));
                }
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
        }
        else
        {

            if (BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->intf_id))
            {
                mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC;
                mep_db_entry->fec_id_or_glob_out_lif.fec_id = BCM_L3_ITF_VAL_GET(endpoint_info->intf_id);
                mep_db_entry->dest_sys_port_agr = BCM_GPORT_INVALID;
            }
            else
            {

                if (BCM_L3_ITF_TYPE_IS_LIF(endpoint_info->intf_id))
                {
                    mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif = BCM_L3_ITF_VAL_GET(endpoint_info->intf_id);
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, endpoint_info->tx_gport,
                                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT,
                                                                    tx_gport_data));

                    mep_db_entry->dest_sys_port_agr = tx_gport_data->sys_port;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error: provided interface is not a valid FEC nor an out-LIF.\n");
                }
            }

            if ((dnx_data_oam.property.oam_injected_over_lsp_cnt_get(unit) == 1)
                && (endpoint_info->type == bcmOAMEndpointTypeBHHMPLS)
                && (endpoint_info->mpls_out_gport == endpoint_info->intf_id))
            {

                mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;

                if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, endpoint_info->mpls_out_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF,
                                     &gport_hw_resources));
                    mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif = gport_hw_resources.global_out_lif;
                }
            }
        }
    }
    else
    {

        if (IS_OAM_SERVER(endpoint_info))
        {
            mep_db_entry->unified_port_access.ccm_eth_up_mep_port.pp_port = endpoint_info->gport;
            mep_db_entry->unified_port_access.ccm_eth_up_mep_port.port_core = 0;
            if (endpoint_info->tx_gport != BCM_GPORT_INVALID)
            {
                sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(endpoint_info->tx_gport);

                if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
                {
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_DESTINATION_JR1,
                                                                         DBAL_FIELD_PORT_ID, &sys_port,
                                                                         &mep_db_entry->server_destination));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_DESTINATION,
                                                                         DBAL_FIELD_PORT_ID, &sys_port,
                                                                         &mep_db_entry->server_destination));
                }
            }
        }
        else
        {

            vlan_port.vlan_port_id = endpoint_info->gport;
            SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, vlan_port.port,
                                                            DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                            &gport_info));

            mep_db_entry->unified_port_access.ccm_eth_up_mep_port.pp_port = gport_info.internal_port_pp_info.pp_port[0];
            mep_db_entry->unified_port_access.ccm_eth_up_mep_port.port_core =
                gport_info.internal_port_pp_info.core_id[0];
        }

    }

    if ((!DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) && !slm_enabled)
    {

        if (!((_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN)) &&
              (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)))
        {

            mep_db_entry->counter_interface = endpoint_info->lm_counter_if + 1;
            mep_db_entry->counter_ptr = endpoint_info->lm_counter_base_id;
        }
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, endpoint_info->gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF, &gport_hw_resources));
        mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif = gport_hw_resources.global_out_lif;
    }

    SHR_EXIT();
exit:
    SHR_FREE(tx_gport_data);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_ccm_endpoint_mep_db_create_oam_pdu(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const bcm_oam_group_info_t * group_info,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_delete_data,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    uint8 is_dual_ended_lm,
    uint8 is_1dm_enabled)
{
    int system_headers_mode, index;
    uint32 udh_header_size;
    int num_short_entries_in_bank, entry_bank, extra_data_header_bank;
    dnx_oam_oamp_trap_tcam_entry_t tcam_data;
    uint8 first_reference, last_reference;
    uint32 mep_pe_profile;
    bcm_port_t up_mep_tod_recyle_port = 0;
    int is_valid_recyle_port = 0;
    dnx_oam_oamp_mep_db_args_t *group_entry_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(group_entry_info, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "Structure for reading data about group", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        mep_db_entry->session_id = endpoint_info->session_id;
        mep_db_entry->timestamp_format = endpoint_info->timestamp_format;
    }

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {

        mep_db_entry->mdl = endpoint_info->level;

        mep_db_entry->mep_id = endpoint_info->name;

        if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE) &&
            !DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
        {

            SHR_IF_ERR_EXIT(dnx_oam_update_accelarated_endpoint_info_with_group_config
                            (unit, group_info->name, mep_db_entry));
        }

        if (endpoint_info->port_state != 0)
        {

            mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN;

            mep_db_entry->flags |=
                ((endpoint_info->port_state) == BCM_OAM_PORT_TLV_UP) ? DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL : 0;
        }

        mep_db_entry->interface_status_tlv_code = endpoint_info->interface_state;

        if (dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_up_mep_tod_recycle_enable))
        {
            up_mep_tod_recyle_port = dnx_data_oam.oamp.up_mep_tod_recycle_port_core_0_get(unit);

            if (up_mep_tod_recyle_port != 0)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_is_valid_get(unit, up_mep_tod_recyle_port, &is_valid_recyle_port));
            }
        }

        if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
        {

            if (!((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) &&
                  !(DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) && (udh_header_size != 0) &&
                  (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }

            if (endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeSelfContained)
            {
                num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
                    dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
                entry_bank = endpoint_info->id / num_short_entries_in_bank;
                extra_data_header_bank = group_info->group_name_index / num_short_entries_in_bank;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, group_info->group_name_index, group_entry_info));
                if ((entry_bank >= extra_data_header_bank) &&
                    (entry_bank < extra_data_header_bank + group_entry_info->exclusive.extra_data.extra_data_len))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error: for self contained accelerated OAM endpoint with 48B MAID, all extra data \n"
                                 "entries must not be in the same bank as the main endpoint entry.\n");
                }

                mep_db_entry->extra_data_header = OAM_ID_TO_MEP_DB_ENTRY(group_info->group_name_index);

                if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48,
                                     &mep_pe_profile));
                    mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                }

                if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info))
                {
                    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_SIGNAL))
                    {
                        SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                        (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48,
                                         &mep_pe_profile));
                        mep_db_entry->extra_data.flags =
                            DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_SD | DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE;
                        mep_db_entry->extra_data.opcodes_to_prepend = DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;
                        mep_db_entry->extra_data.signal_degradation_rx = endpoint_info->rx_signal;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                        (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48,
                                         &mep_pe_profile));
                        if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_ENABLE(endpoint_info, sw_endpoint_info))
                        {

                            mep_db_entry->extra_data.flags = DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_DMAC;
                            mep_db_entry->extra_data.opcodes_to_prepend =
                                DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM | DNX_OAMP_EXTRA_DATA_TO_PREPEND_LMM |
                                DNX_OAMP_EXTRA_DATA_TO_PREPEND_DMM;
                            sal_memset(mep_db_entry->extra_data.dmac, 0, sizeof(bcm_mac_t));
                        }
                    }
                    mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                    mep_db_entry->extra_data_header = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
                    mep_db_entry->extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
                    sal_memcpy(mep_db_entry->extra_data.maid48, group_info->name, BCM_OAM_GROUP_NAME_LENGTH);
                    mep_db_entry->extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID48;

                }
                else if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_DISABLE(endpoint_info, sw_endpoint_info))
                {

                    mep_db_entry->extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(sw_endpoint_info->extra_data_index);
                    mep_db_entry->extra_data.flags = 0;
                    mep_db_entry->extra_data.length = sw_endpoint_info->extra_data_length;
                }
                if (is_1dm_enabled &&
                    _SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_ADDITIONAL_GAL_SPECIAL_LABEL))
                {

                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48,
                                     &mep_pe_profile));
                    mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                }
                else if (is_1dm_enabled)
                {

                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48, &mep_pe_profile));
                    mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                }
                else if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_ADDITIONAL_GAL_SPECIAL_LABEL))
                {

                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48,
                                     &mep_pe_profile));
                    mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                }

                if (sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_JUMBO_DM_TLV)
                {

                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48, &mep_pe_profile));
                    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_ADDITIONAL_GAL_SPECIAL_LABEL))
                    {
                        SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                        (unit,
                                         DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48,
                                         &mep_pe_profile));
                    }
                    mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                }
            }
            else
            {
                mep_db_entry->extra_data_header = group_info->id;
            }
            if (IS_OAM_SERVER(endpoint_info) && DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
            {

                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
            if (DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)
                && (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM) && is_valid_recyle_port)
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD_CCM_MAID_48, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
        }

        else
        {
            if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_ADDITIONAL_GAL_SPECIAL_LABEL))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }

            if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }

            if (IS_OAM_SERVER(endpoint_info) && DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
            {

                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
            if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info))
            {
                if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_SIGNAL))
                {

                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT, &mep_pe_profile));
                    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
                    {
                        mep_db_entry->extra_data.flags = DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE;
                    }
                    mep_db_entry->extra_data.flags |= DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_SD;
                    mep_db_entry->extra_data.opcodes_to_prepend = DNX_OAMP_EXTRA_DATA_TO_PREPEND_CCM;

                    mep_db_entry->extra_data.signal_degradation_rx = endpoint_info->rx_signal;
                }
                else
                {

                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA, &mep_pe_profile));
                    if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_ENABLE(endpoint_info, sw_endpoint_info))
                    {

                        mep_db_entry->extra_data.flags = DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_DMAC;
                        mep_db_entry->extra_data.opcodes_to_prepend =
                            DNX_OAMP_EXTRA_DATA_TO_PREPEND_LMM | DNX_OAMP_EXTRA_DATA_TO_PREPEND_DMM;
                        sal_memset(mep_db_entry->extra_data.dmac, 0, sizeof(bcm_mac_t));
                    }
                }
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                mep_db_entry->extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
                mep_db_entry->extra_data_header = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
            }
            else if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_DISABLE(endpoint_info, sw_endpoint_info))
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
                mep_db_entry->extra_data_header = endpoint_info->extra_data_index;

                mep_db_entry->extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(sw_endpoint_info->extra_data_index);
                mep_db_entry->extra_data.flags = 0;
                mep_db_entry->extra_data.length = sw_endpoint_info->extra_data_length;
            }
            if (DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)
                && (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM) && is_valid_recyle_port)
            {
                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD, &mep_pe_profile));
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
            if (is_1dm_enabled)
            {

                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM, &mep_pe_profile));
                if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_ADDITIONAL_GAL_SPECIAL_LABEL))
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM,
                                     &mep_pe_profile));
                }
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }

            if (sw_endpoint_info->sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_JUMBO_DM_TLV)
            {

                SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV, &mep_pe_profile));
                if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_ADDITIONAL_GAL_SPECIAL_LABEL))
                {
                    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV,
                                     &mep_pe_profile));
                }
                mep_db_entry->mep_pe_profile = (uint8) mep_pe_profile;
            }
        }
    }

    tcam_data.trap_code = BCM_GPORT_TRAP_GET_ID(endpoint_info->remote_gport);

    if (IS_OAM_SERVER(endpoint_info) &&
        (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE) ||
         (mep_hw_profiles_delete_data->tcam_entry.trap_code != tcam_data.trap_code)))
    {
        tcam_data.mep_type = mep_db_entry->mep_type;
        SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.allocate_single
                        (unit, 0, &tcam_data, NULL, &index, &first_reference));
        mep_hw_profiles_write_data->tcam_index = index;
        if (first_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_TRAP_TCAM_FLAG;
            mep_hw_profiles_write_data->tcam_entry.trap_code = tcam_data.trap_code;
            mep_hw_profiles_write_data->tcam_entry.mep_type = tcam_data.mep_type;
        }

        if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
        {
            SHR_IF_ERR_EXIT(algo_oam_db.oam_trap_tcam_index.free_single(unit,
                                                                        mep_hw_profiles_delete_data->tcam_index,
                                                                        &last_reference));
            if (last_reference)
            {
                mep_hw_profiles_delete_data->flags |= DNX_OAM_OAMP_PROFILE_TRAP_TCAM_FLAG;
            }
        }
    }

    SHR_EXIT();
exit:
    SHR_FREE(group_entry_info);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_mep_db_create_prepare_for_ccm_endpoint_replace(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    uint8 *slm_enabled,
    uint8 *is_dual_ended_lm,
    uint8 *is_1dm_enabled,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_delete_data,
    dnx_oam_mep_profile_t * mep_profile)
{
    int counter = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {

        mep_db_entry->mep_type = 0;
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_ccm_endpoint_get(unit, (uint16) endpoint_info->id,
                                                            DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION(endpoint_info),
                                                            mep_db_entry));

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, mep_db_entry->mep_profile, &counter,
                                                                 mep_profile));

    *slm_enabled = mep_profile->slm_lm;

    *is_dual_ended_lm = mep_profile->piggy_back_lm;

    if (mep_profile->dm_measurement_type == DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT4)
    {
        *is_1dm_enabled = TRUE;
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_ccm_endpoint_mep_db_profiles_free(unit, mep_db_entry, mep_hw_profiles_delete_data));

    mep_db_entry->flags = DNX_OAMP_OAM_CCM_MEP_UPDATE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_endpoint_delete_associated_loss_entries(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    dnx_oam_oamp_endpoint_lm_params_t lm_mep_db_entry;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    uint8 dm_exists = 0, lm_exists = 0, lm_stat_exists = 0;
    bcm_oam_loss_t session_loss_ptr;
    bcm_oam_loss_t loss_ptr;
    dnx_oam_oamp_endpoint_dm_params_t delay_params;
    dnx_oam_oamp_endpoint_lm_params_t *loss_params = NULL;
    bcm_oam_delay_t delay_ptr;
    int priority;
    uint32 session_oam_id;
    int rv;
    bcm_oam_delay_t session_delay_ptr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_lm_endpoint_get
                    (unit, (uint16) endpoint_info->id, &lm_mep_db_entry, &lm_exists, &lm_stat_exists));
    SHR_ALLOC_SET_ZERO(loss_params, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (lm_exists)
    {
        loss_ptr.id = endpoint_info->id;
        loss_ptr.pkt_pri_bitmap = 0xff;
        SHR_IF_ERR_EXIT(bcm_dnx_oam_loss_delete(unit, &loss_ptr));
    }

    SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_dm_endpoint_get(unit, (uint16) endpoint_info->id, &delay_params, &dm_exists));
    if (dm_exists)
    {
        delay_ptr.id = endpoint_info->id;
        delay_ptr.pkt_pri_bitmap = 0xff;
        SHR_IF_ERR_EXIT(bcm_dnx_oam_delay_delete(unit, &delay_ptr));
    }

    for (priority = 0; priority < 8; priority++)
    {
        loss_ptr.id = endpoint_info->id;
        rv = dnx_oamp_v1_lm_with_priority_session_map_get(unit, loss_ptr.id, priority, &session_oam_id);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(rv);

            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_lm_endpoint_get
                            (unit, session_oam_id, loss_params, &lm_exists, &lm_stat_exists));
            if (lm_exists)
            {
                session_loss_ptr.id = loss_ptr.id;
                session_loss_ptr.pkt_pri_bitmap = 1 << priority;
                rv = bcm_dnx_oam_loss_delete(unit, &session_loss_ptr);
                if (rv != _SHR_E_NOT_FOUND)
                {
                    SHR_IF_ERR_EXIT(rv);
                }
            }
        }
        rv = dnx_oamp_v1_lm_with_priority_session_map_get(unit, loss_ptr.id, priority, &session_oam_id);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_dm_endpoint_get(unit, session_oam_id, dm_mep_db_entry, &dm_exists));
            if (dm_exists)
            {
                session_delay_ptr.id = endpoint_info->id;
                session_delay_ptr.pkt_pri_bitmap = 1 << priority;
                SHR_IF_ERR_EXIT(bcm_dnx_oam_delay_delete(unit, &session_delay_ptr));
                if (rv != _SHR_E_NOT_FOUND)
                {
                    SHR_IF_ERR_EXIT(rv);
                }
            }
        }
    }

exit:
    SHR_FREE(loss_params);
    SHR_FREE(dm_mep_db_entry);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_endpoint_fill_id(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    const oam_lif_resolution_t * lif_resolution,
    const bcm_oam_group_info_t * group_info,
    uint8 *is_bfd_alloc)
{

    int mep_db_lim;
    int is_48B_maid = 0;

    SHR_FUNC_INIT_VARS(unit);

    mep_db_lim = dnx_data_oam.oamp.mep_db_version_limitation_get(unit);
    if ((mep_db_lim == 1) &&
        !_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_WITH_ID) &&
        (endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeShortEntry) &&
        ((BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(group_info->name) != DNX_OAMP_OAM_MA_NAME_TYPE_ICC) &&
         !DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags)))
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_endpoint_workaround_fill_id(unit, endpoint_info, group_info->name));
    }
    else
    {
        if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {

            SHR_IF_ERR_EXIT(dnx_oam_mpls_lm_dm_verify(unit, endpoint_info, is_bfd_alloc));
        }

        if (!(*is_bfd_alloc))
        {
            is_48B_maid = DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags);
            SHR_IF_ERR_EXIT(dnx_oam_local_accelerated_endpoint_fill_id
                            (unit, endpoint_info, group_info->name, is_48B_maid));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_endpoint_remove_from_mep_db(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    uint8 mep_db_extra_data_length,
    const uint8 is_mpls_lm_dm_without_bfd_endpoint)
{
    uint32 extra_data_length;
    bcm_oam_group_info_t *group_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(group_info, sizeof(bcm_oam_group_info_t),
                       "Info about group associated with endpoint", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info))
    {
        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {

            SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, endpoint_info->group, group_info));
        }

        extra_data_length = 0;

        if (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained)
        {

            if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
            {
                extra_data_length = DNX_OAM_OAMP_NOF_MAID_BITS;
            }
        }

        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) || is_mpls_lm_dm_without_bfd_endpoint)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_clear(unit, (uint16) endpoint_info->id, extra_data_length));
        }

        if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_clear
                            (unit, MEP_DB_ENTRY_TO_OAM_ID(mep_db_entry->flex_lm_dm_entry), extra_data_length));

            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, MEP_DB_ENTRY_TO_OAM_ID(mep_db_entry->flex_lm_dm_entry)));
        }

        if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info) &&
            !DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) &&
            !((DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags) &&
               _SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_SIGNAL))))
        {
            dnx_oamp_ccm_endpoint_extra_data_t extra_data;
            sal_memset(&extra_data, 0, sizeof(dnx_oamp_ccm_endpoint_extra_data_t));
            extra_data.index = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
            extra_data.length = mep_db_extra_data_length;
            SHR_IF_ERR_EXIT(dnx_oamp_v1_ccm_endpoint_extra_data_set(unit, &extra_data));
        }

        SHR_IF_ERR_EXIT(dnx_oam_lmep_remote_endpoints_destroy(unit, endpoint_info->id));
    }

exit:
    SHR_FREE(group_info);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_accelerated_endpoint_destroy_mep_id_dealloc_v1(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const uint8 is_mpls_lm_dm_without_bfd_endpoint)
{

    SHR_FUNC_INIT_VARS(unit);

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) || is_mpls_lm_dm_without_bfd_endpoint)
    {

        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, endpoint_info->id));
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_oam_oamp_v1_lm_dm_set_offset(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    uint8 is_piggy_back,
    uint8 is_dm,
    uint8 is_reply,
    uint8 is_rfc_6374_lsp,
    uint8 is_egress_inj)
{
    int value = 0;
    uint8 is_rfc_6374 = mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374 ? 1 : 0;
    int is_lm = (is_dm == 0) ? 1 : 0;
    int is_jr2_mode = 0;

    is_jr2_mode =
        (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ? 1 : 0;

    value += (is_piggy_back * 54) * is_lm + (is_rfc_6374 * (24 - (is_dm * 4))) + (is_rfc_6374_lsp * 4);

    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {

        value += mep_db_entry->nof_vlan_tags * 4 + 18 + (0x21 * is_egress_inj * is_jr2_mode);
    }
    else if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP)
    {

        value += (16 + (0x1c * is_jr2_mode));
    }
    else if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE)
    {
        value += (12 + (0x1c * is_jr2_mode));

    }

    if (is_rfc_6374)
    {

        value += 0x1c * is_jr2_mode;
        return value;
    }

    if (is_reply && is_dm)
    {

        value += 0x10;
    }
    else if (is_reply)
    {

        value += 8;
    }
    return value;
}

shr_error_e
dnx_oamp_v1_mdb_extra_data_set(
    int unit,
    uint16 oam_id,
    uint8 is_offloaded,
    uint8 is_q_entry,
    uint16 calculated_crc,
    int opcode,
    uint32 nof_data_bits,
    uint8 *data,
    uint8 is_update)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    int pl_index;
    int extra_data_len = 1;
    int extra_data_hdr_seg_len;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
    UTILEX_SET_BIT(oamp_params->flags, is_offloaded, OAMP_MEP_LM_DM_OFFLOADED);
    UTILEX_SET_BIT(oamp_params->flags, is_q_entry, OAMP_MEP_Q_ENTRY);
    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR;
    oamp_params->exclusive.extra_data.crc_val1 = calculated_crc;
    oamp_params->exclusive.extra_data.opcode_bmp = opcode;

    if (nof_data_bits > DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN)
    {
        extra_data_len +=
            (nof_data_bits - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN - 1) / DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN + 1;
        extra_data_hdr_seg_len = DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN;
    }
    else
    {

        sal_memset(oamp_params->exclusive.extra_data.data_segment, 0,
                   sizeof(oamp_params->exclusive.extra_data.data_segment));
        extra_data_hdr_seg_len = nof_data_bits;
    }
    dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment, 0, data, 0, extra_data_hdr_seg_len, FALSE);

    if (is_update)
    {
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_add(unit, oam_id, oamp_params));

    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD;

    for (pl_index = 1; pl_index < extra_data_len; pl_index++)
    {
        int bits_left = 0, bits_to_copy = 0, start_from = 0;
        sal_memset(oamp_params, 0, DATA_SEG_LEN);

        oamp_params->exclusive.extra_data.extra_data_len = MDB_EXTRA_DATA_PLD1_ACCESS + pl_index - 1;

        bits_left =
            nof_data_bits - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN - DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN * (pl_index - 1);
        if (bits_left < DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN)
        {
            bits_to_copy = bits_left;
            start_from = DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN - bits_left;

            sal_memset(oamp_params->exclusive.extra_data.data_segment, 0,
                       sizeof(oamp_params->exclusive.extra_data.data_segment));
        }
        else
        {
            bits_to_copy = DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN;
        }
        dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment, start_from,
                                data,
                                DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN + (pl_index -
                                                                      1) * DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN,
                                bits_to_copy, FALSE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_add(unit, oam_id, oamp_params));
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mdb_extra_data_get(
    int unit,
    uint16 oam_id,
    uint32 extra_data_length,
    uint8 *data)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 nof_payload_access = 0;
    int ii = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Parameter structure for single entry",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_EXTRA_DATA_HDR_ACCESS, oamp_params));

    dnx_oamp_copy_bit_array(data, 0, oamp_params->exclusive.extra_data.data_segment, 0,
                            DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN, TRUE);

    if (extra_data_length > DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN)
    {
        nof_payload_access =
            ((extra_data_length - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN) / (DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN + 1)) + 1;
    }

    for (ii = 0; ii < nof_payload_access; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_EXTRA_DATA_PLD1_ACCESS + ii, oamp_params));

        dnx_oamp_copy_bit_array(data,
                                DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN +
                                (ii * DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN),
                                oamp_params->exclusive.extra_data.data_segment, 0,
                                DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN, TRUE);
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_ccm_endpoint_extra_data_set(
    int unit,
    dnx_oamp_ccm_endpoint_extra_data_t * extra_data)
{
    int i = 0, nof_bits = 0;
    int is_maid = 0, nof_maid_bits = 0;
    uint16 calculated_crc = 0;
    uint8 *extra_header_data = NULL;
    uint8 *data_for_crc_calculation = NULL;
    uint32 crc_length;

    uint32 entry_idx, next_bank_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(extra_header_data, DNX_OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(data_for_crc_calculation, DNX_OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID20))
    {
        is_maid = 1;
        nof_maid_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY;
    }
    else if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID40))
    {
        is_maid = 1;
        nof_maid_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY + DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY;
    }
    else if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_MAID48))
    {
        is_maid = 1;
        nof_maid_bits = DNX_OAM_OAMP_NOF_MAID_BITS;
    }

    if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_DMAC))
    {
        sal_memcpy(extra_header_data, extra_data->dmac, sizeof(bcm_mac_t));
        nof_bits += DNX_LMDM_FLEXIBLE_DA_BITS;
        if (is_maid)
        {

            SHR_IF_ERR_EXIT(dnx_oam_oamp_crc16(unit, (uint8 *) extra_data->maid48,
                                               DNX_OAMP_OAM_GROUP_MAID_OFFSET / UTILEX_NOF_BITS_IN_BYTE,
                                               BCM_OAM_GROUP_NAME_LENGTH, &calculated_crc));

            sal_memcpy(extra_header_data + DNX_LMDM_FLEXIBLE_DA_LENGTH, extra_data->maid48, BCM_OAM_GROUP_NAME_LENGTH);
            nof_bits += nof_maid_bits;
        }
        SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_extra_data_set
                        (unit, extra_data->index, nof_bits,
                         extra_header_data, extra_data->opcodes_to_prepend, calculated_crc, 0,
                         _SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE)));
    }
    else
    {
        if (_SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_SD))
        {
            nof_bits = 0;
            crc_length = (DNX_OAMP_OAM_GROUP_MAID_OFFSET / UTILEX_NOF_BITS_IN_BYTE);
            data_for_crc_calculation[2] = (extra_data->signal_degradation_rx << 6);
            if (is_maid)
            {
                sal_memcpy(data_for_crc_calculation + (DNX_OAMP_OAM_GROUP_MAID_OFFSET / UTILEX_NOF_BITS_IN_BYTE),
                           extra_data->maid48, BCM_OAM_GROUP_NAME_LENGTH);
                crc_length += BCM_OAM_GROUP_NAME_LENGTH;
                sal_memcpy(extra_header_data, extra_data->maid48, BCM_OAM_GROUP_NAME_LENGTH);
                nof_bits += nof_maid_bits;
            }

            SHR_IF_ERR_EXIT(dnx_oam_oamp_crc16(unit, (uint8 *) data_for_crc_calculation, 0,
                                               crc_length, &calculated_crc));
            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_extra_data_set
                            (unit, extra_data->index, nof_bits,
                             extra_header_data, extra_data->opcodes_to_prepend, calculated_crc, 0,
                             _SHR_IS_FLAG_SET(extra_data->flags, DNX_OAMP_CCM_ENDPOINT_EXTRA_DATA_UPDATE)));
        }
        else
        {
            next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));
            for (i = 0; i < extra_data->length; i++)
            {
                entry_idx = MEP_DB_ENTRY_TO_OAM_ID(extra_data->index) + i * next_bank_offset;

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, entry_idx));

                SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, entry_idx));
            }
        }
    }

exit:
    SHR_FREE(extra_header_data);
    SHR_FREE(data_for_crc_calculation);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_ccm_endpoint_extra_data_get(
    int unit,
    dnx_oamp_ccm_endpoint_extra_data_t * extra_data)
{
    uint8 *extra_header_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(extra_header_data, OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (extra_data->index != DNX_OAM_EXTRA_DATA_HEADER_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_extra_data_get(unit, extra_data->index, extra_header_data, NULL, NULL));
    }
    sal_memcpy(extra_data->dmac, extra_header_data, sizeof(bcm_mac_t));
    sal_memcpy(extra_data->maid48, extra_header_data + DNX_LMDM_FLEXIBLE_DA_LENGTH, BCM_OAM_GROUP_NAME_LENGTH);

exit:
    SHR_FREE(extra_header_data);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_rfc_6374_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_ccm_endpoint_t * entry_values,
    uint8 is_last_entry,
    uint8 is_bfd_exist)
{
    dnx_oam_oamp_mep_db_args_t *oamp_bfd_pwe_params = NULL;
    dnx_oam_oamp_mep_db_args_t *oamp_rfc_6374_params = NULL;
    uint8 bfd_update;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_bfd_pwe_params, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "Param struct for creating single entries", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(oamp_rfc_6374_params, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "Param struct for creating single entries", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    bfd_update = _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE) || is_bfd_exist;
    if (bfd_update)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_bfd_pwe_params));
    }
    else
    {
        oamp_bfd_pwe_params->exclusive.oam_only.oam_lmm_da_profile = 0;
        oamp_bfd_pwe_params->flex_lm_dm_ptr = 0;

        oamp_bfd_pwe_params->flags = 0;
    }

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get
                        (unit, MEP_DB_ENTRY_TO_OAM_ID(entry_values->flex_lm_dm_entry), oamp_rfc_6374_params));
    }
    else
    {

        UTILEX_SET_BIT(oamp_bfd_pwe_params->flags,
                       _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_RF6374_ACH), OAMP_MEP_ACH);
        UTILEX_SET_BIT(oamp_bfd_pwe_params->flags,
                       _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_RF6374_GAL), OAMP_MEP_GAL);
    }

    oamp_bfd_pwe_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE;

    UTILEX_SET_BIT(oamp_bfd_pwe_params->flags, bfd_update, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_rfc_6374_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE),
                   OAMP_MEP_UPDATE);

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC))
    {
        oamp_bfd_pwe_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.glob_out_lif;
        UTILEX_SET_BIT(oamp_bfd_pwe_params->fec_id_or_glob_out_lif, TRUE, FIELD_IS_FEC_ID_FLAG);
    }
    else
    {
        oamp_bfd_pwe_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.glob_out_lif;
    }

    oamp_bfd_pwe_params->flex_lm_dm_ptr = entry_values->flex_lm_dm_entry;
    oamp_bfd_pwe_params->mpls_pwe.label = entry_values->label;
    oamp_bfd_pwe_params->itmh_tc_dp_profile = entry_values->itmh_tc_dp_profile;
    oamp_bfd_pwe_params->mep_profile = entry_values->mep_profile;
    oamp_bfd_pwe_params->mep_pe_profile = entry_values->mep_pe_profile;
    oamp_bfd_pwe_params->mpls_pwe.push_profile = entry_values->push_profile;
    oamp_bfd_pwe_params->crps_core_select = entry_values->crps_core_select;
    oamp_bfd_pwe_params->dest_sys_port_agr = entry_values->dest_sys_port_agr;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_bfd_pwe_params));

    oamp_rfc_6374_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    oamp_rfc_6374_params->counter_ptr = entry_values->counter_ptr;
    oamp_rfc_6374_params->counter_interface = entry_values->counter_interface;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.session_id = entry_values->session_id;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.timestamp_format = entry_values->timestamp_format;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.dm_t_flag = entry_values->dm_t_flag;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.dm_ds = entry_values->dm_priority;
    oamp_rfc_6374_params->exclusive.rfc_6374_only.lm_cw_choose = 0;
    if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
    {
        UTILEX_SET_BIT(oamp_rfc_6374_params->flags, is_last_entry, OAMP_MEP_LAST_ENTRY);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add
                    (unit, MEP_DB_ENTRY_TO_OAM_ID(entry_values->flex_lm_dm_entry), oamp_rfc_6374_params));

exit:
    SHR_FREE(oamp_bfd_pwe_params);
    SHR_FREE(oamp_rfc_6374_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_self_contained_endpoint_lm_stat_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_stat_entry, dm_entry_idx, next_bank_offset;
    int num_short_entries_in_bank;
    SHR_FUNC_INIT_VARS(unit);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = entry_values->flex_lm_entry;
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
    SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
    lm_stat_entry = search_params.resulting_entry;

    if (search_params.update_entry)
    {

        if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY))
        {

            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_DM_STAT_EXISTS) &&
                (entry_values->flex_lm_entry == MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr)))
            {

                dm_entry_idx = (lm_stat_entry + next_bank_offset);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, dm_entry_idx, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_DM_STATISTICS);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, lm_stat_entry, oamp_params));

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, dm_entry_idx));
            }
            else
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, lm_stat_entry));

                lm_stat_entry -= next_bank_offset;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, lm_stat_entry, oamp_params));
            }
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_UPDATE) &&
            !_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_LM_STAT_EXISTS) &&
            _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_DM_STAT_EXISTS) &&
            (entry_values->flex_lm_entry == MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr)))
        {

            dm_entry_idx = entry_values->flex_lm_entry + next_bank_offset;
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, dm_entry_idx, oamp_params));
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_DM_STATISTICS);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, dm_entry_idx + next_bank_offset, oamp_params));

            lm_stat_entry = dm_entry_idx;
            sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
            UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
            search_params.clear_last_entry = FALSE;
        }
        else
        {
            sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
        }

        oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
        num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
            dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        if ((oam_id / num_short_entries_in_bank) == (lm_stat_entry / num_short_entries_in_bank))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: adding a loss statistics entry will result in that entry "
                         "being in the same bank as the endpoint entry.\n");
        }
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, lm_stat_entry, oamp_params));

        if (search_params.clear_last_entry)
        {

            lm_stat_entry -= next_bank_offset;
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_stat_entry, oamp_params));
            UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, lm_stat_entry, oamp_params));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_init_y1731_cfg(
    int unit)
{
    uint32 entry_handle_id;
    uint32 bos;
    uint32 mpls_label;
    uint32 exp;
    uint32 ttl;
    uint32 mpls = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_Y1731_GENERAL_CFG, &entry_handle_id));

    mpls_label = MPLS_LABEL_GAL;
    ttl = MPLS_LABEL_GAL_TTL;
    exp = MPLS_LABEL_GAL_EXP;
    bos = MPLS_LABEL_GAL_BOS;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_BOS, &bos, &mpls));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_MPLS_LABEL, &mpls_label, &mpls));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_EXP, &exp, &mpls));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_TTL, &ttl, &mpls));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Y1731_MPLSTP_GAL, INST_SINGLE, mpls);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_oam_bfd_pwe_channel_value_set(unit, 0, BFD_PWE_DEFAULT_CHANNEL));
    SHR_IF_ERR_EXIT(dnx_oam_bfd_pwe_channel_value_set(unit, 1, BFD_MPLS_TP_CC_GACH_DEFAULT_CHANNEL));

    SHR_IF_ERR_EXIT(dnx_oam_y1731_mpls_tp_channel_value_set(unit, Y1731_MPLS_TP_GACH_DEFAULT_CHANNEL));

    SHR_IF_ERR_EXIT(dnx_oam_y1731_pwe_channel_value_set(unit, Y1731_PWE_GACH_DEFAULT_CHANNEL));

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_flexible_crc_tcam_set(
    int unit,
    uint8 tcam_index,
    const dnx_oam_oamp_flex_crc_tcam_t * flex_tcam_entry)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_CLS_FLEX_CRC_TCAM, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, tcam_index));

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_GACH_CHANNEL,
                                      flex_tcam_entry->opcode_or_gach_channel,
                                      flex_tcam_entry->opcode_or_gach_channel_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_GACH_SELECT,
                                      flex_tcam_entry->opcode_or_gach_select,
                                      flex_tcam_entry->opcode_or_gach_select_mask);

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE,
                                      flex_tcam_entry->oamp_mep_pe_profile, flex_tcam_entry->oamp_mep_pe_profile_mask);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_MASK_INDEX, INST_SINGLE,
                                 flex_tcam_entry->crc_mask_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_INDEX, INST_SINGLE, flex_tcam_entry->crc);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, flex_tcam_entry->valid);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_flexible_crc_mask_set(
    int unit,
    uint8 crc_index,
    dnx_oam_oamp_flex_crc_pdu_mask_t * flex_crc_mask)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_FLEX_CRC_MASK, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_MASK_INDEX, crc_index);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PER_BIT_MASK, INST_SINGLE,
                                    flex_crc_mask->flex_crc_per_bit_mask);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PER_BYTE_MASK, INST_SINGLE,
                                    flex_crc_mask->flex_crc_per_byte_mask);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_flexible_crc_init(
    int unit)
{
    uint32 mep_pe_profile = 0;
    uint8 tcam_index = 1;
    uint8 *mask = NULL;
    dnx_oam_oamp_flex_crc_tcam_t *flex_tcam_entry = NULL;
    dnx_oam_oamp_flex_crc_pdu_mask_t *flex_crc_mask = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(flex_tcam_entry, sizeof(dnx_oam_oamp_flex_crc_tcam_t), "Param struct for flexible crc tcam",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(flex_crc_mask, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t), "Param struct for flexible crc mask",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(mask, BITS2BYTES(DNX_OAM_OAMP_CRC_BYTE), "Size of CRC per Bytes field",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(mask, 0xFF, BITS2BYTES(DNX_OAM_OAMP_CRC_BYTE));

    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get(unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48, &mep_pe_profile));

    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;
    flex_tcam_entry->opcode_or_gach_channel = DNX_OAM_OAMP_OPCODE_CCM_CCM_LM;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0;
    flex_tcam_entry->opcode_or_gach_select = 1;
    flex_tcam_entry->opcode_or_gach_select_mask = 0;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index = 0;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));

    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_byte_mask, 70, mask, 0, BCM_OAM_GROUP_NAME_LENGTH, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48, &mep_pe_profile));

    tcam_index++;
    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;
    flex_tcam_entry->opcode_or_gach_channel = DNX_OAM_OAMP_OPCODE_CCM_CCM_LM;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0;
    flex_tcam_entry->opcode_or_gach_select = 1;
    flex_tcam_entry->opcode_or_gach_select_mask = 0;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index++;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));

    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_byte_mask, 70, mask, 0, BCM_OAM_GROUP_NAME_LENGTH, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6, &mep_pe_profile));
    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;

    tcam_index++;
    flex_tcam_entry->opcode_or_gach_channel = 0;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0x1ffff;
    flex_tcam_entry->opcode_or_gach_select = 0;
    flex_tcam_entry->opcode_or_gach_select_mask = 1;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index++;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));

    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_byte_mask, 88, mask, 0, 16, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48, &mep_pe_profile));

    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;
    tcam_index++;
    flex_tcam_entry->opcode_or_gach_channel = DNX_OAM_OAMP_OPCODE_CCM_CCM_LM;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0;
    flex_tcam_entry->opcode_or_gach_select = 1;
    flex_tcam_entry->opcode_or_gach_select_mask = 0;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index++;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));

    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_byte_mask, 70, mask, 0, BCM_OAM_GROUP_NAME_LENGTH, FALSE);

    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_bit_mask, 46, mask, 0, 1, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

    SHR_IF_ERR_EXIT(dnx_oamp_pe_mep_pe_profile_sw_get
                    (unit, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT, &mep_pe_profile));

    flex_tcam_entry->oamp_mep_pe_profile = mep_pe_profile;
    flex_tcam_entry->oamp_mep_pe_profile_mask = 0x0;
    tcam_index++;
    flex_tcam_entry->opcode_or_gach_channel = DNX_OAM_OAMP_OPCODE_CCM_CCM_LM;
    flex_tcam_entry->opcode_or_gach_channel_mask = 0;
    flex_tcam_entry->opcode_or_gach_select = 1;
    flex_tcam_entry->opcode_or_gach_select_mask = 0;
    flex_tcam_entry->crc = 0;
    flex_tcam_entry->crc_mask_index++;
    flex_tcam_entry->valid = TRUE;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_tcam_set(unit, tcam_index, flex_tcam_entry));

    sal_memset(flex_crc_mask, 0, sizeof(dnx_oam_oamp_flex_crc_pdu_mask_t));

    dnx_oamp_copy_bit_array(flex_crc_mask->flex_crc_per_bit_mask, 46, mask, 0, 1, FALSE);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_flexible_crc_mask_set(unit, flex_tcam_entry->crc_mask_index, flex_crc_mask));

exit:
    SHR_FREE(mask);
    SHR_FREE(flex_tcam_entry);
    SHR_FREE(flex_crc_mask);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_dbal_dynamic_memory_access_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_dbal_dynamic_memory_access_get(
    int unit,
    uint32 *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_opcode_for_count_table_init(
    int unit)
{

    int index;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OPCODE_FOR_COUNT, &entry_handle_id));

    for (index = 0; index < 6; index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_INDEX, index);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT, INST_SINGLE, index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_RX_ENABLE, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_TX_ENABLE, INST_SINGLE, FALSE);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_lm_offloaded_endpoint_set(
    int unit,
    uint16 oam_id,
    const dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params)
{
    uint8 set_part2_ptr = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_add(unit, oam_id, oamp_params));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
    oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
    oamp_params->mep_profile = entry_values->mep_profile;
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_MEASURE_NEXT_RECEIVED_SLR))
    {
        oamp_params->counter_ptr = entry_values->slm_counter_pointer;
        oamp_params->counter_interface = entry_values->slm_core_id;
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_add(unit, oam_id, oamp_params));

    if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for LM/DM offloaded MEPs, LM statistics are "
                     "automatically calculated, ADD_STAT_ENTRY flag must be set.\n");
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
    if (oamp_params->flex_lm_dm_ptr == 0)
    {
        if (entry_values->flex_lm_entry == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: the second MEP DB entry has not been created "
                         "yet for this LM/DM offloaded MEP, so the flex_lm_entry must be "
                         "set to the target entry, which cannot be 0.");
        }

        set_part2_ptr = TRUE;
        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_MEASURE_NEXT_RECEIVED_SLR))
        {
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_MEASURE_NEXT_RECEIVED_SLR);
        }
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_DM_OFFLOADED);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, entry_values->flex_lm_entry, oamp_params));
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_STAT_ENABLE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_ENABLE);
    if (set_part2_ptr)
    {

        oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_lm_entry);
    }
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_MEASURE_NEXT_RECEIVED_SLR))
    {
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_MEASURE_NEXT_RECEIVED_SLR);
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry(
    int unit,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    dnx_oam_oamp_lm_dm_search_t * search_params,
    int is_find_only)
{
    uint32 lm_dm_entry, next_bank_offset;
    SHR_FUNC_INIT_VARS(unit);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    if (!IS_LM_DM_TYPE(search_params->search_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: %d is not an LM type nor a DM TYPE.\n", search_params->search_type);
    }

    search_params->update_entry = FALSE;
    search_params->clear_last_entry = TRUE;
    lm_dm_entry = search_params->first_entry;

    if (lm_dm_entry != 0)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_dm_entry, oamp_params));
    }

    if ((lm_dm_entry == 0) || !IS_LM_DM_TYPE(oamp_params->mep_type))
    {

        search_params->resulting_entry = search_params->param_entry;
        search_params->clear_last_entry = FALSE;
    }
    else
    {
        while (TRUE)
        {
            if (oamp_params->mep_type == search_params->search_type)
            {
                search_params->resulting_entry = lm_dm_entry;
                search_params->update_entry = TRUE;
                search_params->clear_last_entry = FALSE;
                break;
            }
            lm_dm_entry += next_bank_offset;
            if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LAST_ENTRY))
            {
                if (is_find_only == LM_DM_FIND_ONLY)
                {
                    search_params->resulting_entry = lm_dm_entry - next_bank_offset;
                }
                else
                {
                    search_params->resulting_entry = lm_dm_entry;
                }
                break;
            }
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_dm_entry, oamp_params));
            if (!IS_LM_DM_TYPE(oamp_params->mep_type))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: an entry of type %d found before last entry.\n",
                             oamp_params->mep_type);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_self_contained_endpoint_lm_db_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *update_ptr)
{
    uint16 flex_lm_dm_ptr = 0;
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_db_entry, next_bank_offset;
    int num_short_entries_in_bank;
    SHR_FUNC_INIT_VARS(unit);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    flex_lm_dm_ptr = oamp_params->flex_lm_dm_ptr;

    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(flex_lm_dm_ptr);
    search_params.param_entry = entry_values->flex_lm_entry;
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
    SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
    lm_db_entry = search_params.resulting_entry;

    if (search_params.update_entry)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_db_entry, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }
    else
    {
        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_MEASURE_NEXT_RECEIVED_SLR))
        {
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_MEASURE_NEXT_RECEIVED_SLR);
        }
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
    }
    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
    num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
        dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    if ((oam_id / num_short_entries_in_bank) == (lm_db_entry / num_short_entries_in_bank))

    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error: adding a loss entry will result in that entry "
                     "being in the same bank as the endpoint entry.\n");
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, lm_db_entry, oamp_params));
    entry_values->flex_lm_entry = lm_db_entry;

    if (search_params.clear_last_entry)
    {

        lm_db_entry -= next_bank_offset;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_db_entry, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, lm_db_entry, oamp_params));
    }

    if (!search_params.update_entry)
    {
        if (flex_lm_dm_ptr == 0)
        {

            oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(lm_db_entry);
        }
        else
        {

            oamp_params->flex_lm_dm_ptr = flex_lm_dm_ptr;
        }
    }

    *update_ptr = !search_params.update_entry && !search_params.clear_last_entry;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_lm_offloaded_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_exists)
{
    uint32 second_hl_entry;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));

    if (!UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_ENABLE) ||
        !UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_STAT_ENABLE))
    {
        *lm_exists = 0;
        SHR_EXIT();
    }

    *lm_exists = 1;
    entry_values->lm_my_tx = oamp_params->exclusive.lm_dm.lm_my_tx;
    entry_values->lm_my_rx = oamp_params->exclusive.lm_dm.lm_my_rx;
    entry_values->lm_peer_tx = oamp_params->exclusive.lm_dm.lm_peer_tx;
    entry_values->lm_peer_rx = oamp_params->exclusive.lm_dm.lm_peer_rx;
    entry_values->last_lm_far = oamp_params->exclusive.lm_dm.last_lm_far;
    entry_values->last_lm_near = oamp_params->exclusive.lm_dm.last_lm_near;
    entry_values->max_lm_far = oamp_params->exclusive.lm_dm.max_lm_far;

    if (oamp_params->flex_lm_dm_ptr != 0)
    {
        second_hl_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, second_hl_entry, oamp_params));
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
            UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            entry_values->acc_lm_far = oamp_params->exclusive.lm_dm.acc_lm_far;
            entry_values->acc_lm_near = oamp_params->exclusive.lm_dm.acc_lm_near;
            entry_values->max_lm_near = oamp_params->exclusive.lm_dm.max_lm_near;
        }
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    entry_values->mep_profile = oamp_params->mep_profile;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));

    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_self_contained_endpoint_lm_db_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_exists)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_db_entry;
    SHR_FUNC_INIT_VARS(unit);

    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
    SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
    lm_db_entry = search_params.resulting_entry;

    if (!search_params.update_entry)
    {
        *lm_exists = 0;
        SHR_EXIT();
    }
    *lm_exists = 1;
    entry_values->lm_db_entry_idx = lm_db_entry;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_db_entry, oamp_params));

    entry_values->lm_my_tx = oamp_params->exclusive.lm_dm.lm_my_tx;
    entry_values->lm_my_rx = oamp_params->exclusive.lm_dm.lm_my_rx;
    entry_values->lm_peer_tx = oamp_params->exclusive.lm_dm.lm_peer_tx;
    entry_values->lm_peer_rx = oamp_params->exclusive.lm_dm.lm_peer_rx;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_self_contained_endpoint_lm_stat_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_stat_exists)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_stat_entry;
    SHR_FUNC_INIT_VARS(unit);

    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
    SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
    lm_stat_entry = search_params.resulting_entry;

    *lm_stat_exists = 0;
    if (search_params.update_entry)
    {
        *lm_stat_exists = 1;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_stat_entry, oamp_params));
        entry_values->lm_stat_entry_idx = lm_stat_entry;

        entry_values->acc_lm_far = oamp_params->exclusive.lm_dm.acc_lm_far;
        entry_values->acc_lm_near = oamp_params->exclusive.lm_dm.acc_lm_near;
        entry_values->last_lm_far = oamp_params->exclusive.lm_dm.last_lm_far;
        entry_values->last_lm_near = oamp_params->exclusive.lm_dm.last_lm_near;
        entry_values->max_lm_far = oamp_params->exclusive.lm_dm.max_lm_far;
        entry_values->max_lm_near = oamp_params->exclusive.lm_dm.max_lm_near;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_dm_offloaded_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params)
{
    uint32 second_mep_db_entry;
    uint8 set_part2_ptr = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for LM/DM offloaded MEPs, DM measurement can"
                     "only be two-way, so two way flag must be set.\n");
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_add(unit, oam_id, oamp_params));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
    oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_add(unit, oam_id, oamp_params));

    second_mep_db_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);

    if (second_mep_db_entry != 0)
    {

        if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_UPDATE))
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, second_mep_db_entry, oamp_params));

            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
            UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, second_mep_db_entry, oamp_params));
        }
    }
    else
    {

        if (entry_values->flex_dm_entry == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: the second MEP DB entry has not been created "
                         "yet for this LM/DM offloaded MEP, so the flex_dm_entry must be"
                         "set to the target entry, which cannot be 0.");
        }

        set_part2_ptr = TRUE;
        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
        oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY;

        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_DM_OFFLOADED);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);

        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, entry_values->flex_dm_entry, oamp_params));
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_DM_STAT_ENABLE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_DM_ENABLE);
    if (set_part2_ptr)
    {

        oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_dm_entry);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_dm_offloaded_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *is_found)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));

    if (!UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_ENABLE) ||
        !UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_STAT_ENABLE))
    {
        *is_found = 0;
        SHR_EXIT();
    }

    *is_found = 1;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr), oamp_params));

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
        UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        sal_memcpy(entry_values->last_delay, oamp_params->exclusive.lm_dm.last_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->max_delay, oamp_params->exclusive.lm_dm.max_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->min_delay, oamp_params->exclusive.lm_dm.min_delay, TWO_WAY_DELAY_SIZE);
        entry_values->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
        entry_values->flex_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error!  This endpoint has no DM entry.\n");
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    entry_values->mep_profile = oamp_params->mep_profile;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));

    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_self_contained_endpoint_dm_stat_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *is_found)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 dm_stat_entry;
    SHR_FUNC_INIT_VARS(unit);

    entry_values->flex_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);

    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
    SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
    dm_stat_entry = search_params.resulting_entry;

    *is_found = 0;
    if (search_params.update_entry)
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, dm_stat_entry, oamp_params));
        *is_found = 1;
        entry_values->dm_stat_entry_idx = dm_stat_entry;
        sal_memcpy(entry_values->last_delay, oamp_params->exclusive.lm_dm.last_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->max_delay, oamp_params->exclusive.lm_dm.max_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->min_delay, oamp_params->exclusive.lm_dm.min_delay, TWO_WAY_DELAY_SIZE);
        entry_values->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
    }
    else
    {
        search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
        SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
        dm_stat_entry = search_params.resulting_entry;
        if (search_params.update_entry)
        {
            *is_found = 1;

            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, dm_stat_entry, oamp_params));
            entry_values->dm_stat_entry_idx = dm_stat_entry;
            sal_memcpy(entry_values->last_delay_one_way, oamp_params->exclusive.lm_dm.last_delay_one_way,
                       ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->max_delay_one_way, oamp_params->exclusive.lm_dm.max_delay_one_way,
                       ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->min_delay_one_way, oamp_params->exclusive.lm_dm.min_delay_one_way,
                       ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->last_delay_one_way_near_end,
                       oamp_params->exclusive.lm_dm.last_delay_one_way_near_end, ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->max_delay_one_way_near_end,
                       oamp_params->exclusive.lm_dm.max_delay_one_way_near_end, ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->min_delay_one_way_near_end,
                       oamp_params->exclusive.lm_dm.min_delay_one_way_near_end, ONE_WAY_DELAY_SIZE);
            entry_values->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_tpid_profile_set(
    int unit,
    uint8 profile,
    uint16 tpid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_TPID_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_tpid_profile_get(
    int unit,
    uint8 profile,
    uint16 *tpid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_TPID_PROFILE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, profile);

    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_extra_data_get(
    int unit,
    int index,
    uint8 *data,
    uint16 *crc_1,
    uint16 *crc_2)
{
    uint32 entry_id, next_bank_offset;
    int i;
    uint32 entry_handle_id, nof_entries, check_crc_value_1, check_crc_value_2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    uint8 *tmp_data = NULL;

    SHR_ALLOC_SET_ZERO(tmp_data, DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY / UTILEX_NOF_BITS_IN_BYTE,
                       "Array for over 32-bit dbal fields", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    entry_id = MEP_DB_ENTRY_TO_OAM_ID(index);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE, tmp_data));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EXT_DATA_LENGTH, INST_SINGLE, &nof_entries));
    if (crc_1 != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_1, INST_SINGLE, &check_crc_value_1));
        *crc_1 = (uint16) check_crc_value_1;
    }
    if (crc_2 != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_CHECK_CRC_VALUE_2, INST_SINGLE, &check_crc_value_2));
        *crc_2 = (uint16) check_crc_value_2;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_MEP_DB, entry_handle_id));

    dnx_oamp_copy_bit_array(data, 0, tmp_data, 0, DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY, TRUE);

    for (i = 1; i != nof_entries; ++i)
    {
        int bits_to_copy = DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY, start_from = 0;

        entry_id += next_bank_offset;

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE, tmp_data));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_MEP_DB, entry_handle_id));

        dnx_oamp_copy_bit_array(data,
                                DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY +
                                DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY * (i - 1), tmp_data, start_from, bits_to_copy,
                                TRUE);
    }

exit:
    SHR_FREE(tmp_data);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_short_entry_type_check_mep_db(
    int unit,
    uint16 oam_id,
    dbal_enum_value_field_oamp_mep_type_e req_mep_type,
    int *sub_index,
    uint8 *is_leader_alloc,
    uint8 *is_leader_type_match)
{
    dnx_oam_oamp_mep_db_args_t *leader_info = NULL;
    uint32 leader_endpoint_id;
    int nof_mep_db_short_entries;
    SHR_FUNC_INIT_VARS(unit);

    *is_leader_type_match = TRUE;

    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    *sub_index = (oam_id % nof_mep_db_short_entries);
    if (*sub_index != 0)
    {

        SHR_ALLOC_SET_ZERO(leader_info, sizeof(dnx_oam_oamp_mep_db_args_t),
                           "Short entry read for MEP-type comparison", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        leader_endpoint_id = oam_id - *sub_index;
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_is_allocated(unit, leader_endpoint_id, is_leader_alloc));
        if (*is_leader_alloc == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, leader_endpoint_id, leader_info));
            if (req_mep_type == leader_info->mep_type)
            {
                *is_leader_type_match = TRUE;
            }
            else
            {
                *is_leader_type_match = FALSE;
            }
        }
    }

exit:
    SHR_FREE(leader_info);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_short_entry_type_verify(
    int unit,
    uint16 oam_id,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    int sub_index;
    dbal_enum_value_field_oamp_mep_type_e req_mep_type;
    uint8 is_leader_alloc, is_leader_type_match;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type(unit, endpoint_info->type, &req_mep_type));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_short_entry_type_check_mep_db
                    (unit, oam_id, req_mep_type, &sub_index, &is_leader_alloc, &is_leader_type_match));

    if (sub_index != 0)
    {
        if (is_leader_alloc == FALSE)
        {

            SHR_ERR_EXIT(_SHR_E_PARAM, "Short entries with a non-zero sub-index can only "
                         "be added after the zero sub-index of the same full entry was "
                         "added, so you must first add entry 0x%08X before you can add entry "
                         "0x%08X", endpoint_info->id - sub_index, endpoint_info->id);
        }

        if (is_leader_type_match == FALSE)
        {

            SHR_ERR_EXIT(_SHR_E_PARAM, "Short entries with a non-zero sub-index must have "
                         "the same MEP-type as the zero sub-index of the same full entry, "
                         "so entry 0x%08X must have the same MEP type as entry 0x%08X",
                         endpoint_info->id, endpoint_info->id - sub_index);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_extra_data_set(
    int unit,
    int index,
    uint32 nof_data_bits,
    uint8 *data,
    int opcode,
    uint16 calculated_crc_1,
    uint16 calculated_crc_2,
    uint8 is_update)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    dnx_oamp_mep_db_memory_type_t memory_type;
    uint32 entry_id, next_bank_offset, max_nof_endpoint_id;
    int i;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));
    max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    entry_id = MEP_DB_ENTRY_TO_OAM_ID(index);

    if (!is_update)
    {
        memory_type.is_2byte_maid = FALSE;
        memory_type.is_extra_pool = (entry_id >= max_nof_endpoint_id);
        memory_type.is_full = TRUE;
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &memory_type, &entry_id));
    }

    if (is_update)
    {
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }
    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR;
    oamp_params->exclusive.extra_data.extra_data_len = (nof_data_bits == 0) ? 1 :
        ((nof_data_bits - 1) / DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY + 1);
    oamp_params->exclusive.extra_data.opcode_bmp = opcode;
    oamp_params->exclusive.extra_data.crc_val1 = calculated_crc_1;
    oamp_params->exclusive.extra_data.crc_val2 = calculated_crc_2;
    dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment, 0, data, 0,
                            DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY, FALSE);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, entry_id, oamp_params));

    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD;

    for (i = 1; i != oamp_params->exclusive.extra_data.extra_data_len; ++i)
    {
        int bits_left = 0, bits_to_copy = 0, start_from = 0;
        entry_id += next_bank_offset;

        if (!is_update)
        {
            memory_type.is_extra_pool = (entry_id >= max_nof_endpoint_id);
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &memory_type, &entry_id));
        }
        bits_left =
            nof_data_bits - DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY - DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY * (i -
                                                                                                                  1);
        if (bits_left < DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY)
        {
            bits_to_copy = bits_left;
            start_from = DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY - bits_left;

            sal_memset(oamp_params->exclusive.extra_data.data_segment, 0,
                       sizeof(oamp_params->exclusive.extra_data.data_segment));
        }
        else
        {
            bits_to_copy = DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY;
        }
        dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment,
                                start_from, data,
                                DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY +
                                DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY * (i - 1), bits_to_copy, FALSE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, entry_id, oamp_params));

    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_endpoint_workaround_fill_id(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    const uint8 *group_name)
{
    dnx_oam_oamp_mep_db_args_t *possible_leader_info = NULL;
    uint32 possible_leader_id = 0, possible_appropriate_entry_id;
    int nof_sub_entries, leader_index, index, full_entry_threshold, nof_icc_entries;
    dbal_enum_value_field_oamp_mep_type_e req_mep_type;
    uint8 possible_leader_found = TRUE, appropriate_entry_not_found = TRUE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type(unit, endpoint_info->type, &req_mep_type));

    SHR_ALLOC_SET_ZERO(possible_leader_info, sizeof(dnx_oam_oamp_mep_db_args_t),
                       "OAM endpoint data read for type comparison", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    nof_sub_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_icc_entries = full_entry_threshold / nof_sub_entries;

    if (nof_icc_entries == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "An OAM short entry endpoint of type %d cannot be created", endpoint_info->type);
    }

    for (leader_index = 0;
         (leader_index < nof_icc_entries) &&
         ((possible_leader_found == TRUE) && (appropriate_entry_not_found == TRUE)); leader_index++)
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_get_next_icc_entry(unit, &possible_leader_id, &possible_leader_found));

        if (possible_leader_found == TRUE)
        {

            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, possible_leader_id, possible_leader_info));
            if (possible_leader_info->mep_type == req_mep_type)
            {

                for (index = 1; index < nof_sub_entries; index++)
                {
                    possible_appropriate_entry_id = possible_leader_id + index;
                    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_is_allocated
                                    (unit, possible_appropriate_entry_id, &appropriate_entry_not_found));
                    if (appropriate_entry_not_found == FALSE)
                        break;
                }
            }

            possible_leader_id += nof_sub_entries;
        }
    }

    if (appropriate_entry_not_found == TRUE)
    {

        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_get_free_icc_id
                        (unit, &possible_appropriate_entry_id, &appropriate_entry_not_found));
        if (appropriate_entry_not_found == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "An OAM short entry endpoint of type %d cannot be created",
                         endpoint_info->type);
        }
    }

    endpoint_info->id = possible_appropriate_entry_id;
    endpoint_info->flags |= BCM_OAM_ENDPOINT_WITH_ID;

    SHR_IF_ERR_EXIT(dnx_oam_local_accelerated_endpoint_fill_id(unit, endpoint_info, group_name, 0));
    endpoint_info->flags &= ~BCM_OAM_ENDPOINT_WITH_ID;

exit:
    SHR_FREE(possible_leader_info);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_scan_count_req_in_6_count_cycle(
    int unit,
    dbal_enum_value_field_ccm_interval_e new_rate,
    uint32 *scan_count)
{
    SHR_FUNC_INIT_VARS(unit);

    if (new_rate == DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX)
    {

        *scan_count = 0;
    }
    else if (new_rate == DBAL_ENUM_FVAL_CCM_INTERVAL_3MS)
    {

        *scan_count = 3;
    }
    else if (new_rate >= DBAL_ENUM_FVAL_CCM_INTERVAL_10MS && new_rate < DBAL_NOF_ENUM_CCM_INTERVAL_VALUES)
    {

        *scan_count = 1;
    }
    else
    {

        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid rate passed %d", new_rate);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_get_transmit_rate_for_opcode(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    const dnx_oam_mep_profile_t * mep_profile_data,
    dbal_enum_value_field_ccm_interval_e * ccm_interval)
{
    SHR_FUNC_INIT_VARS(unit);
    *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX;

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:

            *ccm_interval = ccm_rate;
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile_data->lmm_rate, ccm_interval));
            break;
        case OAMP_MEP_TX_OPCODE_DMM:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval(unit, mep_profile_data->dmm_rate, ccm_interval));
            break;
        case OAMP_MEP_TX_OPCODE_OP0:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval
                            (unit, mep_profile_data->opcode_0_rate, ccm_interval));
            break;
        case OAMP_MEP_TX_OPCODE_OP1:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval
                            (unit, mep_profile_data->opcode_1_rate, ccm_interval));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid opcode passed %d", opcode);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_scan_count_verify_max_per_mep(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dnx_oam_oamp_tx_opcode opcode_i;
    uint32 total_scan_count = 0, scan_count;
    dbal_enum_value_field_ccm_interval_e rate = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (opcode_i = OAMP_MEP_TX_OPCODE_CCM_BFD; opcode_i < OAMP_MEP_TX_OPCODE_MAX_SUPPORTED; opcode_i++)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_transmit_rate_for_opcode
                        (unit, opcode_i, ccm_rate, mep_profile_data, &rate));
        scan_count = 0;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_scan_count_req_in_6_count_cycle(unit, rate, &scan_count));
        total_scan_count += scan_count;
    }

    if (total_scan_count > 6)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Unsupported number of packets requested to be transmitted from a single MEP");
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_scan_count_calc_verify(
    int unit,
    uint8 is_short_entry_type,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dnx_oam_oamp_tx_opcode other_opcode;
    dbal_enum_value_field_ccm_interval_e opcode_rate = 0;
    dbal_enum_value_field_ccm_interval_e other_opcode_rate = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (is_short_entry_type)
    {
        if (opcode == OAMP_MEP_TX_OPCODE_CCM_BFD)
        {

            SHR_EXIT();
        }
        else
        {

            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Short entry endpoints support only CCM %d", opcode);
        }
    }

    if (opcode == OAMP_MEP_TX_OPCODE_OP0 || opcode == OAMP_MEP_TX_OPCODE_OP1)
    {
        if (opcode == OAMP_MEP_TX_OPCODE_OP0)
        {
            other_opcode = OAMP_MEP_TX_OPCODE_OP1;
        }
        else
        {
            other_opcode = OAMP_MEP_TX_OPCODE_OP0;
        }

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_transmit_rate_for_opcode(unit, opcode, 0, mep_profile_data, &opcode_rate));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_transmit_rate_for_opcode(unit, other_opcode, 0, mep_profile_data,
                                                                     &other_opcode_rate));

        if ((opcode_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX)
            && (other_opcode_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "We do not support case of both opcode 0 and 1 rate being non-zero %d %d",
                         opcode_rate, other_opcode_rate);
        }

        if ((opcode_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX) && (opcode_rate < DBAL_ENUM_FVAL_CCM_INTERVAL_1S))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "We do not support case of opcode 0 or 1 rate being less than 1s %d", opcode_rate);
        }
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_scan_count_verify_max_per_mep(unit, opcode, ccm_rate, mep_profile_data));
exit:
    SHR_FUNC_EXIT;
}

static void
dnx_oam_oamp_v1_scan_count_calc_ccm_bfd(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_short_entry_type,
    uint8 ccm_rx_without_tx,
    dnx_oam_mep_profile_t * mep_profile_data)
{

    if (ccm_rx_without_tx)
    {

        mep_profile_data->ccm_count = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
    }
    else
    {
        if (is_short_entry_type)
        {

            mep_profile_data->ccm_count = 0;
        }
        else
        {

            mep_profile_data->ccm_count = (endpoint_id >> 2) & 1;
        }
    }

}

static shr_error_e
dnx_oam_oamp_v1_get_existing_phase_cnt_for_opcode(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    const dnx_oam_mep_profile_t * mep_profile_data,
    uint32 *phase_cnt)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:
            *phase_cnt = mep_profile_data->ccm_count;
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
            *phase_cnt = mep_profile_data->lmm_count;
            break;
        case OAMP_MEP_TX_OPCODE_DMM:
            *phase_cnt = mep_profile_data->dmm_count;
            break;
        case OAMP_MEP_TX_OPCODE_OP0:
            *phase_cnt = mep_profile_data->opcode_0_count;
            break;
        case OAMP_MEP_TX_OPCODE_OP1:
            *phase_cnt = mep_profile_data->opcode_1_count;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid opcode passed %d", opcode);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_scan_count_calc_lm_dm(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_jumbo_tlv_dm,
    uint8 ccm_transmit_phase_is_odd,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dbal_enum_value_field_ccm_interval_e my_rate = 0, other_rate = 0;
    dnx_oam_oamp_tx_opcode other_opcode;
    uint32 my_cnt, other_cnt;
    int dmm_phase = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_DMM;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_LMM;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __func__, opcode);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_transmit_rate_for_opcode(unit, opcode, 0, mep_profile_data, &my_rate));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_transmit_rate_for_opcode(unit, other_opcode, 0, mep_profile_data, &other_rate));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_existing_phase_cnt_for_opcode(unit, opcode, mep_profile_data, &my_cnt));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_existing_phase_cnt_for_opcode
                    (unit, other_opcode, mep_profile_data, &other_cnt));

    if (is_jumbo_tlv_dm)
    {

        dmm_phase =
            (((endpoint_id / dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit)) / JUMBO_TLV_ENTRIES_STEPS) % 8);
        mep_profile_data->dmm_count =
            (dmm_phase ? ((2 * dmm_phase - 2) % 14) : 14) + (mep_profile_data->ccm_count == 0);

        if (ccm_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_3MS)
        {

            if (opcode == OAMP_MEP_TX_OPCODE_LMM)
            {
                mep_profile_data->lmm_count = (ccm_transmit_phase_is_odd) ? 3 : 2;
            }
        }
        else
        {

            if (opcode == OAMP_MEP_TX_OPCODE_LMM)
            {
                mep_profile_data->lmm_count = (ccm_transmit_phase_is_odd) ? 16 : 17;
            }
        }

        if (opcode == OAMP_MEP_TX_OPCODE_DMM)
        {
            mep_profile_data->dmm_count = my_cnt;
            mep_profile_data->lmm_count = other_cnt;
        }
        SHR_EXIT();
    }

    if (my_rate == DBAL_ENUM_FVAL_CCM_INTERVAL_3MS)
    {

        my_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
        if (other_rate)
        {

            other_cnt = (ccm_transmit_phase_is_odd) ? 3 : 2;
        }
    }
    else
    {

        if (my_rate)
        {
            if ((other_rate) && (my_rate < other_rate))
            {

                my_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
                other_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
            }
            else if ((other_rate) && (my_rate >= other_rate))
            {

                my_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
                other_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
            }
            else
            {

                my_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
                other_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
            }
        }

    }

    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        mep_profile_data->lmm_count = my_cnt;
        mep_profile_data->dmm_count = other_cnt;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        mep_profile_data->dmm_count = my_cnt;
        mep_profile_data->lmm_count = other_cnt;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_scan_count_calc_opcode_x(
    int unit,
    uint8 ccm_transmit_phase_is_odd,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dbal_enum_value_field_ccm_interval_e tx_rate = 0;
    SHR_FUNC_INIT_VARS(unit);

    if ((opcode != OAMP_MEP_TX_OPCODE_OP0) && (opcode != OAMP_MEP_TX_OPCODE_OP1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __func__, opcode);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_transmit_rate_for_opcode(unit, opcode, 0, mep_profile_data, &tx_rate));

    if (tx_rate)
    {
        if (opcode == OAMP_MEP_TX_OPCODE_OP0)
        {
            mep_profile_data->opcode_0_count = (ccm_transmit_phase_is_odd) ? 4 : 5;
        }
        else
        {
            mep_profile_data->opcode_1_count = (ccm_transmit_phase_is_odd) ? 4 : 5;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_get_existing_scan_rate_for_opcode(
    int unit,
    uint32 mep_id,
    dnx_oam_oamp_tx_opcode opcode,
    const dnx_oam_mep_profile_t * mep_profile_data,
    uint32 *scan_cnt)
{

    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    int dummy;
    dnx_bfd_tx_period_t tx_rate_data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:

            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, mep_id, oamp_params));
            SHR_IF_ERR_EXIT(algo_bfd_db.
                            bfd_oamp_tx_rate.profile_data_get(unit, oamp_params->exclusive.bfd_only.bfd_tx_rate, &dummy,
                                                              &tx_rate_data));
            *scan_cnt = tx_rate_data.tx_period;
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
            SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get(unit, mep_profile_data->lmm_rate, &dummy,
                                                                          &tx_rate_data));
            *scan_cnt = tx_rate_data.tx_period;
            break;
        case OAMP_MEP_TX_OPCODE_DMM:
            SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.profile_data_get(unit, mep_profile_data->dmm_rate, &dummy,
                                                                          &tx_rate_data));
            *scan_cnt = tx_rate_data.tx_period;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid opcode passed %d", opcode);
            break;
    }
exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_scan_count_calc_rfc_6374_lm_dm(
    int unit,
    uint32 mep_id,
    uint32 tx_rate,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dnx_oam_oamp_tx_opcode other_opcode, other_opcode_2;
    uint32 my_cnt = 0, other_cnt = 0, other_cnt_2 = 0;
    uint32 scan_rate_arr[3] = { 0xffffffff, 0xffffffff, 0xffffffff }, temp = 0xffffffff;
    uint32 other_scan_rate = 0xffffffff, other_scan_rate2 = 0xffffffff, my_scan_rate = 0xffffffff;

    SHR_FUNC_INIT_VARS(unit);

    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_DMM;
        other_opcode_2 = OAMP_MEP_TX_OPCODE_CCM_BFD;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_LMM;
        other_opcode_2 = OAMP_MEP_TX_OPCODE_CCM_BFD;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_CCM_BFD)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_LMM;
        other_opcode_2 = OAMP_MEP_TX_OPCODE_DMM;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __func__, opcode);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_existing_phase_cnt_for_opcode(unit, opcode, mep_profile_data, &my_cnt));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_existing_phase_cnt_for_opcode
                    (unit, other_opcode, mep_profile_data, &other_cnt));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_existing_phase_cnt_for_opcode
                    (unit, other_opcode_2, mep_profile_data, &other_cnt_2));

    my_scan_rate = BCM_OAMP_SW_ENTRY_INFO_PERIOD_TO_MEP_DB_SCAN_RATE(tx_rate);

    if (my_scan_rate != 0xffffffff)
    {

        scan_rate_arr[0] = my_scan_rate + 1;

        if (other_cnt_2 == DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT)
        {
            if (other_cnt != DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT)
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_existing_scan_rate_for_opcode
                                (unit, mep_id, other_opcode, mep_profile_data, &other_scan_rate));

                if (other_scan_rate > my_scan_rate && ((other_scan_rate + 1) % (my_scan_rate + 1) != 0))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid rate passed %d, the rate should be multiple of already configured ones, but in units of 1.6667.",
                                 tx_rate);
                }
                else if (my_scan_rate > other_scan_rate && ((my_scan_rate + 1) % (other_scan_rate + 1) != 0))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid rate passed %d, the rate should be multiple of already configured ones, but in units of 1.6667.",
                                 tx_rate);
                }
            }

        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_existing_scan_rate_for_opcode
                            (unit, mep_id, other_opcode_2, mep_profile_data, &other_scan_rate2));
            scan_rate_arr[1] = other_scan_rate2 + 1;
            if (other_cnt != DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT)
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_get_existing_scan_rate_for_opcode
                                (unit, mep_id, other_opcode, mep_profile_data, &other_scan_rate));
                scan_rate_arr[2] = other_scan_rate + 1;

                if (scan_rate_arr[1] < scan_rate_arr[0])
                {
                    temp = scan_rate_arr[0];
                    scan_rate_arr[0] = scan_rate_arr[1];
                    scan_rate_arr[1] = temp;
                }
                if (scan_rate_arr[2] < scan_rate_arr[1])
                {
                    temp = scan_rate_arr[1];
                    scan_rate_arr[1] = scan_rate_arr[2];
                    scan_rate_arr[2] = temp;
                    if (scan_rate_arr[1] < scan_rate_arr[0])
                    {
                        temp = scan_rate_arr[0];
                        scan_rate_arr[0] = scan_rate_arr[1];
                        scan_rate_arr[1] = temp;
                    }
                }

                if (other_scan_rate2 == 1 && other_scan_rate == 1)
                {

                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid rate passed %d, two opcodes already has maximum tx rate",
                                 tx_rate);
                }

                if (scan_rate_arr[1] % scan_rate_arr[0] != 0 || scan_rate_arr[2] % scan_rate_arr[0] != 0
                    || scan_rate_arr[2] % scan_rate_arr[1] != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid rate passed %d, the rate should be multiple of already configured ones, but in units of 1.6667.",
                                 tx_rate);
                }
            }
            else
            {

                if (other_scan_rate2 > my_scan_rate && ((other_scan_rate2 + 1) % (my_scan_rate + 1) != 0))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid rate passed %d, the rate should be multiple of already configured ones, but in units of 1.6667.",
                                 tx_rate);
                }
                else if (my_scan_rate > other_scan_rate2 && ((my_scan_rate + 1) % (other_scan_rate2 + 1) != 0))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid rate passed %d, the rate should be multiple of already configured ones, but in units of 1.6667.",
                                 tx_rate);
                }
            }
        }
    }

    if (my_scan_rate != 0xffffffff)
    {

        if (my_scan_rate == 1)
        {

            if (other_scan_rate == 1 || other_scan_rate2 == 1)
            {
                my_cnt = 0;

                if (other_scan_rate > other_scan_rate2)
                {
                    other_cnt_2 = 1;
                }
                else
                {
                    other_cnt = 1;
                }
            }
            else
            {
                my_cnt = 0;
                if (other_scan_rate > other_scan_rate2)
                {
                    other_cnt = other_scan_rate != 0xffffffff ? 3 : DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
                    other_cnt_2 = other_scan_rate2 != 0xffffffff ? 1 : DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
                }
                else if (other_scan_rate2 >= other_scan_rate)
                {
                    other_cnt = other_scan_rate != 0xffffffff ? 1 : DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
                    other_cnt_2 = other_scan_rate2 != 0xffffffff ? 3 : DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
                }
            }
        }
        else
        {
            if (other_scan_rate == 1 || other_scan_rate2 == 1)
            {
                if (other_scan_rate > other_scan_rate2)
                {
                    other_cnt_2 = 0;
                    if (other_scan_rate != 0xffffffff)
                    {
                        my_cnt = my_scan_rate < other_scan_rate ? 1 : 3;
                        other_cnt = my_scan_rate < other_scan_rate ? 3 : 1;
                    }
                    else
                    {
                        my_cnt = 1;
                        other_cnt = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
                    }
                }
                else
                {
                    other_cnt = 0;
                    if (other_scan_rate2 != 0xffffffff)
                    {
                        my_cnt = my_scan_rate < other_scan_rate2 ? 1 : 3;
                        other_cnt_2 = my_scan_rate < other_scan_rate2 ? 3 : 1;
                    }
                    else
                    {
                        my_cnt = 1;
                        other_cnt_2 = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
                    }
                }
            }
            else
            {
                my_cnt = 0;
                if (other_scan_rate != 0xffffffff)
                {
                    if (other_scan_rate2 != 0xffffffff && (other_scan_rate2 > other_scan_rate))
                    {
                        if (my_scan_rate <= other_scan_rate)
                        {

                            my_cnt = 0;
                            other_cnt = 1;
                            other_cnt_2 = 2;
                        }
                        else if (my_scan_rate <= other_scan_rate2)
                        {

                            my_cnt = 1;
                            other_cnt = 0;
                            other_cnt_2 = 2;

                        }
                        else
                        {

                            my_cnt = 2;
                            other_cnt = 0;
                            other_cnt_2 = 1;
                        }
                    }
                    else if (other_scan_rate2 != 0xffffffff && (other_scan_rate2 <= other_scan_rate))
                    {
                        if (my_scan_rate <= other_scan_rate2)
                        {

                            my_cnt = 0;
                            other_cnt = 2;
                            other_cnt_2 = 1;
                        }
                        else if (my_scan_rate <= other_scan_rate)
                        {

                            my_cnt = 1;
                            other_cnt = 2;
                            other_cnt_2 = 0;

                        }
                        else
                        {

                            my_cnt = 2;
                            other_cnt = 1;
                            other_cnt_2 = 0;
                        }
                    }
                    else
                    {
                        if (my_scan_rate <= other_scan_rate)
                        {
                            my_cnt = 0;
                            other_cnt = 1;
                        }
                        else
                        {
                            my_cnt = 1;
                            other_cnt = 0;
                        }
                        other_cnt_2 = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
                    }
                }
                else
                {
                    other_cnt = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
                    if ((other_scan_rate2 != 0xffffffff) && (my_scan_rate > other_scan_rate))
                    {
                        my_cnt = 1;
                        other_cnt_2 = 0;
                    }
                    else if ((other_scan_rate2 != 0xffffffff) && (my_scan_rate <= other_scan_rate))
                    {
                        my_cnt = 0;
                        other_cnt_2 = 1;
                    }
                    else
                    {
                        my_cnt = 0;
                        other_cnt_2 = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
                    }
                }
            }
        }
    }
    else
    {

        my_cnt = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
    }

    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        mep_profile_data->lmm_count = my_cnt;
        mep_profile_data->dmm_count = other_cnt;
        mep_profile_data->ccm_count = other_cnt_2;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        mep_profile_data->lmm_count = other_cnt;
        mep_profile_data->dmm_count = my_cnt;
        mep_profile_data->ccm_count = other_cnt_2;
    }
    else
    {
        mep_profile_data->lmm_count = other_cnt;
        mep_profile_data->dmm_count = other_cnt_2;
        mep_profile_data->ccm_count = my_cnt;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_scan_count_calc(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_jumbo_tlv_dm,
    uint8 is_short_entry_type,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    uint8 ccm_rx_without_tx,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    uint8 ccm_transmit_phase_is_odd = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (opcode != OAMP_MEP_TX_OPCODE_CCM_BFD)
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_scan_count_calc_verify
                        (unit, is_short_entry_type, ccm_rate, opcode, mep_profile_data));
    }

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:
            dnx_oam_oamp_v1_scan_count_calc_ccm_bfd(unit, endpoint_id, is_short_entry_type, ccm_rx_without_tx,
                                                    mep_profile_data);
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
        case OAMP_MEP_TX_OPCODE_DMM:
            DNX_OAM_IS_CCM_PHASE_ODD(ccm_transmit_phase_is_odd, is_short_entry_type, endpoint_id);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_scan_count_calc_lm_dm
                            (unit, endpoint_id, is_jumbo_tlv_dm, ccm_transmit_phase_is_odd, ccm_rate, opcode,
                             mep_profile_data));
            break;
        case OAMP_MEP_TX_OPCODE_OP0:
        case OAMP_MEP_TX_OPCODE_OP1:
            DNX_OAM_IS_CCM_PHASE_ODD(ccm_transmit_phase_is_odd, is_short_entry_type, endpoint_id);
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_scan_count_calc_opcode_x
                            (unit, ccm_transmit_phase_is_odd, opcode, mep_profile_data));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __func__, opcode);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_ccm_endpoint_mep_db_create_network_headers(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const bcm_oam_group_info_t * group_info,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    uint8 is_dual_ended_lm,
    const uint8 is_mpls_lm_dm_without_bfd_endpoint,
    dnx_oam_mep_profile_t * mep_profile)
{
    uint8 ccm_rx_without_tx = 0;
    uint32 mpls_tp_mdl_ignore = 0;
    int profile_id;
    uint8 first_reference;
    int prev_ccm_period = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_eth_oam_init
                        (unit, endpoint_info, mep_db_entry, mep_hw_profiles_write_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_ccm_endpoint_mpls_oam_init
                        (unit, endpoint_info, mep_db_entry, mep_hw_profiles_write_data));
    }

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
        {

            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period
                            (unit, mep_db_entry->ccm_interval, &prev_ccm_period));

            if (endpoint_info->ccm_period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED
                && prev_ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED)
            {
                ccm_rx_without_tx = 1;
            }
        }

        if (!ccm_rx_without_tx)
        {

            SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_period_to_ccm_interval
                            (unit, endpoint_info->ccm_period, &mep_db_entry->ccm_interval));
        }

        DNX_OAM_SET_RDI_GEN_METHOD_FIELD_ON_MEP_PROFILE(mep_profile->rdi_gen_method,
                                                        (endpoint_info->flags2 &
                                                         BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE),
                                                        (endpoint_info->flags2 &
                                                         BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE));
    }

    mep_profile->opcode_bit_map = 0xff;

    if ((_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS))
        || (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS)))
    {
        dnx_oam_oamp_statistics_enable(unit, endpoint_info->flags2, mep_profile);
    }

    if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
    {
        mep_profile->maid_check_dis = TRUE;
    }

    SHR_IF_ERR_EXIT(algo_oam_db.mpls_tp_mdl_ignore.get(unit, &mpls_tp_mdl_ignore));
    if (mpls_tp_mdl_ignore)
    {
        if (endpoint_info->type == bcmOAMEndpointTypeBhhSection ||
            endpoint_info->type == bcmOAMEndpointTypeBHHMPLS || endpoint_info->type == bcmOAMEndpointTypeBHHPwe)
        {
            mep_profile->mdl_check_dis = TRUE;
        }
    }

    mep_profile->lmm_offset =
        dnx_oam_oamp_v1_lm_dm_set_offset(unit, mep_db_entry, is_dual_ended_lm, 0, 0,
                                         (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp),
                                         _SHR_IS_FLAG_SET(endpoint_info->flags2,
                                                          BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN));
    mep_profile->lmr_offset =
        dnx_oam_oamp_v1_lm_dm_set_offset(unit, mep_db_entry, is_dual_ended_lm, 0, 1,
                                         (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp),
                                         _SHR_IS_FLAG_SET(endpoint_info->flags2,
                                                          BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN));
    mep_profile->dmm_offset =
        dnx_oam_oamp_v1_lm_dm_set_offset(unit, mep_db_entry, is_dual_ended_lm, 1, 0,
                                         (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp),
                                         _SHR_IS_FLAG_SET(endpoint_info->flags2,
                                                          BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN));
    mep_profile->dmr_offset =
        dnx_oam_oamp_v1_lm_dm_set_offset(unit, mep_db_entry, is_dual_ended_lm, 1, 1,
                                         (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp),
                                         _SHR_IS_FLAG_SET(endpoint_info->flags2,
                                                          BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN));

    if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_scan_count_calc(unit,
                                                        endpoint_info->id, 0,
                                                        (endpoint_info->endpoint_memory_type ==
                                                         bcmOamEndpointMemoryTypeShortEntry),
                                                        mep_db_entry->ccm_interval, ccm_rx_without_tx,
                                                        OAMP_MEP_TX_OPCODE_CCM_BFD, mep_profile));
    }
    else
    {

        if (is_mpls_lm_dm_without_bfd_endpoint)
        {
            mep_profile->ccm_count = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
        }
        mep_profile->lmm_count = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
        mep_profile->dmm_count = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
    }

    mep_profile->cluster_id = 0xFF;

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.allocate_single
                    (unit, 0, mep_profile, NULL, &profile_id, &first_reference));
    if (first_reference)
    {
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG;
        mep_hw_profiles_write_data->mep_profile_index = profile_id;
        sal_memcpy(&(mep_hw_profiles_write_data->mep_profile), mep_profile, sizeof(dnx_oam_mep_profile_t));
    }

    mep_db_entry->mep_profile = profile_id;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_ccm_endpoint_mep_db_create(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const bcm_oam_group_info_t * group_info,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_delete_data)
{

    uint8 slm_enabled = 0;
    uint8 is_dual_ended_lm = 0;
    dnx_bfd_oamp_v1_endpoint_t *bfd_entry_values = NULL;
    dnx_oam_mep_profile_t mep_profile;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int counter = 0;
    uint32 udh_header_size;
    uint8 last_reference;
    uint8 is_1dm_enabled = 0, is_mpls_lm_dm_without_bfd_endpoint = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(oamp_pe_udh_header_size_get(unit, (uint32 *) &udh_header_size));

    sal_memset(&mep_profile, 0, sizeof(dnx_oam_mep_profile_t));

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        SHR_IF_ERR_EXIT(dnx_oam_mpls_lm_dm_without_bfd_endpoint
                        (unit, endpoint_info->id, &is_mpls_lm_dm_without_bfd_endpoint));
    }

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info) && !is_mpls_lm_dm_without_bfd_endpoint)
    {

        SHR_ALLOC_SET_ZERO(bfd_entry_values, sizeof(dnx_bfd_oamp_v1_endpoint_t),
                           "Structure for reading data about BFD EP", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(dnx_bfd_oamp_v1_mep_db_endpoint_get(unit, (uint16) endpoint_info->id, bfd_entry_values));

        if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
        {

            SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, bfd_entry_values->mep_profile, &counter,
                                                                         &mep_profile));

            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_mep_profile.free_single(unit, (int) bfd_entry_values->mep_profile, &last_reference));
        }
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_create_prepare_for_ccm_endpoint_replace
                        (unit, endpoint_info, mep_db_entry, &slm_enabled, &is_dual_ended_lm, &is_1dm_enabled,
                         mep_hw_profiles_delete_data, &mep_profile));
    }

    mep_db_entry->flags |= ((endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeLmDmOffloadedEntry) ?
                            DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED :
                            ((endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeShortEntry) ?
                             DNX_OAMP_OAM_CCM_MEP_Q_ENTRY : 0));

    mep_db_entry->flags |= (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_TX) ? DNX_OAMP_OAM_CCM_MEP_RDI_FROM_PACKET : 0;

    if (DNX_OAM_GROUP_MAID_IS_EXTRA_DATA(group_info->flags))
    {
        mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_48B_MAID;
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
    {
        mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION;
        mep_db_entry->vsi = endpoint_info->vpn;
    }

    if (DNX_OAM_DISSECT_IS_EXTRA_DATA_PER_ENDPOINT(endpoint_info))
    {
        if (!DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
        {
            if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_SIGNAL))
            {
                mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_SIGNAL_DEGRADATION_ENABLE;
                mep_db_entry->signal_degradation_tx = endpoint_info->tx_signal;
            }
            else
            {
                mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_LMDM_FLEXIBLE_DA;
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type
                    (unit, endpoint_info->type, &mep_db_entry->mep_type));

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        if (is_mpls_lm_dm_without_bfd_endpoint)
        {
            mep_db_entry->ach_sel = 0;
        }
        else
        {
            mep_db_entry->ach_sel = bfd_entry_values->ach_sel;
        }

        if (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp)
        {
            mep_db_entry->flags |= DNX_OAMP_OAM_RF6374_GAL;
        }
        mep_db_entry->flags |= DNX_OAMP_OAM_RF6374_ACH;
        mep_db_entry->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(endpoint_info->extra_data_index);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_ccm_endpoint_mep_db_create_system_headers
                    (unit, endpoint_info, group_info, gport_hw_resources, mep_hw_profiles_write_data, mep_db_entry,
                     slm_enabled));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_ccm_endpoint_mep_db_create_network_headers
                    (unit, endpoint_info, group_info, mep_hw_profiles_write_data, mep_db_entry, is_dual_ended_lm,
                     is_mpls_lm_dm_without_bfd_endpoint, &mep_profile));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_ccm_endpoint_mep_db_create_oam_pdu(unit,
                                                                       endpoint_info,
                                                                       group_info,
                                                                       sw_endpoint_info,
                                                                       mep_hw_profiles_write_data,
                                                                       mep_hw_profiles_delete_data,
                                                                       mep_db_entry, is_dual_ended_lm, is_1dm_enabled));

exit:
    SHR_FREE(bfd_entry_values);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_oam_oamp_v1_additional_gal_special_label_program_variable_set(
    int unit,
    uint32 gal)
{
    uint32 tmp_gal;
    uint32 entry_handle_id;
    uint32 const_value;
    uint32 prog_hw_index;
    dbal_enum_value_field_oamp_pe_program_enum_e prog_enum;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    prog_enum = DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL;
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, prog_enum, &prog_hw_index));
    tmp_gal = gal & 0xf;
    tmp_gal = tmp_gal << 20;
    const_value = 0xfffc | tmp_gal;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_PROPERTIES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_INDEX, prog_hw_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONST_VALUE, INST_SINGLE, const_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    prog_enum = DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_1DM;
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, prog_enum, &prog_hw_index));
    const_value = 0xfffc | tmp_gal;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_INDEX, prog_hw_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONST_VALUE, INST_SINGLE, const_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    prog_enum = DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV;
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, prog_enum, &prog_hw_index));
    const_value = 0xfffc | tmp_gal;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_INDEX, prog_hw_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONST_VALUE, INST_SINGLE, const_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    prog_enum = DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV_RSP;
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, prog_enum, &prog_hw_index));
    const_value = 0xfffc | tmp_gal;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_INDEX, prog_hw_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONST_VALUE, INST_SINGLE, const_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_INIT_Y1731_GENERAL_CFG, entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_Y1731_MPLSTP_GAL, INST_SINGLE, &tmp_gal);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    prog_enum = DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48;
    SHR_IF_ERR_EXIT(dnx_oamp_pe_program_sw_enum2hw_get(unit, prog_enum, &prog_hw_index));
    tmp_gal &= 0xfff;
    tmp_gal |= (gal << 12);
    tmp_gal = tmp_gal << 16;
    const_value = 0xfffc | tmp_gal;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_PE_PROGRAMS_PROPERTIES, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_PE_PROGRAM_INDEX, prog_hw_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONST_VALUE, INST_SINGLE, const_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_additional_gal_special_label_profile_set(
    int unit,
    uint32 gal)
{
    uint32 entry_handle_id;
    uint32 cur_additional_gal, normal_label_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_additional_gal_special_label_get(unit, &cur_additional_gal, &normal_label_profile));

    if (cur_additional_gal != MPLS_LABEL_GAL)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_SPECIAL_LABEL_PROFILE_MAP, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_NIBBLE, cur_additional_gal);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_PROFILE,
                                     INST_SINGLE, normal_label_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MPLS_SPECIAL_LABEL_PROFILE_MAP, entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_NIBBLE, cur_additional_gal);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_PROFILE,
                                     INST_SINGLE, normal_label_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (gal != MPLS_LABEL_GAL)
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_SPECIAL_LABEL_PROFILE_MAP, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_NIBBLE, gal);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS, 0);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_PROFILE, INST_SINGLE,
                                   &normal_label_profile);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(dnx_oam_sw_db_additional_gal_special_label_set(unit, gal, normal_label_profile));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_SPECIAL_LABEL_PROFILE_MAP, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_NIBBLE, gal);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_PROFILE,
                                     INST_SINGLE, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_GAL);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MPLS_SPECIAL_LABEL_PROFILE_MAP, entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_NIBBLE, gal);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_SPECIAL_LABEL_PROFILE,
                                     INST_SINGLE, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_GAL);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_additional_gal_special_label_set(
    int unit,
    uint32 gal)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_additional_gal_special_label_profile_set(unit, gal));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_additional_gal_special_label_program_variable_set(unit, gal));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_lm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint8 update_ptr = FALSE;
    uint8 current_oam_lmm_da_profile = 0;
    uint8 current_oam_mep_profile = 0;
    uint8 current_counter_interface = 0;
    uint32 current_counter_pointer = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));

    UTILEX_SET_BIT(oamp_params->flags,
                   _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_UPDATE), OAMP_MEP_UPDATE);

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_lm_offloaded_endpoint_set(unit, oam_id, entry_values, oamp_params));
    }
    else
    {

        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {

            current_oam_lmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;
            current_oam_mep_profile = oamp_params->mep_profile;
            current_counter_interface = oamp_params->counter_interface;
            current_counter_pointer = oamp_params->counter_ptr;

            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_self_contained_endpoint_lm_db_set(unit, oam_id, entry_values,
                                                                                 oamp_params, &update_ptr));

            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY) ||
                (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_UPDATE) &&
                 _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_LM_STAT_EXISTS)))
            {

                SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_self_contained_endpoint_lm_stat_set
                                (unit, oam_id, entry_values, oamp_params));
            }

            if (update_ptr || (current_oam_lmm_da_profile != entry_values->lmm_dmm_da_profile)
                || (current_oam_mep_profile != entry_values->mep_profile)
                || (current_counter_interface != entry_values->slm_core_id)
                || (current_counter_pointer != entry_values->slm_counter_pointer))
            {
                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
                if (update_ptr)
                {

                    oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_lm_entry);
                }
                oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
                oamp_params->mep_profile = entry_values->mep_profile;
                if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_MEASURE_NEXT_RECEIVED_SLR))
                {
                    oamp_params->counter_ptr = entry_values->slm_counter_pointer;
                    oamp_params->counter_interface = entry_values->slm_core_id;
                }
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_lm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    uint8 *lm_exists,
    uint8 *lm_stat_exists)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));

    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;
    entry_values->flex_lm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    entry_values->mep_profile = oamp_params->mep_profile;

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_lm_offloaded_endpoint_get
                        (unit, oam_id, entry_values, oamp_params, lm_exists));
    }
    else
    {

        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {

            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_self_contained_endpoint_lm_db_get
                            (unit, oam_id, entry_values, oamp_params, lm_exists));

            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_self_contained_endpoint_lm_stat_get(unit, oam_id, entry_values,
                                                                                   oamp_params, lm_stat_exists));
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_lm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 is_mpls_lm_dm_entry_exists)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 next_bank_offset;
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 entry_index = 0, dm_stat_entry, flex_lm_dm_entry, second_mep_db_entry;
    uint8 dm_entry_found;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {

        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LM_STAT_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LM_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_LM_STATISTICS);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));

        if (oamp_params->flex_lm_dm_ptr != 0)
        {
            second_mep_db_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_ENABLE) &&
                UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_STAT_ENABLE))
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, second_mep_db_entry, oamp_params));
                if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
                    UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
                {

                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_LM_STATISTICS);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, second_mep_db_entry, oamp_params));
                }
            }
            else
            {

                oamp_params->flex_lm_dm_ptr = 0;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, second_mep_db_entry));
            }
        }
    }
    else
    {

        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {

            if (is_mpls_lm_dm_entry_exists)
            {
                entry_index = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
                flex_lm_dm_entry = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(entry_index);
            }
            else
            {
                flex_lm_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            }

            search_params.first_entry = flex_lm_dm_entry;
            search_params.param_entry = flex_lm_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
            dm_stat_entry = search_params.resulting_entry;

            if (!search_params.update_entry)
            {

                search_params.first_entry = flex_lm_dm_entry;
                search_params.param_entry = flex_lm_dm_entry;
                search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
                SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry
                                (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
                dm_stat_entry = search_params.resulting_entry;
            }

            dm_entry_found = search_params.update_entry;

            search_params.first_entry = flex_lm_dm_entry;
            search_params.param_entry = flex_lm_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));

            if (dm_entry_found)
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, dm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_DM_STATISTICS);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, flex_lm_dm_entry, oamp_params));
                if (flex_lm_dm_entry != dm_stat_entry)
                {

                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, dm_stat_entry));
                }
                else
                {

                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, dm_stat_entry + next_bank_offset));
                }
            }
            else
            {

                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

                if (is_mpls_lm_dm_entry_exists)
                {

                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, entry_index, oamp_params));
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, entry_index, oamp_params));
                    sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                }

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
                if (!is_mpls_lm_dm_entry_exists)
                {
                    oamp_params->flex_lm_dm_ptr = 0;
                }
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, flex_lm_dm_entry));
            }

            if (search_params.update_entry)
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, search_params.resulting_entry));
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_dm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 next_bank_offset;
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 dm_stat_entry;
    int num_short_entries_in_bank;
    uint8 update_ptr = FALSE;
    uint8 current_oam_lmm_da_profile = 0;
    uint8 current_oam_mep_profile = 0;
    uint8 current_oam_mep_pe_profile = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_dm_offloaded_endpoint_set(unit, oam_id, entry_values, oamp_params));
    }
    else
    {

        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {

            current_oam_lmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;
            current_oam_mep_profile = oamp_params->mep_profile;
            current_oam_mep_pe_profile = oamp_params->mep_pe_profile;

            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY) ==
                _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Choose either one-way or two-way measurement.\n");
            }

            search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            search_params.param_entry = entry_values->flex_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
            dm_stat_entry = search_params.resulting_entry;

            if (!search_params.update_entry)
            {

                search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
                SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry
                                (unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
                dm_stat_entry = search_params.resulting_entry;
            }

            if (search_params.update_entry)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, dm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
            }
            else
            {
                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);

                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
            }
            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY))
            {
                oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
            }
            else
            {
                oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
            }
            num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
                dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
            if ((oam_id / num_short_entries_in_bank) == (dm_stat_entry / num_short_entries_in_bank))

            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: adding a delay entry will result in that entry "
                             "being in the same bank as the endpoint entry.\n");
            }
            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, dm_stat_entry, oamp_params));

            if (search_params.clear_last_entry)
            {

                dm_stat_entry -= next_bank_offset;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, dm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, dm_stat_entry, oamp_params));
            }

            update_ptr = !search_params.update_entry && !search_params.clear_last_entry;

            if (update_ptr || (current_oam_lmm_da_profile != entry_values->lmm_dmm_da_profile)
                || (current_oam_mep_profile != entry_values->mep_profile)
                || (current_oam_mep_pe_profile != entry_values->mep_pe_profile))
            {
                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
                if (update_ptr)
                {

                    oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_dm_entry);
                }
                oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
                oamp_params->mep_profile = entry_values->mep_profile;
                oamp_params->mep_pe_profile = entry_values->mep_pe_profile;
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_dm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    uint8 *is_found)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
    entry_values->mep_profile = oamp_params->mep_profile;
    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_dm_offloaded_endpoint_get
                        (unit, oam_id, entry_values, oamp_params, is_found));
    }
    else
    {

        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {

            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_self_contained_endpoint_dm_stat_get(unit, oam_id, entry_values,
                                                                                   oamp_params, is_found));
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_mep_db_dm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 is_mpls_lm_dm_entry_exists)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 next_bank_offset;
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 entry_index, lm_db_entry, lm_stat_entry, flex_lm_dm_entry, second_mep_db_entry;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {

        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_DM_STAT_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_DM_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));

        if (oamp_params->flex_lm_dm_ptr != 0)
        {
            second_mep_db_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_ENABLE) &&
                UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_STAT_ENABLE))
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, second_mep_db_entry, oamp_params));
                if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
                    UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
                {

                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, second_mep_db_entry, oamp_params));
                }
            }
            else
            {

                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, second_mep_db_entry));
                oamp_params->flex_lm_dm_ptr = 0;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));
            }
        }
    }
    else
    {

        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {

            if (is_mpls_lm_dm_entry_exists)
            {
                entry_index = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
                flex_lm_dm_entry = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(entry_index);
            }
            else
            {
                flex_lm_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            }

            search_params.first_entry = flex_lm_dm_entry;
            search_params.param_entry = flex_lm_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
            lm_db_entry = search_params.resulting_entry;

            if (search_params.update_entry)
            {

                search_params.first_entry = flex_lm_dm_entry;
                search_params.param_entry = flex_lm_dm_entry;
                search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
                SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry
                                (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
                lm_stat_entry = search_params.resulting_entry;

                if (search_params.update_entry)
                {

                    if (flex_lm_dm_entry == lm_stat_entry)
                    {

                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_db_entry, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add
                                        (unit, flex_lm_dm_entry + next_bank_offset, oamp_params));
                    }
                    else
                    {

                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_db_entry, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, flex_lm_dm_entry, oamp_params));

                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_stat_entry, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add
                                        (unit, flex_lm_dm_entry + next_bank_offset, oamp_params));

                        if (flex_lm_dm_entry != lm_db_entry)
                        {

                            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, lm_stat_entry));
                        }
                        else
                        {

                            SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, lm_stat_entry + next_bank_offset));
                        }
                    }
                }
                else
                {

                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, lm_db_entry, oamp_params));
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, flex_lm_dm_entry, oamp_params));

                    if (flex_lm_dm_entry != lm_db_entry)
                    {

                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, lm_db_entry));
                    }
                    else
                    {

                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, lm_db_entry + next_bank_offset));
                    }
                }
            }
            else
            {

                if (flex_lm_dm_entry != 0)
                {
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, flex_lm_dm_entry, oamp_params));
                    if (IS_LM_DM_TYPE(oamp_params->mep_type))
                    {
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_delete(unit, flex_lm_dm_entry));
                    }

                    sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, oam_id, oamp_params));
                    if (!is_mpls_lm_dm_entry_exists)
                    {
                        oamp_params->flex_lm_dm_ptr = 0;
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, oam_id, oamp_params));
                    }
                    if (is_mpls_lm_dm_entry_exists)
                    {

                        entry_index = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
                        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_get(unit, entry_index, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_mep_db_add(unit, entry_index, oamp_params));
                    }
                }
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_lm_with_priority_session_map_get(
    int unit,
    int oam_id,
    uint8 priority,
    uint32 *session_oam_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, 0);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, session_oam_id);
    SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_v1_lm_with_priority_session_map_clear(
    int unit,
    int oam_id,
    uint8 priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, 0);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oam_oamp_v1_lmdm_flexible_da_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    const bcm_oam_group_info_t * group_info)
{
    uint8 dm_exists = 0, lm_exists = 0;
#ifdef BCM_DNX2_SUPPORT
    uint8 lm_stat_exists = 0;
#endif
    int extra_data_nof_bits, extra_data_len;
    int num_short_entries_in_bank, entry_bank, extra_data_header_bank;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *lm_mep_db_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(lm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
#ifdef BCM_DNX2_SUPPORT
        if (DNX_OAMP_IS_V1(unit) &&
            !_SHR_IS_FLAG_SET(sw_endpoint_info->sw_state_flags, DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_INTERMEDIATE)
            && (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeShortEntry)
            && (DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)))
        {
            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_dm_endpoint_get(unit, endpoint_info->id, dm_mep_db_entry, &dm_exists));
            SHR_IF_ERR_EXIT(dnx_oamp_v1_mep_db_lm_endpoint_get
                            (unit, endpoint_info->id, lm_mep_db_entry, &lm_exists, &lm_stat_exists));
        }
#endif

        if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_ENABLE(endpoint_info, sw_endpoint_info))
        {
            if (dm_exists || lm_exists)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: LM/DM Flexible DMAC can not be enabled when LM or DM exists\n");
            }
            if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE))
            {
                extra_data_nof_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY + DNX_LMDM_FLEXIBLE_DA_BITS;
            }
            else if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE))
            {
                extra_data_nof_bits = DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY +
                    DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY + DNX_LMDM_FLEXIBLE_DA_BITS;
            }
            else if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
            {
                extra_data_nof_bits = DNX_OAM_OAMP_NOF_MAID_BITS + DNX_LMDM_FLEXIBLE_DA_BITS;
            }
            else
            {
                extra_data_nof_bits = DNX_LMDM_FLEXIBLE_DA_BITS;
            }
            extra_data_len = extra_data_nof_bits / DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY;
            num_short_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
                dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
            entry_bank = endpoint_info->id / num_short_entries_in_bank;
            extra_data_header_bank = endpoint_info->extra_data_index / num_short_entries_in_bank;
            if ((entry_bank >= extra_data_header_bank) && (entry_bank < extra_data_header_bank + extra_data_len))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error: for self contained accelerated OAM endpoint, all extra data \n"
                             "entries must not be in the same bank as the main endpoint entry.\n");
            }

        }
        else if (DNX_OAM_LMDM_FLEXIBLE_DA_IS_SET_TO_DISABLE(endpoint_info, sw_endpoint_info))
        {
            if (dm_exists || lm_exists)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: LM/DM Flexible DMAC can not be disabled when LM or DM exists\n");
            }
        }
    }

exit:
    SHR_FREE(dm_mep_db_entry);
    SHR_FREE(lm_mep_db_entry);
    SHR_FUNC_EXIT;
}

#endif
