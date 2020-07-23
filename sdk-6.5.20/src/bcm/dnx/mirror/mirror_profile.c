/** \file mirror_profile.c
 * 
 *
 * Internal MIRROR profile functionality for DNX. \n
 * This file handles mirror profiles (allocation, creation, retrieval, destruction, etc..)
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MIRROR

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>
#include <bcm/types.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/mirror/mirror.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_mirror_access.h>
#include "mirror_profile.h"
#include "mirror_profile_alloc.h"

/*
 * }
 */

/*
 * MACROs
 * {
 */

#define DNX_MIRROR_DEST_TYPE_FROM_ID_GET(mirror_dest_id)     ((_SHR_GPORT_IS_MIRROR_SNOOP(mirror_dest_id)) ? DBAL_ENUM_FVAL_SNIF_TYPE_SNOOP : \
                                                             ((_SHR_GPORT_IS_MIRROR_STAT_SAMPLE(mirror_dest_id)) ? DBAL_ENUM_FVAL_SNIF_TYPE_STATISTICAL_SAMPLE : \
                                                                     DBAL_ENUM_FVAL_SNIF_TYPE_MIRROR))

/*
 * }
 */

/**
 * \brief - calculates the HW field for inbound/outbound mirroring/snooping probability. \n
 * The probability of mirroring a packet is: \n 
 * dividend >= divisor ? 1 : dividend / divisor \n 
 *  
 * The HW field is calculated as following. Given probability 'P' and HW field nof bits is PROB_NOF_BITS: \n 
 *  P = prob_hw_field/2^PROB_NOF_BITS-1
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dividend - dividend
 *   \param [in] divisor - divisor
 *   \param [in] prob_nof_bits - number of HW bits
 *   \param [in] prob_field - pointer to calculated HW field (probability)
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_mirror_profile_probability_get(
    int unit,
    uint32 dividend,
    uint32 divisor,
    uint32 prob_nof_bits,
    uint32 *prob_field)
{
    uint64 result;
    uint32 field_mask = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(&field_mask, 0, prob_nof_bits - 1));
    if (dividend >= divisor)
    {
        *prob_field = field_mask;
    }
    else
    {
        COMPILER_64_SET(result, 0, dividend);
        COMPILER_64_UMUL_32(result, field_mask);
        COMPILER_64_UDIV_32(result, divisor);
        *prob_field = COMPILER_64_LO(result);
        if (COMPILER_64_HI(result))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "overflow\n");
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read mirror attributes from HW (dbal)
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest_id - Mirror profile gport
 *   \param [in] mirror_dest - Mirror profile attributes
 *   
 * \remark
 *   * Mirror related HW. See dbal tables: \n
 *     DBAL_TABLE_SNIF_COMMAND_TABLE, DBAL_TABLE_SNIF_COUNTERS_TABLE
 * \see
 *   * None
 */
static shr_error_e
dnx_mirror_profile_hw_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_mirror_destination_t * mirror_dest)
{
    uint32 entry_handle_id, array32[1];
    uint32 snif_type, dest, snif_probability, crop_enable, tc, tc_ow, dp, dp_ow,
        in_pp_port, in_pp_port_ow, drop_fwd_if_snif_dropped,
        drop_snif_if_fwd_dropped, st_vsq_ptr, st_vsq_ptr_ow, lag_lb_key,
        lag_lb_key_ow, admit_profile, admit_profile_ow, metadata_ow, ext_stat_opcode_ow,
        ext_stat_data_ow, ext_stat_valid_ow, cud_type;
    int action_profile_id, i, profile, mod_profile_index;
    uint32 ext_stat_valid = 0, ext_stat_id[BCM_MIRROR_EXT_STAT_ID_COUNT] = { 0 };
    uint32 ext_stat_type_id[BCM_MIRROR_EXT_STAT_ID_COUNT] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    entry_handle_id = 0;
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    snif_type = DNX_MIRROR_DEST_TYPE_FROM_ID_GET(mirror_dest_id);

    /*
     * Read values from HW(DBAL SNIF command table). 
     * Choose table key according to application type (mirror, snoop, sampling)
     */
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_COMMAND_TABLE, &entry_handle_id));
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

        /*
         * read all table values
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &dest);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SNIF_PROBABILITY, INST_SINGLE, &snif_probability);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CROP_ENABLE, INST_SINGLE, &crop_enable);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TC_OW, INST_SINGLE, &tc_ow);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TC, INST_SINGLE, &tc);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DP_OW, INST_SINGLE, &dp_ow);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DP, INST_SINGLE, &dp);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT_OW, INST_SINGLE, &in_pp_port_ow);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT, INST_SINGLE, &in_pp_port);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ST_VSQ_PTR_OW, INST_SINGLE, &st_vsq_ptr_ow);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ST_VSQ_PTR, INST_SINGLE, &st_vsq_ptr);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAG_LB_KEY_OW, INST_SINGLE, &lag_lb_key_ow);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAG_LB_KEY, INST_SINGLE, &lag_lb_key);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE_OW, INST_SINGLE, &admit_profile_ow);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE, INST_SINGLE, &admit_profile);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CUD_TYPE, INST_SINGLE, &cud_type);
        dbal_value_field32_request(unit, entry_handle_id,
                                   DBAL_FIELD_DROP_SNIFF_IF_FWD_DROPPED, INST_SINGLE, &drop_snif_if_fwd_dropped);
        dbal_value_field32_request(unit, entry_handle_id,
                                   DBAL_FIELD_DROP_FWD_IF_SNIFF_DROPPED, INST_SINGLE, &drop_fwd_if_snif_dropped);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * After all values were read from HW, start filling the mirror_dest struct accordingly
     */
    {
        /*
         * Drop strength field
         */
        mirror_dest->flags |= (drop_snif_if_fwd_dropped) ? BCM_MIRROR_DEST_DROP_SNIFF_IF_FWD_DROPPED : 0;
        mirror_dest->flags |= (drop_fwd_if_snif_dropped) ? BCM_MIRROR_DEST_DROP_FWD_IF_SNIFF_DROPPED : 0;
        /*
         * Destination field. 
         * Decode destination field as it has sub fields
         */
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                            (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, dest, &(mirror_dest->gport)));
        }

        /*
         * Crop size field. 
         * Set crop size: 
         * Crop enable - only 256B are cropped 
         * Crop disable - whole packet is copied 
         */
        if (crop_enable)
        {
            mirror_dest->packet_copy_size = dnx_data_snif.ingress.crop_size_get(unit);
        }
        else
        {
            mirror_dest->packet_copy_size = 0;
        }

        /*
         * Probability field. 
         * Set divisor to maximal value for better precision. 
         */
        {
            /** dividend */
            mirror_dest->sample_rate_dividend = snif_probability;
            /** divisor - set to maximal value */
            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                                  DBAL_TABLE_SNIF_COMMAND_TABLE,
                                                                  DBAL_FIELD_SNIF_PROBABILITY,
                                                                  FALSE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, array32));
            mirror_dest->sample_rate_divisor = array32[0];
        }

        /*
         * Packet control fields. Each field has a corresponding override field. 
         * If the field is not overwritten, then the snif copy gets the value from the original packet (forward copy). 
         * Otherwise, if the field is overwritten, then the value should be indicated.                                                                                                          .
         */
        mirror_dest->packet_control_updates.valid = 0;
        /** TC field */
        if (tc_ow)
        {
            mirror_dest->packet_control_updates.prio = tc;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_PRIO;
        }
        /** DP field */
        if (dp_ow)
        {
            mirror_dest->packet_control_updates.color = dp;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_COLOR;
        }
        /** in pp port field */
        if (in_pp_port_ow)
        {
            mirror_dest->packet_control_updates.in_port = in_pp_port;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_IN_PORT;
        }
        /** statistics VSQ ptr field */
        if (st_vsq_ptr_ow)
        {
            mirror_dest->packet_control_updates.vsq = st_vsq_ptr;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_VSQ;
        }
        /** trunk(LAG) hash field */
        if (lag_lb_key_ow)
        {
            mirror_dest->packet_control_updates.trunk_hash_result = lag_lb_key;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_TRUNK_HASH_RESULT;
        }
        /*
         * Admit profile field
         * Admit profile 0 - ECN disabled 
         * Admit profile 1 - ECN enabled 
         */
        if (admit_profile_ow)
        {
            if (admit_profile == 1)
            {
                mirror_dest->packet_control_updates.ecn_value = 1;
            }
            else
            {
                mirror_dest->packet_control_updates.ecn_value = 0;
            }

            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_ECN_VALUE;
        }

        if (_SHR_GPORT_IS_MIRROR_STAT_SAMPLE(mirror_dest_id) ^ cud_type)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "mirror is stastic sampling but CUD type is 0, "
                         "or  mirror is not stastic sampling but CUD type is 1.\n");
        }
    }

    /*
     * Getting metadata counting information
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_COUNTING_TABLE, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    /*
     * read table value
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_META_DATA_OW, INST_SINGLE, &metadata_ow);
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_INT_STAT_OBJ_META_DATA, INST_SINGLE,
                               &mirror_dest->packet_control_updates.metadata);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_OPCODE_OW, INST_SINGLE,
                               &ext_stat_opcode_ow);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_OW, INST_SINGLE, &ext_stat_data_ow);
    for (i = 0; i < BCM_MIRROR_EXT_STAT_ID_COUNT; i++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_ID, i, &ext_stat_id[i]);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_OPCODE, i, &ext_stat_type_id[i]);
    }
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_VALID_OW, INST_SINGLE,
                               &ext_stat_valid_ow);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_VALID, INST_SINGLE, &ext_stat_valid);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (metadata_ow)
    {
        mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_METADATA;
    }

    if (ext_stat_valid_ow)
    {
        mirror_dest->ext_stat_valid = ext_stat_valid;
        mirror_dest->flags2 |= BCM_MIRROR_DEST_FLAGS2_UPDATE_EXT_STAT_VALID;
    }
    if (ext_stat_data_ow)
    {
        for (i = 0; i < BCM_MIRROR_EXT_STAT_ID_COUNT; i++)
        {
            mirror_dest->ext_stat_id[i] = ext_stat_id[i];
        }
        mirror_dest->flags |= BCM_MIRROR_DEST_UPDATE_EXT_COUNTERS;
    }
    if (ext_stat_opcode_ow)
    {
        for (i = 0; i < BCM_MIRROR_EXT_STAT_ID_COUNT; i++)
        {
            mirror_dest->ext_stat_type_id[i] = ext_stat_type_id[i];
        }
        mirror_dest->flags2 |= BCM_MIRROR_DEST_FLAGS2_UPDATE_EXT_STAT_TYPES;
    }

    /** set mirror on drop flag and bit map */
    mirror_dest->drop_group_bmp = 0;
    for (mod_profile_index = 0; mod_profile_index < dnx_data_snif.ingress.nof_mirror_on_drop_profiles_get(unit);
         mod_profile_index++)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.mirror_on_drop_profile.get(unit, mod_profile_index, &profile));
        if (profile == action_profile_id)
        {
            mirror_dest->flags2 |= BCM_MIRROR_DEST_FLAGS2_MIRROR_ON_DROP;
            SHR_BITSET(&(mirror_dest->drop_group_bmp), mod_profile_index);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write mirror on drop attributes to HW (dbal)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest - Mirror profile attributes
 *   \param [in] action_profile_id - profile ID
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_mirror_profile_mirror_on_drop_hw_set(
    int unit,
    bcm_mirror_destination_t * mirror_dest,
    int action_profile_id)
{
    uint32 entry_handle_id;
    uint32 destination, dest_val;
    dbal_fields_e dbal_dest_type;
    int mod_profile_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** set profile to sw */
    SHR_BIT_ITER((uint32 *) &(mirror_dest->drop_group_bmp), dnx_data_snif.ingress.nof_mirror_on_drop_profiles_get(unit),
                 mod_profile_index)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.mirror_on_drop_profile.set(unit, mod_profile_index, action_profile_id));
    }

    /** set profile to hw */
    /** alloc handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_MIRROR_ON_DROP_COMMAND, &entry_handle_id));

    /** set destination - voq */
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, mirror_dest->gport, &destination));
    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                    (unit, DBAL_FIELD_DESTINATION, destination, &dbal_dest_type, &dest_val));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, INST_ALL, dest_val);

    /** set sniff command id */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, INST_ALL, action_profile_id);

    /** set crop */
    if (mirror_dest->packet_copy_size == 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CROP_ENABLE, INST_SINGLE, FALSE);
    }
    else if (mirror_dest->packet_copy_size == dnx_data_snif.ingress.crop_size_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CROP_ENABLE, INST_SINGLE, TRUE);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cropping size=%d is invalid", mirror_dest->packet_copy_size);
    }

    /** set cud type to be sniff */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD_TYPE, INST_SINGLE,
                                 dnx_data_snif.general.cud_type_get(unit));

    /*
     * Packet control fields. Each field has a corresponding override field.
     * If the field is not overwritten, then the snif copy gets the value from the original packet (forward copy).
     * Otherwise, if the field is overwritten, then the value should be indicated.                                                                                                          .
     */
    /*
     * TC field
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_PRIO)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC,
                                     INST_SINGLE, mirror_dest->packet_control_updates.prio);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_OW, INST_SINGLE, FALSE);
    }
    /*
     * DP field
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_COLOR)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP,
                                     INST_SINGLE, mirror_dest->packet_control_updates.color);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_OW, INST_SINGLE, FALSE);
    }
    /*
     * in pp port field
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_IN_PORT)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_IN_PP_PORT, INST_SINGLE, mirror_dest->packet_control_updates.in_port);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT_OW, INST_SINGLE, FALSE);
    }
    /*
     * statistics VSQ ptr field
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_VSQ)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ST_VSQ_PTR_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_ST_VSQ_PTR, INST_SINGLE, mirror_dest->packet_control_updates.vsq);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ST_VSQ_PTR_OW, INST_SINGLE, FALSE);
    }

    /*
     * Admit profile field
     * Admit profile 0 - ECN disabled
     * Admit profile 1 - ECN enabled
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_ECN_VALUE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE_OW, INST_SINGLE, TRUE);
        if (mirror_dest->packet_control_updates.ecn_value > 0)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE, INST_SINGLE, 1);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE, INST_SINGLE, 0);
        }
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE_OW, INST_SINGLE, FALSE);
    }

    /*
     * write all mod profiles to hw
     */
    SHR_BIT_ITER((uint32 *) &(mirror_dest->drop_group_bmp), dnx_data_snif.ingress.nof_mirror_on_drop_profiles_get(unit),
                 mod_profile_index)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MOD_PROFILE_ID, mod_profile_index);

        /** commit mirror on drop profile */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write mirror profile attributes to HW (dbal)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest - Mirror profile attributes
 *   \param [in] action_profile_id - profile ID
 *
 * \remark
 *   * Mirror related HW. See dbal tables: \n
 *     DBAL_TABLE_SNIF_COMMAND_TABLE, DBAL_TABLE_SNIF_COUNTERS_TABLE
 * \see
 *   * None
 */
static shr_error_e
dnx_mirror_profile_hw_set(
    int unit,
    bcm_mirror_destination_t * mirror_dest,
    int action_profile_id)
{
    uint32 snif_probability, drop_fwd_if_snif_dropped, drop_snif_if_fwd_dropped;
    int nof_bits;
    uint32 entry_handle_id;
    int snif_type, i;
    uint32 destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    snif_type = DNX_MIRROR_DEST_TYPE_GET(mirror_dest->flags);

    /*
     * Calculate HW field for sniff probability
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_SNIF_COMMAND_TABLE, DBAL_FIELD_SNIF_PROBABILITY, FALSE, 0, 0, &nof_bits));
    SHR_IF_ERR_EXIT(dnx_mirror_profile_probability_get
                    (unit, mirror_dest->sample_rate_dividend, mirror_dest->sample_rate_divisor, nof_bits,
                     &snif_probability));

    /*
     * Write to HW (DBAL SNIF command table).
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_COMMAND_TABLE, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    /*
     * Drop strength field
     */
    drop_snif_if_fwd_dropped = (mirror_dest->flags & BCM_MIRROR_DEST_DROP_SNIFF_IF_FWD_DROPPED) ? 1 : 0;
    drop_fwd_if_snif_dropped = (mirror_dest->flags & BCM_MIRROR_DEST_DROP_FWD_IF_SNIFF_DROPPED) ? 1 : 0;
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_DROP_SNIFF_IF_FWD_DROPPED, INST_SINGLE, drop_snif_if_fwd_dropped);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_DROP_FWD_IF_SNIFF_DROPPED, INST_SINGLE, drop_fwd_if_snif_dropped);

    /*
     * destination field. 
     * Convert destination gport to dbal type (port, queue, multicast, etc..) and ID. 
     */
    if (mirror_dest->gport != BCM_GPORT_INVALID)
    {
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, mirror_dest->gport, &destination));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination);
    }

    /**
     * set the DESTINATION_OW only if gport is valid and it's supported
     */
    if (dnx_data_snif.general.feature_get(unit, dnx_data_snif_general_original_destination_is_supported))
    {
        if (mirror_dest->gport == BCM_GPORT_INVALID)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION_OW, INST_SINGLE, FALSE);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION_OW, INST_SINGLE, TRUE);
        }
    }

    /*
     * Probability field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_PROBABILITY, INST_SINGLE, snif_probability);

    /*
     * Crop enable field. The size should be given in bytes. 
     * 256 - Crop 256B of the packet (crop enable)
     * 0 - No cropping (crop disable)
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CROP_ENABLE, INST_SINGLE,
                                 mirror_dest->packet_copy_size ? TRUE : FALSE);

    /*
     * Packet control fields. Each field has a corresponding override field. 
     * If the field is not overwritten, then the snif copy gets the value from the original packet (forward copy). 
     * Otherwise, if the field is overwritten, then the value should be indicated.                                                                                                          .
     */
    /*
     * TC field 
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_PRIO)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC,
                                     INST_SINGLE, mirror_dest->packet_control_updates.prio);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_OW, INST_SINGLE, FALSE);
    }
    /*
     * DP field 
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_COLOR)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP,
                                     INST_SINGLE, mirror_dest->packet_control_updates.color);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_OW, INST_SINGLE, FALSE);
    }
    /*
     * in pp port field
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_IN_PORT)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_IN_PP_PORT, INST_SINGLE, mirror_dest->packet_control_updates.in_port);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT_OW, INST_SINGLE, FALSE);
    }
    /*
     * statistics VSQ ptr field
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_VSQ)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ST_VSQ_PTR_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_ST_VSQ_PTR, INST_SINGLE, mirror_dest->packet_control_updates.vsq);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ST_VSQ_PTR_OW, INST_SINGLE, FALSE);
    }
    /*
     * trunk(LAG) hash field
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_TRUNK_HASH_RESULT)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_LB_KEY_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_LAG_LB_KEY, INST_SINGLE,
                                     mirror_dest->packet_control_updates.trunk_hash_result);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_LB_KEY_OW, INST_SINGLE, FALSE);
    }

    /*
     * Admit profile field
     * Admit profile 0 - ECN disabled 
     * Admit profile 1 - ECN enabled 
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_ECN_VALUE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE_OW, INST_SINGLE, TRUE);
        if (mirror_dest->packet_control_updates.ecn_value > 0)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE, INST_SINGLE, 1);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE, INST_SINGLE, 0);
        }
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE_OW, INST_SINGLE, FALSE);
    }

    if (mirror_dest->flags & BCM_MIRROR_DEST_IS_STAT_SAMPLE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD_TYPE, INST_SINGLE, 1);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD_TYPE, INST_SINGLE, 0);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * setting metadata counting parameter and external statistics
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_COUNTING_TABLE, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_METADATA)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_META_DATA_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_INT_STAT_OBJ_META_DATA, INST_SINGLE,
                                     mirror_dest->packet_control_updates.metadata);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_META_DATA_OW, INST_SINGLE, FALSE);
    }

    if (mirror_dest->flags2 & BCM_MIRROR_DEST_FLAGS2_UPDATE_EXT_STAT_VALID)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_VALID_OW, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_VALID, INST_SINGLE,
                                     mirror_dest->ext_stat_valid);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_VALID_OW, INST_SINGLE, FALSE);
    }

    if (mirror_dest->flags2 & BCM_MIRROR_DEST_FLAGS2_UPDATE_EXT_STAT_TYPES)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_OPCODE_OW, INST_SINGLE, TRUE);
        for (i = 0; i < BCM_MIRROR_EXT_STAT_ID_COUNT; i++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_OPCODE, i,
                                         mirror_dest->ext_stat_type_id[i]);
        }
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_OPCODE_OW, INST_SINGLE, FALSE);
    }

    if (mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_EXT_COUNTERS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_OW, INST_SINGLE, TRUE);
        for (i = 0; i < BCM_MIRROR_EXT_STAT_ID_COUNT; i++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_ID, i,
                                         mirror_dest->ext_stat_id[i]);
        }
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXT_STAT_DATA_OW, INST_SINGLE, FALSE);
    }

    /*
     * clearing counting command parameters
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_TYPE_OW, INST_ALL, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_ID_OW, INST_ALL, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_VALID_OW, INST_ALL, FALSE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** set mirror on drop dedicated profile */
    if (mirror_dest->flags2 & BCM_MIRROR_DEST_FLAGS2_MIRROR_ON_DROP)
    {
        int mod_profile_index;
        SHR_BIT_ITER((uint32 *) &(mirror_dest->drop_group_bmp),
                     dnx_data_snif.ingress.nof_mirror_on_drop_profiles_get(unit), mod_profile_index)
        {
            SHR_IF_ERR_EXIT(dnx_mirror_profile_mirror_on_drop_hw_set(unit, mirror_dest, action_profile_id));
        }
    }

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_network_header_termination)
        && !(mirror_dest->flags & BCM_MIRROR_DEST_REPLACE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE, entry_handle_id));

        /** key construction */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

        /** Overwrite ITPP delta to 0 by default for SNIFF packets */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_DELTA, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITPP_DELTA_OW, INST_SINGLE, TRUE);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get sniff FTMH header stamping attributes from HW (dbal)
 *
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest_id - Mirror profile gport
 *   \param [in] mirror_dest - Mirror profile attributes
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *   * FTMH fields are to get according to FTMH system configuration (legacy/non-legacy)
 *   * Mirror related HW. See dbal tables: \n
 *     SNIF_SYSTEM_HEADER_TABLE_LEGACY, SNIF_SYSTEM_HEADER_TABLE_LEGACY
 * \see
 */
static shr_error_e
dnx_mirror_header_stamping_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_mirror_destination_t * mirror_dest)
{
    int snif_type, system_headers_mode, action_profile_id;
    uint32 entry_handle_id, is_mc = FALSE, out_lif = 0, is_outbound_dis = FALSE;
    uint8 fhei_stamp_en = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Read system_headers_mode */
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    snif_type = DNX_MIRROR_DEST_TYPE_FROM_ID_GET(mirror_dest_id);

    /*
     * Choosing table according legacy/new header mode type
     */
    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE_LEGACY, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE, &entry_handle_id));
    }

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    /*
     * MC-OUTLIF related fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_MC, INST_SINGLE, &is_mc);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MC_OUTLIF, INST_SINGLE, &out_lif);

    /*
     * Outbound mirror disable field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUTBOUND_MIRR_DISABLE, INST_SINGLE, &is_outbound_dis);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if ((is_mc == FALSE) && (out_lif != 0))
    {
        mirror_dest->encap_id = out_lif;
        mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
    }

    if (is_outbound_dis == TRUE)
    {
        mirror_dest->flags |= BCM_MIRROR_DEST_OUT_MIRROR_DISABLE;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE, entry_handle_id));
    /** key construction */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_STAMP_EN, INST_SINGLE, &fhei_stamp_en);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (fhei_stamp_en)
    {
        mirror_dest->packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write sniff FTMH header stamping attributes to HW (dbal)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest - Mirror profile attributes
 *   \param [in] action_profile_id - profile ID
 *
 * \remark
 *   * FTMH fields are set according FTMH system configuration (legacy/non-legacy)
 *   * Mirror related HW. See dbal tables: \n
 *     SNIF_SYSTEM_HEADER_TABLE_LEGACY, SNIF_SYSTEM_HEADER_TABLE_LEGACY
 * \see
 *   * None
 */
static shr_error_e
dnx_mirror_header_stamping_set(
    int unit,
    bcm_mirror_destination_t * mirror_dest,
    int action_profile_id)
{
    uint32 entry_handle_id;
    int snif_type;
    uint32 destination;
    dbal_enum_value_field_tm_action_type_e tm_action_type;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Read system_headers_mode */
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    snif_type = DNX_MIRROR_DEST_TYPE_GET(mirror_dest->flags);

    /*
     * Choosing table according legacy/new header mode type
     */
    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE_LEGACY, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE, &entry_handle_id));
    }

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    /*
     * TM-ACTION-TYPE - mask also need to be set according the size of the field
     */
    if (mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP)
    {
        tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_SNOOP;
    }
    else if (mirror_dest->flags & BCM_MIRROR_DEST_IS_STAT_SAMPLE)
    {
        tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_STAT_SAMPLE;
    }
    else
    {
        tm_action_type = DBAL_ENUM_FVAL_TM_ACTION_TYPE_MIRROR;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_ACTION_TYPE, INST_SINGLE, tm_action_type);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_TM_ACTION_TYPE_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);

    /*
     * MC related fields
     */
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_IS_MC_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    if (BCM_GPORT_IS_MCAST(mirror_dest->gport))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_OUTLIF, INST_SINGLE,
                                     BCM_GPORT_MCAST_GET(mirror_dest->gport));
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MC_OUTLIF_MASK, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);

    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, INST_SINGLE, FALSE);

        /*
         * Encap ID operation
         */
        if ((mirror_dest->encap_id != 0) && (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_OUTLIF, INST_SINGLE,
                                         mirror_dest->encap_id);
            dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MC_OUTLIF_MASK, INST_SINGLE,
                                                       DBAL_PREDEF_VAL_MAX_VALUE);
        }
    }

    /*
     * Outbound mirror disable
     */
    if (mirror_dest->flags & BCM_MIRROR_DEST_OUT_MIRROR_DISABLE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTBOUND_MIRR_DISABLE, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTBOUND_MIRR_DISABLE_MASK, INST_SINGLE, TRUE);
    }

    /*
     * Stamping TC
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_PRIO)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, INST_SINGLE,
                                     mirror_dest->packet_control_updates.prio);
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_TC_MASK, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }

    /*
     * Stamping ECN
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_ECN_VALUE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_ENABLE, INST_SINGLE,
                                     mirror_dest->packet_control_updates.ecn_value);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_ENABLE_MASK, INST_SINGLE, TRUE);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Stamping mirror destination on DSP extension
     */
    if ((dnx_data_stack.general.stacking_enable_get(unit) || dnx_data_headers.ftmh.add_dsp_ext_get(unit)) &&
        !dnx_data_snif.ingress.keep_orig_dest_on_dsp_ext_get(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE, entry_handle_id));
        /** key construction */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, mirror_dest->gport, &destination));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSP_EXT_DESTINATION_STAMP_EN, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSP_EXT_DESTINATION, INST_SINGLE, destination);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Stamping trap code/qualifier on FHEI
     */
    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE, entry_handle_id));
        /** key construction */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_PPH_FHEI_STAMP_EN, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** handle mirror on drop profile */
    if (mirror_dest->flags2 & BCM_MIRROR_DEST_FLAGS2_MIRROR_ON_DROP)
    {
        int mod_profile_index;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_MIRROR_ON_DROP_COMMAND, &entry_handle_id));

        /** set tm action type */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_ACTION_TYPE, INST_SINGLE, tm_action_type);

        SHR_BIT_ITER((uint32 *) &(mirror_dest->drop_group_bmp),
                     dnx_data_snif.ingress.nof_mirror_on_drop_profiles_get(unit), mod_profile_index)
        {
            /*
             * set profile key field 
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MOD_PROFILE_ID, mod_profile_index);
            /** commit */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read egress sniff attributes from HW (dbal)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest_id - Mirror profile gport
 *   \param [in] mirror_dest - Mirror profile attributes
 *
 * \remark
 *   * Mirror related HW. See dbal tables: \n
 *     DBAL_TABLE_SNIF_EGRESS_COMMAND_TABLE
 * \see
 *   * None
 */
static shr_error_e
dnx_mirror_egress_profile_hw_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_mirror_destination_t * mirror_dest)
{
    uint32 entry_handle_id, array32;
    uint32 snif_type, snif_probability, add_orig_head, crop_enable;
    int action_profile_id;
    uint32 rcy_priority;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    entry_handle_id = 0;
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    snif_type = DNX_MIRROR_DEST_TYPE_FROM_ID_GET(mirror_dest_id);

    /*
     * Read values from HW(DBAL SNIF command table).
     * Choose table key according to application type (mirror, snoop, sampling)
     */
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_EGRESS_COMMAND_TABLE, &entry_handle_id));
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

        /*
         * read relevant table values
         */
        dbal_value_field32_request(unit, entry_handle_id,
                                   DBAL_FIELD_SNIF_EGRESS_PROBABILITY, INST_SINGLE, &snif_probability);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SNIF_RCY_PRIORITY, INST_SINGLE, &rcy_priority);
        dbal_value_field32_request(unit, entry_handle_id,
                                   DBAL_FIELD_SNIF_APPEND_ORIG_HEADERS, INST_SINGLE, &add_orig_head);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SNIF_CROP_ENABLE, INST_SINGLE, &crop_enable);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        mirror_dest->is_recycle_strict_priority = !rcy_priority;
    }

    /*
     * After all values were read from HW, start filling the mirror_dest struct accordingly
     */
    {
        /*
         * add original system header according get value
         */
        mirror_dest->flags |= add_orig_head ? BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER : 0;

        /*
         * Crop size field.
         * Set crop size:
         * Crop enable - only 128B are cropped
         * Crop disable - whole packet is copied
         */
        if (crop_enable)
        {
            mirror_dest->egress_packet_copy_size = dnx_data_snif.egress.crop_size_get(unit);;
        }
        else
        {
            mirror_dest->egress_packet_copy_size = 0;
        }

        /*
         * Probability field. the dividend is the probability and the divisor is the max value according the field
         */
        mirror_dest->egress_sample_rate_dividend = snif_probability;
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                              DBAL_TABLE_SNIF_EGRESS_COMMAND_TABLE,
                                                              DBAL_FIELD_SNIF_EGRESS_PROBABILITY,
                                                              FALSE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &array32));
        mirror_dest->egress_sample_rate_divisor = array32;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Write egress sniff attributes to HW (dbal)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] mirror_dest - Mirror profile attributes
 *   \param [in] action_profile_id - profile ID
 *
 * \remark
 *   * Mirror related HW. See dbal tables: \n
 *     DBAL_TABLE_SNIF_EGRESS_COMMAND_TABLE
 * \see
 *   * None
 */
static shr_error_e
dnx_mirror_egress_profile_hw_set(
    int unit,
    bcm_mirror_destination_t * mirror_dest,
    int action_profile_id)
{
    uint32 snif_probability, add_orig_head;
    uint32 entry_handle_id;
    int snif_type;
    int nof_bits;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    snif_type = DNX_MIRROR_DEST_TYPE_GET(mirror_dest->flags);

    /*
     * Choose table key according to application type (mirror, snoop, sampling)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_EGRESS_COMMAND_TABLE, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    /*
     * Probability field
     */
    if (dnx_data_snif.egress.feature_get(unit, dnx_data_snif_egress_probability_sample))
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                        (unit, DBAL_TABLE_SNIF_EGRESS_COMMAND_TABLE, DBAL_FIELD_SNIF_EGRESS_PROBABILITY, FALSE, 0, 0,
                         &nof_bits));
        SHR_IF_ERR_EXIT(dnx_mirror_profile_probability_get
                        (unit, mirror_dest->egress_sample_rate_dividend, mirror_dest->egress_sample_rate_divisor,
                         nof_bits, &snif_probability));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_EGRESS_PROBABILITY, INST_SINGLE,
                                     snif_probability);
    }

    /*
     * snif recycle priority and command
     */
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_SNIF_RCY_PRIORITY, INST_SINGLE, !mirror_dest->is_recycle_strict_priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_RCY_CMD, INST_SINGLE, action_profile_id);

    /*
     * Crop enable field. The size should be given in bytes.
     * 128 - Crop 128B of the packet (crop enable)
     * 0 - No cropping (crop disable)
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_CROP_ENABLE, INST_SINGLE,
                                 mirror_dest->egress_packet_copy_size ? TRUE : FALSE);

    /*
     * original header indication
     */
    add_orig_head = (mirror_dest->flags & BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER) ? TRUE : FALSE;
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_SNIF_APPEND_ORIG_HEADERS, INST_SINGLE, add_orig_head);

    /*
     * snif enable indication
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_count_command_get(
    int unit,
    bcm_core_t core_id,
    bcm_gport_t mirror_dest_id,
    int counter_cmd_id,
    bcm_mirror_profile_cmd_count_t * mirror_count_info)
{
    uint32 entry_handle_id;
    uint32 snif_type, action_profile_id, is_count_ow, type_ow, obj_id_ow;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    entry_handle_id = 0;
    snif_type = DNX_MIRROR_DEST_TYPE_FROM_ID_GET(mirror_dest_id);

    /*
     * Read from HW (DBAL SNIF command table).
     * Choose table key according to application type (mirror, snoop, sampling)
     */
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_COUNTING_TABLE, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, ((core_id == BCM_CORE_ALL) ? 0 : core_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    /*
     * read all table values
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_VALID_OW, counter_cmd_id, &is_count_ow);
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_INT_STAT_OBJ_VALID, counter_cmd_id,
                               (uint32 *) &mirror_count_info->is_counted);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_ID_OW, counter_cmd_id, &obj_id_ow);
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_INT_STAT_OBJ_ID, counter_cmd_id,
                               (uint32 *) &mirror_count_info->object_stat_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_TYPE_OW, counter_cmd_id, &type_ow);
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_INT_STAT_OBJ_TYPE, counter_cmd_id, (uint32 *) &mirror_count_info->type_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (is_count_ow == FALSE)
    {
        mirror_count_info->is_counted = BCM_MIRROR_COUNT_AS_ORIG;
    }
    if (obj_id_ow == FALSE)
    {
        mirror_count_info->object_stat_id = BCM_MIRROR_COUNT_AS_ORIG;
    }
    if (type_ow == FALSE)
    {
        mirror_count_info->type_id = BCM_MIRROR_COUNT_AS_ORIG;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_count_command_set(
    int unit,
    bcm_core_t core_id,
    bcm_gport_t mirror_dest_id,
    int counter_cmd_id,
    bcm_mirror_profile_cmd_count_t * mirror_count_info)
{
    uint32 action_profile_id;
    uint32 entry_handle_id;
    int snif_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    snif_type = DNX_MIRROR_DEST_TYPE_FROM_ID_GET(mirror_dest_id);

    /*
     * Write to HW (DBAL SNIF command table).
     * Choose table key according to application type (mirror, snoop, sampling)
     */
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_COUNTING_TABLE, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);

    /*
     * is counted field
     */
    if (mirror_count_info->is_counted == BCM_MIRROR_COUNT_AS_ORIG)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_VALID_OW, counter_cmd_id, FALSE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_VALID_OW, counter_cmd_id, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_INT_STAT_OBJ_VALID, counter_cmd_id, mirror_count_info->is_counted);
    }

    /*
     * stat id field
     */
    if (mirror_count_info->object_stat_id == BCM_MIRROR_COUNT_AS_ORIG)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_ID_OW, counter_cmd_id, FALSE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_ID_OW, counter_cmd_id, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_INT_STAT_OBJ_ID, counter_cmd_id, mirror_count_info->object_stat_id);
    }
    /*
     * stat type field
     */
    if (mirror_count_info->type_id == BCM_MIRROR_COUNT_AS_ORIG)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_TYPE_OW, counter_cmd_id, FALSE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_STAT_OBJ_TYPE_OW, counter_cmd_id, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     DBAL_FIELD_INT_STAT_OBJ_TYPE, counter_cmd_id, mirror_count_info->type_id);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_create(
    int unit,
    bcm_mirror_destination_t * mirror_dest)
{
    int action_profile_id;

    SHR_FUNC_INIT_VARS(unit);

    action_profile_id = 0;

    /** profile action id information exist only when with id option is set */
    if (mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID)
    {
        action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
    }

    /** Allocate sniff profile only if replace flag is not set */
    if ((mirror_dest->flags & BCM_MIRROR_DEST_REPLACE) == 0)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_profile_allocate(unit, mirror_dest, &action_profile_id));
    }

    /** Set gport according to sniff type and action_profile_id */
    if (mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP)
    {
        BCM_GPORT_MIRROR_SNOOP_SET(mirror_dest->mirror_dest_id, action_profile_id);
    }
    else if (mirror_dest->flags & BCM_MIRROR_DEST_IS_STAT_SAMPLE)
    {
        BCM_GPORT_MIRROR_STAT_SAMPLE_SET(mirror_dest->mirror_dest_id, action_profile_id);
    }
    else
    {
        BCM_GPORT_MIRROR_MIRROR_SET(mirror_dest->mirror_dest_id, action_profile_id);
    }

    /*
     * Configure allocated profile in HW for ingress
     */
    SHR_IF_ERR_EXIT(dnx_mirror_profile_hw_set(unit, mirror_dest, action_profile_id));

    /*
     * Configure FTMH header stamping
     */
    SHR_IF_ERR_EXIT(dnx_mirror_header_stamping_set(unit, mirror_dest, action_profile_id));

    /*
     * Configure allocated profile in HW for egress only if the profile matches
     */
    if (action_profile_id < dnx_data_snif.egress.mirror_snoop_nof_profiles_get(unit) &&
        (mirror_dest->flags & BCM_MIRROR_DEST_IS_STAT_SAMPLE) == 0)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_egress_profile_hw_set(unit, mirror_dest, action_profile_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_mirror_destination_t * mirror_dest)
{
    int action_profile_id;

    SHR_FUNC_INIT_VARS(unit);

    /** retrieve Mirror attributes from HW */
    SHR_IF_ERR_EXIT(dnx_mirror_profile_hw_get(unit, mirror_dest_id, mirror_dest));

    /** retrieve Mirror attributes from FTMH header stamping */
    SHR_IF_ERR_EXIT(dnx_mirror_header_stamping_get(unit, mirror_dest_id, mirror_dest));

    /*
     * Get the profile from egress HW if:
     *     it is mirror/snoop, and
     *     the profile matches
     */
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    if (!_SHR_GPORT_IS_MIRROR_STAT_SAMPLE(mirror_dest_id)
        && (action_profile_id < dnx_data_snif.egress.mirror_snoop_nof_profiles_get(unit)))
    {
        SHR_IF_ERR_EXIT(dnx_mirror_egress_profile_hw_get(unit, mirror_dest_id, mirror_dest));
    }

    mirror_dest->mirror_dest_id = mirror_dest_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_destroy(
    int unit,
    bcm_gport_t mirror_dest_id)
{
    int snif_type = 0;
    int action_profile_id, profile, mod_profile_index;
    uint32 entry_handle_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Derive SNIFF type from mirror destination gport 
     */
    if (BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest_id))
    {
        snif_type = DBAL_ENUM_FVAL_SNIF_TYPE_SNOOP;
    }
    else if (_SHR_GPORT_IS_MIRROR_STAT_SAMPLE(mirror_dest_id))
    {
        snif_type = DBAL_ENUM_FVAL_SNIF_TYPE_STATISTICAL_SAMPLE;
    }
    else if (BCM_GPORT_IS_MIRROR_MIRROR(mirror_dest_id))
    {
        snif_type = DBAL_ENUM_FVAL_SNIF_TYPE_MIRROR;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "An unsupported gport type: 0x%x", mirror_dest_id);
    }

    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    /*
     * restore the following DBAL tables to default values:
     * DBAL_TABLE_SNIF_COMMAND_TABLE
     * DBAL_TABLE_SNIF_COUNTING_TABLE
     * DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE
     * DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE
     * DBAL_TABLE_SNIF_EGRESS_COMMAND_TABLE
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_COMMAND_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_COUNTING_TABLE, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** Read system_headers_mode */
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE_LEGACY, entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE, entry_handle_id));
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);
    if (dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);
    if (dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Configure allocated profile in HW for egress only if the profile matches
     */
    if (action_profile_id < dnx_data_snif.egress.mirror_snoop_nof_profiles_get(unit) &&
        (snif_type != DBAL_ENUM_FVAL_SNIF_TYPE_STATISTICAL_SAMPLE))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SNIF_EGRESS_COMMAND_TABLE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, action_profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, snif_type);
        if (dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS) == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    /**  clear mirror on drop profile */
    for (mod_profile_index = 0; mod_profile_index < dnx_data_snif.ingress.nof_mirror_on_drop_profiles_get(unit);
         mod_profile_index++)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.mirror_on_drop_profile.get(unit, mod_profile_index, &profile));
        if (profile == action_profile_id)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_MIRROR_ON_DROP_COMMAND, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MOD_PROFILE_ID, mod_profile_index);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
            /** set sw state to invalid profile to mark that no mirror on drop profile exists */
            SHR_IF_ERR_EXIT(dnx_mirror_db.
                            mirror_on_drop_profile.set(unit, mod_profile_index, DNX_MIRROR_ON_DROP_INVALID_PROFILE));
        }
    }

    /** Release allocated resource manager */
    SHR_IF_ERR_EXIT(dnx_mirror_profile_deallocate(unit, mirror_dest_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_tables_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 profile_id, snif_code, tmp_profile_id;
    int trap_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write Mirror profile to command code map table starting from DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_MIRR_PROFILE_TO_CODE_MAP, &entry_handle_id));
    for (profile_id = 0, snif_code = DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND(unit);
         profile_id < dnx_data_snif.ingress.mirror_nof_profiles_get(unit); profile_id++, snif_code++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MIRROR_PROFILE_ID, profile_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIRROR_CODE, INST_SINGLE, snif_code);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Write sniff code to profile id map table starting from DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_CODE_TO_PROFILE_MAP, &entry_handle_id));
    for (profile_id = 0, snif_code = DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND(unit);
         profile_id < dnx_data_snif.ingress.nof_profiles_get(unit); profile_id++, snif_code++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_CODE, snif_code);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_SNIF_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SNIF_COMMAND_ID, INST_SINGLE, profile_id);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Write snoop/mirror recycle command to code map table starting from DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND.
     * snoop strength field will get the highest value of the field
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_RCY_CMD_MAP, &entry_handle_id));
    for (profile_id = 0; profile_id < dnx_data_snif.egress.mirror_snoop_nof_profiles_get(unit); profile_id++)
    {
        tmp_profile_id = DNX_MIRROR_EGRESS_PROFILE_GET(profile_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RECYCLE_COMMAND_ID, tmp_profile_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIRROR_CODE, INST_SINGLE,
                                     profile_id + DNX_RX_TRAP_CODE_SNIF_PROFILE_START_IND(unit));
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgTxFieldSnoop0 + profile_id, &trap_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RCY_SNOOP_CODE, INST_SINGLE, trap_id);
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_RCY_SNOOP_STRENGTH, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * mirror profile need to be enabled for all pp ports
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_LLR_CONTEXT_PROPERTIES, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LL_MIRROR_PROFILE_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIRROR_CODE_SEL, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
