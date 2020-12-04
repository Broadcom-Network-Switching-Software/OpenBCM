/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/flow.h>
#include <bcm/qos.h>
#include <bcm/l3.h>
#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/qos/qos.h>
#include <sal/appl/sal.h>
#include <shared/utilex/utilex_str.h>
#include <shared/bitop.h>
#include "flow_def.h"
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <bcm_int/dnx/algo/tunnel/algo_tunnel.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#include <soc/dnx/swstate/auto_generated/access/flow_access.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>

extern dnx_flow_app_config_t *dnx_flow_objects[];
extern dnx_flow_common_field_desc_t *dnx_flow_term_common_fields[];
extern dnx_flow_common_field_desc_t *dnx_flow_init_common_fields[];

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

/**********************************************************************************************
*                                        verify functions                                     *
 */

#define DNX_FLOW_HANDLE_VALIDATE(handle_id)                                 \
    if ((handle_id >= dnx_flow_number_of_apps()) || (handle_id < 1))        \
    {                                                                       \
        SHR_ERR_EXIT(_SHR_E_PARAM, "ilegal flow handle ID, %d", handle_id); \
    }

/* Check if verify is needed for flow handle matching application */
#define DNX_FLOW_APP_VERIFY_CHECK(unit,flow_handle,flow_app_info) \
    { \
        uint8 is_verify_disabled; \
        SHR_IF_ERR_EXIT(flow_db.flow_application_info.is_verify_disabled.get(unit, flow_handle, &is_verify_disabled)); \
        if (is_verify_disabled) \
        { \
            SHR_EXIT(); \
        } \
    }

static shr_error_e
dnx_flow_special_fields_validate(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_special_fields_t * special_fields)
{
    int nof_app_special_fields = 0, app_idx, idx;
    uint8 special_fields_set[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (!flow_app_info->special_fields[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid configuration, no special fields in app %s configuration",
                     flow_app_info->app_name);
    }

    while (flow_app_info->special_fields[nof_app_special_fields] != DBAL_FIELD_EMPTY)
    {
        nof_app_special_fields++;
    }

    if (special_fields->actual_nof_special_fields > nof_app_special_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid configuration, requested %d special fields, nof %s app special fields is %d",
                     special_fields->actual_nof_special_fields, flow_app_info->app_name, nof_app_special_fields);
    }

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_MATCH)
    {
        if (special_fields->actual_nof_special_fields != nof_app_special_fields)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "For app_type == MATCH - all key special fields must be configured. #App special fields: %d, got %d",
                         nof_app_special_fields, special_fields->actual_nof_special_fields);
        }
    }

    /** Iterate over the special fields. Make sure that they're supported by the application */
    for (idx = 0; idx < special_fields->actual_nof_special_fields; idx++)
    {
        for (app_idx = 0; app_idx < nof_app_special_fields; app_idx++)
        {
            if (special_fields->special_fields[idx].field_id == flow_app_info->special_fields[app_idx])
            {
                if (special_fields_set[app_idx])
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Special field %s was set multiple times",
                                 dbal_field_to_string(unit, special_fields->special_fields[idx].field_id));
                }
                /*
                 * Mark this field as was already set by user 
                 */
                special_fields_set[app_idx]++;
                break;
            }
        }
        if (app_idx == nof_app_special_fields)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Special field %s is not supported in application %s",
                         dbal_field_to_string(unit, special_fields->special_fields[idx].field_id),
                         flow_app_info->app_name);
        }
        if (FLOW_FIELD_TYPE_IS_IPV4(special_fields->special_fields[idx].field_id))
        {
            if (special_fields->special_fields[idx].shr_var_uint32)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "IPV4 sip, dip fields must be set through shr_var_ipv4 field in special fields");
            }
        }
        else
        {
            if (special_fields->special_fields[idx].shr_var_ipv4)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "shr_var_ipv4 is only relevant for ipv4 type fields");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** for encap access, validate that field is not replaced with different value. */
static shr_error_e
dnx_flow_encap_validate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_encap_access_t encap_access)
{
    bcm_flow_initiator_info_t get_initiator_info = { 0 };
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 lif_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Get GPort hw resources (global and local tunnel outlif) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_flow_field_encap_access_get
                    (unit, 0, FLOW_APP_TYPE_INIT, flow_handle_info, gport_hw_resources, &get_initiator_info));

    if (get_initiator_info.encap_access != encap_access)
    {
        char original_encap_access[FLOW_STR_MAX] = { 0 };
        char new_encap_access[FLOW_STR_MAX] = { 0 };
        SHR_IF_ERR_EXIT(flow_field_encap_access_print_cb
                        (unit, &get_initiator_info.encap_access, original_encap_access));
        SHR_IF_ERR_EXIT(flow_field_encap_access_print_cb(unit, &encap_access, new_encap_access));
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid configuration - cannot replace encap access. Original: %s, new: %s",
                     original_encap_access, new_encap_access);
    }

exit:
    SHR_FUNC_EXIT;
}

/** common function to verify create operations for all app_types */
static shr_error_e
dnx_flow_create_verify_common(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *bcm_app_info,
    bcm_flow_special_fields_t * special_fields)
{
    int field;
    uint32 qos_map_id;
    int num_common_fields = 0;
    uint32 valid_elements_clear = 0;
    uint32 valid_elements_set = 0;
    dnx_flow_app_config_t *flow_app_info;
    dnx_flow_common_field_desc_t **common_fields_struct = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    if (flow_app_type != FLOW_APP_TYPE_MATCH)
    {
        SHR_NULL_CHECK(bcm_app_info, _SHR_E_PARAM, "bcm_info");
    }

    DNX_FLOW_HANDLE_VALIDATE(flow_handle_info->flow_handle);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    DNX_FLOW_APP_VERIFY_CHECK(unit, flow_handle_info->flow_handle, flow_app_info);
    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_type, &num_common_fields));

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        common_fields_struct = dnx_flow_term_common_fields;
        valid_elements_clear = ((bcm_flow_terminator_info_t *) bcm_app_info)->valid_elements_clear;
        valid_elements_set = ((bcm_flow_terminator_info_t *) bcm_app_info)->valid_elements_set;

    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        common_fields_struct = dnx_flow_init_common_fields;
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

    /** General validations */
    if (flow_app_info->flow_app_type != flow_app_type)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mismatch between caller API app type and application app type");
    }

    if (num_common_fields)
    {
        /** Iterate over the common field. Make sure that there're no set flags which are not supported by the application  */
        /** Check if the user has set a valid field on an unsupported application field */
        if ((valid_elements_set ^ flow_app_info->valid_common_fields_bitmap)
            || (valid_elements_clear ^ flow_app_info->valid_common_fields_bitmap))
        {
            for (field = 0; field < num_common_fields; field++)
            {
                if ((_SHR_IS_FLAG_SET(valid_elements_set, (1 << field))
                     || _SHR_IS_FLAG_SET(valid_elements_clear, (1 << field)))
                    && !_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, (1 << field)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported combination: Application %s, common field set, %s (%d)",
                                 flow_app_info->app_name,
                                 common_fields_struct ? common_fields_struct[field]->field_name : "", field);
                }
            }
        }

        /** Make sure that no common field was marked as set and clear at the same time  */
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
        {
            if (valid_elements_set & valid_elements_clear)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid configuration, VALID flag can only be set on either valid_elements_set, valid_elemetns_clear");
            }
        }

        if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE)
            && (!flow_app_info->special_fields[0]))
        {
            if (valid_elements_set == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Nothing is set in valid elements set");
            }
        }
    }

    /** Qos terminator Specific validations */
    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        /** qos verification */
        if (_SHR_IS_FLAG_SET(valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID))
        {
            qos_map_id = ((bcm_flow_terminator_info_t *) bcm_app_info)->qos_map_id;
            if (qos_map_id != DNX_QOS_INITIAL_MAP_ID)
            {
                if (!DNX_QOS_MAP_IS_INGRESS(qos_map_id) || !DNX_QOS_MAP_IS_REMARK(qos_map_id)
                    || !DNX_QOS_MAP_IS_PHB(qos_map_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "invalid qos_map_id, 0x%x", qos_map_id);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "QOS_MAP_ID value is invalid");
            }
        }
    }

    /** Encap access verifications  */
    if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        /** Make sure that in case of replace - encap access remains the same */
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE)
            && _SHR_IS_FLAG_SET(valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID))
        {
            SHR_IF_ERR_EXIT(dnx_flow_encap_validate
                            (unit, flow_handle_info, ((bcm_flow_initiator_info_t *) bcm_app_info)->encap_access));
        }
    }

    /** DEST_INFO specific validations */
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
        /*
         * Qos ingress model validate 
         */
        if (_SHR_IS_FLAG_SET(valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID))
        {
            if ((((bcm_flow_terminator_info_t *) bcm_app_info)->ingress_qos_model.ingress_phb ==
                 bcmQosIngressModelStuck)
                || (((bcm_flow_terminator_info_t *) bcm_app_info)->ingress_qos_model.ingress_remark ==
                    bcmQosIngressModelStuck)
                || (((bcm_flow_terminator_info_t *) bcm_app_info)->ingress_qos_model.ingress_ttl ==
                    bcmQosIngressModelStuck))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported qos model - stuck");
            }
        }

    }

    /** special fields validations */
    if (special_fields)
    {
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_validate(unit, flow_app_info, special_fields));
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

static shr_error_e
dnx_flow_delete_verify_common(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * special_fields)
{
    dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");

    DNX_FLOW_HANDLE_VALIDATE(flow_handle_info->flow_handle);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    DNX_FLOW_APP_VERIFY_CHECK(unit, flow_handle_info->flow_handle, flow_app_info);

    if (special_fields)
    {
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_validate(unit, flow_app_info, special_fields));
    }

    if (flow_app_type == FLOW_APP_TYPE_MATCH && (!special_fields))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Special fields must exists application %s", flow_app_info->app_name);
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
 * Verify flow lif get
 */
static shr_error_e
dnx_flow_get_verify_common(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *info,
    bcm_flow_special_fields_t * special_fields)
{
    dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");

    if (flow_app_type != FLOW_APP_TYPE_MATCH)
    {
        SHR_NULL_CHECK(info, _SHR_E_PARAM, "bcm_info");
    }

    DNX_FLOW_HANDLE_VALIDATE(flow_handle_info->flow_handle);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    DNX_FLOW_APP_VERIFY_CHECK(unit, flow_handle_info->flow_handle, flow_app_info);

    if (special_fields)
    {
        /*
         * If the application supports special fields - special_fields can't be null
         */
        if (flow_app_info->special_fields[0] != DBAL_FIELD_EMPTY)
        {
            SHR_NULL_CHECK(special_fields, _SHR_E_PARAM, "special_fields");
        }
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_validate(unit, flow_app_info, special_fields));
    }

    if (flow_app_type == FLOW_APP_TYPE_MATCH && (!special_fields))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Special fields must exists application %s", flow_app_info->app_name);
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
 * Verify flow lif traverse
 */
static shr_error_e
dnx_flow_traverse_verify_common(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    void *cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb");
    SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "user_data");

    /*
     * If flow handle doesn't indicate to traverse all applications - verify it with common verify 
     */
    if (flow_handle_info->flow_handle != BCM_FLOW_HANDLE_TRAVERSE_ALL)
    {
        DNX_FLOW_HANDLE_VALIDATE(flow_handle_info->flow_handle);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify flow handle control
 */
static shr_error_e
dnx_flow_handle_control_verify(
    int unit,
    bcm_flow_handle_t flow_handle,
    bcm_flow_handle_control_t type,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLOW_HANDLE_VALIDATE(flow_handle);
    if ((type <= bcmFlowHandleControlInvalid) || (type >= bcmbcmFlowHandleControl__Count))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid control type %d", type);
    }
    if ((type == bcmFlowHandleControlVerifyEnable) && ((value != TRUE) && (value != FALSE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flow control verify enable supports 1,0 (got %d)", value);
    }
    else if ((type == bcmFlowHandleControlErrorRecoveryEnable) && ((value != TRUE) && (value != FALSE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flow control error recovery enable supports 1,0 (got %d)", value);
    }

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
 * Find the first set bit position in a uint32 number
 */
int
dnx_flow_first_bit_set_find(
    uint32 number)
{
    int cnt = 0;

    if (number)
    {
        while (!(number & (1 << cnt++)));
    }
    else
    {
        cnt = -1;
    }

    return cnt;
}

/*
 * Internal ingress flow lif replace
 */
static shr_error_e
dnx_flow_commit_error_check(
    int unit,
    int entry_handle_id)
{
    dbal_entry_handle_t *entry_handle_ptr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle_ptr));
    if (entry_handle_ptr->error_info.error_exists)
    {
        if (entry_handle_ptr->error_info.field_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "There was an issue with field %s",
                         dbal_field_to_string(unit, entry_handle_ptr->error_info.field_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error with entry commit");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_initiator_sip_create(
    int unit,
    uint32 sip,
    uint32 entry_handle_id)
{
    uint8 is_first_sip_ref;
    int sip_profile;
    source_address_entry_t source_address_entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Initialize variables */
    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    source_address_entry.address_type = source_address_type_ipv4;
    sal_memcpy(&source_address_entry.address.ipv4_address, &sip, sizeof(bcm_ip_t));

    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.allocate_single
                    (unit, 0, &source_address_entry, NULL, (int *) &sip_profile, &is_first_sip_ref));

    /** write to HW if new SIP index was allocated */
    if (is_first_sip_ref)
    {
        uint32 sip_entry_handle_id;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &sip_entry_handle_id));
        /** Set data field */
        dbal_entry_value_field32_set(unit, sip_entry_handle_id, DBAL_FIELD_IPV4_SIP, INST_SINGLE, sip);

        /** Set key field*/
        dbal_entry_key_field32_set(unit, sip_entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, sip_profile);

        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sip_entry_handle_id, DBAL_COMMIT));
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, sip_profile);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_initiator_sip_get(
    int unit,
    uint32 sip_profile,
    uint32 *sip)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &entry_handle_id));
    /** Set key field*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, sip_profile);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP, INST_SINGLE, sip));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_initiator_sip_delete(
    int unit,
    uint32 sip_profile)
{
    uint32 entry_handle_id;
    uint8 remove_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Free SIP template */
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.free_single
                    (unit, sip_profile, &remove_profile));

    if (remove_profile)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, sip_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_initiator_sip_replace(
    int unit,
    uint32 sip,
    uint32 entry_handle_id)
{
    uint8 is_first_profile_ref;
    uint8 is_last_profile_ref;
    source_address_entry_t source_address_entry;
    uint32 new_sip_profile, old_sip_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, &old_sip_profile));

    /** Initialize variables */
    is_first_profile_ref = 0;
    is_last_profile_ref = 0;
    new_sip_profile = old_sip_profile;
    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    source_address_entry.address_type = source_address_type_ipv4;
    sal_memcpy(&source_address_entry.address.ipv4_address, &sip, sizeof(bcm_ip_t));

    /** Exchange SIP template*/
    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.exchange
                    (unit, 0, &source_address_entry, old_sip_profile, NULL,
                     (int *) &new_sip_profile, &is_first_profile_ref, &is_last_profile_ref));

    /** write to HW if new SIP profile was allocated */
    if (is_first_profile_ref)
    {
        uint32 sip_entry_handle_id;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4, &sip_entry_handle_id));
        /** Set data field */
        dbal_entry_value_field32_set(unit, sip_entry_handle_id, DBAL_FIELD_IPV4_SIP, INST_SINGLE, sip);

        /** Set key field*/
        dbal_entry_key_field32_set(unit, sip_entry_handle_id, DBAL_FIELD_SA_SIP_PROFILE, new_sip_profile);

        /** Write to HW */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sip_entry_handle_id, DBAL_COMMIT));
    }

    /** If last profile was deallocated remove the old entry from HW */
    if (is_last_profile_ref)
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_sip_delete(unit, old_sip_profile));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, new_sip_profile);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_special_fields_set(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    int field;

    SHR_FUNC_INIT_VARS(unit);

    for (field = 0; field < special_fields->actual_nof_special_fields; field++)
    {
        if (special_fields->special_fields[field].field_id == DBAL_FIELD_IPV4_SIP)
        {
            SHR_IF_ERR_EXIT(dnx_flow_initiator_sip_create
                            (unit, special_fields->special_fields[field].shr_var_ipv4, entry_handle_id));
        }
        else if (FLOW_FIELD_TYPE_IS_IPV4(special_fields->special_fields[field].field_id))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, special_fields->special_fields[field].field_id,
                                         INST_SINGLE, special_fields->special_fields[field].shr_var_ipv4);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, special_fields->special_fields[field].field_id,
                                         INST_SINGLE, special_fields->special_fields[field].shr_var_uint32);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_special_fields_replace(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    int field;

    SHR_FUNC_INIT_VARS(unit);

    for (field = 0; field < special_fields->actual_nof_special_fields; field++)
    {
        /*
         * If is_clear is off --> set requested value
         */
        if (!special_fields->special_fields[field].is_clear)
        {
            if (special_fields->special_fields[field].field_id == DBAL_FIELD_IPV4_SIP)
            {
                SHR_IF_ERR_EXIT(dnx_flow_initiator_sip_replace
                                (unit, special_fields->special_fields[field].shr_var_ipv4, entry_handle_id));
            }
            else if (FLOW_FIELD_TYPE_IS_IPV4(special_fields->special_fields[field].field_id))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, special_fields->special_fields[field].field_id,
                                             INST_SINGLE, special_fields->special_fields[field].shr_var_ipv4);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, special_fields->special_fields[field].field_id,
                                             INST_SINGLE, special_fields->special_fields[field].shr_var_uint32);
            }
        }
        /*
         * is_clear is on --> field should be ignored. If existed on previous entry - take its previous value
         */
        else
        {
            int rv;
            uint32 value = 0;
            /*
             * In case of SIP - field is actually the sip profile, SOURCE_IDX
             */
            dbal_fields_e field_id =
                (special_fields->special_fields[field].field_id ==
                 DBAL_FIELD_IPV4_SIP) ? DBAL_FIELD_SOURCE_IDX : special_fields->special_fields[field].field_id;

            rv = dnx_flow_field_value32_get(unit, get_entry_handle_id, field_id, &value);

            if (rv == _SHR_E_NONE)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, value);
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

exit:
    SHR_FUNC_EXIT;
}

/*
 * Iterate over the special fields. If a field exists on entry - get its value and update actual_nof_special_fields
 */
static shr_error_e
dnx_flow_special_fields_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * special_fields)
{
    int sip_field_idx = -1, idx = 0, rv, field;
    uint32 value, num_special_fields_in_app = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->special_fields[0] != DBAL_FIELD_EMPTY)
    {
        /*
         * Get number of application special fields
         */
        while (flow_app_info->special_fields[num_special_fields_in_app] != DBAL_FIELD_EMPTY)
        {
            num_special_fields_in_app++;
        }

        special_fields->actual_nof_special_fields = 0;
        for (field = 0; field < num_special_fields_in_app; field++)
        {
            if (flow_app_info->special_fields[field] == DBAL_FIELD_IPV4_SIP)
            {
                sip_field_idx = field;
                continue;
            }

            rv = dnx_flow_field_value32_get(unit, entry_handle_id, flow_app_info->special_fields[field], &value);
            if (rv == _SHR_E_NONE)
            {
                special_fields->special_fields[idx].field_id = flow_app_info->special_fields[field];
                if (FLOW_FIELD_TYPE_IS_IPV4(flow_app_info->special_fields[field]))
                {
                    special_fields->special_fields[idx].shr_var_ipv4 = value;
                }
                else
                {
                    special_fields->special_fields[idx].shr_var_uint32 = value;
                }
                idx++;
            }
        }
        special_fields->actual_nof_special_fields = idx;
    }

    if (sip_field_idx >= 0)
    {
        rv = dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, &value);
        if (rv == _SHR_E_NONE)
        {
            uint32 sip;
            special_fields->actual_nof_special_fields++;
            SHR_IF_ERR_EXIT(dnx_flow_initiator_sip_get(unit, value, &sip));
            special_fields->special_fields[sip_field_idx].field_id = DBAL_FIELD_IPV4_SIP;
            special_fields->special_fields[sip_field_idx].shr_var_ipv4 = sip;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

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
    bcm_flow_terminator_info_t get_terminator_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_TERM, &num_common_fields));

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
     * get_initiator_info will be used for profiles delete 
     */
    sal_memcpy(&get_terminator_info, terminator_info, sizeof(bcm_flow_terminator_info_t));

    /*
     * Iterate over the flow termination common fields. If requested by the user - call relevant callback
     */
    for (field = 0; field < num_common_fields; field++)
    {
        /*
         * If the application does not support this common field, skip all set/get actions
         * Reasoning:
         * If not skipped, the user should be aware which common fields are not supported for the application
         * and set the corresponding "valid_elements_clear" bit in order to skip get during replace
         */
        if (!_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, (1 << field)))
        {
            continue;
        }
        /*
         * If valid_elements is set --> field should be replaced 
         */
        if (_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, (1 << field)))
        {
            /*
             * For standard fields - perform simple set
             * For profiles etc - a replace cb can be defined, in case the old profile should be deleted after creating the new one etc
             */
            if (dnx_flow_term_common_fields[field]->field_cbs.field_replace_cb)
            {
                SHR_IF_ERR_EXIT(dnx_flow_term_common_fields[field]->
                                field_cbs.field_replace_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM, flow_handle_info,
                                                           gport_hw_resources, terminator_info));
            }
            else if (dnx_flow_term_common_fields[field]->field_cbs.field_set_cb)
            {
                SHR_IF_ERR_EXIT(dnx_flow_term_common_fields[field]->
                                field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM, flow_handle_info,
                                                       gport_hw_resources, terminator_info));
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
                                                                                flow_handle_info, gport_hw_resources,
                                                                                terminator_info);
                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT(dnx_flow_term_common_fields[field]->
                                    field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM, flow_handle_info,
                                                           gport_hw_resources, terminator_info));
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
        /*
         * Valid elements clear - use field default value. If it's a profile field and it was previously set - need to
         * restore to default profile
         */
        else
        {
            if ((dnx_flow_term_common_fields[field]->is_profile)
                && (dnx_flow_term_common_fields[field]->field_cbs.field_replace_cb))
            {
                rv = dnx_flow_term_common_fields[field]->field_cbs.field_get_cb(unit, get_entry_handle_id,
                                                                                FLOW_APP_TYPE_TERM,
                                                                                flow_handle_info, gport_hw_resources,
                                                                                &get_terminator_info);
                /*
                 * If profile existed on entry - need to restore it to default value
                 */
                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT(dnx_flow_term_common_fields[field]->
                                    field_cbs.field_replace_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM,
                                                               flow_handle_info, gport_hw_resources, terminator_info));
                }
            }
        }
    }

    /*
     * Iterate over the special fields 
     */
    if (special_fields)
    {
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_replace
                        (unit, flow_app_info, entry_handle_id, get_entry_handle_id, special_fields));
    }

    /** Check if errors occured on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, flow_handle_info, &gport_hw_resources, terminator_info, special_fields,
                         &inlif_info));
    }

    /** Write to HW */
    inlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    local_inlif = gport_hw_resources.local_in_lif;
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw
                    (unit, _SHR_CORE_ALL, flow_handle_info, entry_handle_id, &local_inlif, &inlif_info));

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
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                    (unit, lif_alloc_flags, DNX_ALGO_LIF_INGRESS, NULL, &global_lif));

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_TERM, &num_common_fields));

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
                                                       gport_hw_resources, terminator_info));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported combination: Application %s, common field set, %s",
                             flow_app_info->app_name, dnx_flow_term_common_fields[field]->field_name);
            }
        }
    }

    /** Iterate over the special fields, and set them */
    if (special_fields)
    {
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit, flow_app_info, entry_handle_id, special_fields));
    }

    /** Check if errors occured on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, flow_handle_info, &gport_hw_resources, terminator_info, special_fields,
                         &lif_table_mngr_inlif_info));
    }

    /*
     * Call lif table manager set
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw
                    (unit, _SHR_CORE_ALL, flow_handle_info, entry_handle_id, &local_inlif, &lif_table_mngr_inlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_flow_initiator_info_global_lif_alloc_info_set(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    global_lif_alloc_info_t * alloc_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID) &&
        (initiator_info->l3_intf_id != 0) && (initiator_info->l3_intf_id != BCM_IF_INVALID))
    {
        /** ipv4 or ipv6 over UDP is possible + GTP. Take the smaller between them */
        if (sal_strcmp(flow_app_info->app_name, "GTP_INITIATOR") == 0)
        {
            alloc_info->bta = dnx_data_lif.out_lif.global_gtp_concat_ip_outlif_bta_sop_get(unit);
        }
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/*
 * Set egress flow lif global lif
 */
static shr_error_e
dnx_flow_initiator_info_create_global_lif(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    int *global_lif_id)
{
    uint32 lif_alloc_flags = 0;
    global_lif_alloc_info_t alloc_info = { 0 };
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** WITH_ID flag is used - get global outLIF ID */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID))
    {
        if (BCM_GPORT_IS_TUNNEL(flow_handle_info->flow_id))
        {
            *global_lif_id = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info->flow_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Flag BCM_FLOW_HANDLE_INFO_WITH_ID does not support flow_id that is not a gport tunnel (flow_id=0x%08x)\n",
                         flow_handle_info->flow_id);
        }
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }

    SHR_IF_ERR_EXIT(dnx_flow_initiator_info_global_lif_alloc_info_set
                    (unit, flow_app_info, initiator_info, &alloc_info));
    /** Allocate LIF - WITH_ID if the flag was set */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                    (unit, lif_alloc_flags, DNX_ALGO_LIF_EGRESS, &alloc_info, global_lif_id));

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sal_memset(&lif_table_mngr_outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_INIT, &num_common_fields));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    /** Create global lif */
    SHR_IF_ERR_EXIT(dnx_flow_initiator_info_create_global_lif
                    (unit, flow_handle_info, flow_app_info, initiator_info, &global_lif_id));

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
                /*
                 * Special treatment for encap access - mandatory for initiator, set on lif table manager outlif info
                 * struct
                 */
                if (dnx_flow_init_common_fields[field]->init_field_enabler ==
                    BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID)
                {
                    SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert(unit,
                                                                                      initiator_info->encap_access,
                                                                                      &lif_table_mngr_outlif_info.logical_phase));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                                    field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT, flow_handle_info,
                                                           gport_hw_resources, initiator_info));
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "common field has no set CB Application %s, common field set, %s",
                             flow_app_info->app_name, dnx_flow_init_common_fields[field]->field_name);
            }
        }
    }

    /** Iterate over the special fields, and set them */
    if (special_fields)
    {
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit, flow_app_info, entry_handle_id, special_fields));
    }

    /** Check if errors occured on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    /*
     * If a specific application callback is defined - call it 
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, flow_handle_info, &gport_hw_resources, initiator_info, special_fields,
                         &lif_table_mngr_outlif_info));
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw
                    (unit, flow_handle_info, entry_handle_id, &local_outlif, &lif_table_mngr_outlif_info));

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
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    int rv;
    int field;
    int num_common_fields;
    bcm_flow_initiator_info_t get_initiator_info = { 0 };
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sal_memset(&lif_table_mngr_outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));
    lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_INIT, &num_common_fields));

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
     * get_initiator_info will be used for profiles delete 
     */
    sal_memcpy(&get_initiator_info, initiator_info, sizeof(bcm_flow_initiator_info_t));

    /*
     * Iterate over the flow initiator common fields. If requested by the user - call relevant callback
     */
    for (field = 0; field < num_common_fields; field++)
    {
        /*
         * If the application does not support this common field, skip all set/get actions
         * Reasoning:
         * If not skipped, the user should be aware which common fields are not supported for the application
         * and set the corresponding "valid_elements_clear" bit in order to skip get during replace
         */
        if (!_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, (1 << field)))
        {
            continue;
        }
        /*
         * If valid_elements_set is set --> field should be replaced 
         */
        if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, (1 << field)))
        {
            /*
             * For standard fields - perform simple set
             * For profiles etc - a replace cb can be defined, in case the old profile should be deleted after creating the new one etc
             */
            if (dnx_flow_init_common_fields[field]->field_cbs.field_replace_cb)
            {
                SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                                field_cbs.field_replace_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT, flow_handle_info,
                                                           gport_hw_resources, initiator_info));
            }
            else if (dnx_flow_init_common_fields[field]->field_cbs.field_set_cb)
            {
                /*
                 * Special treatment for encap access - mandatory for initiator, set on lif table manager outlif info
                 * struct
                 */
                if (dnx_flow_init_common_fields[field]->init_field_enabler ==
                    BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID)
                {
                    SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert(unit,
                                                                                      initiator_info->encap_access,
                                                                                      &lif_table_mngr_outlif_info.logical_phase));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                                    field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT, flow_handle_info,
                                                           gport_hw_resources, initiator_info));
                }
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
                                                                                flow_handle_info, gport_hw_resources,
                                                                                initiator_info);
                if (rv == _SHR_E_NONE)
                {
                    /*
                     * Special treatment for encap access - mandatory for initiator, set on lif table manager outlif
                     * info struct
                     */
                    if (dnx_flow_init_common_fields[field]->init_field_enabler ==
                        BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID)
                    {
                        SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert(unit,
                                                                                          initiator_info->encap_access,
                                                                                          &lif_table_mngr_outlif_info.logical_phase));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                                        field_cbs.field_set_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT,
                                                               flow_handle_info, gport_hw_resources, initiator_info));
                    }
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
        /*
         * Valid elements clear - use field default value. If it's a profile field and it was previously set - need to
         * restore to default profile
         */
        else
        {
            if ((dnx_flow_init_common_fields[field]->is_profile)
                && (dnx_flow_init_common_fields[field]->field_cbs.field_replace_cb))
            {
                rv = dnx_flow_init_common_fields[field]->field_cbs.field_get_cb(unit, get_entry_handle_id,
                                                                                FLOW_APP_TYPE_INIT,
                                                                                flow_handle_info, gport_hw_resources,
                                                                                &get_initiator_info);
                /*
                 * If profile existed on entry - need to replace it with default
                 */
                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT(dnx_flow_init_common_fields[field]->
                                    field_cbs.field_replace_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT,
                                                               flow_handle_info, gport_hw_resources, initiator_info));
                }
            }
        }

    }

    /*
     * Iterate over the special fields 
     */
    if (special_fields)
    {
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_replace
                        (unit, flow_app_info, entry_handle_id, get_entry_handle_id, special_fields));
    }

    /** Check if errors occured on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    /*
     * If a specific application callback is defined - call it 
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, flow_handle_info, &gport_hw_resources, initiator_info, special_fields,
                         &lif_table_mngr_outlif_info));
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw
                    (unit, flow_handle_info, entry_handle_id, &gport_hw_resources.local_out_lif,
                     &lif_table_mngr_outlif_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Internal ingress flow lif get
 */
static shr_error_e
dnx_flow_terminator_info_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    lif_table_mngr_inlif_info_t inlif_info;
    uint32 entry_handle_id = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int field, num_common_fields;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_TERM, &num_common_fields));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /** Take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

    /**  Get tunnel inLIF info from lif tble manager  */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, entry_handle_id, &inlif_info));

    /*
     * Iterate over all common fields in application. Perform get, ignore not_found rv
     * Mark the found fields in the valid_elements_set bitmap
     */
    terminator_info->valid_elements_set = 0;
    for (field = 0; field < num_common_fields; field++)
    {
        if (dnx_flow_term_common_fields[field]->field_cbs.field_get_cb)
        {
            rv = dnx_flow_term_common_fields[field]->field_cbs.field_get_cb(unit,
                                                                            entry_handle_id,
                                                                            FLOW_APP_TYPE_TERM,
                                                                            flow_handle_info,
                                                                            gport_hw_resources, terminator_info);
            if (rv == _SHR_E_NONE)
            {
                terminator_info->valid_elements_set |= dnx_flow_term_common_fields[field]->term_field_enabler;
            }
            else if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv, "Error, retrieving flow terminator field %s",
                             dnx_flow_term_common_fields[field]->field_name);
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_get(unit, flow_app_info, entry_handle_id, special_fields));

exit:
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
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_INIT, &num_common_fields));

    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_flow_initiator_info_get_lif_table_manager_outlif
                    (unit, flow_handle_info, initiator_info, entry_handle_id, &outlif_info));

    /*
     * Iterate over all common fields, and call relevant get callback.
     * If not found - ignore, field didn't exist on entry.
     * Mark the found fields in the valid_elements_set bitmap.
     */
    initiator_info->valid_elements_set = 0;
    for (field = 0; field < num_common_fields; field++)
    {
        if (dnx_flow_init_common_fields[field]->field_cbs.field_get_cb)
        {
            rv = dnx_flow_init_common_fields[field]->field_cbs.field_get_cb(unit,
                                                                            entry_handle_id,
                                                                            FLOW_APP_TYPE_INIT,
                                                                            flow_handle_info,
                                                                            gport_hw_resources, initiator_info);
            if (rv == _SHR_E_NONE)
            {
                initiator_info->valid_elements_set |= dnx_flow_init_common_fields[field]->init_field_enabler;
            }
            else if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv, "Error, retrieving flow initiator field %s",
                             dnx_flow_init_common_fields[field]->field_name);
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_get(unit, flow_app_info, entry_handle_id, special_fields));

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
        if (FLOW_FIELD_TYPE_IS_IPV4(key_special_fields->special_fields[ii].field_id))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, key_special_fields->special_fields[ii].field_id,
                                       key_special_fields->special_fields[ii].shr_var_ipv4);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, key_special_fields->special_fields[ii].field_id,
                                       key_special_fields->special_fields[ii].shr_var_uint32);
        }
    }

    /** Check if errors occured on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                                 gport_hw_resources.local_in_lif);

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, flow_handle_info, &gport_hw_resources, NULL, key_special_fields, NULL));
    }

    /** Check if errors occured on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));
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
        if (FLOW_FIELD_TYPE_IS_IPV4(key_special_fields->special_fields[ii].field_id))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, key_special_fields->special_fields[ii].field_id,
                                       key_special_fields->special_fields[ii].shr_var_ipv4);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, key_special_fields->special_fields[ii].field_id,
                                       key_special_fields->special_fields[ii].shr_var_uint32);
        }
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, flow_handle_info, NULL, NULL, key_special_fields, NULL));
    }

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
    int lif_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    for (ii = 0; ii < key_special_fields->actual_nof_special_fields; ii++)
    {
        if (FLOW_FIELD_TYPE_IS_IPV4(key_special_fields->special_fields[ii].field_id))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, key_special_fields->special_fields[ii].field_id,
                                       key_special_fields->special_fields[ii].shr_var_ipv4);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, key_special_fields->special_fields[ii].field_id,
                                       key_special_fields->special_fields[ii].shr_var_uint32);
        }
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, flow_handle_info, NULL, NULL, key_special_fields, NULL));
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, (uint32 *) &local_lif_info.local_lif));

    /*
     * Retreive flow id from lif 
     */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                    (unit, lif_flags, _SHR_CORE_ALL, local_lif_info.local_lif, &flow_handle_info->flow_id));

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
    int num_apps;
    uint8 is_init;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Initialize flow SW state.
     */
    SHR_IF_ERR_EXIT(flow_db.is_init(unit, &is_init));

    if (!is_init)
    {
        SHR_IF_ERR_EXIT(flow_db.init(unit));
    }

    num_apps = dnx_flow_number_of_apps();
    SHR_IF_ERR_EXIT(flow_db.flow_application_info.alloc(unit, num_apps));

    /*
     * Initialize the FLOW LIF-Table-Mngr APIs
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_init(unit));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/* Flow de-init procedures */
shr_error_e
dnx_flow_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * de-initialize the FLOW LIF-Table-Mngr APIs
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_terminator_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    dnx_flow_app_config_t *flow_app_info;
    bcm_flow_special_fields_t special_fields = { 0 };
    int global_lif;
    lif_mapping_local_lif_info_t local_lif_info;
    uint32 key_field;
    dbal_physical_tables_e physical_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    /*
     * Get physical table id
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                    (unit, flow_app_info->flow_table, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF, &key_field));
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = key_field;
        local_lif_info.phy_table = physical_table_id;
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get
                        (unit, DNX_ALGO_LIF_INGRESS, &local_lif_info, &global_lif));
        BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, global_lif);

        SHR_IF_ERR_EXIT(dnx_flow_terminator_info_get(unit, flow_handle_info, &terminator_info, &special_fields));

        /** Invoke callback function */
        SHR_IF_ERR_EXIT((*cb) (unit, flow_handle_info, &terminator_info, NULL, user_data));

        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_initiator_info_traverse(
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
    dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

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

shr_error_e
dnx_flow_match_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_match_info_traverse_cb cb,
    void *user_data)
{
    bcm_flow_special_fields_t key_special_fields = { 0 };
    uint32 entry_handle_id;
    int is_end;
    int global_lif;
    lif_mapping_local_lif_info_t local_lif_info;
    uint32 core_id = DBAL_CORE_DEFAULT;
    dnx_flow_app_config_t *flow_app_info;
    int nof_app_special_fields = 0, key_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_flow_traverse_verify_common(unit, flow_handle_info, (void *) &cb, user_data));

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** Add KEY rule to skip duplicated entry */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                           DBAL_CONDITION_EQUAL_TO, &core_id, NULL));

    /*
     * Get number of key special fields, serves as key for match applications
     * Assign each key field id
     */
    while (flow_app_info->special_fields[nof_app_special_fields] != DBAL_FIELD_EMPTY)
    {
        key_special_fields.special_fields[nof_app_special_fields].field_id =
            flow_app_info->special_fields[nof_app_special_fields];
        nof_app_special_fields++;
    }

    key_special_fields.actual_nof_special_fields = nof_app_special_fields;

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        /*
         * Get each key
         */
        for (key_idx = 0; key_idx < nof_app_special_fields; key_idx++)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, key_special_fields.special_fields[key_idx].field_id,
                             &key_special_fields.special_fields[key_idx].shr_var_uint32));
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, (uint32 *) &local_lif_info.local_lif));

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get
                        (unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif));

        BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, global_lif);

        /** Invoke callback function */
        SHR_IF_ERR_EXIT((*cb) (unit, flow_handle_info, &key_special_fields, user_data));

        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
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

    SHR_INVOKE_VERIFY_DNX(dnx_flow_delete_verify_common(unit, FLOW_APP_TYPE_TERM, flow_handle_info, NULL));

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_TERM, &num_common_fields));

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
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_term_common_fields[field]->
                                      field_cbs.field_delete_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM,
                                                                flow_handle_info, gport_hw_resources, &terminator_info),
                                      _SHR_E_NOT_FOUND);
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
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_get_verify_common
                          (unit, FLOW_APP_TYPE_TERM, flow_handle_info, terminator_info, special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_terminator_info_get(unit, flow_handle_info, terminator_info, special_fields));

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

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_flow_traverse_verify_common(unit, flow_handle_info, (void *) &cb, user_data));

    /*
     * Traverse all terminator applications 
     */
    if (flow_handle_info->flow_handle == BCM_FLOW_HANDLE_TRAVERSE_ALL)
    {
        int num_apps = dnx_flow_number_of_apps();
        int app_idx;
        bcm_flow_handle_info_t traverse_flow_handle_info = { 0 };
        for (app_idx = FLOW_APP_FIRST_IDX; app_idx < num_apps; app_idx++)
        {
            if (dnx_flow_objects[app_idx]->flow_app_type == FLOW_APP_TYPE_TERM)
            {
                traverse_flow_handle_info.flow_handle = app_idx;
                SHR_IF_ERR_EXIT(dnx_flow_terminator_info_traverse(unit, &traverse_flow_handle_info, cb, user_data));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_terminator_info_traverse(unit, flow_handle_info, cb, user_data));
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
    dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_delete_verify_common(unit, FLOW_APP_TYPE_INIT, flow_handle_info, NULL));

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, FLOW_APP_TYPE_INIT, &num_common_fields));

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

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
                                                                flow_handle_info, gport_hw_resources, &initiator_info),
                                      _SHR_E_NOT_FOUND);
        }
    }

    if (flow_app_info->flow_table == DBAL_TABLE_EEDB_IPV4_TUNNEL)
    {
        uint32 sip_profile;
        uint8 is_valid;

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                        (unit, _SHR_CORE_ALL, gport_hw_resources.local_out_lif, flow_app_info->flow_table, FALSE,
                         DBAL_FIELD_SOURCE_IDX, &is_valid));

        if (is_valid)
        {
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, &sip_profile));
            SHR_IF_ERR_EXIT(dnx_flow_initiator_sip_delete(unit, sip_profile));
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

    SHR_INVOKE_VERIFY_DNX(dnx_flow_get_verify_common
                          (unit, FLOW_APP_TYPE_INIT, flow_handle_info, initiator_info, special_fields));

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
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_flow_traverse_verify_common(unit, flow_handle_info, (void *) &cb, user_data));

    /*
     * Traverse all initiator applications 
     */
    if (flow_handle_info->flow_handle == BCM_FLOW_HANDLE_TRAVERSE_ALL)
    {
        int num_apps = dnx_flow_number_of_apps();
        int app_idx;
        bcm_flow_handle_info_t traverse_flow_handle_info = { 0 };
        for (app_idx = FLOW_APP_FIRST_IDX; app_idx < num_apps; app_idx++)
        {
            if (dnx_flow_objects[app_idx]->flow_app_type == FLOW_APP_TYPE_INIT)
            {
                traverse_flow_handle_info.flow_handle = app_idx;
                SHR_IF_ERR_EXIT(dnx_flow_initiator_info_traverse(unit, &traverse_flow_handle_info, cb, user_data));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_traverse(unit, flow_handle_info, cb, user_data));
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

    SHR_INVOKE_VERIFY_DNX(dnx_flow_delete_verify_common
                          (unit, FLOW_APP_TYPE_MATCH, flow_handle_info, key_special_fields));

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

    SHR_INVOKE_VERIFY_DNX(dnx_flow_get_verify_common
                          (unit, FLOW_APP_TYPE_MATCH, flow_handle_info, NULL, key_special_fields));

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
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_flow_traverse_verify_common(unit, flow_handle_info, (void *) &cb, user_data));

    /*
     * Traverse all match applications 
     */
    if (flow_handle_info->flow_handle == BCM_FLOW_HANDLE_TRAVERSE_ALL)
    {
        int num_apps = dnx_flow_number_of_apps();
        int app_idx;
        bcm_flow_handle_info_t traverse_flow_handle_info = { 0 };
        for (app_idx = FLOW_APP_FIRST_IDX; app_idx < num_apps; app_idx++)
        {
            if (dnx_flow_objects[app_idx]->flow_app_type == FLOW_APP_TYPE_MATCH)
            {
                traverse_flow_handle_info.flow_handle = app_idx;
                SHR_IF_ERR_EXIT(dnx_flow_match_info_traverse(unit, &traverse_flow_handle_info, cb, user_data));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_match_info_traverse(unit, flow_handle_info, cb, user_data));
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
 * \param [in] flow_handle - handle which relates to the relevant FLOW application\n
 * \param [in] field_name - the requested field name string \n
 * \param [out] field_id - the relevant field id that is related to this field string\n
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
    dnx_flow_app_config_t *flow_app_info;
    int iter;
    int nof_app_special_fields;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    DNX_FLOW_HANDLE_VALIDATE(flow_handle);

    flow_app_info = dnx_flow_objects[flow_handle];

    nof_app_special_fields = sizeof(flow_app_info->special_fields) / sizeof(flow_app_info->special_fields[0]);

    /** running over all the special fields related to the application and look for corresponding field */
    for (iter = 0; iter < nof_app_special_fields; iter++)
    {
        if (!sal_strcasecmp(field_name, dbal_field_to_string(unit, flow_app_info->special_fields[iter])))
        {
            *field_id = flow_app_info->special_fields[iter];
            break;
        }
    }

    if (iter == nof_app_special_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported field %s for appilcation %s\n", field_name, flow_app_info->app_name);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/* Set flow handle matching application property according to property enum */
shr_error_e
bcm_dnx_flow_handle_control_set(
    int unit,
    bcm_flow_handle_t flow_handle,
    bcm_flow_handle_control_t type,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_flow_handle_control_verify(unit, flow_handle, type, value));

    switch (type)
    {
        case bcmFlowHandleControlVerifyEnable:
            SHR_IF_ERR_EXIT(flow_db.flow_application_info.is_verify_disabled.set(unit, flow_handle, !value));
            break;
        case bcmFlowHandleControlErrorRecoveryEnable:
            SHR_IF_ERR_EXIT(flow_db.flow_application_info.is_error_recovery_disabled.set(unit, flow_handle, !value));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported type %d", type);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/* Get flow handle matching application property according to property enum */
shr_error_e
bcm_dnx_flow_handle_control_get(
    int unit,
    bcm_flow_handle_t flow_handle,
    bcm_flow_handle_control_t type,
    uint32 *value)
{
    uint8 val = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    *value = 0;

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_flow_handle_control_verify(unit, flow_handle, type, *value));

    switch (type)
    {
        case bcmFlowHandleControlVerifyEnable:
            SHR_IF_ERR_EXIT(flow_db.flow_application_info.is_verify_disabled.get(unit, flow_handle, &val));
            break;
        case bcmFlowHandleControlErrorRecoveryEnable:
            SHR_IF_ERR_EXIT(flow_db.flow_application_info.is_error_recovery_disabled.get(unit, flow_handle, &val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported type %d", type);
    }
    *value = !val;

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/* Initialize bcm_flow_initiator_info_t struct*/
void
bcm_dnx_flow_initiator_info_t_init(
    bcm_flow_initiator_info_t * info)
{
    if (info != NULL)
    {
        sal_memset(info, 0, sizeof(bcm_flow_initiator_info_t));
    }
    return;
}

/* Initialize bcm_flow_terminator_info_t struct*/
void
bcm_dnx_flow_terminator_info_t_init(
    bcm_flow_terminator_info_t * info)
{
    if (info != NULL)
    {
        sal_memset(info, 0, sizeof(bcm_flow_terminator_info_t));
    }
    return;
}

/* Initialize bcm_flow_special_fields_t struct*/
void
bcm_dnx_flow_special_fields_t_init(
    bcm_flow_special_fields_t * info)
{
    if (info != NULL)
    {
        sal_memset(info, 0, sizeof(bcm_flow_special_fields_t));
    }
    return;
}

/* Initialize bcm_flow_dest_info_t struct*/
void
bcm_dnx_flow_dest_info_t_init(
    bcm_flow_dest_info_t * info)
{
    if (info != NULL)
    {
        sal_memset(info, 0, sizeof(bcm_flow_dest_info_t));
    }
    return;
}

/* Initialize bcm_flow_l3_ingress_info_t struct*/
void
bcm_dnx_flow_l3_ingress_info_t_init(
    bcm_flow_l3_ingress_info_t * info)
{
    if (info != NULL)
    {
        sal_memset(info, 0, sizeof(bcm_flow_l3_ingress_info_t));
    }
    return;
}
/**********************************************************************************************
*                                        APIs - END                                           *
 */
