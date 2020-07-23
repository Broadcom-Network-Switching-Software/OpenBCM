/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file field_range.c
 *
 * Field range procedures for DNX.
 *
 * Management for range qualifier.
 */
#include <shared/bsl.h>
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/**
 * Include files.
 * {
 */
#include <bcm_int/dnx/field/field_range.h>
#include <bcm/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <include/soc/drv.h>
#include <bcm_int/dnx/field/field_map.h>
#include <src/bcm/dnx/field/map/field_map_local.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_range_access.h>

/**
 * }Include files
 */
/**
 * Range Types array.
 * Used in the mapping function dnx_field_range_type_text.
 * {
 */
const char *dnx_field_range_types_names[DNX_FIELD_RANGE_TYPE_NOF] = {
    [DNX_FIELD_RANGE_TYPE_L4_SRC_PORT] = "L4 Source Port Range",
    [DNX_FIELD_RANGE_TYPE_L4_DST_PORT] = "L4 Destination Port Range",
    [DNX_FIELD_RANGE_TYPE_OUT_LIF] = "Out LIF Range",
    [DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE] = "Packet Header Size Range",
    [DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE] = "Extended L4 OPs Packet Header Size Range",
    [DNX_FIELD_RANGE_TYPE_IN_TTL] = "In Ttl Range",
    [DNX_FIELD_RANGE_TYPE_FFC1_LOW] = "UserDefined1 Low Range",
    [DNX_FIELD_RANGE_TYPE_FFC1_HIGH] = "UserDefined1 High Range",
    [DNX_FIELD_RANGE_TYPE_FFC2_LOW] = "UserDefined2 Low Range",
    [DNX_FIELD_RANGE_TYPE_FFC2_HIGH] = "UserDefined2 High Range",
};
/**
 * See header in field_range.h
 */
shr_error_e
dnx_field_range_info_t_init(
    int unit,
    dnx_field_range_info_t * range_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");
    /**
     * Make the range and the min and max value invalid.
     */
    range_info_p->range_type = DNX_FIELD_RANGE_TYPE_INVALID;
    range_info_p->min_val = DNX_FIELD_RANGE_VALUE_INVALID;
    range_info_p->max_val = DNX_FIELD_RANGE_VALUE_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_range.h
 */
char *
dnx_field_range_type_text(
    dnx_field_range_type_e range_type)
{
    char *range_type_name;
    if ((range_type < DNX_FIELD_RANGE_TYPE_FIRST) || (range_type >= DNX_FIELD_RANGE_TYPE_NOF))
    {
        range_type_name = "Invalid Range Type";
    }
    else if (dnx_field_range_types_names[range_type] == NULL)
    {
        range_type_name = "Unnamed Range Type";
    }
    else
    {
        range_type_name = (char *) dnx_field_range_types_names[range_type];
    }

    return range_type_name;
}
/**
 * \brief
 *  Verify all parameters.
 * \param [in] unit       - Device ID
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] range_id   - Identifier of the range: User-provided range ID
 * \param [in] range_info_p - The range_info struct.
 *            Using the range_type to verify the stage, range_id, min and max val for different types.
 * \param [in] is_for_set - Shows if the verify function is used in set or in get function.
 *                            In get function we need to check only the field_stage and the range_id!
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_range_verify(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 range_id,
    dnx_field_range_info_t * range_info_p,
    uint8 is_for_set)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");

    /**
     * Switching between all range types and verify the parameters
     */
    switch (range_info_p->range_type)
    {
        case DNX_FIELD_RANGE_TYPE_L4_SRC_PORT:
        case DNX_FIELD_RANGE_TYPE_L4_DST_PORT:
            /*
             * Range ID For Extended L4 OPS under IPMF1 stage was already verified
             */
            if (!dnx_data_field.features.extended_l4_ops_get(unit) || field_stage == DNX_FIELD_STAGE_EPMF)
            {
                /**
                 * For L4 Ops the maximum range_id must be DNX_DATA_MAX_FIELD_BASE_IPMF1_NOF_L4_OPS_RANGES_LEGACY for IPMF1 and IPMF2
                 * or DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_L4_OPS_RANGES_LEGACY for EPMF.
                 */
                if (!DNX_FIELD_RANGE_L4_RANGE_ID_VALID(unit, field_stage, range_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " The RANGE_ID: %d is bigger than DNX_DATA_MAX_FIELD_IPMF1_NOF_L4_OPS_RANGES_LEGACY/DNX_DATA_MAX_FIELD_EPMF_NOF_RANGES_FOR_L4_OPS! The range for L4 Ports is [0-%d]! \n",
                                 range_id, DNX_DATA_MAX_FIELD_BASE_IPMF1_NOF_L4_OPS_RANGES_LEGACY);
                }
            }
            /**
             * The range [min_val,max_val] is being checked with the MACRO DNX_FIELD_RANGE_L4_RANGE_VALID.
             * We check it only if the verify is used for set.
             */
            if (is_for_set == DNX_FIELD_RANGE_IS_FOR_SET)
            {
                if (!DNX_FIELD_RANGE_L4_RANGE_VALID(range_info_p->min_val, range_info_p->max_val))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "The range for L4 Ops Range qualifier must be [DNX_FIELD_RANGE_QUALS_MIN_VAL-DNX_FIELD_RANGE_L4_OPS_MAX_VAL]. Range given is [%d - %d] \n",
                                 range_info_p->min_val, range_info_p->max_val);
                }
            }
            break;
        case DNX_FIELD_RANGE_TYPE_OUT_LIF:
            /**
             * For IPMF1,IPMF2 the range_id must be smaller then DNX_DATA_MAX_FIELD_IPMF1_NOF_RANGES_FOR_OUT_LIF.
             * For IPMF3 the range_id must be smaller then DNX_DATA_MAX_FIELD_IPMF3_NOF_RANGES_FOR_OUT_LIF.
             * The range [min_val,max_val] is being checked with the MACRO DNX_FIELD_RANGE_OUT_LIF_RANGE_VALID.
             */
            if (field_stage == DNX_FIELD_STAGE_IPMF1 || field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                if (!DNX_FIELD_RANGE_OUT_LIF_IPMF1_RANGE_ID_VALID(unit, range_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " The RANGE_ID: %d is bigger then DNX_DATA_MAX_FIELD_IPMF1_NOF_RANGES_FOR_OUT_LIF! \n",
                                 range_id);
                }
                if (is_for_set == DNX_FIELD_RANGE_IS_FOR_SET)
                {
                    if (!DNX_FIELD_RANGE_OUT_LIF_RANGE_VALID(range_info_p->min_val, range_info_p->max_val))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     " The range for OUT LIFF Range qualifier must be [DNX_FIELD_RANGE_QUALS_MIN_VAL-DNX_FIELD_RANGE_OUT_LIF_MAX_VAL]. Range given is [%d - %d]\n",
                                     range_info_p->min_val, range_info_p->max_val);
                    }
                }
            }
            else if (field_stage == DNX_FIELD_STAGE_IPMF3)
            {
                if (!DNX_FIELD_RANGE_OUT_LIF_IPMF3_RANGE_ID_VALID(unit, range_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " The RANGE_ID: %d is bigger then DNX_DATA_MAX_FIELD_IPMF3_NOF_RANGES_FOR_OUT_LIF! \n",
                                 range_id);
                }
                if (is_for_set == DNX_FIELD_RANGE_IS_FOR_SET)
                {
                    if (!DNX_FIELD_RANGE_OUT_LIF_RANGE_VALID(range_info_p->min_val, range_info_p->max_val))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     " The range for OUT LIF Range qualifier must be [DNX_FIELD_RANGE_QUALS_MIN_VAL-DNX_FIELD_RANGE_OUT_LIF_MAX_VAL]. Range given is [%d - %d]\n",
                                     range_info_p->min_val, range_info_p->max_val);
                    }
                }
            }
            break;
        case DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE:
            /**
             * For Packet Header the stage must be IPMF1 or IPMF2.
             * For IPMF1,IPMF2 the range_id must be smaller then DNX_DATA_MAX_FIELD_IPMF1_NOF_PKT_HDR_RANGES.
             * The range [min_val,max_val] is being checked with the MACRO DNX_FIELD_RANGE_PKT_HDR_RANGE_VALID.
             */
            if (field_stage == DNX_FIELD_STAGE_IPMF1 || field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                if (!DNX_FIELD_RANGE_PKT_HDR_RANGE_ID_VALID(unit, range_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 " The RANGE_ID: %d is bigger then DNX_DATA_MAX_FIELD_IPMF1_NOF_PKT_HDR_RANGES! \n",
                                 range_id);
                }
                if (is_for_set == DNX_FIELD_RANGE_IS_FOR_SET)
                {
                    if (!DNX_FIELD_RANGE_PKT_HDR_RANGE_VALID(range_info_p->min_val, range_info_p->max_val))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     " The range for PKT HDR Range qualifier must be [DNX_FIELD_RANGE_QUALS_MIN_VAL-255]. Range given is [%d - %d]\n",
                                     range_info_p->min_val, range_info_p->max_val);
                    }
                }
            }
            break;
        case DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE:
        case DNX_FIELD_RANGE_TYPE_IN_TTL:
        case DNX_FIELD_RANGE_TYPE_FFC1_LOW:
        case DNX_FIELD_RANGE_TYPE_FFC1_HIGH:
        case DNX_FIELD_RANGE_TYPE_FFC2_LOW:
        case DNX_FIELD_RANGE_TYPE_FFC2_HIGH:
            if (!dnx_data_field.features.extended_l4_ops_get(unit)
                || (field_stage != DNX_FIELD_STAGE_IPMF1 && field_stage != DNX_FIELD_STAGE_IPMF2))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Range type %s only usable with Extended L4 OPs under IPMF1 and IPMF2 stages only\n",
                             dnx_field_range_type_text(range_info_p->range_type));
            }
            /**
             * The range [min_val,max_val] is being checked with the MACRO DNX_FIELD_RANGE_L4_RANGE_VALID.
             * We check it only if the verify is used for set.
             */
            if (is_for_set == DNX_FIELD_RANGE_IS_FOR_SET)
            {
                if (!DNX_FIELD_RANGE_L4_RANGE_VALID(range_info_p->min_val, range_info_p->max_val))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "The range for L4 Ops Range qualifier %s must be [DNX_FIELD_RANGE_QUALS_MIN_VAL-DNX_FIELD_RANGE_L4_OPS_MAX_VAL]. Range given is [%d - %d] \n",
                                 dnx_field_range_type_text(range_info_p->range_type), range_info_p->min_val,
                                 range_info_p->max_val);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal range type = %s for range_id %d\n",
                         dnx_field_range_type_text(range_info_p->range_type), range_id);
            break;
    }

    if (is_for_set)
    {
        dnx_field_range_info_t internal_range_info;
        SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &internal_range_info));
        internal_range_info.range_type = range_info_p->range_type;
        /** Get the range_info. */
        SHR_IF_ERR_EXIT(dnx_field_range_get(unit, field_stage, &range_id, &internal_range_info));
        if (internal_range_info.min_val != DNX_FIELD_RANGE_VALUE_INVALID ||
            internal_range_info.max_val != DNX_FIELD_RANGE_VALUE_INVALID)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Range Id %d was overwritten! Old Range type %s min-max = %d-%d.\r\n",
                         range_id, dnx_field_range_type_text(range_info_p->range_type),
                         internal_range_info.min_val, internal_range_info.max_val);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_range.h
 */
shr_error_e
dnx_field_range_type_nof_range_ids_extended(
    int unit,
    dnx_field_range_type_e dnx_range_type,
    uint32 range_indexes[],
    int *nof_range_ids)
{
    dnx_field_range_type_e range_type_iter;
    uint32 ii;
    uint32 range_type_count = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Loop on all configured Extended L4 OPs range indexes to find the correct index to be used for the given
     * range_type/range_id
     */
    for (ii = 0; ii < dnx_data_field.L4_Ops.nof_ext_types_get(unit); ii++)
    {
        SHR_IF_ERR_EXIT(dnx_field_range_sw_db.ext_l4_ops_range_types.get(unit, ii, &range_type_iter));
        if (range_type_iter == dnx_range_type)
        {
            range_indexes[range_type_count++] = ii;
        }
    }
    *nof_range_ids = range_type_count * dnx_data_field.L4_Ops.nof_range_entries_get(unit);
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Retrieves information about a range type for Extended L4 OPs range types
* \param [in] unit      - Device Id.
* \param [in] dnx_range_type    - The range type
* \param [in] range_id       - The range ID
* \param [out] range_type_info_p - The information about the range type.
* \return
*   shr_error_e - Error Type
* \see
*   * None
*/
static shr_error_e
dnx_field_range_type_info_extended(
    int unit,
    dnx_field_range_type_e dnx_range_type,
    uint32 range_id,
    dnx_field_map_range_type_info_t * range_type_info_p)
{
    int nof_range_ids = 0;
    uint32 range_indexes[DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES] = { 0 };
    uint32 range_interval;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(range_type_info_p, _SHR_E_PARAM, "range_type_info_p");

    SHR_IF_ERR_EXIT(dnx_field_range_type_nof_range_ids_extended(unit, dnx_range_type, range_indexes, &nof_range_ids));

    if (nof_range_ids == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Range type %s is currently not configured in Extended L4 OPs\n"
                     "Please use bcm_switch_control_indexed with type bcmSwitchRangeType to configure range types in Extended L4 OPs\n",
                     dnx_field_range_type_text(dnx_range_type));
    }
    if (nof_range_ids <= range_id)
    {
        SHR_ERR_EXIT(_SHR_E_BADID,
                     "Range type %s is currently configured with maximum %d ids (given range_id:%d is out of bounds)\n",
                     dnx_field_range_type_text(dnx_range_type), nof_range_ids, range_id);
    }
    /**
     * First initialize range_type_info_p
     */
    sal_memset(range_type_info_p, 0, sizeof(*range_type_info_p));
    /**
     * Fetch table id
     */
    range_type_info_p->table_id = DBAL_TABLE_FIELD_IPMF1_L4_OPS_RANGES;
    /**
     *  Fill the dbal fields.
     */
    range_interval = range_id / dnx_data_field.L4_Ops.nof_range_entries_get(unit);
    if (range_interval >= dnx_data_field.L4_Ops.nof_ext_types_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid range interval: %d\n", range_interval);
    }
    range_type_info_p->range_id_dbal_field = DBAL_FIELD_FIELD_PMF_RANGE_ID;
    switch (range_indexes[range_interval])
    {
        case 0:
            range_type_info_p->range_min_dbal_field = DBAL_FIELD_MIN_SRC_L4_PORT;
            range_type_info_p->range_max_dbal_field = DBAL_FIELD_MAX_SRC_L4_PORT;
            break;
        case 1:
            range_type_info_p->range_min_dbal_field = DBAL_FIELD_MIN_DEST_L4_PORT;
            range_type_info_p->range_max_dbal_field = DBAL_FIELD_MAX_DEST_L4_PORT;
            break;
        case 2:
            range_type_info_p->range_min_dbal_field = DBAL_FIELD_MIN_FIELD_2;
            range_type_info_p->range_max_dbal_field = DBAL_FIELD_MAX_FIELD_2;
            break;
        case 3:
            range_type_info_p->range_min_dbal_field = DBAL_FIELD_MIN_FIELD_3;
            range_type_info_p->range_max_dbal_field = DBAL_FIELD_MAX_FIELD_3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid range index: %d\n", range_indexes[range_interval]);
            break;
    }

    /*
     * Indicate whether the HW returns the range ID or a bitmap of range IDs.
     * Used to determine the default value for the range.
     */
    range_type_info_p->id_not_bitmap = FALSE;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Retrieves information about a range type
* \param [in] unit      - Device Id.
* \param [in] field_stage - The Field stage.
* \param [in] dnx_range_type    - The range type
* \param [in] range_id       - The range ID
* \param [out] range_type_info_p - The information about the range type.
* \return
*   shr_error_e - Error Type
* \see
*   * None
*/
static shr_error_e
dnx_field_range_type_info(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_range_type_e dnx_range_type,
    uint32 range_id,
    dnx_field_map_range_type_info_t * range_type_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(range_type_info_p, _SHR_E_PARAM, "range_type_info_p");

    if (dnx_data_field.features.extended_l4_ops_get(unit) &&
        DNX_FIELD_RANGE_EXTENDED_L4_OPS(dnx_range_type) && field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        SHR_IF_ERR_EXIT(dnx_field_range_type_info_extended(unit, dnx_range_type, range_id, range_type_info_p));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_map_range_type_info_legacy(unit, field_stage, dnx_range_type, range_type_info_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_range.h
 */
shr_error_e
dnx_field_range_set(
    int unit,
    uint32 flags,
    dnx_field_stage_e field_stage,
    uint32 range_id,
    dnx_field_range_info_t * range_info_p)
{
    uint32 entry_handle_id;
    dnx_field_map_range_type_info_t range_type_info;
    dnx_field_range_info_t range_info_internal;
    int absolute_min_val;
    int absolute_max_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");

    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "Setting Range info to range ID %d...\n"), range_id));

    sal_memcpy(&range_info_internal, range_info_p, sizeof(range_info_internal));
    /*
     * Initialize to keep compiler happy.
     */
    absolute_max_val = 0;

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * We treat IPMF2 stage in field_range same as IPMF1 stage
         */
        field_stage = DNX_FIELD_STAGE_IPMF1;
    }

    /**
     * Get all needed dbal Fields.
     */
    SHR_IF_ERR_EXIT(dnx_field_range_type_info
                    (unit, field_stage, range_info_internal.range_type, range_id, &range_type_info));
    /*
     * Range_id can be any value between [0-(<number of range memories> * <range memories size>)]to address the data
     * inside all L4 Ops range memories, but the actual inner index inside each range memory is between
     * [0-dnx_data_field.L4_Ops.nof_range_entries_get(unit)-1], that's why we mod the value by range memory size to get
     * the offset inside the range memory
     */

    if (dnx_data_field.features.extended_l4_ops_get(unit) &&
        DNX_FIELD_RANGE_EXTENDED_L4_OPS(range_info_internal.range_type) && field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        range_id = range_id % dnx_data_field.L4_Ops.nof_range_entries_get(unit);
    }

    /** Setting absolute_max_val to silence the compiler.*/
    absolute_max_val = -1;

    SHR_IF_ERR_EXIT(dnx_field_range_absolute_min_max_values(unit, range_info_internal.range_type,
                                                            &absolute_min_val, &absolute_max_val));

    /** Sanity check. Should not be reachable.*/
    if (absolute_max_val == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "absolute_max_val not set by dnx_field_range_absolute_min_max_values, or set to (-1).\n");
    }

    /*
     * For values that are DNX_FIELD_RANGE_VALUE_INVALID, set the default values.
     * If both min and max values are to be default values:
     * range_type_info.id_not_bitmap is FALSE -
     *  If the range's qualifier provides a bitmap of range IDs, the default range should accept any value.
     * range_type_info.id_not_bitmap is TRUE -
     *  If the range's qualifier provides the first matching range ID, the default range should accept no value.
     */

    /**
     * If the range is a range that is disables by default and neither min nor max was set,
     * give values that will never hit.
     */
    if (range_type_info.id_not_bitmap &&
        range_info_internal.min_val == DNX_FIELD_RANGE_VALUE_INVALID &&
        range_info_internal.max_val == DNX_FIELD_RANGE_VALUE_INVALID)
    {
        range_info_internal.min_val = 1;
        range_info_internal.max_val = 0;
    }
    else
    {
        /**
         * If the min_val was not initialized, set the specific minimum for this range!
         */
        if (range_info_internal.min_val == DNX_FIELD_RANGE_VALUE_INVALID)
        {
            range_info_internal.min_val = absolute_min_val;
        }
        /**
         * If the max_val was not initialized, set the specific maximum for this range!
         */
        if (range_info_internal.max_val == DNX_FIELD_RANGE_VALUE_INVALID)
        {
            range_info_internal.max_val = absolute_max_val;
        }
    }

    /**
     * Verify the range_id, min and max for each range_type
     */
    SHR_INVOKE_VERIFY_DNX(dnx_field_range_verify
                          (unit, field_stage, range_id, &range_info_internal, DNX_FIELD_RANGE_IS_FOR_SET));

    /**
     * Allocate DBAL handle to the returned dbal_table_id.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, range_type_info.table_id, &entry_handle_id));
    /**
     * Setting the DBAL table key.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, range_type_info.range_id_dbal_field, range_id);
    /**
     * Setting the DBAL fields.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, range_type_info.range_min_dbal_field, INST_SINGLE,
                                 range_info_internal.min_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, range_type_info.range_max_dbal_field, INST_SINGLE,
                                 range_info_internal.max_val);
    /**
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See header in field_range.h
 */
shr_error_e
dnx_field_range_get(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 *range_id_p,
    dnx_field_range_info_t * range_info_p)
{
    uint32 entry_handle_id;
    dnx_field_map_range_type_info_t range_type_info;
    uint32 inner_range_id = *range_id_p;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * We treat IPMF2 stage in field_range same as IPMF1 stage
         */
        field_stage = DNX_FIELD_STAGE_IPMF1;
    }

    /**
     * Verify the stage, * range_id_p for each range_type
     */
    SHR_INVOKE_VERIFY_DNX(dnx_field_range_verify
                          (unit, field_stage, *range_id_p, range_info_p, DNX_FIELD_RANGE_IS_FOR_GET));
    /**
     * Get all needed dbal Fields.
     */
    SHR_IF_ERR_EXIT(dnx_field_range_type_info
                    (unit, field_stage, range_info_p->range_type, *range_id_p, &range_type_info));
    /*
     * Range_id can be any value between 0-(<number of range memories> * <range memories size>)to address the data
     * inside all L4 Ops range memories, but the actual inner index inside each range memory is between
     * [0-dnx_data_field.L4_Ops.nof_range_entries_get(unit)-1], that's why we mod the value by range memory size to get
     * the offset inside the range memory
     */
    if (dnx_data_field.features.extended_l4_ops_get(unit) &&
        DNX_FIELD_RANGE_EXTENDED_L4_OPS(range_info_p->range_type) && field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        inner_range_id = *range_id_p % dnx_data_field.L4_Ops.nof_range_entries_get(unit);
    }
    /**
     * Allocate DBAL handle to the returned dbal_table_name
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, range_type_info.table_id, &entry_handle_id));
    /**
     * Setting the DBAL table key.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, range_type_info.range_id_dbal_field, inner_range_id);
    /**
     * Get the min_val
     */
    dbal_value_field32_request(unit, entry_handle_id, range_type_info.range_min_dbal_field, INST_SINGLE,
                               &(range_info_p->min_val));
    /**
     * Get the max_val
     */
    dbal_value_field32_request(unit, entry_handle_id, range_type_info.range_max_dbal_field, INST_SINGLE,
                               &(range_info_p->max_val));
    /**
     * Performing the action after this call the pointers that we set in field32_request()
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* See field_presel.h */
shr_error_e
dnx_field_range_absolute_min_max_values(
    int unit,
    dnx_field_range_type_e range_type,
    int *absolute_min_val_p,
    int *absolute_max_val_p)
{
    SHR_FUNC_INIT_VARS(unit);

    (*absolute_min_val_p) = DNX_FIELD_RANGE_QUALS_MIN_VAL;

    switch (range_type)
    {
        case DNX_FIELD_RANGE_TYPE_FFC1_LOW:
        case DNX_FIELD_RANGE_TYPE_FFC1_HIGH:
        case DNX_FIELD_RANGE_TYPE_FFC2_LOW:
        case DNX_FIELD_RANGE_TYPE_FFC2_HIGH:
        case DNX_FIELD_RANGE_TYPE_L4_SRC_PORT:
        case DNX_FIELD_RANGE_TYPE_L4_DST_PORT:
        {
            (*absolute_max_val_p) = DNX_FIELD_RANGE_L4_OPS_MAX_VAL;
            break;
        }

        case DNX_FIELD_RANGE_TYPE_OUT_LIF:
        {
            (*absolute_max_val_p) = DNX_FIELD_RANGE_OUT_LIF_MAX_VAL;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_IN_TTL:
        case DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE:
        case DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE:
        {
            (*absolute_max_val_p) = DNX_FIELD_RANGE_PKT_HDR_MAX_VAL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid range type");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  Return the number of range IDs available for a specific range type in Extended L4 OPs.
 * \param [in] unit          - Device ID
 * \param [in] range_type    - Range type
 * \param [out] nof_ranges_p - maximum number of ranges for range type and stage
 * \return
 *   shr_error_e - Error Type
 * \remark
 */
static shr_error_e
dnx_field_range_nof_range_ids_extended_l4_ops(
    int unit,
    dnx_field_range_type_e range_type,
    int *nof_ranges_p)
{
    uint32 range_indexes[DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_range_type_nof_range_ids_extended(unit, range_type, range_indexes, nof_ranges_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  Return the number of range IDs available for a specific range type in a specidfic stage.
 * \param [in] unit          - Device ID
 * \param [in] range_type    - Range type
 * \param [in] field_stage   - Stage
 * \param [out] nof_ranges_p - maximum number of ranges for range type and stage
 * \return
 *   shr_error_e - Error Type
 * \remark
 */
static shr_error_e
dnx_field_range_nof_range_ids_legacy(
    int unit,
    dnx_field_range_type_e range_type,
    dnx_field_stage_e field_stage,
    int *nof_ranges_p)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (range_type)
    {
        case DNX_FIELD_RANGE_TYPE_L4_SRC_PORT:
        case DNX_FIELD_RANGE_TYPE_L4_DST_PORT:
        {
            (*nof_ranges_p) = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_l4_ops_ranges_legacy;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_OUT_LIF:
        {
            (*nof_ranges_p) = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_out_lif_ranges;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE:
        {
            (*nof_ranges_p) = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_pkt_hdr_ranges;
            break;
        }
            /*
             * This code's only purpose is supporting legacy code init, nof_ext_l4_ops_ranges should be always 0
             */
        case DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE:
        case DNX_FIELD_RANGE_TYPE_IN_TTL:
        case DNX_FIELD_RANGE_TYPE_FFC1_LOW:
        case DNX_FIELD_RANGE_TYPE_FFC1_HIGH:
        case DNX_FIELD_RANGE_TYPE_FFC2_LOW:
        case DNX_FIELD_RANGE_TYPE_FFC2_HIGH:
        {
            (*nof_ranges_p) = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ext_l4_ops_ranges;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid range type");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* See field_range.h */
shr_error_e
dnx_field_range_nof_range_ids(
    int unit,
    dnx_field_range_type_e range_type,
    dnx_field_stage_e field_stage,
    int *nof_ranges_p)
{

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_field.features.extended_l4_ops_get(unit)
        && (field_stage == DNX_FIELD_STAGE_IPMF1 || field_stage == DNX_FIELD_STAGE_IPMF2)
        && DNX_FIELD_RANGE_EXTENDED_L4_OPS(range_type))
    {
        /*
         * Extended L4 OPs case
         */
        SHR_IF_ERR_EXIT(dnx_field_range_nof_range_ids_extended_l4_ops(unit, range_type, nof_ranges_p));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_range_nof_range_ids_legacy(unit, range_type, field_stage, nof_ranges_p));
    }
exit:
    SHR_FUNC_EXIT;
}

/* See field_range.h */
shr_error_e
dnx_field_range_init(
    int unit)
{
    int nof_range_ids;
    uint32 range_id;
    dnx_field_stage_e dnx_stage;
    bcm_field_stage_t bcm_stage;
    dnx_field_range_type_e dnx_range_type;
    dnx_field_range_info_t dnx_range_info;
    bcm_field_range_type_t bcm_range_type_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_range_sw_db.init(unit));
    SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &dnx_range_info));

    if (dnx_data_field.features.extended_l4_ops_get(unit))
    {
        bcm_switch_control_key_t key;
        bcm_switch_control_info_t value;
        uint32 range_type_idx;

        /** Set Src Port to be on first type selector (for backwards compitability) */
        key.type = bcmSwitchRangeTypeSelect;
        key.index = 0;
        value.value = bcmFieldRangeTypeL4SrcPort;
        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, value));
        /*
         * Set all other Extended L4 OPs range types selectors to DST_PORT type by default
         */
        for (range_type_idx = 1; range_type_idx < dnx_data_field.L4_Ops.nof_ext_types_get(unit); range_type_idx++)
        {
            key.type = bcmSwitchRangeTypeSelect;
            key.index = range_type_idx;
            value.value = bcmFieldRangeTypeL4DstPort;
            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, value));
        }
        /** Modify OP to be first pair and (for backwards compitability) */
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchRangeOperator, bcmSwitchRangeOperatorFirstPairAnd));
    }

    for (bcm_range_type_idx = 0; bcm_range_type_idx < bcmFieldRangeTypeCount; bcm_range_type_idx++)
    {
        SHR_SET_CURRENT_ERR(dnx_field_map_range_type_bcm_to_dnx_int(unit, bcm_range_type_idx, &dnx_range_type));
        if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
        {
            continue;
        }
        SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());
        dnx_range_info.range_type = dnx_range_type;
        DNX_FIELD_STAGE_ITERATOR(dnx_stage)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, dnx_stage, &bcm_stage));
            SHR_IF_ERR_EXIT(dnx_field_range_nof_range_ids(unit, dnx_range_type, dnx_stage, &nof_range_ids));
            for (range_id = 0; range_id < nof_range_ids; range_id++)
            {
                SHR_IF_ERR_EXIT(dnx_field_range_set(unit, 0, dnx_stage, range_id, &dnx_range_info));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_range_ffc_config_set(
    int unit,
    uint32 flags,
    uint32 configurable_range_index,
    dnx_field_range_type_qual_info_t * dnx_qual_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_key_l4_ops_ffc_set
                    (unit, dnx_qual_info->dnx_qual, configurable_range_index, &(dnx_qual_info->qual_info)));

    SHR_IF_ERR_EXIT(dnx_field_range_sw_db.ext_l4_ops_ffc_quals.set(unit, configurable_range_index, dnx_qual_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_range_ffc_config_get(
    int unit,
    uint32 flags,
    uint32 configurable_range_index,
    dnx_field_range_type_qual_info_t * dnx_qual_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_range_sw_db.ext_l4_ops_ffc_quals.get(unit, configurable_range_index, dnx_qual_info));

exit:
    SHR_FUNC_EXIT;
}
