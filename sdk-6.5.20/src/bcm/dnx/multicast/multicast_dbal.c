/** \file multicast_dbal.c
 * 
 *
 * General MULTICAST functionality for DNX.
 * Dedicated set of MULTICAST APIs are distributed between multicast_*.c files: \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/common/multicast.h>

#include "multicast_imp.h"
#include <soc/dnx/dnx_state_snapshot_manager.h>
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
 * see .h file
 */
shr_error_e
dnx_multicast_egr_core_enable_set(
    int unit,
    uint32 group_id,
    int core_id,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MULTICAST_GROUP_EGR_CORES, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP_ID, group_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_REP_ID, core_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ENABLE, INST_SINGLE, enable);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write an MCDB entry through DBAL interface
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] entry_val - structure filled with the entry values
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_entry_set(
    int unit,
    uint32 entry_id,
    dnx_multicast_mcdb_dbal_field_t * entry_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Our implementation of MCDB entry configuration does not promise to return the SW state
     * related to each entry to it's original state. This will cause swstate comparison error for
     * create and destroy sequence
     * comparison journal for this part should be skipped
     */
    DNX_STATE_COMPARISON_SUPPRESS(unit, TRUE);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MCDB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MCDB_INDEX, entry_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, entry_val->format);

    switch (entry_val->format)
    {
        case DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE,
                                         entry_val->next_entry_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                         entry_val->dest[0]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, entry_val->cud);
            break;
        case DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE,
                                         entry_val->next_entry_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, entry_val->cud);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 0, entry_val->dest[0]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 1, entry_val->dest[1]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCM, INST_SINGLE, 0);
            break;
        case DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE,
                                         entry_val->next_entry_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, entry_val->cud);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_PTR, INST_SINGLE, entry_val->bmp_ptr);
            break;
        case DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE,
                                         entry_val->next_entry_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK_LIST_PTR, INST_SINGLE,
                                         entry_val->link_list_ptr);
            break;
        case DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE,
                                         entry_val->next_entry_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP, INST_SINGLE, entry_val->bmp_val);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_OFFSET, INST_SINGLE,
                                         entry_val->bmp_offset);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCM, INST_SINGLE, 0);
            break;
        case DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE,
                                         entry_val->next_entry_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, entry_val->cud);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, INST_SINGLE,
                                         entry_val->dest[0]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCM, INST_SINGLE, 0);
            break;
        case DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE,
                                         entry_val->next_entry_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP, INST_SINGLE, entry_val->bmp_val);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_OFFSET, INST_SINGLE,
                                         entry_val->bmp_offset);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCM, INST_SINGLE, 0);
            break;
        case DBAL_RESULT_TYPE_MCDB_TDM_EXT_FORMAT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, entry_val->cud);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, 0,
                                         entry_val->dest[0]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, 1,
                                         entry_val->dest[1]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, 2,
                                         entry_val->dest[2]);
            break;
        case DBAL_RESULT_TYPE_MCDB_TDM_FORMAT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, entry_val->cud);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 0, entry_val->dest[0]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 1, entry_val->dest[1]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 2, entry_val->dest[2]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 3, entry_val->dest[3]);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_CONFIG, "unexpected format %u found", entry_val->format);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * previous entry pointers
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MCDB_LINK_LIST_PTR, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MCDB_INDEX, entry_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PREV_PTR, INST_SINGLE, entry_val->prev_entry);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DNX_STATE_COMPARISON_SUPPRESS(unit, FALSE);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get an MCDB entry through DBAL interface
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] entry_val - structure filled with the entry values
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_entry_get(
    int unit,
    uint32 entry_id,
    dnx_multicast_mcdb_dbal_field_t * entry_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MCDB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MCDB_INDEX, entry_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    sal_memset(entry_val, 0, sizeof(*entry_val));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &entry_val->format));

    switch (entry_val->format)
    {
        case DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE, &entry_val->next_entry_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &entry_val->dest[0]));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, &entry_val->cud));
            break;
        case DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE, &entry_val->next_entry_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, &entry_val->cud));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 0, &entry_val->dest[0]));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 1, &entry_val->dest[1]));
            break;
        case DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE, &entry_val->next_entry_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, &entry_val->cud));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_BMP_PTR, INST_SINGLE, &entry_val->bmp_ptr));
            break;
        case DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE, &entry_val->next_entry_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_LINK_LIST_PTR, INST_SINGLE, &entry_val->link_list_ptr));
            break;
        case DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE, &entry_val->next_entry_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_BMP, INST_SINGLE, &entry_val->bmp_val));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_BMP_OFFSET, INST_SINGLE, &entry_val->bmp_offset));
            break;
        case DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE, &entry_val->next_entry_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, &entry_val->cud));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, INST_SINGLE,
                             &entry_val->dest[0]));
            break;
        case DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE, &entry_val->next_entry_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_BMP, INST_SINGLE, &entry_val->bmp_val));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_BMP_OFFSET, INST_SINGLE, &entry_val->bmp_offset));
            break;
        case DBAL_RESULT_TYPE_MCDB_TDM_EXT_FORMAT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, &entry_val->cud));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, 0, &entry_val->dest[0]));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, 1, &entry_val->dest[1]));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, 2, &entry_val->dest[2]));
            break;
        case DBAL_RESULT_TYPE_MCDB_TDM_FORMAT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, &entry_val->cud));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 0, &entry_val->dest[0]));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 1, &entry_val->dest[1]));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 2, &entry_val->dest[2]));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, 3, &entry_val->dest[3]));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_CONFIG, "unexpected format %u found", entry_val->format);
    }

    /**
     * previous entry pointers
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MCDB_LINK_LIST_PTR, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MCDB_INDEX, entry_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PREV_PTR, INST_SINGLE, &entry_val->prev_entry);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - update next entry pointer of an MCDB entry through DBAL interface
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] next_entry_id - pointer to the next MCDB entry in the link list
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_entry_next_ptr_set(
    int unit,
    uint32 entry_id,
    uint32 next_entry_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MCDB_LINK_LIST_PTR, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MCDB_INDEX, entry_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PTR, INST_SINGLE, next_entry_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - update previous entry pointer of an MCDB entry through DBAL interface
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] prev_entry_id - pointer to the previous MCDB entry in the link list
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_entry_prev_ptr_set(
    int unit,
    uint32 entry_id,
    uint32 prev_entry_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Our implementation of MCDB entry configuration does not promise to return the SW state
     * related to each entry to it's original state. This will cause swstate comparison error for
     * create and destroy sequence
     * comparison journal for this part should be skipped
     */
    DNX_STATE_COMPARISON_SUPPRESS(unit, TRUE);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MCDB_LINK_LIST_PTR, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MCDB_INDEX, entry_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PREV_PTR, INST_SINGLE, prev_entry_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DNX_STATE_COMPARISON_SUPPRESS(unit, FALSE);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - gets the multicast group status (open/closed)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] is_open - open/close
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_group_open_get(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 *is_open)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MULTICAST_GROUP_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP_ID, group);
    if (DNX_MULTICAST_IS_EGRESS_BITMAP(flags))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EGRESS_BITMAP_IS_OPEN, INST_SINGLE, is_open);
    }
    else if (DNX_MULTICAST_IS_INGRESS_BITMAP(flags))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_BITMAP_IS_OPEN, INST_SINGLE, is_open);
    }
    else if (DNX_MULTICAST_IS_INGRESS(flags))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_GROUP_IS_OPEN, INST_SINGLE, is_open);
    }
    else
    {
        /*
         * DNX_MULTICAST_IS_EGRESS(flags) 
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EGRESS_GROUP_IS_OPEN, INST_SINGLE, is_open);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - sets the multicast group status (open/closed)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] is_open - open/close
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_group_open_set(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 is_open)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MULTICAST_GROUP_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP_ID, group);
    if (DNX_MULTICAST_IS_BITMAP(flags))
    {
        if (DNX_MULTICAST_IS_EGRESS(flags))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_BITMAP_IS_OPEN, INST_SINGLE, is_open);
        }
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_BITMAP_IS_OPEN, INST_SINGLE,
                                         is_open);
        }
    }
    else
    {
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_GROUP_IS_OPEN, INST_SINGLE, is_open);
        }
        if (DNX_MULTICAST_IS_EGRESS(flags))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_GROUP_IS_OPEN, INST_SINGLE, is_open);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_multicast_stat_count_copies_set(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    int arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (flags & BCM_MULICAST_STAT_EXTERNAL)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_MULTICAST_INGRESS_EXTERNAL_STAT_COUNT_COPIES, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_MULTICAST_INGRESS_INTERNAL_STAT_COUNT_COPIES, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMAND_ID, command_id);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE, INST_SINGLE, (arg == TRUE));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_multicast_stat_count_copies_get(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    int *arg)
{
    uint32 entry_handle_id;
    uint32 count_all_as_one;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (flags & BCM_MULICAST_STAT_EXTERNAL)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_MULTICAST_INGRESS_EXTERNAL_STAT_COUNT_COPIES, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_MULTICAST_INGRESS_INTERNAL_STAT_COUNT_COPIES, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMAND_ID, command_id);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNT_ALL_COPIES_AS_ONE, INST_SINGLE,
                               &count_all_as_one);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *arg = (int) count_all_as_one;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
