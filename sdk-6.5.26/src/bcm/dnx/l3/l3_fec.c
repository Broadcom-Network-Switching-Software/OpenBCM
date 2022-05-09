/** \file l3_fec.c
 *
 *  l3 FEC procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3
/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <bcm/types.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <shared/util.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_encap.h>
#include <bcm_int/dnx/failover/failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <include/bcm_int/dnx/auto_generated/dnx_l3_dispatch.h>
#include <include/bcm_int/dnx/auto_generated/dnx_stat_dispatch.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/qos/qos.h>

/*
 * }
 */
/*
 * Globals
 * {
 */
const uint32 L3_FEC_ENTRY_HIT_BIT_SELECTION_FLAG[DNX_DATA_MAX_MDB_DIRECT_NOF_FECS_IN_SUPER_FEC] =
    { DBAL_ENTRY_ATTR_FLAG_HIT_PRIMARY, DBAL_ENTRY_ATTR_FLAG_HIT_SECONDARY };
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
 * }
 */

/**
* \brief
*  Convert FEC RT dbal field into a FEC RT enum.
* \param [in] unit - Relevant unit.
* \param [in] fec_entry_type - FEC result type (DBAL_FIELD_FEC_DESTINATION_*)
* \param [out] dbal_enum - Return value of the FEC RT enum that was selected.
*/
static shr_error_e
dnx_l3_fec_dbal_fec_entry_type_convert(
    int unit,
    dbal_fields_e fec_entry_type,
    dbal_enum_value_field_irpp_fec_entry_format_e * dbal_enum)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (fec_entry_type)
    {
        case DBAL_FIELD_FEC_DESTINATION_LIF0:
            *dbal_enum = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0;
            break;
        case DBAL_FIELD_FEC_DESTINATION_LIF0_HTM:
            *dbal_enum = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_HTM;
            break;
        case DBAL_FIELD_FEC_DESTINATION_MC_RPF:
            *dbal_enum = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION;
            break;
        case DBAL_FIELD_FEC_DESTINATION_EEI:
            *dbal_enum = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_EEI;
            break;
        case DBAL_FIELD_FEC_DESTINATION_LIF0_17BIT_LIF1:
            *dbal_enum = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_17BIT_LIF1;
            break;
        case DBAL_FIELD_FEC_JR1_DESTINATION_ETH_RIF_ARP:
            *dbal_enum = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_ETH_RIF_ARP;
            break;
        case DBAL_FIELD_FEC_DESTINATION_LIF0_LIF1_MC_RPF:
            *dbal_enum = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_LIF1_MC_RPF;
            break;
        case DBAL_FIELD_FEC_DESTINATION_LIF0_MC_RPF:
            *dbal_enum = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_MC_RPF;
            break;
        case DBAL_FIELD_FEC_JR1_DESTINATION_EEI:
            *dbal_enum = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_EEI;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown FEC entry field ID %d.\r\n", fec_entry_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Decide the super FEC type of the FEC entry based on whether protection or/and statistics are enabled.
* \param [in] unit - Relevant unit.
* \param [in] protection_enable - Indication whether protection is enabled
* \param [in] stat_enable - Indication whether statistics are enabled
* \param [in] stat_direct - Indication whether direct statistics are enabled (stat_id = fec_id)
* \param [out] super_fec_resource_type - Return value of the super FEC resource type that was selected.
*/
static void
dnx_l3_fec_decide_super_fec_resource_type(
    int unit,
    uint8 protection_enable,
    uint8 stat_enable,
    uint8 stat_direct,
    dbal_enum_value_field_fec_resource_type_e * super_fec_resource_type)
{

    {
        /** Find new super fec type based on protection_enable and statistics_enable */
        if (protection_enable == TRUE)
        {
            *super_fec_resource_type =
                (stat_enable == TRUE) ? DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT :
                DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT;
        }
        else
        {
            *super_fec_resource_type =
                (stat_enable == TRUE) ? DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_W_STAT :
                DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_NO_STAT;
        }
    }

}
/**
 * \brief
 *   Determine the FEC table result (' fec_result_type') type according the EGRESS structure ('egr') parameters.
 *   FEC result type with a LIF cannot be chosen unless either (intf) or (intf and encap_id) are defined.
 *   \param [in] unit - Relevant unit.
 *   \param [in] egr - Egress structure that describes an entry.
 *   \param [out] fec_entry_type - Return FEC table result type.
 */
void
dnx_l3_egress_decide_fec_entry_type(
    int unit,
    bcm_l3_egress_t * egr,
    dbal_fields_e * fec_entry_type)
{
    dbal_enum_value_field_mc_rpf_mode_e mc_rpf_mode = L3_EGRESS_GET_MC_RPF_TYPE(egr->flags);
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * In case the MC RPF mode isn't disabled, use one of the FEC result types that have a MC RPF field.
     */
    if (mc_rpf_mode != DBAL_ENUM_FVAL_MC_RPF_MODE_RESERVED)
    {
        {
            /*
             * If the intf field and the encap_id field are not valid, then the user didn't specify any out-LIFs.
             * If both field values are valid then the user specified an out-LIF for the explicit RPF with tunnel encapsulation.
             */
            if (!BCM_L3_ITF_VAL_GET(egr->intf) && !BCM_L3_ITF_VAL_GET(egr->encap_id))
            {
                *fec_entry_type = DBAL_FIELD_FEC_DESTINATION_MC_RPF;
            }
            else if (!BCM_L3_ITF_VAL_GET(egr->encap_id))
            {
                *fec_entry_type = DBAL_FIELD_FEC_DESTINATION_LIF0_MC_RPF;
            }
            else
            {
                *fec_entry_type = DBAL_FIELD_FEC_DESTINATION_LIF0_LIF1_MC_RPF;
            }
        }
    }
    else if (egr->hierarchical_gport)
    {
        *fec_entry_type = DBAL_FIELD_FEC_DESTINATION_LIF0_HTM;
    }
    else if (egr->mpls_action == BCM_MPLS_EGRESS_ACTION_PHP)
    {
        *fec_entry_type = DBAL_FIELD_FEC_DESTINATION_EEI;
    }
    else if (egr->mpls_label != BCM_MPLS_LABEL_INVALID)
    {
        /*
         * In case of JR1 system headers mode and we have PUSH command it is going to EEI
         * In case of JR2 system headers mode it is going to FHEI
         */
        /*
         * Jericho mode beginning
         * {
         */
        if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            && (egr->mpls_action == BCM_MPLS_EGRESS_ACTION_PUSH))
        {
            *fec_entry_type = DBAL_FIELD_FEC_JR1_DESTINATION_EEI;
        }
        /*
         * Jericho mode end
         * }
         */
        else
        {
            *fec_entry_type = DBAL_FIELD_FEC_DESTINATION_EEI;
        }
    }
    /** BCM_L3_ENCAP_SPACE_OPTIMIZED is relevant only in Jericho mode */
    /*
     * Jericho mode beginning
     * {
     */
    else if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
             && (_SHR_IS_FLAG_SET(egr->flags, BCM_L3_ENCAP_SPACE_OPTIMIZED)))
    {
        *fec_entry_type = DBAL_FIELD_FEC_JR1_DESTINATION_EEI;
    }
    /*
     * Jericho mode end
     * }
     */
    /*
     * At least one LIF is populated
     */
    else if (BCM_L3_ITF_VAL_GET(egr->intf) != 0)
    {
        if (!BCM_L3_ITF_VAL_GET(egr->encap_id))
        {
            /*
             * In case the interface or the encapsulation ID arn't valid a single LIF FEC entry will be used.
             * It is assumed and verified that at least one of them is valid in case the MC RPF is disabled.
             */
            *fec_entry_type = DBAL_FIELD_FEC_DESTINATION_LIF0;
        }
        else
        {
            /*
             * The interface and the encapsulation ID are valid, a two LIFs FEC entry will be used.
             */

            if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
            {
                {
                    *fec_entry_type = DBAL_FIELD_FEC_DESTINATION_LIF0_17BIT_LIF1;
                }
            }
            /*
             * Jericho mode beginning
             * {
             */
            else
            {
                *fec_entry_type = DBAL_FIELD_FEC_JR1_DESTINATION_ETH_RIF_ARP;
            }
            /*
             * Jericho mode end
             * }
             */
        }
    }
    else
    {
        *fec_entry_type = DBAL_FIELD_FEC_DESTINATION_MC_RPF;
    }

    SHR_VOID_FUNC_EXIT;
}

static shr_error_e
dnx_l3_egress_fec_entry_stat_get(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    dbal_result_type_t super_fec_result_type,
    int nof_statistics,
    l3_fec_entry_info_t * fec_entry_info)
{
    uint32 stat_pp_profile;
    int stat_instance = (nof_statistics == 2) ? L3_FEC_ID_TO_FEC_INSTANCE(fec_entry_info->fec_id) : 0;
    dbal_table_field_info_t field_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, stat_instance, &stat_pp_profile));

    /*
     * Stat CMD (stat PP profile) can be reduced due to limitations in the FEC entry size.
     * ARR prefix will be added to fill the stat CMD MSBs to 5 bits.
     * The stat CMD will be read with its hw value -> ((arr_prefix << stat_cmd_size) | logical_value).
     * The user will receive the stat CMD with logical values from 0 to 2^stat_cmd_size-1.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, DBAL_FIELD_STAT_OBJECT_CMD, FALSE,
                                               super_fec_result_type, stat_instance, &field_info));

    /** In case an ARR is present for the STAT_OBJECT_CMD, need to update the stat CMD from hw to logical value. */
    if (field_info.arr_prefix_size)
    {
        stat_pp_profile = (stat_pp_profile & ((1 << field_info.field_nof_bits) - 1));
    }

    if (nof_statistics)
    {
        /** Get stat_object_id for indirect stats*/
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id,
                                                            DBAL_FIELD_STAT_OBJECT_ID, stat_instance,
                                                            &fec_entry_info->stat_obj_id));
    }
    else
    {
        /** Get stat_obj_id for direct stats */
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            
            /** If stat PP profile is valid, set the stat ID to indicate direct stat ID */
            fec_entry_info->stat_obj_id = STAT_PP_USE_DIRECT_STAT_ID;
        }
        else
        {
            /** If stat PP profile is not valid, set the stat ID to 0 */
            fec_entry_info->stat_obj_id = 0;
        }
    }
    STAT_PP_ENGINE_PROFILE_SET(fec_entry_info->stat_obj_cmd, stat_pp_profile, bcmStatCounterInterfaceIngressReceivePp);

exit:
    SHR_FUNC_EXIT;
}

/**
 * See the .h file for more information.
 */
shr_error_e
dnx_l3_egress_fec_entry_get(
    int unit,
    l3_fec_entry_info_t * fec_entry_info)
{
    int instance;
    uint32 entry_handle_id;
    uint32 super_fec_id;
    uint32 fec_entry_size = 0;
    uint32 eei_val = 0;
    uint32 *fec_entry_val = NULL, *decoded_fec_entry_data = NULL;
    dbal_enum_value_field_irpp_fec_entry_format_e fec_entry;
    dbal_result_type_t super_fec_result_type = 0;
    dbal_fields_e dbal_eei_type;
    dbal_fields_e fec_entry_type;
    dbal_core_mode_e core_mode;
    dbal_tables_e table_id;
    mdb_physical_table_e fec_db;
    const dnx_data_l3_fec_fec_result_type_fields_t *result_type_data;
    const dnx_data_l3_fec_fec_resource_info_t *fec_resource_info;
    const dnx_data_l3_fec_super_fec_result_types_map_t *result_types_map;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    super_fec_id = DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_entry_info->fec_id);
    instance = L3_FEC_ID_TO_FEC_INSTANCE(fec_entry_info->fec_id);

    /*
     * Get FEC physical DB based on FEC ID
     */
    SHR_IF_ERR_EXIT(algo_l3_fec_db_stage_map_get(unit, fec_entry_info->fec_id, &fec_db));

    table_id = dnx_data_l3.fec.fec_tables_info_get(unit, fec_db)->dbal_table;
    /*
     * Get the core mode
     */
    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, table_id, &core_mode));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * Set DBAL keys
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID, super_fec_id);

    if (core_mode == DBAL_CORE_MODE_DPC)
    {
        uint32 relative_fecz_id;
        /*
         * Set CORE_ID key for DPC tables
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

        /*
         * Get FEC information
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_convert_fecz_id(unit, fec_entry_info->fec_id, &relative_fecz_id));
        SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fecz.direction.get(unit, relative_fecz_id, &fec_entry_info->direction));
        SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fecz.hierarchy.get(unit, relative_fecz_id, &fec_entry_info->hierarchy));
    }

    /** If hit bit retrieval is supported, get the hit bit if the needed flags are provided. */
    if (!dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit))
    {
        if (_SHR_IS_FLAG_SET(fec_entry_info->flags, L3_FEC_ENTRY_INFO_GET_HIT_BIT)
            || (_SHR_IS_FLAG_SET(fec_entry_info->flags, L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT)))
        {
            /*
             * Enter ONLY if one of the following bits is set:
             * L3_FEC_ENTRY_INFO_GET_HIT_BIT, L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT
             */
            uint32 attr_val;
            uint32 attr_type =
                (L3_FEC_ENTRY_HIT_BIT_SELECTION_FLAG[fec_entry_info->fec_id & 0x1] | DBAL_ENTRY_ATTR_FLAG_HIT_GET);

            if (fec_entry_info->flags & L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT)
            {
                /*
                 * If the L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT bit is set then read the 'hit' bit AND
                 * do clear it.
                 */
                attr_type |= DBAL_ENTRY_ATTR_FLAG_HIT_CLEAR;
            }
            /**
             * Set the attribute
             */
            SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_HITBIT, attr_type));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle_id, DBAL_ENTRY_ATTR_HITBIT, &attr_val));

            if (attr_val & DBAL_PHYSICAL_KEY_PRIMARY_AND_SECONDARY_HITBIT_ACCESSED)
            {
                fec_entry_info->flags |= L3_FEC_ENTRY_INFO_HIT_BIT_VALUE;
            }
            else
            {
                fec_entry_info->flags &= ~(L3_FEC_ENTRY_INFO_HIT_BIT_VALUE);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &super_fec_result_type));

    /** Retrieve the FEC result type mapping information based on super FEC result type*/
    result_types_map = dnx_data_l3.fec.super_fec_result_types_map_get(unit, super_fec_result_type);

    /** Retrieve the FEC resource information */
    fec_resource_info = dnx_data_l3.fec.fec_resource_info_get(unit, result_types_map->fec_resource_type);

    if (fec_resource_info->is_statistic_type)
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_fec_entry_stat_get
                        (unit, entry_handle_id, table_id, super_fec_result_type,
                         fec_resource_info->nof_statistics, fec_entry_info));
    }
    /** Get the Size of the FEC entry, and alloc it */
    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, result_types_map->fec_type_in_super_fec, &fec_entry_size));

    SHR_ALLOC_SET_ZERO(fec_entry_val, sizeof(uint32) * BITS2WORDS(fec_entry_size),
                       "FEC entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(decoded_fec_entry_data, sizeof(uint32) * BITS2WORDS(fec_entry_size),
                       "decoded FEC entry data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Retrieve the FEC entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                            result_types_map->fec_type_in_super_fec, instance,
                                                            fec_entry_val));

    SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, result_types_map->fec_type_in_super_fec,
                                                    fec_entry_val, &fec_entry_type, decoded_fec_entry_data));

    /*
     * Convert the FEC dbal field result type into FEC enum result type
     */
    SHR_IF_ERR_EXIT(dnx_l3_fec_dbal_fec_entry_type_convert(unit, fec_entry_type, &fec_entry));
    /**
     * destination resolution
     */
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type, DBAL_FIELD_DESTINATION,
                                                    &fec_entry_info->destination, decoded_fec_entry_data));
    result_type_data = dnx_data_l3.fec.fec_result_type_fields_get(unit, fec_entry);
    /**
     * global outlif resolution
     */
    if (result_type_data->has_global_outlif)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type, DBAL_FIELD_GLOBAL_OUT_LIF_0,
                                                        &fec_entry_info->glob_out_lif, decoded_fec_entry_data));
    }
    /**
     * global outlif 2nd resolution
     */
    if (result_type_data->has_global_outlif_second)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type, DBAL_FIELD_GLOBAL_OUT_LIF_1,
                                                        &fec_entry_info->glob_out_lif_2nd, decoded_fec_entry_data));
    }
    /**
     * mc rpf mode resolution
     */
    if (result_type_data->has_mc_rpf)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type, DBAL_FIELD_MC_RPF_MODE,
                                                        &fec_entry_info->mc_rpf_mode, decoded_fec_entry_data));
    }
    /**
    * hierarchical tm flow resolution
     */
    if (result_type_data->has_htm)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type, DBAL_FIELD_HIERARCHICAL_TM_FLOW,
                                                        &fec_entry_info->hierarchical_gport, decoded_fec_entry_data));
    }
    /**
     * eei resolution
     */
    if (result_type_data->has_eei)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type, DBAL_FIELD_EEI, &fec_entry_info->eei,
                                                        decoded_fec_entry_data));
    }
    /*
     * JR1_DESTINATION_* result types handling
     */
    if (dnx_data_l3.fec.fer_hw_version_get(unit) == DNX_L3_FER_HW_VERSION_1)
    {
        if (fec_entry == DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_EEI)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type, DBAL_FIELD_EEI, &fec_entry_info->eei,
                                                            decoded_fec_entry_data));
            SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                            (unit, DBAL_FIELD_EEI, fec_entry_info->eei, &dbal_eei_type, &eei_val));
            if (dbal_eei_type == DBAL_FIELD_EEI_ENCAPSULATION_POINTER)
            {
                fec_entry_info->flags |= L3_FEC_ENTRY_INFO_EEI_ENC_POINTER;
            }
            else
            {
                fec_entry_info->flags |= L3_FEC_ENTRY_INFO_EEI_PUSH_COMMAND;
            }

        }
        if (fec_entry == DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_ETH_RIF_ARP)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, fec_entry_type, DBAL_FIELD_GLOBAL_OUT_LIF_0,
                                                            &fec_entry_info->glob_out_lif, decoded_fec_entry_data));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, fec_entry_type, DBAL_FIELD_EEI, &eei_val, decoded_fec_entry_data));
            fec_entry_info->glob_out_lif_2nd = L3_FEC_EEI_POINTER_TO_ENCAP_ID(eei_val);
        }
    }
    fec_entry_info->fec_entry_type = fec_entry_type;

    /** Get protection information */
    if (fec_resource_info->is_protection_type)
    {
        fec_entry_info->protection_enable = TRUE;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                         &(fec_entry_info->protection_ptr)));
    }
    else
    {
        fec_entry_info->protection_enable = FALSE;
    }

exit:
    SHR_FREE(fec_entry_val);
    SHR_FREE(decoded_fec_entry_data);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_l3_egress_fec_entry_stat_update(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    dbal_result_type_t super_fec_result_type,
    int nof_statistics,
    l3_fec_entry_info_t * fec_entry_info)
{
    uint32 stat_pp_profile = fec_entry_info->stat_obj_cmd;
    int stat_instance = (nof_statistics == 2) ? L3_FEC_ID_TO_FEC_INSTANCE(fec_entry_info->fec_id) : 0;
    dbal_table_field_info_t field_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Stat CMD (stat PP profile) can be reduced due to limitations in the FEC entry size.
     * ARR prefix will be added to fill the stat CMD MSBs to 5 bits.
     * The user will manage the stat CMD with logical values from 0 to 2^stat_cmd_size-1.
     * The stat CMD will be written with its hw value -> ((arr_prefix << stat_cmd_size) | logical_value).
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, DBAL_FIELD_STAT_OBJECT_CMD, FALSE,
                                               super_fec_result_type, stat_instance, &field_info));

    /** In case an ARR is present for the STAT_OBJECT_CMD, need to update the stat CMD from logical to hw value. */
    if (field_info.arr_prefix_size)
    {
        stat_pp_profile = ((field_info.arr_prefix << field_info.field_nof_bits) | stat_pp_profile);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, stat_instance,
                                 STAT_PP_PROFILE_ID_GET(stat_pp_profile));
    /** Set only if result is indirect stat */
    if (nof_statistics)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, stat_instance,
                                     fec_entry_info->stat_obj_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See the .h file for more information.
 */
shr_error_e
dnx_l3_egress_fec_entry_update(
    int unit,
    l3_fec_entry_info_t * fec_entry_info,
    dnx_l3_fec_entry_actions_e entry_action)
{
    uint32 entry_handle_id;
    dbal_tables_e fec_table;
    mdb_physical_table_e fec_db;
    dbal_enum_value_field_irpp_fec_entry_format_e fec_type;

    /** Key fields */
    uint32 super_fec_id;
    int instance = 0;

    /** Result fields */
    dbal_enum_value_field_fec_resource_type_e fec_resource_type;
    dbal_fields_e fec_type_in_super_fec;
    dbal_core_mode_e core_mode;
    uint32 fec_entry_size = 0;
    uint32 *fec_entry = NULL;
    const dnx_data_l3_fec_super_fec_result_types_map_t *result_type_map;
    const dnx_data_l3_fec_fec_resource_info_t *fec_resource_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Parse key fields from FEC ID */
    super_fec_id = DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_entry_info->fec_id);

    /*
     * Get the FEC resource type from the res mngr
     * It indicates whether the FEC has protection or not and whether it has statistics or not.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_super_fec_type_get(unit, fec_entry_info->fec_id, &fec_resource_type));
    /** Retrieve the FEC resource information*/
    fec_resource_info = dnx_data_l3.fec.fec_resource_info_get(unit, fec_resource_type);

    /** Get FEC physical DB based on FEC ID */
    SHR_IF_ERR_EXIT(algo_l3_fec_db_stage_map_get(unit, fec_entry_info->fec_id, &fec_db));

    /** Get FEC DBAL table */
    fec_table = dnx_data_l3.fec.fec_tables_info_get(unit, fec_db)->dbal_table;
    instance = L3_FEC_ID_TO_FEC_INSTANCE(fec_entry_info->fec_id);

    result_type_map = dnx_data_l3.fec.super_fec_result_types_map_get(unit, fec_resource_info->dbal_result_type);

    /** Get FEC type in Super FEC */
    fec_type_in_super_fec = result_type_map->fec_type_in_super_fec;

    /** Get the Size of the FEC entry, and alloc it */
    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, fec_type_in_super_fec, &fec_entry_size));

    SHR_ALLOC_SET_ZERO(fec_entry, sizeof(uint32) * BITS2WORDS(fec_entry_size),
                       "FEC entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get the core mode of the table*/
    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, fec_table, &core_mode));

    /** Fill entry according to table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, fec_table, &entry_handle_id));

    /** Set key fields  */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID, super_fec_id);
    if (core_mode == DBAL_CORE_MODE_DPC)
    {
        /**Set CORE_ID key field for tables that are DPC*/
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    }
    /** Set result fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 fec_resource_info->dbal_result_type);

    if (fec_resource_info->is_statistic_type)
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_fec_entry_stat_update
                        (unit, entry_handle_id, fec_table, fec_resource_info->dbal_result_type,
                         fec_resource_info->nof_statistics, fec_entry_info));
    }
    if (fec_resource_info->is_protection_type)
    {
        /*
         * Protection is always updated, not matter if the current fec
         * is the prioritize or non-prioritized one
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                     fec_entry_info->protection_ptr);
    }

    /**
     * For delete, just clear the FEC-entry content,so skip fec entry build
     */
    if (entry_action == DNX_L3_FEC_ENTRY_ACTION_DELETE)
    {
        /*
         * entry type is irrelevant, but still need one valid type
         */
        fec_entry_info->fec_entry_type = DBAL_FIELD_FEC_DESTINATION_LIF0;
    }
    else
    {
        const dnx_data_l3_fec_fec_result_type_fields_t *result_type_data;
        /*
         * Build the fec entry
         */
        SHR_IF_ERR_EXIT(dnx_l3_fec_dbal_fec_entry_type_convert(unit, fec_entry_info->fec_entry_type, &fec_type));
        /*
         * destination - always set
         */
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, fec_entry_info->fec_entry_type, DBAL_FIELD_DESTINATION,
                                                        &fec_entry_info->destination, fec_entry));
        result_type_data = dnx_data_l3.fec.fec_result_type_fields_get(unit, fec_type);
        /*
         * Rest of the fields are set according to the fec entry type
         */
        if (result_type_data->has_global_outlif)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, fec_entry_info->fec_entry_type, DBAL_FIELD_GLOBAL_OUT_LIF_0,
                             &fec_entry_info->glob_out_lif, fec_entry));
        }
        if (result_type_data->has_global_outlif_second)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, fec_entry_info->fec_entry_type, DBAL_FIELD_GLOBAL_OUT_LIF_1,
                             &fec_entry_info->glob_out_lif_2nd, fec_entry));
        }
        if (result_type_data->has_mc_rpf)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, fec_entry_info->fec_entry_type, DBAL_FIELD_MC_RPF_MODE,
                             &fec_entry_info->mc_rpf_mode, fec_entry));
        }
        if (result_type_data->has_htm)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, fec_entry_info->fec_entry_type, DBAL_FIELD_HIERARCHICAL_TM_FLOW,
                             &fec_entry_info->hierarchical_gport, fec_entry));
        }
        if (result_type_data->has_eei)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                            (unit, fec_entry_info->fec_entry_type, DBAL_FIELD_EEI, &fec_entry_info->eei, fec_entry));
        }
        /*
         * JR1_DESTINATION_* entry types are handled separately
         */
        if (dnx_data_l3.fec.fer_hw_version_get(unit) == DNX_L3_FER_HW_VERSION_1)
        {
            if (fec_type == DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_EEI)
            {
                if (_SHR_IS_FLAG_SET(fec_entry_info->flags, L3_FEC_ENTRY_INFO_EEI_PUSH_COMMAND))
                {
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                    (unit, fec_entry_info->fec_entry_type, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND,
                                     &fec_entry_info->eei, fec_entry));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                    (unit, fec_entry_info->fec_entry_type, DBAL_FIELD_EEI_ENCAPSULATION_POINTER,
                                     &fec_entry_info->eei, fec_entry));
                }
            }
            if (fec_type == DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_ETH_RIF_ARP)
            {
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, fec_entry_info->fec_entry_type, DBAL_FIELD_GLOBAL_OUT_LIF_0,
                                 &fec_entry_info->glob_out_lif, fec_entry));
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, fec_entry_info->fec_entry_type, DBAL_FIELD_EEI,
                                 &fec_entry_info->glob_out_lif_2nd, fec_entry));
            }
        }
        /*
         * Set the SW state that holds the information for the FEC DB that supports more than a single hierarchy and
         * direction
         */
        if (core_mode == DBAL_CORE_MODE_DPC)
        {
            uint32 relative_fecz_id;
            SHR_IF_ERR_EXIT(dnx_algo_l3_convert_fecz_id(unit, fec_entry_info->fec_id, &relative_fecz_id));
            SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fecz.
                            direction.set(unit, relative_fecz_id, fec_entry_info->direction));

            SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fecz.
                            hierarchy.set(unit, relative_fecz_id, fec_entry_info->hierarchy));
        }
    }

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, fec_entry_info->fec_entry_type, instance, fec_entry);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** If hit bit retrieval is supported, then check if it needs to be cleared. */
    if (!dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit))
    {
        /*
         * If required, clear the 'hit bit' after adding this new entry.
         */
        if (fec_entry_info->flags & L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT)
        {
            /*
             * Enter ONLY if L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT bit is set
             */
            uint32 attr_type;
            /*
             * Reuse the DBAL handle for another operation
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, fec_table, entry_handle_id));
            /*
             * Set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID, super_fec_id);
            if (core_mode == DBAL_CORE_MODE_DPC)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
            }
            /*
             * If the L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT bit is set then
             * do clear it (The 'get' is not meaningful).
             * Note that the key is 'super_fec_id' which stands for both 'primary' and 'secondary'
             */
            attr_type =
                DBAL_ENTRY_ATTR_FLAG_HIT_CLEAR | DBAL_ENTRY_ATTR_FLAG_HIT_PRIMARY | DBAL_ENTRY_ATTR_FLAG_HIT_SECONDARY;
            /**
             * Set the attribute
             */
            SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_HITBIT, attr_type));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        }
    }

exit:
    SHR_FREE(fec_entry);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   HW tables write function for bcm_l3_egress_create with BCM_L3_INGRESS_ONLY flag.
*   Fills FEC table of index taken from if_id with destination and OutLif and/or OutRif information.
*
*    \param [in] unit - Relevant unit.
*    \param [in] flags -
*      Similar to bcm_l3_egress_create api input
*    \param [in] egr - Similar to bcm_l3_egress_create api input
*      Note that if (egr->flags2 & BCM_L3_FLAGS2_SKIP_HIT_CLEAR) is set then
*      the 'hit' bit is NOT cleared after writing the entry.
*      This is optional since it is expensive in real time.
*    \param [in] if_id - Similar to bcm_l3_egress_create api input
* \return
*    shr_error_e - Error return value
*/
static shr_error_e
dnx_l3_egress_create_fec_hw_write(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    bcm_if_t * if_id)
{
    l3_fec_entry_info_t fec_entry_info;
    int system_headers_mode;
    uint32 old_use_push = 0, old_push_profile = 0;
    uint32 new_use_push = 0;
    dbal_enum_value_field_irpp_fec_entry_format_e fec_entry = 0;
    const dnx_data_l3_fec_fec_result_type_fields_t *result_type_data;
    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * Jericho mode beginning
     * push profile process
     * retrieve the current profile first
     * reference.
     * {
     */
    if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        && _SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
    {
        int push_cmd;
        uint32 label;
        l3_fec_entry_info_t old_fec_entry_info;

        sal_memset(&old_fec_entry_info, 0, sizeof(l3_fec_entry_info_t));
        old_fec_entry_info.fec_id = BCM_L3_ITF_VAL_GET(*if_id);
        SHR_IF_ERR_EXIT(dnx_l3_egress_fec_entry_get(unit, &old_fec_entry_info));

        if (_SHR_IS_FLAG_SET(old_fec_entry_info.flags, L3_FEC_ENTRY_INFO_EEI_PUSH_COMMAND))
        {
            SHR_IF_ERR_EXIT(dnx_mpls_eei_push_information_retrieve(unit, old_fec_entry_info.eei, &label, &push_cmd));
            old_use_push = 1;
            old_push_profile = push_cmd;
        }

    }
    /*
     * Jericho mode end
     * }
     */

    sal_memset(&fec_entry_info, 0, sizeof(l3_fec_entry_info_t));
    dnx_l3_egress_decide_fec_entry_type(unit, egr, &fec_entry_info.fec_entry_type);

    fec_entry_info.fec_id = BCM_L3_ITF_VAL_GET(*if_id);

    /** Destination */
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, egr->port, &fec_entry_info.destination));

    /** Set the MC RPF mode */
    fec_entry_info.mc_rpf_mode = L3_EGRESS_GET_MC_RPF_TYPE(egr->flags);

    SHR_IF_ERR_EXIT(dnx_l3_fec_dbal_fec_entry_type_convert(unit, fec_entry_info.fec_entry_type, &fec_entry));

    result_type_data = (dnx_data_l3.fec.fec_result_type_fields_get(unit, fec_entry));
    /** Set two or single out lif, according to fec entry type */
    if (result_type_data->has_global_outlif)
    {
        fec_entry_info.glob_out_lif = BCM_L3_ITF_VAL_GET(egr->intf);
    }
    if (result_type_data->has_global_outlif_second)
    {
        fec_entry_info.glob_out_lif_2nd = BCM_L3_ITF_VAL_GET(egr->encap_id);

    }
    /** FEC type EEI - encode relevant eei info and set it */
    if (result_type_data->has_eei)
    {
        uint32 eei_value = 0;
        uint32 remark_profile;

        if (egr->mpls_action == BCM_MPLS_EGRESS_ACTION_PHP)
        {
            dbal_enum_value_field_model_type_e qos_model;
            dbal_enum_value_field_model_type_e ttl_model;

            if (egr->egress_qos_model.egress_qos == bcmQosEgressModelUniform)
            {
                qos_model = DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM;
            }
            else
            {
                qos_model = DBAL_ENUM_FVAL_MODEL_TYPE_PIPE;
            }
            if (egr->egress_qos_model.egress_ttl == bcmQosEgressModelUniform)
            {
                ttl_model = DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM;
            }
            else
            {
                ttl_model = DBAL_ENUM_FVAL_MODEL_TYPE_PIPE;
            }

            fec_entry_info.flags |= L3_FEC_ENTRY_INFO_EEI_PHP;
            SHR_IF_ERR_EXIT(dnx_mpls_eei_php_information_fill
                            (unit, egr->qos_map_id, qos_model, ttl_model, DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS, &eei_value));

            SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                            (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_POP_COMMAND, &eei_value, &fec_entry_info.eei));
        }
        else
        {
            if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
            {
                uint32 profile = DNX_QOS_MAP_PROFILE_GET(egr->qos_map_id);

                remark_profile = DBAL_ENUM_FVAL_SWAP_QOS_PROFILE_DEFAULT + profile;
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, DBAL_FIELD_SWAP_QOS_PROFILE, &remark_profile,
                                 &eei_value));
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, DBAL_FIELD_MPLS_LABEL,
                                 &egr->mpls_label, &eei_value));

                SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, &eei_value,
                                 &fec_entry_info.eei));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, &egr->mpls_label,
                                 &fec_entry_info.eei));
            }
        }
    }

    if (result_type_data->has_htm)
    {
        fec_entry_info.hierarchical_gport = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(egr->hierarchical_gport);
    }

    /*
     * JR1_DESTINATION_* result types are handled separately
     */
    if (dnx_data_l3.fec.fer_hw_version_get(unit) == DNX_L3_FER_HW_VERSION_1)
    {
        /** BCM_L3_ENCAP_SPACE_OPTIMIZED is relevant only in Jericho mode */
        /*
         * Jericho mode beginning
         * {
         */
        if (fec_entry == DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_EEI)
        {
            if (egr->mpls_action == BCM_MPLS_EGRESS_ACTION_PUSH)
            {
                bcm_mpls_egress_label_t label_entry;
                int push_cmd = 0;

                new_use_push = 1;
                bcm_mpls_egress_label_t_init(&label_entry);
                dnx_fill_mpls_label_entry_with_l3_egr_info(unit, egr, &label_entry);

                fec_entry_info.flags |= L3_FEC_ENTRY_INFO_EEI_PUSH_COMMAND;
                if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE) && old_use_push)
                {
                    push_cmd = old_push_profile;
                    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_create_push_entry_from_eei
                                    (unit, &label_entry, FALSE, TRUE, &push_cmd));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_create_push_entry_from_eei
                                    (unit, &label_entry, FALSE, FALSE, &push_cmd));
                }

                SHR_IF_ERR_EXIT(dnx_mpls_eei_push_information_fill
                                (unit, egr->mpls_label, push_cmd, &fec_entry_info.eei));

            }
            else
            {
                fec_entry_info.flags |= L3_FEC_ENTRY_INFO_EEI_ENC_POINTER;
                SHR_IF_ERR_EXIT(dnx_mpls_eei_enc_pointer_information_fill
                                (unit, BCM_L3_ITF_VAL_GET(egr->intf), 0, &fec_entry_info.eei));
            }
        }
        if (fec_entry == DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_ETH_RIF_ARP)
        {
            fec_entry_info.glob_out_lif = BCM_L3_ITF_VAL_GET(egr->intf);
            fec_entry_info.glob_out_lif_2nd =
                (0xc00000 | L3_FEC_ENCAP_ID_TO_EEI_POINTER(BCM_L3_ITF_VAL_GET(egr->encap_id)));
        }
        /*
         * Jericho mode end
         * }
         */
    }

    /** set protection information */
    if (DNX_FAILOVER_IS_PROTECTION_ENABLED(egr->failover_id)) /** protection is enabled */
    {
        fec_entry_info.protection_enable = TRUE;

        /** facility protection */
        if (L3_EGRESS_FAILOVER_ID_IS_FACILITY(egr->failover_id))
        {
            fec_entry_info.protection_ptr = dnx_data_failover.path_select.fec_facility_protection_get(unit);
        }
        /** set the provided protection pointer */
        else
        {
            BCM_FAILOVER_ID_GET(fec_entry_info.protection_ptr, egr->failover_id);
        }
    }
    /** disable protection */
    else
    {
        fec_entry_info.protection_enable = FALSE;
    }

    /** statistics info */
    fec_entry_info.stat_obj_cmd = egr->stat_pp_profile;
    fec_entry_info.stat_obj_id = egr->stat_id;

    /*
     * Jericho mode beginning
     * push profile process
     * reference.
     * {
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        if (old_use_push && !new_use_push)
        {
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_delete_push_entry_from_eei(unit, old_push_profile));
        }
    }
    /*
     * Jericho mode end
     * }
     */
    /** If hit bit retrieval is supported, check HIT_BIT flags */
    if (!dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit))
    {
        if (egr->flags2 & BCM_L3_FLAGS2_SKIP_HIT_CLEAR)
        {
            fec_entry_info.flags &= ~L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT;
        }
        else
        {
            fec_entry_info.flags |= L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT;
        }
    }
    SHR_IF_ERR_EXIT(dnx_l3_egress_fec_entry_update(unit, &fec_entry_info, DNX_L3_FEC_ENTRY_ACTION_ADD));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* delete a L3 egress FEC entry from HW table
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] fec_index  - fec index.
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
*/
static shr_error_e
dnx_l3_egress_fec_entry_delete(
    int unit,
    uint32 fec_index)
{
    l3_fec_entry_info_t fec_entry;
    uint8 is_protected;
    uint8 fec_in_pair_is_allocated;
    uint32 super_fec_id;
    uint32 entry_handle_id;
    dbal_core_mode_e core_mode;
    dbal_tables_e table_id;
    mdb_physical_table_e fec_db;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&fec_entry, 0, sizeof(l3_fec_entry_info_t));

    /** Verify whether the FEC is protected and if it is verify that the primary FEC is deleted first. */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, fec_index, &is_protected));
    fec_entry.fec_id = BCM_L3_ITF_VAL_GET(fec_index);

    /** Retrieve indication whether the other FEC in the pair is allocated or not. */
    if (L3_FEC_ID_IS_ODD_FEC_ID(fec_index))
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, L3_FEC_ID_TO_EVEN_FEC_ID(fec_index),
                                                     &fec_in_pair_is_allocated));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, L3_FEC_ID_TO_ODD_FEC_ID(fec_index),
                                                     &fec_in_pair_is_allocated));
    }

    /*
     * In case clearing a FEC from a pair in which the other FEC is allocated (or the primary FEC in the protected pair)
     * we will set part of the super FEC entry to 0s.
     * In case clearing a FEC from a pair in which the other FEC doesn't exist (or the secondary FEC in a protected pair),
     * we will clear the whole entry.
     */
    if ((fec_in_pair_is_allocated && is_protected == FALSE)
        || (is_protected == TRUE && !L3_FEC_ID_IS_ODD_FEC_ID(fec_index)))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_fec_entry_update(unit, &fec_entry, DNX_L3_FEC_ENTRY_ACTION_DELETE));
    }
    else
    {
        super_fec_id = DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_index);
        /*
         * Get FEC DB according to direction and hierarchy
         */
        SHR_IF_ERR_EXIT(algo_l3_fec_db_stage_map_get(unit, fec_index, &fec_db));

        /** FEC table */
        table_id = dnx_data_l3.fec.fec_tables_info_get(unit, fec_db)->dbal_table;
        SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, table_id, &core_mode));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        /** Set DBAL keys*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SUPER_FEC_ID, super_fec_id);
        if (core_mode == DBAL_CORE_MODE_DPC)
        {
            dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        }
        /** Delete the full super FEC entry */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Convert fec type (BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY) to failover type.
 *
 */
static void
dnx_l3_egress_fec_type_to_failover_type_convert(
    int unit,
    uint32 flags,
    int *failover_type,
    int *hierarchy)
{
    SHR_FUNC_INIT_VARS(unit);

    *failover_type = BCM_FAILOVER_TYPE_FEC;
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_3RD_HIERARCHY))
    {
        *hierarchy = 2;
    }
    else if (_SHR_IS_FLAG_SET(flags, BCM_L3_2ND_HIERARCHY))
    {
        *hierarchy = 1;
    }
    else
    {
        *hierarchy = 0;
    }

    SHR_VOID_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for bcm_l3_egress_get with BCM_L3_INGRESS_ONLY flag.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] intf - Similar to bcm_l3_egress_destroy api input
 *
 * \return
 *  shr_error_e - Error return value
 * ****************************************************
 */
shr_error_e
dnx_l3_egress_fec_get_verify(
    int unit,
    bcm_if_t intf)
{

    int fec_index;
    uint8 is_allocated = 0;
    uint32 max_fec_id;

    SHR_FUNC_INIT_VARS(unit);

    fec_index = BCM_L3_ITF_VAL_GET(intf);

    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));

    /*
     * Verify that FEC is in range
     */
    if (fec_index > max_fec_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEC ID %d is out of range - should be positive and smaller than %d\n",
                     fec_index, max_fec_id);
    }

    /*
     * Verify that FEC is allocated
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, fec_index, &is_allocated));

    if (is_allocated == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot destroy FEC with ID %d as it is not allocated \n", intf);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for bcm_l3_egress_destroy with BCM_L3_INGRESS_ONLY flag.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] intf - Similar to bcm_l3_egress_destroy api input
 *
 * \return
 *  shr_error_e - Error return value
 * ****************************************************
 */
shr_error_e
dnx_l3_egress_fec_destroy_verify(
    int unit,
    bcm_if_t intf)
{

    int fec_index;
    uint8 is_protected = 0;
    l3_fec_entry_info_t fec_entry_info;
    SHR_FUNC_INIT_VARS(unit);

    fec_index = BCM_L3_ITF_VAL_GET(intf);

    /*
     * All the GET verifier verifications are relevant for the destroy function
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_fec_get_verify(unit, intf));

    /*
     * Verify whether the FEC is protected and if it is verify that the primary FEC is deleted first.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, fec_index, &is_protected));
    if (is_protected == TRUE)
    {
        fec_entry_info.protection_enable = TRUE;
        /** If the FEC is odd, then it is the secondary one. */
        if (L3_FEC_ID_IS_ODD_FEC_ID(fec_index))
        {
            fec_entry_info.fec_id = L3_FEC_ID_TO_EVEN_FEC_ID(fec_index);
            SHR_IF_ERR_EXIT(dnx_l3_egress_fec_entry_get(unit, &fec_entry_info));

            if (fec_entry_info.destination || fec_entry_info.glob_out_lif)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The secondary FEC (ID %d) in a protected pair can only be destroyed after the primary.\n",
                             intf);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for bcm_l3_egress_create with BCM_L3_INGRESS_ONLY flag.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] flags - Similar to bcm_l3_egress_create api input
 *   \param [in] egr -Similar to bcm_l3_egress_create api input
 *   \param [in] if_id - Similar to bcm_l3_egress_create api input
 *
 * \return
 *  shr_error_e - Error return value
 * ****************************************************
 */
shr_error_e
dnx_l3_egress_create_fec_verify(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    bcm_if_t * if_id)
{
    int failover_type;
    int fec_hierarchy;
    int fec_id;
    uint8 is_allocated = 0;
    int system_headers_mode = 0;
    /*
     * uint32 fec_index;
     */
    dbal_enum_value_field_mc_rpf_mode_e mc_rpf_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /** Verify supported flags */
    if (_SHR_IS_FLAG_SET(egr->flags, ~dnx_data_l3.fec.supported_fec_flags_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags are passed to bcm_l3_egress_create API - 0x%08X", (egr->flags));
    }

    /** Verify supported flags2 */
    if (_SHR_IS_FLAG_SET(egr->flags2, ~dnx_data_l3.fec.supported_fec_flags2_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags2 are passed to bcm_l3_egress_create API - 0x%08X", (egr->flags2));
    }

    /** BCM_L3_ENCAP_SPACE_OPTIMIZED is relevant only in Jericho mode */
    /*
     * Jericho mode beginning
     * {
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        if (_SHR_IS_FLAG_SET(egr->flags, BCM_L3_ENCAP_SPACE_OPTIMIZED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_L3_ENCAP_SPACE_OPTIMIZED flag should  be set only under Jericho system_headers_mode..\r\n");
        }

        if ((egr->mpls_action != BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH) && (egr->mpls_label != BCM_MPLS_LABEL_INVALID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "mpls_label(0x%x) is relevant only in case of BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH for SYSTEM_HEADERS_MODE_JERICHO2_MODE.\n",
                         egr->mpls_label);
        }
    }
    else
    {
        if ((egr->mpls_action != BCM_MPLS_EGRESS_ACTION_SWAP)
            && (egr->mpls_action != BCM_MPLS_EGRESS_ACTION_PUSH) && (egr->mpls_label != BCM_MPLS_LABEL_INVALID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "mpls_label(0x%x) is relevant only in case of BCM_MPLS_EGRESS_ACTION_SWAP or PUSH.\n",
                         egr->mpls_label);
        }

        /**
         * When EEI is used for PUSH or Pointer , STAT is not supported
         */
        if (((egr->mpls_action == BCM_MPLS_EGRESS_ACTION_PUSH)
             || _SHR_IS_FLAG_SET(egr->flags, BCM_L3_ENCAP_SPACE_OPTIMIZED))
            && (egr->stat_pp_profile != STAT_PP_PROFILE_INVALID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC using EEI(PUSH or Pointer) does not support STAT!!.\n");
        }
    }

    /*
     * Jericho mode end
     * }
     */

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE) && !_SHR_IS_FLAG_SET(flags, BCM_L3_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "configuring FEC with BCM_L3_REPLACE must be called with BCM_L3_WITH_ID");
    }

    /** Verify the supported MC RPF flags*/
    if (!dnx_data_l3.fec.feature_get(unit, dnx_data_l3_fec_mc_rpf_sip_based_supported))
    {
        if (((egr->flags & BCM_L3_MC_RPF_SIP_BASE) == BCM_L3_MC_RPF_SIP_BASE)
            && ((egr->flags & BCM_L3_MC_RPF_EXPLICIT) != BCM_L3_MC_RPF_EXPLICIT)
            && ((egr->flags & BCM_L3_MC_RPF_EXPLICIT_ECMP) != BCM_L3_MC_RPF_EXPLICIT_ECMP)
            && ((egr->flags & BCM_L3_MC_NO_RPF) != BCM_L3_MC_NO_RPF))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Using BCM_L3_MC_RPF_SIP_BASE flag is not supported!");
        }
    }
    /** Set the MC_RPF mode based on the provided flags*/
    mc_rpf_mode = L3_EGRESS_GET_MC_RPF_TYPE(egr->flags);

    /*
     * destination must be of type MC ID
     */
    if ((dnx_data_l3.fec.fer_hw_version_get(unit) == DNX_L3_FER_HW_VERSION_2)
        && (mc_rpf_mode != DBAL_ENUM_FVAL_MC_RPF_MODE_RESERVED) && BCM_L3_ITF_VAL_GET(egr->intf)
        && !BCM_GPORT_IS_MCAST(egr->port))
    {
        /*
         * There are two result types that are available for HW version 2 when MC RPF is configured.
         * FEC_MC_DESTINATION_W_RPF and FEC_DESTINATION_MC_RPF.
         * If egr->intf is configured, then the destination field is smaller and can save only a MC group gport in it.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "egr.port(0x%x) must be encoded as GPORT_MC.\n", egr->port);
    }
    /** Check if black hole is a valid destination*/
    if ((dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_no_black_hole_for_unknown_destination))
        && BCM_GPORT_IS_BLACK_HOLE(egr->port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Black hole is not a valid gport destination!\n");
    }
    /*
     * For WITH_ID configuration - check ID is in valid range
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_WITH_ID))
    {
        uint32 max_fec_id;

        SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));

        if (BCM_L3_ITF_VAL_GET(*if_id) > max_fec_id
            || BCM_L3_ITF_VAL_GET(*if_id) < dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC ID %d is out of range - should be bigger than %d and smaller than %d\n",
                         BCM_L3_ITF_VAL_GET(*if_id), dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit) - 1, max_fec_id);
        }

        /** Verify that the FEC index is in the correct range according to the ECMP_RANGE_OVERLAP flag */
        if (!_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP)
            && BCM_L3_ITF_VAL_GET(*if_id) < dnx_data_l3.ecmp.total_nof_ecmp_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The FEC index 0x%x is in the ECMP range (less than 0x%x) but BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP flag is not provided\n",
                         BCM_L3_ITF_VAL_GET(*if_id), dnx_data_l3.ecmp.total_nof_ecmp_get(unit));
        }
        else if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP)
                 && BCM_L3_ITF_VAL_GET(*if_id) >= dnx_data_l3.ecmp.total_nof_ecmp_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The FEC index 0x%x is not in the ECMP range (above than 0x%x) but BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP flag is provided\n",
                         BCM_L3_ITF_VAL_GET(*if_id), dnx_data_l3.ecmp.total_nof_ecmp_get(unit));
        }

        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(*if_id), &is_allocated));
        /**
         * If protection is enabled, we need to carry out different validity checks for allocated FEC.
         */
        if (DNX_FAILOVER_IS_PROTECTION_ENABLED(egr->failover_id) == TRUE)
        {
            /**
             * If FEC is primary, super FEC should already be allocated unless updating the entry.
             * If FEC is secondary, then we need to verify that the FEC hasn't been allocated.
             */
            if (DNX_FAILOVER_IS_STANDBY(egr->failover_if_id) == TRUE && is_allocated == 1
                && !_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS, "Secondary failover FEC with ID %d has already been allocated\n", *if_id);
            }
            else if (DNX_FAILOVER_IS_STANDBY(egr->failover_if_id) == FALSE && is_allocated == 0)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "Primary failover FEC with ID %d has not been allocated. Need to allocate secondary first.\n",
                             *if_id);
            }
            if (DNX_FAILOVER_IS_STANDBY(egr->failover_if_id) == TRUE)
            {
                if (L3_FEC_ID_TO_EVEN_FEC_ID(BCM_L3_ITF_VAL_GET(*if_id)) > max_fec_id
                    || L3_FEC_ID_TO_EVEN_FEC_ID(BCM_L3_ITF_VAL_GET(*if_id)) <
                    dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Primary FEC ID %d is out of range - should be bigger than %d and smaller than %d\n",
                                 L3_FEC_ID_TO_EVEN_FEC_ID(BCM_L3_ITF_VAL_GET(*if_id)),
                                 dnx_data_l3.fec.first_valid_fec_ecmp_id_get(unit) - 1, max_fec_id);
                }
            }
        }
        else
        {
            /**
             * If the FEC is allocated but REPLACE flag hasn't been provided, then an EXISTS error will be returned.
             * Otherwise, if the FEC is not allocated but REPLACE flag is provided, NOT_FOUND error will be returned.
             */
            if (is_allocated == 1 && !_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS, "FEC with ID %d has already been allocated\n", *if_id);
            }
            else if (is_allocated == 0 && _SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot update FEC with ID %d as it is not allocated \n", *if_id);
            }
        }
        /*
         * At this point, we verified that the entry exists.
         * If the entry is being updated (replace flag is provided),
         * then it needs to be verified that the user is not attempting to update the result type.
         */
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
        {

            bcm_l3_egress_t egr_old;
            dbal_enum_value_field_fec_resource_type_e super_fec_resource_type = 0;
            dbal_enum_value_field_fec_resource_type_e super_fec_new_resource_type;
            uint8 is_protected;
            uint8 is_stat_fec;
            uint8 is_stat_direct;
            is_protected = (DNX_FAILOVER_IS_PROTECTION_ENABLED(egr->failover_id)) ? TRUE : FALSE;
            is_stat_fec = (egr->stat_pp_profile != STAT_PP_PROFILE_INVALID) ? TRUE : FALSE;
            
            is_stat_direct = (is_stat_fec && (egr->stat_id == STAT_PP_USE_DIRECT_STAT_ID)) ? TRUE : FALSE;
            dnx_l3_fec_decide_super_fec_resource_type(unit, is_protected, is_stat_fec, is_stat_direct,
                                                      &super_fec_resource_type);

            SHR_IF_ERR_EXIT(dnx_algo_l3_fec_super_fec_type_get
                            (unit, BCM_L3_ITF_VAL_GET(*if_id), &super_fec_new_resource_type));
            if (super_fec_resource_type != super_fec_new_resource_type)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Cannot update result type of Super-FEC entry (two FECs that share the same line in the FEC table\n");
            }

            if (!BCM_L3_ITF_TYPE_IS_FEC(*if_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "if_id(0x%x) must be of type FEC.", *if_id);
            }

            /** Modifying protection enable/disable is forbidden */
            bcm_l3_egress_t_init(&egr_old);
            SHR_IF_ERR_EXIT(bcm_dnx_l3_egress_get(unit, *if_id, &egr_old));

            if (DNX_FAILOVER_IS_PROTECTION_ENABLED(egr->failover_id) !=
                DNX_FAILOVER_IS_PROTECTION_ENABLED(egr_old.failover_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Modifying protection enable/disable state is not allowed. "
                             "Old failover_id = %d, New failover_id = %d \r\n", egr_old.failover_id, egr->failover_id);
            }

            if (egr->failover_if_id != egr_old.failover_if_id)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Modifying protection failover_if_id is not allowed. "
                             "Old failover_if_id = %d, New failover_if_id = %d \r\n", egr_old.failover_if_id,
                             egr->failover_if_id);
            }
        }
    }
    else
    {
        /** If allocating without ID make sure that the flag BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP is not added */
        if (_SHR_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP flag cannot be used without BCM_L3_WITH_ID flag.\n");
        }
    }

    if (_SHR_IS_FLAG_SET(egr->mpls_flags, BCM_MPLS_EGRESS_LABEL_EXP_SET)
        || _SHR_IS_FLAG_SET(egr->mpls_flags, BCM_MPLS_EGRESS_LABEL_TTL_SET))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "mpls flag BCM_MPLS_EGRESS_LABEL_EXP_SET and BCM_MPLS_EGRESS_LABEL_TTL_SET are not supported.replaced by egress_qos_model");
    }
    if ((egr->egress_qos_model.egress_qos != bcmQosEgressModelUniform)
        && (egr->egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "QoS model only support uniform and pipeNextNameSpace");
    }
    if (egr->egress_qos_model.egress_qos == bcmQosEgressModelUniform)
    {
        if (egr->mpls_exp != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "egr->mpls_exp must be 0");
        }
    }
    else
    {
        if (egr->mpls_exp > dnx_data_qos.qos.packet_max_exp_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "exp value %d is out of range", egr->mpls_exp);
        }
    }
    if ((egr->egress_qos_model.egress_ttl != bcmQosEgressModelUniform)
        && (egr->egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TTL model only support uniform and pipeMyNameSpace");
    }
    if ((egr->egress_qos_model.egress_ttl == bcmQosEgressModelUniform) && (egr->mpls_ttl != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mpls_ttl must be in case ttl model is uniform");
    }

    if (_SHR_IS_FLAG_SET(egr->flags, BCM_L3_2ND_HIERARCHY) && _SHR_IS_FLAG_SET(egr->flags, BCM_L3_3RD_HIERARCHY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Two hierarchy levels were selected when a single hierarchy per FEC is supported.");
    }

    if (mc_rpf_mode != DBAL_ENUM_FVAL_MC_RPF_MODE_RESERVED)
    {
        /*
         * FEC entry with an MC RPF field for SIP-based RPF can't use the encapsulation ID field, it needs to be empty.
         */
        if (BCM_L3_ITF_VAL_GET(egr->encap_id) && mc_rpf_mode == DBAL_ENUM_FVAL_MC_RPF_MODE_USE_SIP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "You can't specify encapsulation ID when enabling SIP-based MC RPF.\r\n");
        }

        /*
         * Explicit MC RPF require a valid out LIF (intf field).
         */
        if (dnx_data_l3.fec.fer_hw_version_get(unit) == DNX_L3_FER_HW_VERSION_2)
        {
            if (!BCM_GPORT_IS_FORWARD_PORT(egr->port) && (mc_rpf_mode == DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT
                                                          || mc_rpf_mode == DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT_ECMP)
                && (BCM_L3_ITF_VAL_GET(egr->intf) == 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Explicit and Explicit_ECMP MC RPF require a valid out-LIF (bcm_l3_egress_t.intf field) if the destination is not a forward port.\r\n");
            }
        }
        else
        {
            if ((mc_rpf_mode == DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT
                 || mc_rpf_mode == DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT_ECMP) && (BCM_L3_ITF_VAL_GET(egr->intf) == 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Explicit and Explicit_ECMP MC RPF require a valid out-LIF (bcm_l3_egress_t.intf field).\r\n");
            }
        }
    }

    /** Verify protection information */
    if (DNX_FAILOVER_IS_PROTECTION_ENABLED(egr->failover_id))
    {
        /*
         * If protection is facility, verify FEC hierarchy is 3rd.
         * Otherwise, for regular protection, verify protection pointer is valid.
         */
        if (L3_EGRESS_FAILOVER_ID_IS_FACILITY(egr->failover_id) == TRUE)
        {
            /** Facility protection is only allowed in 3rd hierarchy */
            if (_SHR_IS_FLAG_SET(egr->flags, BCM_L3_3RD_HIERARCHY) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Facility protection is only allowed in 3rd hierarchy FEC.\r\n");
            }
        }
        /** regular protection -> verify protection pointer is valid */
        else
        {
            /** Convert FEC type to failover type and hierarchy*/
            dnx_l3_egress_fec_type_to_failover_type_convert(unit, egr->flags, &failover_type, &fec_hierarchy);

            /**  verify given protection pointer is valid */
            SHR_IF_ERR_EXIT(dnx_failover_id_valid_verify(unit, failover_type, fec_hierarchy, egr->failover_id));
        }

        /** Working (primary) fec validation */
        if (DNX_FAILOVER_IS_STANDBY(egr->failover_if_id) == FALSE)
        {
            fec_id = BCM_L3_ITF_VAL_GET(egr->failover_if_id);

            /** If working FEC, verify that the failover FEC ID is already allocated */
            SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, fec_id, &is_allocated));
            if (is_allocated == 0)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "FEC with ID %d provided as failover ID has not been allocated\n",
                             fec_id);
            }
            /** Verify that the provided fec in failover_if_id is odd (secondary) */
            if (L3_FEC_ID_IS_ODD_FEC_ID(fec_id) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "FEC ID must be odd for standby FEC.\r\n");
            }
        }

        /** if fec id is provided (WITH_ID allocation), verify fec id is odd for standby FEC and even for working FEC */
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_WITH_ID))
        {

            fec_id = BCM_L3_ITF_VAL_GET(*if_id);

            if (DNX_FAILOVER_IS_STANDBY(egr->failover_if_id) == TRUE) /**  Standby fec */
            {
                if (L3_FEC_ID_IS_ODD_FEC_ID(fec_id) == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "FEC ID must be odd for standby FEC.\r\n");
                }
            }
            /** Working fec */
            else
            {
                if (L3_FEC_ID_IS_EVEN_FEC_ID(fec_id) == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "FEC ID must be even for working FEC.\r\n");
                }
            }
        }
    }

    if (BCM_L3_ITF_VAL_GET(egr->encap_id) != 0 && (BCM_L3_ITF_VAL_GET(egr->intf) == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Creating FEC without interface ID but with encapsulation ID set is forbidden.\n");
    }

    if (BCM_L3_ITF_TYPE_IS_LIF(egr->encap_id) && BCM_L3_ITF_TYPE_IS_LIF(egr->intf))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEC with 2 out LIFs format is not supported.\n");
    }
    /** Verify that if the intf field has a LIF, it is encoded as such. */
    if ((BCM_L3_ITF_VAL_GET(egr->intf) >= dnx_data_l3.rif.nof_rifs_get(unit)) && !BCM_L3_ITF_TYPE_IS_LIF(egr->intf))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The intf value 0x%04x is in the LIF range (above 0x%04x) but it's lacking the LIF type encoding.\n",
                     egr->intf, dnx_data_l3.rif.nof_rifs_get(unit));
    }
    /** Verify that if the intf field holds a RIF, it is encoded as such. */
    if ((BCM_L3_ITF_VAL_GET(egr->intf) < dnx_data_l3.rif.nof_rifs_get(unit)) && BCM_L3_ITF_TYPE_IS_LIF(egr->intf))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The intf value 0x%04x is in the RIF range (below 0x%04x) but it's lacking the RIF type encoding.\n",
                     egr->intf, dnx_data_l3.rif.nof_rifs_get(unit));
    }

    /** verify Hierarchical TM-Flow (HTM) */
    if (egr->hierarchical_gport)
    {
        if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(egr->hierarchical_gport))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "egr.hierarchical_gport(0x%x) must be encoded with _SHR_GPORT_TYPE_UCAST_QUEUE_GROUP.\n",
                         egr->hierarchical_gport);
        }
        if (_SHR_IS_FLAG_SET(egr->flags, BCM_L3_3RD_HIERARCHY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "configuring egr->hierarchical_gport(HTM) not supported if 3RD_HIERARCHY set.\n");
        }
        if (BCM_L3_ITF_TYPE_IS_LIF(egr->encap_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "configuring 2nd outlif through egr.encap_id(0x%x) not supported for HTM.\n",
                         egr->encap_id);
        }
        /*
         * protection not supported for HTM
         */
        
        if (DNX_FAILOVER_IS_PROTECTION_ENABLED(egr->failover_id) == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "configuring protection egr.failover_id(0x%x) not supported for HTM.\n",
                         egr->failover_id);
        }
        /*
         * destination must be of type FEC
         */
        if (!BCM_GPORT_IS_FORWARD_PORT(egr->port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "egr.port(0x%x) must be encoded as GPORT_FORWARD_PORT.\n", egr->port);
        }
    }
    if (dnx_data_l3.fec.fer_hw_version_get(unit) == DNX_L3_FER_HW_VERSION_1)
    {
        /*
         * Statistics check
         */
        if (egr->stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            if ((BCM_L3_ITF_VAL_GET(egr->intf) != 0) && (BCM_L3_ITF_VAL_GET(egr->encap_id) != 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "stat_pp can only be associated with fecs which point to 1 lif");
            }
            /*
             * Protection
             */
            if (egr->failover_id)
            {
                bcm_stat_pp_profile_info_t stat_pp_profile_info;

                SHR_IF_ERR_EXIT(bcm_dnx_stat_pp_profile_get(unit, egr->stat_pp_profile, &stat_pp_profile_info));

                if (!stat_pp_profile_info.is_protection_fec)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "For protection fec, stat pp profile must be created with is_protection_fec set");
                }

                /*
                 * Backup fec - stat id will be primary stat id - 1
                 */
                if ((!egr->failover_if_id) && (egr->stat_id != 0))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "For protection fec, backup stat id must be 0");
                }
            }

            if (DNX_FAILOVER_IS_STANDBY(egr->failover_if_id) == FALSE)
            {
                SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                                (unit, _SHR_CORE_ALL, egr->stat_id, egr->stat_pp_profile,
                                 bcmStatCounterInterfaceIngressReceivePp));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for bcm_l3_egress_allocation_get
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] flags - Control flags
 *   \param [in] egr - Egress structure that describes an entry.
 *   \param [in] nof_members - Number of members
 *   \param [in] if_id - ID of the entry.
 *
 * \return
 *  shr_error_e - Error return value
 */
shr_error_e
dnx_l3_egress_allocation_get_verify(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    uint32 nof_members,
    bcm_if_t * if_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(if_id, _SHR_E_PARAM, "output L3 FEC ID");
    /** Verify input flags */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_EGRESS_ONLY) || !_SHR_IS_FLAG_SET(flags, BCM_L3_INGRESS_ONLY))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "bcm_l3_egress_allocation_get API is supported only with BCM_L3_INGRESS_ONLY flag.");
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_WITH_ID))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "bcm_l3_egress_allocation_get API is only supported for without ID case.");
    }

    /** Make sure that the number of members required is between 1 and the maximum possible unique members */
    if (nof_members > dnx_data_l3.ecmp.max_group_size_get(unit) || nof_members < 1)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "FEC number of members required value should be between 1 and %d.\n",
                     dnx_data_l3.ecmp.max_group_size_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get allocation info
 *
 * \param [in] unit - Unit ID
 * \param [in] flags - flags
 * \param [in] egr - l3 egress info
 * \param [out] is_alloc_required - is FEC allocation required
 * \param [out] is_protected - is protection enabled (requires super fec)
 * \param [out] is_stat_fec - are stats enabled
 * \param [out] is_stat_direct - are direct stats enabled (stat_id = fec_id)
 *
 * \return
 *    shr_error_e - Error return value
 */
static shr_error_e
dnx_l3_egress_create_fec_allocate_info_get(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    uint8 *is_alloc_required,
    uint8 *is_protected,
    uint8 *is_stat_fec,
    uint8 *is_stat_direct)
{

    SHR_FUNC_INIT_VARS(unit);

    /** defaults */
    *is_protected = FALSE;
    *is_alloc_required = TRUE;
    *is_stat_fec = FALSE;

    /** if protection is enabled, special allocation handling is required */
    if (DNX_FAILOVER_IS_PROTECTION_ENABLED(egr->failover_id))
    {
        /** protection is enabled => need to allocate super FEC */
        *is_protected = TRUE;

        /*
         * If current FEC is working(primary) then no allocation is required since
         * the super FEC was already allocated
         */
        if (DNX_FAILOVER_IS_STANDBY(egr->failover_if_id) == FALSE)
        {
            /** no need for allocation */
            *is_alloc_required = FALSE;
        }
    }
    if (egr->stat_pp_profile != STAT_PP_PROFILE_INVALID)
    {
        *is_stat_fec = TRUE;
        
        *is_stat_direct = ((*is_stat_fec) && (egr->stat_id == STAT_PP_USE_DIRECT_STAT_ID)) ? TRUE : FALSE;
    }

    SHR_FUNC_EXIT;
}

/**
* \brief
*   Allocation function for bcm_l3_egress_create with BCM_L3_INGRESS_ONLY flag.
*   Allocats FEC index inside FEC table.
*   Allocated index will be returned inside if_id parameter.
*
*    \param [in] unit - Relevant unit.
*    \param [in] flags - Similar to bcm_l3_egress_create api input
*    \param [in] egr - Similar to bcm_l3_egress_create api input
*    \param [in] if_id - Similar to bcm_l3_egress_create api input
* \return
*    shr_error_e - Error return value
*****************************************************/
shr_error_e
dnx_l3_egress_create_fec_allocate(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    bcm_if_t * if_id)
{
    int fec_index = 0;
    uint32 alloc_flags = 0;
    uint32 hierarchy = 0;
    uint32 direction = 0;
    mdb_physical_table_e fec_db;
    dbal_enum_value_field_fec_resource_type_e super_fec_resource_type = 0;
    uint8 is_alloc_required, is_protected = 0;
    uint8 is_stat_fec = 0;
    uint8 is_stat_direct = 0;
    uint32 range_start;
    uint32 range_size;

    SHR_FUNC_INIT_VARS(unit);

    hierarchy = DNX_L3_FEC_HIERARCHY_FROM_FLAGS_GET(egr->flags);
    direction = DNX_L3_FEC_DIR_FROM_FLAGS_GET(egr->flags2);
    /** Get allocation info */
    SHR_IF_ERR_EXIT(dnx_l3_egress_create_fec_allocate_info_get
                    (unit, flags, egr, &is_alloc_required, &is_protected, &is_stat_fec, &is_stat_direct));

    /** Allocate fec if required */
    if (is_alloc_required == TRUE)
    {
        /**Get database according to hierarchy and direction*/
        SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fec_db.get(unit, direction, hierarchy, &fec_db));
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_WITH_ID))
        {
            fec_index = BCM_L3_ITF_VAL_GET(*if_id);
            alloc_flags |= DNX_ALGO_L3_FEC_WITH_ID;
            /** allocation should be aligned to super fec start index (even ID) */
            if (is_protected)
            {
                fec_index = L3_FEC_ID_TO_EVEN_FEC_ID(fec_index);
            }
            SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_get(unit, fec_db, &range_start, &range_size));
            if ((fec_index < range_start) || (fec_index >= range_start + range_size))
            {
                if (dnx_data_l3.fec.fer_hw_version_get(unit) == DNX_L3_FER_HW_VERSION_1)
                {
                    SHR_ERR_EXIT(BCM_E_PARAM,
                                 "The requested FEC ID %d isn't in the hierarchy %d range, allowed range is (%d - %d)."
                                 "\n", fec_index, hierarchy, range_start, range_start + range_size - 1);
                }
                else
                {
                }
            }

        }
        dnx_l3_fec_decide_super_fec_resource_type(unit, is_protected, is_stat_fec, is_stat_direct,
                                                  &super_fec_resource_type);
        /** Allocate FEC (fec_index) */
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocate(unit, &fec_index, alloc_flags, fec_db, super_fec_resource_type));
    }
    /*
     * When protection is enabled, a super fec is allocated containing the working and standby fecs.
     * This section returns the correct fec ID within the super fec (either the working or standby)
     */
    if (is_protected)
    {
        /** standby fec */
        if (DNX_FAILOVER_IS_STANDBY(egr->failover_if_id))
        {
            fec_index = L3_FEC_ID_TO_ODD_FEC_ID(fec_index);
        }
        /** working fec */
        else
        {
            /** fec_index is provided in egr->failover_if_id  */
            fec_index = BCM_L3_ITF_VAL_GET(egr->failover_if_id);
            fec_index = L3_FEC_ID_TO_EVEN_FEC_ID(fec_index);
        }
    }

    /*
     * Return Allocated ID inside if_id
     */
    BCM_L3_ITF_SET(*if_id, BCM_L3_ITF_TYPE_FEC, fec_index);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Deallocate an L3 egress FEC sw resource
*
* \param [in] unit - Relevant unit.
* \param [in] fec_index - l3 intf id for fec.
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
*/

shr_error_e
dnx_l3_egress_destroy_fec_deallocate(
    int unit,
    int fec_index)
{
    uint8 is_protected;
    int fec_per_super_fec = 2;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_protection_state_get(unit, fec_index, &is_protected));

    /*
     * In case protection is enabled, we will free the pair of FECs together when freeing the stand-by FEC
     * (the one which is odd in value).
     * Otherwise a single FEC ID will be freed.
     */
    if (is_protected)
    {
        /*
         * In case the FEC is the stand-by one (it is odd in value), then we will free the pair of FECs
         * by providing the first one in the pair.
         */
        if (fec_index % fec_per_super_fec == 1)
        {
            /** FEC is stand-by */
            fec_index = L3_FEC_ID_TO_EVEN_FEC_ID(fec_index);
            SHR_IF_ERR_EXIT(dnx_algo_l3_fec_deallocate(unit, fec_index));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_deallocate(unit, fec_index));
    }

exit:
    SHR_FUNC_EXIT;
}

void
dnx_fill_mpls_label_entry_with_l3_egr_info(
    int unit,
    bcm_l3_egress_t * egr,
    bcm_mpls_egress_label_t * label_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((label_entry->action = egr->mpls_action) == BCM_MPLS_EGRESS_ACTION_PUSH)
    {
        label_entry->egress_qos_model.egress_qos = egr->egress_qos_model.egress_qos;
        label_entry->egress_qos_model.egress_ttl = egr->egress_qos_model.egress_ttl;
        label_entry->qos_map_id = egr->qos_map_id;
        label_entry->exp = egr->mpls_exp;
        label_entry->ttl = egr->mpls_ttl;
        label_entry->pkt_pri = egr->mpls_pkt_pri;
        label_entry->pkt_cfi = egr->mpls_pkt_cfi;
    }

    SHR_VOID_FUNC_EXIT;
}

void
dnx_fill_l3_egr_info_with_mpls_label_entry(
    int unit,
    bcm_l3_egress_t * egr,
    bcm_mpls_egress_label_t * label_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    if (egr->mpls_action == BCM_MPLS_EGRESS_ACTION_PUSH)
    {
        egr->egress_qos_model.egress_qos = label_entry->egress_qos_model.egress_qos;
        egr->egress_qos_model.egress_ttl = label_entry->egress_qos_model.egress_ttl;
        egr->qos_map_id = label_entry->qos_map_id;
        egr->mpls_exp = label_entry->exp;
        egr->mpls_ttl = label_entry->ttl;
        egr->mpls_pkt_pri = label_entry->pkt_pri;
        egr->mpls_pkt_cfi = label_entry->pkt_cfi;
    }

    SHR_VOID_FUNC_EXIT;
}

/*See l3_fec.h for prototype define*/
shr_error_e
dnx_l3_egress_fec_info_get(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t * egr)
{
    l3_fec_entry_info_t fec_entry_info;
    dbal_enum_value_field_irpp_fec_entry_format_e fec_entry;
    uint32 hierarchy = 0;
    dnx_mpls_encap_t encap_info;
    bcm_mpls_egress_label_t label_array[2];
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * Verify ingress (FEC)
     */
    L3_FEC_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_egress_fec_get_verify(unit, intf));

    sal_memset(&fec_entry_info, 0, sizeof(l3_fec_entry_info_t));

    fec_entry_info.fec_id = BCM_L3_ITF_VAL_GET(intf);

    /*
     * a. Always request the 'hit bit.
     * b. Convert input bits BCM_L3_HIT_CLEAR to internal bits on 'fec_entry_info'.
     */
    fec_entry_info.flags |= L3_FEC_ENTRY_INFO_GET_HIT_BIT;
    if (egr->flags & BCM_L3_HIT_CLEAR)
    {
        fec_entry_info.flags |= L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT;
    }
    SHR_IF_ERR_EXIT(dnx_l3_egress_fec_entry_get(unit, &fec_entry_info));
    /*
     * Convert the internal output bit L3_FEC_ENTRY_INFO_HIT_BIT_VALUE, to the BCM_L3_FLAGS2_HIT_BIT_VALUE bit.
     */
    if (fec_entry_info.flags & L3_FEC_ENTRY_INFO_HIT_BIT_VALUE)
    {
        egr->flags |= BCM_L3_HIT;
    }
    else
    {
        egr->flags &= ~BCM_L3_HIT;
    }
    /*
     * Fill port from destination
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, fec_entry_info.destination, &egr->port));

    if (fec_entry_info.glob_out_lif_2nd)
    {
        BCM_L3_ITF_SET(egr->encap_id, BCM_L3_ITF_TYPE_LIF, fec_entry_info.glob_out_lif_2nd);
        /*
         * For 2 pointers entry, 1st out LIF always be out-RIF
         */
        BCM_L3_ITF_SET(egr->intf, BCM_L3_ITF_TYPE_RIF, fec_entry_info.glob_out_lif);
    }
    else if (fec_entry_info.glob_out_lif)
    {
        if (fec_entry_info.glob_out_lif < dnx_data_l3.rif.nof_rifs_get(unit))
        {
            BCM_L3_ITF_SET(egr->intf, BCM_L3_ITF_TYPE_RIF, fec_entry_info.glob_out_lif);
        }
        else
        {
            BCM_L3_ITF_SET(egr->intf, BCM_L3_ITF_TYPE_LIF, fec_entry_info.glob_out_lif);
        }
    }
    /** BCM_L3_ENCAP_SPACE_OPTIMIZED is relevant only in Jericho mode */
    /*
     * Jericho mode beginning
     * {
     */
    else if ((fec_entry_info.eei) && (_SHR_IS_FLAG_SET(fec_entry_info.flags, L3_FEC_ENTRY_INFO_EEI_ENC_POINTER)))
    {
        uint8 tmp;
        SHR_IF_ERR_EXIT(dnx_mpls_eei_enc_pointer_information_retrieve(unit, fec_entry_info.eei, &egr->intf, &tmp));
        egr->flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }
    else if ((fec_entry_info.eei) && (_SHR_IS_FLAG_SET(fec_entry_info.flags, L3_FEC_ENTRY_INFO_EEI_PUSH_COMMAND)))
    {
        int push_cmd;
        dnx_mpls_encap_t_init(unit, &encap_info);
        bcm_mpls_egress_label_t_init(&label_array[0]);
        bcm_mpls_egress_label_t_init(&label_array[1]);
        encap_info.label_array = label_array;

        SHR_IF_ERR_EXIT(dnx_mpls_eei_push_information_retrieve(unit, fec_entry_info.eei, &egr->mpls_label, &push_cmd));
        SHR_IF_ERR_EXIT(dnx_mpls_encap_get_from_push_cmd(unit, push_cmd, &encap_info));

        egr->mpls_action = BCM_MPLS_EGRESS_ACTION_PUSH;
        dnx_fill_l3_egr_info_with_mpls_label_entry(unit, egr, label_array);

    }
    /*
     * Jericho mode end
     * }
     */
    else if (fec_entry_info.eei)
    {
        uint32 eei_val;
        dbal_fields_e dbal_eei_type;
        uint32 profile = DNX_QOS_INITIAL_MAP_ID;

        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_EEI, fec_entry_info.eei, &dbal_eei_type, &eei_val));
        switch (dbal_eei_type)
        {
            case DBAL_FIELD_EEI_MPLS_POP_COMMAND:
            {
                dbal_enum_value_field_model_type_e qos_model;
                dbal_enum_value_field_model_type_e ttl_model;
                dbal_enum_value_field_jr_fwd_code_e upper_layer_type;
                egr->mpls_action = BCM_MPLS_EGRESS_ACTION_PHP;
                SHR_IF_ERR_EXIT(dnx_mpls_eei_php_information_retrieve
                                (unit, eei_val, &egr->qos_map_id, &qos_model, &ttl_model, &upper_layer_type));

                if (qos_model == DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM)
                {
                    egr->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
                }
                else
                {
                    egr->egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
                }

                if (ttl_model == DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM)
                {
                    egr->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
                }
                else
                {
                    egr->egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
                }
                break;
            }
            case DBAL_FIELD_EEI_MPLS_MULTI_SWAP:
            {
                uint32 remark_profile;
                uint32 label;

                if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
                {
                    egr->mpls_action = BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH;
                }
                else
                {
                    egr->mpls_action = BCM_MPLS_EGRESS_ACTION_SWAP;
                }

                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                (unit, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, DBAL_FIELD_SWAP_QOS_PROFILE,
                                 &remark_profile, &eei_val));
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                (unit, DBAL_FIELD_EEI_MPLS_MULTI_SWAP, DBAL_FIELD_MPLS_LABEL, &label, &eei_val));
                egr->mpls_label = label;

                profile = remark_profile - DBAL_ENUM_FVAL_SWAP_QOS_PROFILE_DEFAULT;
                /** EEI identifier 1:1 mapping qos profile*/
                if (profile != DNX_QOS_INITIAL_MAP_ID)
                {
                    egr->qos_map_id = profile;
                    DNX_QOS_REMARK_MAP_SET(egr->qos_map_id);
                    DNX_QOS_EGRESS_MAP_SET(egr->qos_map_id);
                }
                break;
            }
            case DBAL_FIELD_EEI_MPLS_SWAP_COMMAND:
            {
                if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
                {
                    egr->mpls_action = BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH;
                }
                else
                {
                    egr->mpls_action = BCM_MPLS_EGRESS_ACTION_SWAP;
                }
                egr->mpls_label = eei_val;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "EEI identifier %d is not supported", dbal_eei_type);
            }
        }
    }

    SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get(unit, BCM_L3_ITF_VAL_GET(intf), &hierarchy));

    egr->flags |= L3_FEC_FLAGS_FROM_HIERARCHY_GET(hierarchy);

    /*
     * Convert the FEC dbal field result type into FEC enum result type
     */
    SHR_IF_ERR_EXIT(dnx_l3_fec_dbal_fec_entry_type_convert(unit, fec_entry_info.fec_entry_type, &fec_entry));

    if (dnx_data_l3.fec.fec_result_type_fields_get(unit, fec_entry)->has_mc_rpf)
    {
        egr->flags |= L3_EGRESS_RPF_FLAG_GET(fec_entry_info.mc_rpf_mode);
    }
    egr->flags2 |= ((BCM_L3_ITF_VAL_GET(intf)) < dnx_data_l3.ecmp.total_nof_ecmp_get(unit)) ?
        BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP : 0;

    if (dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_separate_fwd_rpf_dbs))
    {
        if (fec_entry_info.direction == DNX_L3_FEC_DIRECTION_FWD)
        {
            egr->flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }
        else
        {
            egr->flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
        }
        egr->flags |= L3_FEC_FLAGS_FROM_HIERARCHY_GET(fec_entry_info.hierarchy);
    }

    /** Fill protection information  */
    if (fec_entry_info.protection_enable)
    {
        /** facility protection */
        if (fec_entry_info.protection_ptr == dnx_data_failover.path_select.fec_facility_protection_get(unit))
        {
            egr->failover_id = BCM_FAILOVER_ID_LOCAL;
        }
        else
        {
            BCM_FAILOVER_SET(egr->failover_id, fec_entry_info.protection_ptr, BCM_FAILOVER_TYPE_FEC);
        }

        /** For primary FEC we need to fill in the failover_if_id */
        if (L3_FEC_ID_IS_EVEN_FEC_ID(BCM_L3_ITF_VAL_GET(intf)))
        {
            egr->failover_if_id = L3_FEC_ID_TO_ODD_FEC_ID(intf);
        }
    }
    /** protection is disabled */
    else
    {
        egr->failover_id = DNX_FAILOVER_PROTECTION_DISABLE_VALUE;
    }

    if (fec_entry_info.hierarchical_gport)
    {
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(egr->hierarchical_gport, BCM_CORE_ALL,
                                                     fec_entry_info.hierarchical_gport);
    }

    egr->stat_id = fec_entry_info.stat_obj_id;
    egr->stat_pp_profile = fec_entry_info.stat_obj_cmd;

exit:
    SHR_FUNC_EXIT;

}

/*See l3_fec.h for prototype define*/
shr_error_e
dnx_l3_egress_fec_info_delete(
    int unit,
    bcm_if_t intf)
{
    int fec_index;
    int system_headers_mode;
    l3_fec_entry_info_t fec_entry_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify ingress (FEC)
     */
    L3_FEC_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_egress_fec_destroy_verify(unit, intf));

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    fec_index = BCM_L3_ITF_VAL_GET(intf);

    /*
     * Jericho mode beginning
     * push profile process
     * Push profile should be released, and HW table should cleared if this is the last
     * reference.
     * {
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        int push_cmd;
        uint32 label;

        sal_memset(&fec_entry_info, 0, sizeof(l3_fec_entry_info_t));
        fec_entry_info.fec_id = BCM_L3_ITF_VAL_GET(intf);
        SHR_IF_ERR_EXIT(dnx_l3_egress_fec_entry_get(unit, &fec_entry_info));
        if (_SHR_IS_FLAG_SET(fec_entry_info.flags, L3_FEC_ENTRY_INFO_EEI_PUSH_COMMAND))
        {
            SHR_IF_ERR_EXIT(dnx_mpls_eei_push_information_retrieve(unit, fec_entry_info.eei, &label, &push_cmd));
            SHR_IF_ERR_EXIT(dnx_mpls_tunnel_delete_push_entry_from_eei(unit, push_cmd));
        }
    }
    /*
     * Jericho mode end
     * }
     */

    SHR_IF_ERR_EXIT(dnx_l3_egress_fec_entry_delete(unit, fec_index));

    SHR_IF_ERR_EXIT(dnx_l3_egress_destroy_fec_deallocate(unit, fec_index));

exit:
    SHR_FUNC_EXIT;

}

/*See l3_fec.h for prototype define*/
shr_error_e
dnx_l3_egress_create_fec(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    bcm_if_t * if_id)
{

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify ingress (FEC)
     */
    L3_FEC_INVOKE_VERIFY_IF_ENABLE(unit, dnx_l3_egress_create_fec_verify(unit, flags, egr, if_id));
    /*
     * SW Allocations
     */
    /** Allocate ingress (FEC) */
    if (!_SHR_IS_FLAG_SET(flags, BCM_L3_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_l3_egress_create_fec_allocate(unit, flags, egr, if_id));
    }

    /*
     * Write to HW tables
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_create_fec_hw_write(unit, flags, egr, if_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The aim of this API is to retrieve the first valid FEC index of a number of free consecutive indexes.
 * \param [in] unit - Relevant unit.
 * \param [in] flags -
 *      Similar to bcm_l3_egress_create api input
 * \param [in] egr - Structure that describes a valid FEC entry.
 * \param [in] nof_members - Number of members needed for future allocation.
 * \param [out] if_id - The index of the first FEC of the range.
 * \return
 *    shr_error_e - Error return value
 */
shr_error_e
bcm_dnx_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    uint32 nof_members,
    bcm_if_t * if_id)
{
    mdb_physical_table_e db;
    dbal_enum_value_field_hierarchy_level_e hierarchy;
    dbal_enum_value_field_fec_resource_type_e fec_resource_type = 0;
    uint8 is_protected;
    uint8 is_stat_fec;
    uint8 is_stat_direct;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_l3_egress_allocation_get_verify(unit, flags, egr, nof_members, if_id));
    hierarchy = DNX_L3_FEC_HIERARCHY_FROM_FLAGS_GET(egr->flags);
    is_protected = (DNX_FAILOVER_IS_PROTECTION_ENABLED(egr->failover_id)) ? TRUE : FALSE;
    is_stat_fec = (egr->stat_pp_profile != STAT_PP_PROFILE_INVALID) ? TRUE : FALSE;
    
    is_stat_direct = (is_stat_fec && (egr->stat_id == STAT_PP_USE_DIRECT_STAT_ID)) ? TRUE : FALSE;

    dnx_l3_fec_decide_super_fec_resource_type(unit, is_protected, is_stat_fec, is_stat_direct, &fec_resource_type);

    SHR_IF_ERR_EXIT(algo_l3_db.fec_db_info.fec_db.get(unit, DNX_L3_FEC_DIRECTION_FWD, hierarchy, &db));
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_get(unit, nof_members, db, fec_resource_type, if_id));

exit:
    SHR_FUNC_EXIT;
}
