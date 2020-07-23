/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <bcm/flow.h>
#include <bcm/qos.h>
#include <bcm/l3.h>
#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/qos/qos.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_str.h>
#include "flow_def.h"

#define BCM_FLOW_LIF_UNSUPPORTED_OPTION(option) (_SHR_IS_FLAG_SET(info->valid_elements_set, option)) || (_SHR_IS_FLAG_SET(info->valid_elements_clear, option))

extern dnx_flow_app_config_t *dnx_flow_objects[];
extern dnx_flow_common_field_desc_t *dnx_flow_term_common_fields[];
extern dnx_flow_common_field_desc_t *dnx_flow_init_common_fields[];
lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info = { 0 };

/**********************************************************************************************
*                                        verify functions                                     *
 */

static shr_error_e
dnx_flow_create_verify_common(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *bcm_app_info,
    bcm_flow_special_fields_t * special_fields)
{
    int field;
    int num_common_fields = 0;
    uint32 valid_elements_clear = 0;
    uint32 valid_elements_set = 0;
    dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    if (flow_app_type != FLOW_APP_TYPE_MATCH)
    {
        SHR_NULL_CHECK(bcm_app_info, _SHR_E_PARAM, "bcm_info");
    }
    SHR_NULL_CHECK(dnx_flow_objects[flow_handle_info->flow_handle], _SHR_E_PARAM,
                   "dnx_flow_objects[flow_handle_info->flow_handle]");

    if ((flow_handle_info->flow_handle >= dnx_flow_number_of_apps()) || (flow_handle_info->flow_handle < 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal flow handle ID, %d", flow_handle_info->flow_handle);
    }

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    if (flow_app_info->flow_app_common_config->is_verify_disabled)
    {
        SHR_EXIT();
    }

    if (flow_app_info->flow_app_type != flow_app_type)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mismatch between caller API app type and application app type");
    }

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_type, &num_common_fields));

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        valid_elements_clear = ((bcm_flow_terminator_info_t *) bcm_app_info)->valid_elements_clear;
        valid_elements_set = ((bcm_flow_terminator_info_t *) bcm_app_info)->valid_elements_set;

    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        valid_elements_clear = ((bcm_flow_initiator_info_t *) bcm_app_info)->valid_elements_clear;
        valid_elements_set = ((bcm_flow_initiator_info_t *) bcm_app_info)->valid_elements_set;

    }
    else if (flow_app_type == FLOW_APP_TYPE_MATCH)
    {
        valid_elements_clear = 0;
        valid_elements_set = 0;

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid flow app type, %d", flow_app_type);
    }

    /*
     * Iterate over the common field. Make sure that there're no set flags which are not supported by the application 
     */
    if (num_common_fields)
    {
        /*
         * Check if the user has set a valid field on an unsupported application field
         */
        if ((valid_elements_set ^ flow_app_info->valid_common_fields_bitmap)
            || (valid_elements_clear ^ flow_app_info->valid_common_fields_bitmap))
        {
            for (field = 0; field < num_common_fields; field++)
            {
                if ((_SHR_IS_FLAG_SET(valid_elements_set, field) || _SHR_IS_FLAG_SET(valid_elements_clear, field))
                    && !_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported combination: Application %s, common field set, %s",
                                 flow_app_info->app_name, dnx_flow_term_common_fields[field]->field_name);
                }
            }
        }
    }

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        
        if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
        {
            if ((_SHR_IS_FLAG_SET(valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID))
                && (!_SHR_IS_FLAG_SET(valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_FLOW_SERVICE_TYPE_VALID)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Flag BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID can only be used when BCM_FLOW_TERMINATOR_ELEMENT_FLOW_SERVICE_TYPE_VALID is set");
            }
        }
    }

    /*
     * Make sure that no common field was marked as set and clear at the same time 
     */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        if (valid_elements_set & valid_elements_clear)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid configuration, VALID flag can only be set on either valid_elements_set, valid_elemetns_clear");
        }
    }

    if (special_fields)
    {
        int max_num_app_special_fields, num_app_special_fields = 0, app_idx, idx;

        if (!flow_app_info->special_fields[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid configuration, no special fields in app %s configuration",
                         flow_app_info->app_name);
        }
        max_num_app_special_fields = sizeof(flow_app_info->special_fields) / sizeof(flow_app_info->special_fields[0]);

        while ((num_app_special_fields < max_num_app_special_fields)
               && (flow_app_info->special_fields[num_app_special_fields] != DBAL_FIELD_EMPTY))
        {
            num_app_special_fields++;
        }

        if (special_fields->actual_nof_special_fields > num_app_special_fields)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid configuration, requested %d special fields, nof %s app special fields is %d",
                         special_fields->actual_nof_special_fields, flow_app_info->app_name, num_app_special_fields);
        }

        /*
         * Iterate over the special fields. Make sure that they're supported by the application
         */
        for (idx = 0; idx < special_fields->actual_nof_special_fields; idx++)
        {
            for (app_idx = 0; app_idx < num_app_special_fields; app_idx++)
            {
                if (special_fields->special_fields[idx].field_id == flow_app_info->special_fields[app_idx])
                {
                    break;
                }
            }
            if (app_idx == num_app_special_fields)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Special field %s is not supported in application %s",
                             dbal_field_to_string(unit, special_fields->special_fields[idx].field_id),
                             flow_app_info->app_name);
            }
        }
    }

    /*
     * Invoke specific verify function Do we need casting here for the bcm_app_info to the correct bcm struct
     */
    if (flow_app_info->verify_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->verify_cb(unit, flow_handle_info, bcm_app_info, special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify ingress flow lif get
 */
static shr_error_e
dnx_flow_terminator_info_get_verify(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * info,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "initiator_info");
    SHR_NULL_CHECK(dnx_flow_objects[flow_handle_info->flow_handle], _SHR_E_PARAM,
                   "dnx_flow_objects[flow_handle_info->flow_handle]");

    if ((flow_handle_info->flow_handle >= dnx_flow_number_of_apps()) || (flow_handle_info->flow_handle < 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal flow handle ID, %d", flow_handle_info->flow_handle);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify ingress flow lif delete
 */
static shr_error_e
dnx_flow_terminator_info_destroy_verify(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    SHR_NULL_CHECK(dnx_flow_objects[flow_handle_info->flow_handle], _SHR_E_PARAM,
                   "dnx_flow_objects[flow_handle_info->flow_handle]");

    if ((flow_handle_info->flow_handle >= dnx_flow_number_of_apps()) || (flow_handle_info->flow_handle < 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal flow handle ID, %d", flow_handle_info->flow_handle);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify ingress flow lif traverse
 */
static shr_error_e
dnx_flow_terminator_info_traverse_verify(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "flow_handle_info");
    SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "terminator_info");
    SHR_NULL_CHECK(dnx_flow_objects[flow_handle_info->flow_handle], _SHR_E_PARAM,
                   "dnx_flow_objects[flow_handle_info->flow_handle]");

    if ((flow_handle_info->flow_handle >= dnx_flow_number_of_apps()) || (flow_handle_info->flow_handle < 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal flow handle ID, %d", flow_handle_info->flow_handle);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify egress flow lif get
 */
static shr_error_e
dnx_flow_initiator_info_get_verify(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * info,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "initiator_info");
    SHR_NULL_CHECK(dnx_flow_objects[flow_handle_info->flow_handle], _SHR_E_PARAM,
                   "dnx_flow_objects[flow_handle_info->flow_handle]");

    if ((flow_handle_info->flow_handle >= dnx_flow_number_of_apps()) || (flow_handle_info->flow_handle < 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal flow handle ID, %d", flow_handle_info->flow_handle);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify egress flow lif delete
 */
static shr_error_e
dnx_flow_initiator_info_destroy_verify(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    SHR_NULL_CHECK(dnx_flow_objects[flow_handle_info->flow_handle], _SHR_E_PARAM,
                   "dnx_flow_objects[flow_handle_info->flow_handle]");

    if ((flow_handle_info->flow_handle >= dnx_flow_number_of_apps()) || (flow_handle_info->flow_handle < 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal flow handle ID, %d", flow_handle_info->flow_handle);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify egress flow lif traverse
 */
static shr_error_e
dnx_flow_initiator_info_traverse_verify(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "flow_handle_info");
    SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "terminator_info");
    SHR_NULL_CHECK(dnx_flow_objects[flow_handle_info->flow_handle], _SHR_E_PARAM,
                   "dnx_flow_objects[flow_handle_info->flow_handle]");

    if ((flow_handle_info->flow_handle >= dnx_flow_number_of_apps()) || (flow_handle_info->flow_handle < 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal flow handle ID, %d", flow_handle_info->flow_handle);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Match APIs verify function
 */
static shr_error_e
dnx_flow_match_info_verify(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields)
{
    dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    SHR_NULL_CHECK(special_fields, _SHR_E_PARAM, "special_fields");

    if ((flow_handle_info->flow_handle >= dnx_flow_number_of_apps()) || (flow_handle_info->flow_handle < 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal flow handle ID, %d", flow_handle_info->flow_handle);
    }

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    if (flow_app_info->flow_app_common_config->is_verify_disabled)
    {
        SHR_EXIT();
    }

    if (special_fields)
    {
        int num_app_special_fields, app_idx, idx;

        num_app_special_fields = sizeof(flow_app_info->special_fields) / sizeof(flow_app_info->special_fields[0]);

        if (special_fields->actual_nof_special_fields > num_app_special_fields)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid configuration, requested %d special fields, nof %s app special fields is %d",
                         special_fields->actual_nof_special_fields, flow_app_info->app_name, num_app_special_fields);
        }

        /*
         * Iterate over the special fields. Make sure that they're supported by the application
         */
        for (idx = 0; idx < special_fields->actual_nof_special_fields; idx++)
        {
            for (app_idx = 0; app_idx < num_app_special_fields; app_idx++)
            {
                if (special_fields->special_fields[idx].field_id == flow_app_info->special_fields[app_idx])
                {
                    break;
                }
            }
            if (app_idx == num_app_special_fields)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Special field %s is not supported in application %s",
                             dbal_field_to_string(unit, special_fields->special_fields[idx].field_id),
                             flow_app_info->app_name);
            }
        }
    }

    /*
     * Invoke specific verify function Do we need casting here for the bcm_app_info to the correct bcm struct
     */
    if (flow_app_info->verify_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->verify_cb(unit, flow_handle_info, NULL, special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Match traverse API verify
 */
shr_error_e
dnx_flow_match_info_traverse_verify(
    int unit,
    bcm_flow_match_info_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb");
    SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "user_data");

exit:
    SHR_FUNC_EXIT;
}

/**********************************************************************************************
*                                        verify functions - END                               *
 */

/**********************************************************************************************
*                                        Internal functions                                   *
 */

/*
 * Internal ingress flow lif replace
 */
static shr_error_e
dnx_flow_terminator_info_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    int local_inlif;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 entry_handle_id, get_entry_handle_id = 0;
    lif_table_mngr_inlif_info_t inlif_info;
    int rv;
    int field;
    int num_common_fields;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    num_common_fields = dnx_flow_number_of_terminator_common_fields();

    /** Take handle to write to DBAL table*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /** Take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &get_entry_handle_id));

    /** Get Local inLIF ID */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, get_entry_handle_id, &inlif_info));
    inlif_info.global_lif = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info->flow_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, inlif_info.global_lif);

    /*
     * Iterate over the flow termination common fields. If requested by the user - call relevant callback
     */
    for (field = 0; field < num_common_fields; field++)
    {
        if (_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, (1 << field)))
        {
            if (dnx_flow_term_common_fields[field]->field_cbs.field_set_cb)
            {
                SHR_IF_ERR_EXIT(dnx_flow_term_common_fields[field]->
                                field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM, flow_handle_info,
                                                       terminator_info));
            }
        }
        /*
         * If no flag was set - keep original field value. Perform get on original entry. If exists - set previous
         * value. Otherwise - leave default value
         */
        else if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, (1 << field)))
        {
            if (dnx_flow_term_common_fields[field]->field_cbs.field_get_cb)
            {
                rv = dnx_flow_term_common_fields[field]->field_cbs.field_get_cb(unit, get_entry_handle_id,
                                                                                FLOW_APP_TYPE_TERM,
                                                                                flow_handle_info, terminator_info);
                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT(dnx_flow_term_common_fields[field]->
                                    field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM, flow_handle_info,
                                                           terminator_info));
                }
                else if (rv == _SHR_E_NOT_FOUND)
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                }
                else
                {
                    SHR_SET_CURRENT_ERR(rv);
                    SHR_EXIT();
                }
            }
        }
    }

    /** Write to HW */
    inlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    local_inlif = gport_hw_resources.local_in_lif;
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &local_inlif, &inlif_info));

    /** Free resources
     *  iterate on all common fields which were replaced.
     *  If a delete cb exists - call it*/
    for (field = 0; field < num_common_fields; field++)
    {
        if (_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, (1 << field)))
        {
            
            if (dnx_flow_term_common_fields[field]->field_cbs.field_delete_cb)
            {
                SHR_IF_ERR_EXIT(dnx_flow_term_common_fields[field]->
                                field_cbs.field_delete_cb(unit, get_entry_handle_id, FLOW_APP_TYPE_TERM,
                                                          flow_handle_info, terminator_info));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Get ingress flow lif matching lif table manager info
 */
static shr_error_e
dnx_flow_terminator_info_get_lif_table_manager_info(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 *entry_handle_id,
    lif_table_mngr_inlif_info_t * inlif_info)
{
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get local inLIF from GPort hw resources (global and local inLIF) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    /** Take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, entry_handle_id));

    /**  Get tunnel inLIF info from lif tble manager  */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, *entry_handle_id, inlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Create ingress flow lif global lif
 */
static shr_error_e
dnx_flow_terminator_info_create_global_lif(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    lif_table_mngr_inlif_info_t * lif_table_mngr_inlif_info,
    uint32 entry_handle_id)
{
    int global_lif = 0;
    int lif_alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID))
    {
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        global_lif = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info->flow_id);
        if (!global_lif)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Gport is not given when flag BCM_FLOW_HANDLE_INFO_WITH_ID is set");
        }
    }
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, lif_alloc_flags, DNX_ALGO_LIF_INGRESS, &global_lif));

    BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, global_lif);

    lif_table_mngr_inlif_info->global_lif = global_lif;

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, global_lif);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Internal ingress flow lif create
 */
static shr_error_e
dnx_flow_terminator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    int local_inlif;
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_table_mngr_inlif_info = { 0 };
    uint32 field;
    int num_common_fields;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    num_common_fields = dnx_flow_number_of_terminator_common_fields();

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /** Global lif */
    SHR_IF_ERR_EXIT(dnx_flow_terminator_info_create_global_lif
                    (unit, flow_handle_info, &lif_table_mngr_inlif_info, entry_handle_id));

    /*
     * Iterate over common fields. If a field was set by the user - call relevant set callback
     */
    for (field = 0; field < num_common_fields; field++)
    {
        if (_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, (1 << field)))
        {
            if (dnx_flow_term_common_fields[field]->field_cbs.field_set_cb)
            {
                SHR_IF_ERR_EXIT(dnx_flow_term_common_fields[field]->
                                field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM, flow_handle_info,
                                                       terminator_info));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported combination: Application %s, common field set, %s",
                             flow_app_info->app_name, dnx_flow_term_common_fields[field]->field_name);
            }
        }
    }

    /*
     * Call lif table manager set
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &local_inlif, &lif_table_mngr_inlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Set egress flow lif global lif
 */
static shr_error_e
dnx_flow_initiator_info_create_global_lif(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    int *global_lif_id)
{
    uint32 lif_alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** WITH_ID flag is used - get global outLIF ID */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID))
    {
        if (BCM_GPORT_IS_TUNNEL(flow_handle_info->flow_id))
        {
            *global_lif_id = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info->flow_id);
            lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        }
    }

    /** Allocate LIF - WITH_ID if the flag was set */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, lif_alloc_flags, DNX_ALGO_LIF_EGRESS, global_lif_id));

    BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, *global_lif_id);

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * Internal egress flow lif create
 */
static shr_error_e
dnx_flow_initiator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    int local_outlif;
    int global_lif_id;
    uint32 entry_handle_id;
    int field;
    int num_common_fields;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sal_memset(&lif_table_mngr_outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    num_common_fields = dnx_flow_number_of_initiator_common_fields();

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    /** Create global lif */
    SHR_IF_ERR_EXIT(dnx_flow_initiator_info_create_global_lif(unit, flow_handle_info, &global_lif_id));

    lif_table_mngr_outlif_info.global_lif = global_lif_id;

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /*
     * Iterate on the common fields. If a field flag was set by the user - call relevant callback 
     */
    for (field = 0; field < num_common_fields; field++)
    {
        if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, (1 << field)))
        {
            if (dnx_flow_init_common_fields[field]->field_cbs.field_set_cb)
            {
                SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                                field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT, flow_handle_info,
                                                       initiator_info));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported combination: Application %s, common field set, %s",
                             flow_app_info->app_name, dnx_flow_init_common_fields[field]->field_name);
            }
        }
    }

    /** Iterate over the special fields, and set them */
    
    if (special_fields)
    {
        for (field = 0; field < special_fields->actual_nof_special_fields; field++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, special_fields->special_fields[field].field_id,
                                         INST_SINGLE, special_fields->special_fields[field].shr_var_uint32);
        }
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, &local_outlif, &lif_table_mngr_outlif_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Get egress flow lif matching lif table manager entry info
 */
static shr_error_e
dnx_flow_initiator_info_get_lif_table_manager_outlif(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    uint32 entry_handle_id,
    lif_table_mngr_outlif_info_t * outlif_info)
{
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    /** Get GPort hw resources (global and local tunnel outlif) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                    (unit, gport_hw_resources.local_out_lif, entry_handle_id, outlif_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Internal replace egress flow lif
 */
static shr_error_e
dnx_flow_initiator_info_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    int global_lif_id = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    uint32 lif_get_flags = 0;
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info_get = { 0 };
    uint32 entry_handle_id, get_entry_handle_id = 0;
    int rv;
    int field;
    int num_common_fields;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sal_memset(&lif_table_mngr_outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));
    lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

    num_common_fields = dnx_flow_number_of_initiator_common_fields();

    global_lif_id = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info->flow_id);

    lif_table_mngr_outlif_info.global_lif = global_lif_id;

    /** get GPort HW resources (global and local tunnel outlif) */
    lif_get_flags =
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_get_flags, &gport_hw_resources));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &get_entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_flow_initiator_info_get_lif_table_manager_outlif
                    (unit, flow_handle_info, initiator_info, get_entry_handle_id, &lif_table_mngr_outlif_info_get));

    lif_table_mngr_outlif_info_get.global_lif = global_lif_id;

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /*
     * Iterate over the flow initiator common fields. If requested by the user - call relevant callback
     */
    for (field = 0; field < num_common_fields; field++)
    {
        if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, (1 << field)))
        {
            if (dnx_flow_init_common_fields[field]->field_cbs.field_set_cb)
            {
                SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                                field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT, flow_handle_info,
                                                       initiator_info));
            }
        }
        /*
         * If no flag was set on a field - if field was set previously - take its value. Otherwise - leave default
         * value 
         */
        else if (!_SHR_IS_FLAG_SET(initiator_info->valid_elements_clear, (1 << field)))
        {
            if (dnx_flow_init_common_fields[field]->field_cbs.field_get_cb)
            {
                rv = dnx_flow_init_common_fields[field]->field_cbs.field_get_cb(unit, get_entry_handle_id,
                                                                                FLOW_APP_TYPE_INIT,
                                                                                flow_handle_info, initiator_info);
                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                                    field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT, flow_handle_info,
                                                           initiator_info));
                }
                else if (rv == _SHR_E_NOT_FOUND)
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                }
                else
                {
                    SHR_SET_CURRENT_ERR(rv);
                    SHR_EXIT();
                }
            }
        }
    }

    /*
     * Iterate over the special fields 
     */
    if (special_fields)
    {
        for (field = 0; field < special_fields->actual_nof_special_fields; field++)
        {
            /*
             * If is_clear is off --> set requested value 
             */
            if (!special_fields->special_fields[field].is_clear)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, special_fields->special_fields[field].field_id,
                                             INST_SINGLE, special_fields->special_fields[field].shr_var_uint32);
            }
            /*
             * is_clear is on --> field should be ignored. If existed on previous entry - take its previous value 
             */
            else
            {
                uint32 shr_32_var = 0;

                rv = dnx_flow_field_value32_get(unit, get_entry_handle_id,
                                                special_fields->special_fields[field].field_id, &shr_32_var);

                if (rv == _SHR_E_NONE)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, special_fields->special_fields[field].field_id,
                                                 INST_SINGLE, shr_32_var);
                }
                else if (rv == _SHR_E_NOT_FOUND)
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                }
                else
                {
                    SHR_SET_CURRENT_ERR(rv);
                    SHR_EXIT();
                }
            }
        }
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, &gport_hw_resources.local_out_lif, &lif_table_mngr_outlif_info));

    /** Free resources */
    for (field = 0; field < num_common_fields; field++)
    {
        /*
         * If a field was replaced - remove the old value through a callback function (if exists) 
         */
        if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, field))
        {
            if (dnx_flow_init_common_fields[field]->field_cbs.field_delete_cb)
            {
                SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                                field_cbs.field_delete_cb(unit, get_entry_handle_id, FLOW_APP_TYPE_INIT,
                                                          flow_handle_info, initiator_info));
            }
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Internal egress flow lif get
 */
static shr_error_e
dnx_flow_initiator_info_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    lif_table_mngr_outlif_info_t outlif_info;
    uint32 entry_handle_id = 0;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int field, num_common_fields;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    num_common_fields = dnx_flow_number_of_initiator_common_fields();

    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_flow_initiator_info_get_lif_table_manager_outlif
                    (unit, flow_handle_info, initiator_info, entry_handle_id, &outlif_info));

    /*
     * Iterate over all common fields, and call relevant get callback. If not found - ignore, field didn't exist on
     * entry 
     */
    for (field = 0; field < num_common_fields; field++)
    {
        if (dnx_flow_init_common_fields[field]->field_cbs.field_get_cb)
        {
            SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                            field_cbs.field_get_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT, flow_handle_info,
                                                   initiator_info));
        }
    }

    /*
     * Iterate over the special fields. If a field exists on entry - get its value and update actual_nof_special_fields 
     */
    if (flow_app_info->special_fields[0] != DBAL_FIELD_EMPTY)
    {
        int idx = 0, rv;
        uint32 value, num_special_fields_in_app = 0;

        if (!special_fields)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Missing special fields array to fill");
        }
        while (flow_app_info->special_fields[num_special_fields_in_app] != DBAL_FIELD_EMPTY)
        {
            num_special_fields_in_app++;
        }
        special_fields->actual_nof_special_fields = 0;
        for (field = 0; field < num_special_fields_in_app; field++)
        {
            rv = dnx_flow_field_value32_get(unit, entry_handle_id, flow_app_info->special_fields[field], &value);
            if (rv == _SHR_E_NONE)
            {
                special_fields->special_fields[idx].shr_var_uint32 = value;
                special_fields->special_fields[idx].field_id = flow_app_info->special_fields[field];
                idx++;
                special_fields->actual_nof_special_fields++;
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Internal match add entry
 */
static shr_error_e
dnx_flow_match_info_add(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    int ii;
    uint32 entry_handle_id;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    for (ii = 0; ii < key_special_fields->actual_nof_special_fields; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key_special_fields->special_fields[ii].field_id,
                                   key_special_fields->special_fields[ii].shr_var_uint32);
    }

    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                                 gport_hw_resources.local_in_lif);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Internal match entry delete
 */
static shr_error_e
dnx_flow_match_info_delete(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    int ii;
    uint32 entry_handle_id;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));
    for (ii = 0; ii < key_special_fields->actual_nof_special_fields; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key_special_fields->special_fields[ii].field_id,
                                   key_special_fields->special_fields[ii].shr_var_uint32);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Internal match entry get
 */
static shr_error_e
dnx_flow_match_info_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    int ii;
    uint32 entry_handle_id;
    lif_mapping_local_lif_info_t local_lif_info;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    for (ii = 0; ii < key_special_fields->actual_nof_special_fields; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key_special_fields->special_fields[ii].field_id,
                                   key_special_fields->special_fields[ii].shr_var_uint32);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, (uint32 *) &local_lif_info.local_lif));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* Flow init procedures
 * This function is used for applications and field validations */
shr_error_e
dnx_flow_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**********************************************************************************************
*                                  Internal functions - END                                   *
 */

/**********************************************************************************************
*                                        APIs                                                 *
 */

/**
 * \brief
 * Get flow handle based on application.
 *
 * \param [in]  unit - Relevant unit.
 * \param [in]  flow_name - relevant application matching name \n
 * \param [out] handle - the flow handle \n
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_handle_get(
    int unit,
    const char *flow_name,
    bcm_flow_handle_t * handle)
{
    int idx, num_apps;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    num_apps = dnx_flow_number_of_apps();

    for (idx = 1; idx < num_apps; idx++)
    {
        if (!sal_strncasecmp(flow_name, dnx_flow_objects[idx]->app_name, FLOW_STR_MAX))
        {
            *handle = idx;
            break;
        }
    }
    if (idx == num_apps)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Couldn't find matching application object by the name %s", flow_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create the terminator flow lif.
 *
 * \param [in]  unit - Relevant unit.
 * \param [in,out] flow_handle_info - pointer to a struct containing the relevant general flow attributes \n
 * \param [in,out] terminator_info - pointer to a struct containing the relevant terminator flow attributes \n
 * \param [in,out] special_fields - list of fields which are not common (not part of the terminator flow struct) \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_terminator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_create_verify_common
                          (unit, FLOW_APP_TYPE_TERM, flow_handle_info, terminator_info, special_fields));

    /*
     * Set
     */
    if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_flow_terminator_info_create(unit, flow_handle_info, terminator_info, special_fields));
    }
    /*
     * Replace
     */
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_terminator_info_replace(unit, flow_handle_info, terminator_info, special_fields));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Remove the terminator flow lif.
 *
 * \param [in]  unit - Relevant unit.
 * \param [in,out] flow_handle_info - pointer to a struct containing the relevant general flow attributes \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_terminator_info_destroy(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info)
{
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int field, num_common_fields;
    uint32 entry_handle_id;
    bcm_flow_terminator_info_t terminator_info;
    dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_terminator_info_destroy_verify(unit, flow_handle_info));

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    num_common_fields = dnx_flow_number_of_terminator_common_fields();

    /** Take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, dnx_flow_objects[flow_handle_info->flow_handle]->flow_table, &entry_handle_id));

    /** Get local inLIF from GPort hw resources (global and local inLIF) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Iterate over all common fields. If exists in application and delete callback exists - delete it
     */
    for (field = 0; field < num_common_fields; field++)
    {
        if ((dnx_flow_term_common_fields[field]) && (dnx_flow_term_common_fields[field]->field_cbs.field_delete_cb))
        {
            SHR_IF_ERR_EXIT(dnx_flow_term_common_fields[field]->
                            field_cbs.field_delete_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM, flow_handle_info,
                                                      &terminator_info));
        }
    }

    /** Delete entry from GTP tunnel in-LIF table.
     *  Delete local in LIF */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, flow_app_info->flow_table, 0));
    /** Delete global inLIF */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free
                    (unit, DNX_ALGO_LIF_INGRESS, BCM_GPORT_TUNNEL_ID_GET(flow_handle_info->flow_id)));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get a terminator flow lif.
 *
 * \param [in]  unit - Relevant unit.
 * \param [in,out] flow_handle_info - pointer to a struct containing the relevant general flow attributes \n
 * \param [in,out] terminator_info - pointer to a struct containing the relevant terminator flow attributes \n
 * \param [in,out] special_fields - list of fields which are not common (not part of the terminator flow struct) \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_terminator_info_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t inlif_info;
    uint32 field;
    int num_common_fields;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_terminator_info_get_verify(unit, flow_handle_info, terminator_info, special_fields));

    num_common_fields = dnx_flow_number_of_terminator_common_fields();

    SHR_IF_ERR_EXIT(dnx_flow_terminator_info_get_lif_table_manager_info
                    (unit, flow_handle_info, &entry_handle_id, &inlif_info));

    /*
     * Iterate over all common fields in application. Perform get, ignore not_found rv 
     */
    for (field = 0; field < num_common_fields; field++)
    {
        if (dnx_flow_term_common_fields[field]->field_cbs.field_get_cb)
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_term_common_fields[field]->
                                      field_cbs.field_get_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM,
                                                             flow_handle_info, terminator_info), _SHR_E_NOT_FOUND);
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all entries related to the lif application and run a callback function provided by the user.
 *
 * \param [in]  unit - Relevant unit.
 * \param [in] flow_handle_info - flow_handle_info
 * \param [in] cb - user callback
 * \param [in] user_data - user data
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_terminator_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id, entry_handle_id2;
    int is_end;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    lif_table_mngr_inlif_info_t inlif_info;
    uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 field;
    int num_common_fields;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    num_common_fields = dnx_flow_number_of_terminator_common_fields();

    
    flow_handle_info->flow_handle = 1;

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_flow_terminator_info_traverse_verify(unit, flow_handle_info, cb, user_data));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_GTP, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, dbal_field));
        inlif_info.global_lif = dbal_field[0];
        BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, inlif_info.global_lif);

        SHR_IF_ERR_EXIT(dnx_flow_terminator_info_get_lif_table_manager_info
                        (unit, flow_handle_info, &entry_handle_id2, &inlif_info));

        /*
         * Iterate over all application's common fields and perform get cb 
         */
        for (field = 0; field < num_common_fields; field++)
        {
            if (dnx_flow_term_common_fields[field]->field_cbs.field_get_cb)
            {
                SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_term_common_fields[field]->
                                          field_cbs.field_get_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM,
                                                                 flow_handle_info, &terminator_info), _SHR_E_NOT_FOUND);
            }
        }

        /** Invoke callback function */
        SHR_IF_ERR_EXIT((*cb) (unit, flow_handle_info, &terminator_info, NULL, user_data));

        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create a initiator flow lif.
 *
 * \param [in]  unit - Relevant unit.
 * \param [in,out] flow_handle_info - pointer to a struct containing the relevant general flow attributes \n
 * \param [in,out] initiator_info - pointer to a struct containing the relevant initiator flow attributes \n
 * \param [in,out] special_fields - list of fields which are not common (not part of the terminator flow struct) \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_initiator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_create_verify_common
                          (unit, FLOW_APP_TYPE_INIT, flow_handle_info, initiator_info, special_fields));

    /*
     * Set
     */
    if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_create(unit, flow_handle_info, initiator_info, special_fields));
    }
    /*
     * Replace
     */
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_replace(unit, flow_handle_info, initiator_info, special_fields));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create a initiator flow lif.
 *
 * \param [in]  unit - Relevant unit.
 * \param [in,out] flow_handle_info - pointer to a struct containing the relevant general flow attributes \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_initiator_info_destroy(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info)
{
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int field, num_common_fields;
    uint32 entry_handle_id;
    bcm_flow_initiator_info_t initiator_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_initiator_info_destroy_verify(unit, flow_handle_info));

    num_common_fields = dnx_flow_number_of_initiator_common_fields();

    /** Take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, dnx_flow_objects[flow_handle_info->flow_handle]->flow_table, &entry_handle_id));

    /** Get local inLIF from GPort hw resources (global and local inLIF) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.outlif_dbal_result_type);

    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, gport_hw_resources.global_out_lif));

    /*
     * Iterate on all common fields. If a delete callback exists - call it 
     */
    for (field = 0; field < num_common_fields; field++)
    {
        if (dnx_flow_init_common_fields[field]->field_cbs.field_delete_cb)
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_init_common_fields[field]->
                                      field_cbs.field_delete_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT,
                                                                flow_handle_info, &initiator_info), _SHR_E_NOT_FOUND);
        }
    }

    /** Remove tunnel data from EEDB outlif table */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, gport_hw_resources.local_out_lif, 0));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get an initiator flow lif.
 *
 * \param [in]  unit - Relevant unit.
 * \param [in,out] flow_handle_info - pointer to a struct containing the relevant general flow attributes \n
 * \param [in,out] initiator_info - pointer to a struct containing the relevant initiator flow attributes \n
 * \param [in,out] special_fields - list of fields which are not common (not part of the terminator flow struct) \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_initiator_info_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_initiator_info_get_verify(unit, flow_handle_info, initiator_info, special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_initiator_info_get(unit, flow_handle_info, initiator_info, special_fields));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all entries related to the lif application and run a callback function provided by the user.
 *
 * \param [in]  unit - Relevant unit.
 * \param [in] flow_handle_info - flow_handle_info
 * \param [in] cb - user callback
 * \param [in] user_data - user data
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_initiator_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    int global_lif;
    lif_mapping_local_lif_info_t local_lif_info;
    uint32 key_field;
    bcm_flow_initiator_info_t initiator_info;
    bcm_flow_special_fields_t special_fields;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    
    flow_handle_info->flow_handle = 2;

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_flow_initiator_info_traverse_verify(unit, flow_handle_info, cb, user_data));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_GTP, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &key_field));
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = key_field;
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get
                        (unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif));
        BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, global_lif);
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_get(unit, flow_handle_info, &initiator_info, &special_fields));

        /** Invoke callback function */
        SHR_IF_ERR_EXIT((*cb) (unit, flow_handle_info, &initiator_info, &special_fields, user_data));

        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add match entry
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flow_handle_info - pointer to a struct containing the relevant general flow attributes \n
 * \param [in] key_special_fields - list of key fields \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_match_info_add(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_create_verify_common
                          (unit, FLOW_APP_TYPE_MATCH, flow_handle_info, NULL, key_special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_match_info_add(unit, flow_handle_info, key_special_fields));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete match entry
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flow_handle_info - pointer to a struct containing the relevant general flow attributes \n
 * \param [in] key_special_fields - list of key fields \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_match_info_delete(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_match_info_verify(unit, flow_handle_info, key_special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_match_info_delete(unit, flow_handle_info, key_special_fields));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get match entry
 * Note: Get is currently done only by the keys (to retreive the in-lif)
 *
 * \param [in] unit - Relevant unit.
 * \param [in,out] flow_handle_info - pointer to a struct containing the relevant general flow attributes \n
 * \param [in] key_special_fields - list of key fields \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_match_info_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_match_info_verify(unit, flow_handle_info, key_special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_match_info_get(unit, flow_handle_info, key_special_fields));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all entries on match table
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flow_handle_info - flow_handle_info
 * \param [in] cb - cb function to be called \n
 * \param [out] user_data - Additional user data \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_match_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_match_info_traverse_cb cb,
    void *user_data)
{
    bcm_flow_special_fields_t key_special_fields;
    uint32 entry_handle_id;
    int is_end;
    int global_lif;
    lif_mapping_local_lif_info_t local_lif_info;
    uint32 core_id = DBAL_CORE_DEFAULT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_flow_match_info_traverse_verify(unit, cb, user_data));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GTP_CLASSIFICATION, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** Add KEY rule to skip duplicated entry */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                           DBAL_CONDITION_EQUAL_TO, &core_id, NULL));

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        sal_memset(&key_special_fields, 0, sizeof(key_special_fields));
        sal_memset(flow_handle_info, 0, sizeof(bcm_flow_handle_info_t));
        key_special_fields.actual_nof_special_fields = 2;
        key_special_fields.special_fields[0].field_id = DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN;
        key_special_fields.special_fields[1].field_id = DBAL_FIELD_TUNNEL_ENDPOINT_IDENTIFIER;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN,
                         &key_special_fields.special_fields[0].shr_var_uint32));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ENDPOINT_IDENTIFIER,
                         &key_special_fields.special_fields[1].shr_var_uint32));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, (uint32 *) &local_lif_info.local_lif));

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get
                        (unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif));

        BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, global_lif);

        
        flow_handle_info->flow_handle = 3;

        /** Invoke callback function */
        SHR_IF_ERR_EXIT((*cb) (unit, flow_handle_info, &key_special_fields, user_data));

        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get field id according to field name
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flow_handle - handle which relates to the relevant table \n
 * \param [in] field_name - field name string \n
 * \param [out] field_id - the returned field id \n
 *
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
shr_error_e
bcm_dnx_flow_logical_field_id_get(
    int unit,
    bcm_flow_handle_t flow_handle,
    const char *field_name,
    bcm_flow_field_id_t * field_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (!sal_strcasecmp(field_name, "NEXT_LAYER_NETWORK_DOMAIN"))
    {
        *field_id = DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN;
    }
    else if (!sal_strcasecmp(field_name, "TUNNEL_ENDPOINT_IDENTIFIER"))
    {
        *field_id = DBAL_FIELD_TUNNEL_ENDPOINT_IDENTIFIER;
    }
    else if (!sal_strcasecmp(field_name, "TEID"))
    {
        *field_id = DBAL_FIELD_RAW_DATA;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported field %s\n", field_name);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**********************************************************************************************
*                                        APIs - END                                           *
 */
