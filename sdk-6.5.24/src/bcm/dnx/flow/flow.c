#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/flow/flow.h>
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
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <bcm_int/dnx/algo/tunnel/algo_tunnel.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#include <soc/dnx/swstate/auto_generated/access/flow_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_table_mngr_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <soc/dnx/field/tcam/tcam_handler.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include "src/bcm/dnx/algo/lif_mngr/auto_generated/dbx_pre_compiled_lif_mdb_auto_generated_xml_parser.h"
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

extern dnx_flow_app_config_t *dnx_flow_objects[];
extern dnx_flow_common_field_desc_t *dnx_flow_term_common_fields[];
extern dnx_flow_common_field_desc_t *dnx_flow_init_common_fields[];
extern flow_special_field_info_t flow_special_fields_db[];

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

#define DNX_FLOW_VALID_PHASES_PER_DBAL_TABLE_NOF_ELEMENTS 100
#define DNX_FLOW_DBAL_TABLE_PHASE_TO_VALID_RES_TYPES_NOF_ELEMENTS 500
#define FLOW_INITIATOR_SUPPORTED_FLAGS  (0)
#define FLOW_TERMINATOR_SUPPORTED_FLAGS (0)

/* Set encap access - if given by user (!= invalid) --> use it. Otherwise - take default */
#define DNX_FLOW_SET_ENCAP_ACCESS(encap_access, initiator_encap_access, default_encap_access) \
        encap_access = (initiator_encap_access == bcmEncapAccessInvalid) ? default_encap_access : initiator_encap_access;

#define FLOW_ILLEGAL_FLAG_CHECK(flag, str)                              \
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, flag))                \
    {                                                                   \
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flag %s is not supported for operation", str);    \
    }

static shr_error_e dnx_flow_match_key_fields_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dbal_table_type_e table_type,
    bcm_flow_special_fields_t * key_special_fields,
    int nof_app_special_fields);

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
flow_special_fields_validate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_special_fields_t * special_fields)
{
    int nof_app_special_fields = 0, app_idx, idx;
    uint8 special_fields_set[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS] = { 0 };
    const flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);

    while (flow_app_info->special_fields[nof_app_special_fields] != FLOW_S_F_EMPTY)
    {
        nof_app_special_fields++;
    }

    if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
    {
        if (special_fields->actual_nof_special_fields < nof_app_special_fields)
        {
            /** for tcam tables user can add more than the number of app special fields because of mask fields. */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "For Match applications - all key special fields must be configured. #App special fields: %d, got %d",
                         nof_app_special_fields, special_fields->actual_nof_special_fields);
        }
    }
    else
    {
        if (special_fields->actual_nof_special_fields > nof_app_special_fields)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid configuration, requested %d special fields, nof %s app special fields is %d",
                         special_fields->actual_nof_special_fields, flow_app_info->app_name, nof_app_special_fields);
        }
    }

    /** Iterate over the special fields. Make sure that they're supported by the application */
    for (idx = 0; idx < special_fields->actual_nof_special_fields; idx++)
    {
        flow_special_fields_e user_field = special_fields->special_fields[idx].field_id;

        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, user_field, &curr_field));

        if ((curr_field->payload_type != FLOW_PAYLOAD_TYPE_UINT32)
            && special_fields->special_fields[idx].shr_var_uint32)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Special field %s is not unit32 type, which means the shr_var_uint32 should be 0 and not %d",
                         dnx_flow_special_field_to_string(unit, user_field),
                         special_fields->special_fields[idx].shr_var_uint32);
        }

        if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE) &&
            special_fields->special_fields[idx].is_clear)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "illegal use of is_clear special field %s in create",
                         dnx_flow_special_field_to_string(unit, user_field));
        }

        /** in case of replace, cannot reset or clear unchangeable lif special field */
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE) &&
            FLOW_APP_IS_MATCH(flow_app_info->flow_app_type) &&
            dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MATCH_UNCHANGEABLE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot change or clear unchangeable special field %s in replace",
                         dnx_flow_special_field_to_string(unit, user_field));
        }

        /** in case of replace, cannot reset or clear unchangeable lif special field */
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE) &&
            FLOW_APP_IS_LIF(flow_app_info->flow_app_type) &&
            dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_UNCHANGEABLE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot change or clear unchangeable special field %s in replace",
                         dnx_flow_special_field_to_string(unit, user_field));
        }

        /** in case of replace, cannot clear mandatory match special fields */
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE) &&
            FLOW_APP_IS_MATCH(flow_app_info->flow_app_type) &&
            dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MATCH_MANDATORY) &&
            special_fields->special_fields[idx].is_clear)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot clear mandatory special field %s in replace",
                         dnx_flow_special_field_to_string(unit, user_field));
        }
        /** in case of replace, cannot clear mandatory lif special fields */
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE) &&
            FLOW_APP_IS_LIF(flow_app_info->flow_app_type) &&
            dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_MANDATORY) &&
            special_fields->special_fields[idx].is_clear)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot clear mandatory special field %s in replace",
                         dnx_flow_special_field_to_string(unit, user_field));
        }

        for (app_idx = 0; app_idx < nof_app_special_fields; app_idx++)
        {
            if (user_field == flow_app_info->special_fields[app_idx])
            {
                if (special_fields_set[app_idx])
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Special field %s was set multiple times",
                                 dnx_flow_special_field_to_string(unit, user_field));
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
                         dnx_flow_special_field_to_string(unit, user_field), flow_app_info->app_name);
        }
    }
    /** in create, pass on all special fields, verify that all mandatory fields were set */
    if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        for (app_idx = 0; app_idx < nof_app_special_fields; app_idx++)
        {
            /** if special field was set, no need to check it */
            if (special_fields_set[app_idx])
            {
                continue;
            }

            SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, flow_app_info->special_fields[app_idx], &curr_field));

            /** if special field was not set, verify it is not mandatory or unchangeable */
            /** match verify */
            if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type) &&
                (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MATCH_MANDATORY) ||
                 dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MATCH_UNCHANGEABLE)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Special field %s is mandatory for match applications and must be set",
                             dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[app_idx]));
            }
            /** lif verify */
            if (FLOW_APP_IS_LIF(flow_app_info->flow_app_type) &&
                (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_MANDATORY) ||
                 dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_UNCHANGEABLE)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Special field %s is mandatory for lif applications and must be set",
                             dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[app_idx]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** for encap access, validate that field is not replaced with different value.
 *  The encap phases aren't relevant only for EEDB entries, not virtual entries. */
static shr_error_e
dnx_flow_encap_validate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info)
{
    bcm_encap_access_t tmp_encap_access, encap_access_default_mapping, encap_access;
    dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    encap_access = initiator_info->encap_access;
    encap_access_default_mapping = flow_app_info->encap_access_default_mapping;

    DNX_FLOW_SET_ENCAP_ACCESS(tmp_encap_access, encap_access, encap_access_default_mapping);

    /*
     * For create - make sure that encap id is valid according to arch (sw state) 
     */
    if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        uint32 encap_access_bitmap[1];
        uint8 found;
        dnx_algo_local_outlif_logical_phase_e logical_phase;

        /*
         * Encap access can't be invalid and must be in range
         */
        if ((tmp_encap_access <= bcmEncapAccessInvalid) || (tmp_encap_access > bcmEncapAccessAc))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Encap access is invalid");
        }

        SHR_IF_ERR_EXIT(flow_db.
                        valid_phases_per_dbal_table.find(unit, &flow_app_info->flow_table, &encap_access_bitmap[0],
                                                         &found));

        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "entry not found in applications valid encap access hash table for application %s",
                         flow_app_info->app_name);
        }
        SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert(unit, tmp_encap_access, &logical_phase));
        if (!SHR_IS_BITSET(encap_access_bitmap, logical_phase))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "encap access %s is not supported for application %s", flow_app_info->app_name,
                         flow_encap_access_to_string(unit, tmp_encap_access));
        }
    }

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE)
        && (encap_access != bcmEncapAccessInvalid))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "On replace, encap access must be set to bcmEncapAccessInvalid");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
common_fields_validate(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    uint32 flags,
    void *bcm_app_info)
{
    uint32 valid_elements_clear = 0;
    uint32 valid_elements_set = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        valid_elements_clear = ((bcm_flow_terminator_info_t *) bcm_app_info)->valid_elements_clear;
        valid_elements_set = ((bcm_flow_terminator_info_t *) bcm_app_info)->valid_elements_set;
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        valid_elements_clear = ((bcm_flow_initiator_info_t *) bcm_app_info)->valid_elements_clear;
        valid_elements_set = ((bcm_flow_initiator_info_t *) bcm_app_info)->valid_elements_set;
    }
    else if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
    {
        valid_elements_clear = 0;
        valid_elements_set = 0;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid flow app type, %d", flow_app_info->flow_app_type);
    }

    /** Make sure that the user has set a valid field on an unsupported application field */
    if ((~flow_app_info->valid_common_fields_bitmap) & valid_elements_set)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Unsupported common bitmap (0x%x) was set to application %s, valid common bitmap is 0x%x",
                     valid_elements_set, flow_app_info->app_name, flow_app_info->valid_common_fields_bitmap);
    }
    /** Make sure that the user has cleared a valid field on an unsupported application field */
    if ((~flow_app_info->valid_common_fields_bitmap) & valid_elements_clear)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Unsupported common bitmap (0x%x) was cleared to application %s, valid common bitmap is 0x%x",
                     valid_elements_clear, flow_app_info->app_name, flow_app_info->valid_common_fields_bitmap);
    }

    /** Make sure that no common field was marked as set and clear at the same time  */
    if (_SHR_IS_FLAG_SET(flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        if (valid_elements_set & valid_elements_clear)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid configuration, VALID flag can only be set on either valid_elements_set, valid_elemetns_clear");
        }
    }

    /** make sure that in case of create, and has no special fields, at least one common fields was set */
    if (!_SHR_IS_FLAG_SET(flags, BCM_FLOW_HANDLE_INFO_REPLACE) && (!flow_app_info->special_fields[0]))
    {
        if (valid_elements_set == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Nothing is set in valid elements set");
        }
    }

    /** make sure that in case of termination, l2 and l3 ingress info were not both set */
    if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) &&
        (valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
        (valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Cannot set both BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID and BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_term_specific_validations(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * bcm_app_info,
    bcm_flow_special_fields_t * special_fields)
{
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(bcm_app_info->flags, ~FLOW_TERMINATOR_SUPPORTED_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid TERMINATOR app 'flags' provided - flags = %x, supported_flags = %x",
                     bcm_app_info->flags, FLOW_TERMINATOR_SUPPORTED_FLAGS);
    }
    /** BCM_FLOW_HANDLE_NATIVE not supported for term for now */
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_NATIVE, "BCM_FLOW_HANDLE_NATIVE");

    /** QOS verification */
    if (_SHR_IS_FLAG_SET(bcm_app_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID))
    {
        uint32 qos_map_id = bcm_app_info->qos_map_id;
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

    /** QOS ingress model validate  */
    if (_SHR_IS_FLAG_SET(bcm_app_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID))
    {
        if ((bcm_app_info->ingress_qos_model.ingress_phb == bcmQosIngressModelStuck)
            || (bcm_app_info->ingress_qos_model.ingress_remark == bcmQosIngressModelStuck)
            || (bcm_app_info->ingress_qos_model.ingress_ttl == bcmQosIngressModelStuck))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported qos model - stuck");
        }
    }

    /** l2 learn info flag validation */
    if (_SHR_IS_FLAG_SET(bcm_app_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID))
    {
        /** verify that the flag BCM_FLOW_L2_LEARN_INFO_DEST_ONLY is set just if FEC.
         * Because just for FEC, need to know if it is with LIF or not.
         * Without this verification, it will be impossible to know if need to add the flag in "get" API
         */
        if ((_SHR_IS_FLAG_SET(bcm_app_info->l2_learn_info.l2_learn_info_flags, BCM_FLOW_L2_LEARN_INFO_DEST_ONLY)) &&
            (!_SHR_GPORT_IS_FORWARD_PORT(bcm_app_info->l2_learn_info.dest_port)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_FLOW_L2_LEARN_INFO_DEST_ONLY flag expected just for dest_port type FEC");
        }
    }

    /** ingress info validations */
    /** service_type part - not relevant for vlan_port_ll_terminator app in JR2 */
    if (sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR, FLOW_STR_MAX) ||
        dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
    {
        if ((bcm_app_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
            (bcm_app_info->l3_ingress_info.service_type >= bcmFlowServiceTypeCount))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Service type configured %d, max value is %d",
                         bcm_app_info->l3_ingress_info.service_type, (bcmFlowServiceTypeCount - 1));
        }
        if ((bcm_app_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
            (bcm_app_info->l2_ingress_info.service_type >= bcmFlowServiceTypeCount))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Service type configured %d, max value is %d",
                         bcm_app_info->l2_ingress_info.service_type, (bcmFlowServiceTypeCount - 1));
        }
    }

    /** restrictions which are relevant only for application that have both l2_ingress_info and l3_ingress_info */
    if ((flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID))
    {
        /** l2 ingress info validation - cannot receive default value */
        if ((bcm_app_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
            (bcm_app_info->l2_ingress_info.ingress_network_group_id == DEFAULT_IN_LIF_ORIENTATION))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID param cannot be default value (%d) in app %s",
                         DEFAULT_IN_LIF_ORIENTATION, flow_app_info->app_name);
        }

        /** l3 ingress info validation - cannot receive default value */
        if ((bcm_app_info->valid_elements_set & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
            (bcm_app_info->l3_ingress_info.urpf_mode == bcmL3IngressUrpfDisable))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID param cannot be default value (%d) in app %s",
                         bcmL3IngressUrpfDisable, flow_app_info->app_name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_qos_validate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * bcm_app_info,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_special_field_t special_field_data;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    if ((!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE)) &&
        (!_SHR_IS_FLAG_SET(bcm_app_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID)))
    {
        if (special_fields)
        {
            rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_DSCP, &special_field_data);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "cannot set special_field FLOW_S_F_QOS_DSCP without setting common field BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID");
            }

            rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_EXP, &special_field_data);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "cannot set special_field FLOW_S_F_QOS_EXP without setting common field BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID");
            }

            rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_PRI, &special_field_data);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "cannot set special_field FLOW_S_F_QOS_PRI without setting common field BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID");
            }

            rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_CFI, &special_field_data);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "cannot set special_field FLOW_S_F_QOS_CFI without setting common field BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID");
            }
        }
    }
    else if ((!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE)) &&
             (_SHR_IS_FLAG_SET(bcm_app_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID)))
    {
        /** if BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID was set, check that either both pri and cfi were set, or none of them */
        uint8 pri_found, cfi_found;

        rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_PRI, &special_field_data);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        pri_found = (rv == _SHR_E_NONE) ? TRUE : FALSE;

        rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_QOS_CFI, &special_field_data);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        cfi_found = (rv == _SHR_E_NONE) ? TRUE : FALSE;

        if (pri_found != cfi_found)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "special_fields FLOW_S_F_QOS_PRI and FLOW_S_F_QOS_CFI must be set together");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_mandatory_l3_intf_validate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * bcm_app_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** in create, check if l3_intf is mandatory and was not set */
    if ((!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE)) &&
        (!(bcm_app_info->valid_elements_set & BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID is a mandatory field that must be set");
    }
    /** in replace, check if l3_intf is mandatory and was cleared */
    if ((_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE)) &&
        (bcm_app_info->valid_elements_clear & BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID is a mandatory field and cannot be cleared");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
flow_init_specific_validations(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * bcm_app_info,
    bcm_flow_special_fields_t * special_fields)
{
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    int is_virtual_lif;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(bcm_app_info->flags, ~FLOW_INITIATOR_SUPPORTED_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid initiator app 'flags' provided - flags = %x, supported_flags = %x",
                     bcm_app_info->flags, FLOW_INITIATOR_SUPPORTED_FLAGS);
    }

    /** Encap access validation handling, relevant only for EEDB entries, not for virtual entries */
    is_virtual_lif = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF);
    if (!is_virtual_lif)
    {
        /** Encap access validation handling  */
        SHR_IF_ERR_EXIT(dnx_flow_encap_validate(unit, flow_handle_info, bcm_app_info));
    }

    /** Qos validations */
    SHR_IF_ERR_EXIT(dnx_flow_qos_validate(unit, flow_handle_info, bcm_app_info, special_fields));

    /** mandatory l3_intf validation */
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_MANDATORY_L3_INTF))
    {
        SHR_IF_ERR_EXIT(dnx_flow_mandatory_l3_intf_validate(unit, flow_handle_info, bcm_app_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/** common function to verify create operations for all app_types */
static shr_error_e
dnx_flow_create_verify(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    void *bcm_app_info,
    bcm_flow_special_fields_t * special_fields)
{
    int num_common_fields = 0;
    dnx_flow_app_config_t *flow_app_info;
    dnx_flow_app_type_e flow_app_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    DNX_FLOW_HANDLE_VALIDATE(flow_handle_info->flow_handle);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    flow_app_type = flow_app_info->flow_app_type;
    if ((flow_app_type == FLOW_APP_TYPE_TERM) || (flow_app_type == FLOW_APP_TYPE_INIT))
    {
        SHR_NULL_CHECK(bcm_app_info, _SHR_E_PARAM, "bcm_info");
    }

    DNX_FLOW_APP_VERIFY_CHECK(unit, flow_handle_info->flow_handle, flow_app_info);
    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_type, &num_common_fields));

    /** General validations */
    if (flow_app_info->flow_app_type != flow_app_type)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mismatch between caller API app type and application app type");
    }

    if (num_common_fields)
    {
        SHR_IF_ERR_EXIT(common_fields_validate(unit, flow_app_info, flow_handle_info->flags, bcm_app_info));
    }

    /** Verify that the only allowed flow handler flags are used */
    if ((flow_handle_info->flags & flow_app_info->valid_flow_flags) != flow_handle_info->flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid configuration, a set flow handler flag isn't valid for application %s",
                     flow_app_info->app_name);
    }

    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL, "BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL");

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC, "BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC");
        FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_NATIVE, "BCM_FLOW_HANDLE_NATIVE");
        FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM, "BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM");
        if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Flag BCM_FLOW_HANDLE_INFO_WITH_ID Must be set for replace operations");
        }
    }

    /** BUD flag validations */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD))
    {
        if (!FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BUD is supported only for Match applications");
        }
        if (flow_app_info->second_pass_table == DBAL_TABLE_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BUD is not supported for this application");
        }
    }

    /** core_bm validations */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM))
    {
        int core_iter, nof_cores_set = 0;
        int nof_cores = dnx_data_device.general.nof_cores_get(unit);
        for (core_iter = 0; core_iter < nof_cores; core_iter++)
        {
            if (flow_handle_info->core_bitmap & SAL_BIT(core_iter))
            {
                nof_cores_set++;
            }
        }
        if ((nof_cores_set == nof_cores) || (nof_cores_set == 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for core_bitmap (%x), setting  all/no cores is not legal",
                         flow_handle_info->core_bitmap);
        }
    }
    else
    {
        if (flow_handle_info->core_bitmap != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When using core_bitmap (%x) BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM must be set",
                         flow_handle_info->core_bitmap);
        }
    }

    if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_IF_ERR_EXIT(flow_init_specific_validations
                        (unit, flow_handle_info, ((bcm_flow_initiator_info_t *) bcm_app_info), special_fields));
    }

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(flow_term_specific_validations
                        (unit, flow_handle_info, ((bcm_flow_terminator_info_t *) bcm_app_info), special_fields));
    }

    /** special fields validations */
    if (special_fields)
    {
        SHR_IF_ERR_EXIT(flow_special_fields_validate(unit, flow_handle_info, flow_app_info, special_fields));
    }

    
    if ((_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID)) &&
        (!(_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))))
    {
        uint8 is_allocated = FALSE;
        int global_lif = _SHR_GPORT_FLOW_LIF_VAL_GET(flow_handle_info->flow_id);

        if (!global_lif)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Gport is not given when flag BCM_FLOW_HANDLE_INFO_WITH_ID is set");
        }

        SHR_IF_ERR_EXIT(flow_lif_mgmt_is_gport_allocated(unit, flow_handle_info, flow_app_info, &is_allocated));

        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS,
                         "Error, flag BCM_FLOW_HANDLE_INFO_WITH_ID with an already allocated gport tunnel (flow_id=0x%08x)\n",
                         flow_handle_info->flow_id);
        }
    }

    /*
     * Invoke specific verify function Do we need casting here for the bcm_app_info to the correct bcm struct
     */
    if (flow_app_info->verify_cb)
    {
        dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_SET, FLOW_COMMAND_CONTINUE };

        SHR_IF_ERR_EXIT(flow_app_info->verify_cb
                        (unit, flow_handle_info, &flow_cmd_control, bcm_app_info, special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_delete_verify(
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

    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_REPLACE, "BCM_FLOW_HANDLE_INFO_REPLACE");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC, "BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL, "BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_NATIVE, "BCM_FLOW_HANDLE_NATIVE");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM, "BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM");

    if (special_fields)
    {
        SHR_IF_ERR_EXIT(flow_special_fields_validate(unit, flow_handle_info, flow_app_info, special_fields));
    }

    if ((FLOW_APP_IS_MATCH(flow_app_type)) && (!special_fields))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Special fields must exist in application %s", flow_app_info->app_name);
    }

    /** BUD flag validations   */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD))
    {
        if (!(FLOW_APP_IS_MATCH(flow_app_info->flow_app_type)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BUD is supported only for Match applications");
        }
        if (flow_app_info->second_pass_table == DBAL_TABLE_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BUD is not supported for this application");
        }
    }

    /*
     * Invoke specific verify function Do we need casting here for the bcm_app_info to the correct bcm struct
     */
    if (flow_app_info->verify_cb)
    {
        dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_DELETE, FLOW_COMMAND_CONTINUE };

        SHR_IF_ERR_EXIT(flow_app_info->verify_cb(unit, flow_handle_info, &flow_cmd_control, NULL, special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify flow lif get
 */
static shr_error_e
dnx_flow_get_verify(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *info,
    bcm_flow_special_fields_t * special_fields)
{
    dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");

    if ((flow_app_type == FLOW_APP_TYPE_TERM) || (flow_app_type == FLOW_APP_TYPE_INIT))
    {
        SHR_NULL_CHECK(info, _SHR_E_PARAM, "bcm_info");
    }

    DNX_FLOW_HANDLE_VALIDATE(flow_handle_info->flow_handle);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    DNX_FLOW_APP_VERIFY_CHECK(unit, flow_handle_info->flow_handle, flow_app_info);

    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_REPLACE, "BCM_FLOW_HANDLE_INFO_REPLACE");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC, "BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL, "BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_NATIVE, "BCM_FLOW_HANDLE_NATIVE");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM, "BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM");

    /*
     * If the application supports special fields - special_fields can't be NULL
     */
    if (flow_app_info->special_fields[0] != FLOW_S_F_EMPTY)
    {
        SHR_NULL_CHECK(special_fields, _SHR_E_PARAM, "special_fields");
    }

    /** BUD flag validations   */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD))
    {
        if (!(FLOW_APP_IS_MATCH(flow_app_info->flow_app_type)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BUD is supported only for Match applications");
        }
        if (flow_app_info->second_pass_table == DBAL_TABLE_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BUD is not supported for this application");
        }
    }

    /*
     * Invoke specific verify function Do we need casting here for the bcm_app_info to the correct bcm struct
     */
    if (flow_app_info->verify_cb)
    {
        dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_GET, FLOW_COMMAND_CONTINUE };

        SHR_IF_ERR_EXIT(flow_app_info->verify_cb(unit, flow_handle_info, &flow_cmd_control, NULL, special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify flow lif traverse
 */
static shr_error_e
dnx_flow_traverse_verify(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    void *cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_handle_info, _SHR_E_PARAM, "flow_handle_info");
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb");

    if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL))
    {
        /** in case BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL is set no need to pass user data */
        SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "user_data");
    }

    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_REPLACE, "BCM_FLOW_HANDLE_INFO_REPLACE");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC, "BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_NATIVE, "BCM_FLOW_HANDLE_NATIVE");
    FLOW_ILLEGAL_FLAG_CHECK(BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM, "BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM");

    /*
     * If flow handle doesn't indicate to traverse all applications - verify it with common verify 
     */
    if (flow_handle_info->flow_handle != BCM_FLOW_HANDLE_TRAVERSE_ALL)
    {
        dnx_flow_app_config_t *flow_app_info;
        DNX_FLOW_HANDLE_VALIDATE(flow_handle_info->flow_handle);
        flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

        /** BUD flag validations   */
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD))
        {
            if (!(FLOW_APP_IS_MATCH(flow_app_info->flow_app_type)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "BUD is supported only for Match applications");
            }
            if (flow_app_info->second_pass_table == DBAL_TABLE_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "BUD is not supported for this application");
            }
        }
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

/* Flow init procedure - encap access
 * This function is used initiatlizing encap access related sw states:
 * 1. application --> valid encap access bitmap
 * 2. application + encap access --> valid result types
 * Those will be used for verifications during create and result type selection
 */
static shr_error_e
dnx_flow_app_encap_access_init(
    int unit)
{
    char etps_to_phases_xml_path[RHFILE_MAX_SIZE];
    void *curTop = NULL, *curPhase;
    char *image_name = NULL;
    uint8 success;
    uint8 is_standard_image = FALSE;
    sw_state_htbl_init_info_t hash_tbl_init_info;
    flow_app_encap_info_t flow_app_encap_info;
    uint32 res_type_bitmap[1] = { 0 };
    uint32 encap_access_bitmap[1] = { 0 };
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_get(unit, &image_name));
    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));

    if (is_standard_image)
    {
        sal_strncpy(etps_to_phases_xml_path, "mdb/auto_generated/", RHFILE_MAX_SIZE - 1);
    }
    else
    {
        sal_strncpy(etps_to_phases_xml_path, "bare_metal/mdb/auto_generated/", RHFILE_MAX_SIZE - 1);
    }
    sal_strncat_s(etps_to_phases_xml_path, image_name, sizeof(etps_to_phases_xml_path));
    sal_strncat_s(etps_to_phases_xml_path, "/", sizeof(etps_to_phases_xml_path));
    sal_strncat_s(etps_to_phases_xml_path, image_name, sizeof(etps_to_phases_xml_path));
    sal_strncat_s(etps_to_phases_xml_path, "_etps_in_eedb_phases.xml", sizeof(etps_to_phases_xml_path));

    /*
     * Read XMl
     */
    if (is_standard_image)
    {
        curTop =
            dbx_pre_compiled_lif_phases_top_get(unit, etps_to_phases_xml_path, "PhasesEedbCatalog",
                                                CONF_OPEN_PER_DEVICE);
    }
    else
    {
        curTop = dbx_pre_compiled_lif_phases_top_get(unit, etps_to_phases_xml_path, "PhasesEedbCatalog", 0);
    }
    if (curTop == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find etps to phase XML file: %s\n", etps_to_phases_xml_path);
    }

    /*
     * Create the sw states 
     */
    sal_memset(&hash_tbl_init_info, 0, sizeof(sw_state_htbl_init_info_t));
    hash_tbl_init_info.max_nof_elements = DNX_FLOW_DBAL_TABLE_PHASE_TO_VALID_RES_TYPES_NOF_ELEMENTS;
    hash_tbl_init_info.expected_nof_elements = hash_tbl_init_info.max_nof_elements;
    hash_tbl_init_info.print_cb_name = "dnx_algo_flow_dbal_table_to_valid_result_types_print_cb";

    SHR_IF_ERR_EXIT(flow_db.dbal_table_to_valid_result_types.create(unit, &hash_tbl_init_info));

    sal_memset(&hash_tbl_init_info, 0, sizeof(sw_state_htbl_init_info_t));
    hash_tbl_init_info.max_nof_elements = DNX_FLOW_VALID_PHASES_PER_DBAL_TABLE_NOF_ELEMENTS;
    hash_tbl_init_info.expected_nof_elements = hash_tbl_init_info.max_nof_elements;

    SHR_IF_ERR_EXIT(flow_db.valid_phases_per_dbal_table.create(unit, &hash_tbl_init_info));

    /*
     * Run over all phases and look for flow applications, if found - update
     */
    RHDATA_ITERATOR(curPhase, curTop, "EedbPhase")
    {
        int logical_phase = 0;
        int nof_mappings = 0;
        char phase_name[10];
        void *curElement;
        sw_state_htbl_init_info_t hash_tbl_init_info;

        RHDATA_GET_XSTR_STOP(curPhase, "Name", phase_name, 10);
        RHDATA_GET_INT_STOP(curPhase, "NumOfMappings", nof_mappings);

        /*
         * Assuming phase names are EEDB_A -> EEDB_H,
         * the mapping to logical phase is done by taking the 6th char and reduce 'A'
         */
        logical_phase = phase_name[5] - 'A';

        sal_memset(&hash_tbl_init_info, 0, sizeof(sw_state_htbl_init_info_t));

        hash_tbl_init_info.max_nof_elements = nof_mappings;
        hash_tbl_init_info.expected_nof_elements = hash_tbl_init_info.max_nof_elements;

        RHDATA_ITERATOR(curElement, curPhase, "Element")
        {
            /*
             * Add all elements in XML to SW state.
             * Translate strings to DBAL IDs
             */
            char dbal_table_name[DBAL_MAX_STRING_LENGTH], dbal_result_type_name[DBAL_MAX_STRING_LENGTH];
            dbal_tables_e dbal_table_id;
            int is_table_active;
            int result_type;

            RHDATA_GET_XSTR_STOP(curElement, "AppDb", dbal_table_name, DBAL_MAX_STRING_LENGTH);
            RHDATA_GET_XSTR_STOP(curElement, "ResultType", dbal_result_type_name, DBAL_MAX_STRING_LENGTH);

            SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, dbal_table_name, &dbal_table_id));
            SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, dbal_table_id, &is_table_active));
            if (!is_table_active)
            {
                continue;
            }

            if (sal_strncmp(dbal_result_type_name, "", DBAL_MAX_STRING_LENGTH) == 0)
            {
                result_type = -1;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_tables_result_type_by_name_get
                                (unit, dbal_table_id, dbal_result_type_name, &result_type));
            }

            if (result_type >= 0)
            {
                sal_memset(&flow_app_encap_info, 0, sizeof(flow_app_encap_info_t));
                sal_memset(&res_type_bitmap, 0, sizeof(res_type_bitmap));

                flow_app_encap_info.dbal_table = dbal_table_id;
                flow_app_encap_info.encap_access = logical_phase;

                res_type_bitmap[0] = 0;

                SHR_IF_ERR_EXIT(flow_db.
                                dbal_table_to_valid_result_types.find(unit, &flow_app_encap_info, &res_type_bitmap[0],
                                                                      &found));

                SHR_BITSET(res_type_bitmap, result_type);

                SHR_IF_ERR_EXIT(flow_db.
                                dbal_table_to_valid_result_types.insert(unit, &flow_app_encap_info, &res_type_bitmap[0],
                                                                        &success));

                if (success == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Internal error - failed to add table %s + encap access %d to result type %s sw state",
                                 dbal_logical_table_to_string(unit, dbal_table_id), logical_phase,
                                 dbal_result_type_name);
                }
            }

            encap_access_bitmap[0] = 0;
            SHR_IF_ERR_EXIT(flow_db.
                            valid_phases_per_dbal_table.find(unit, &dbal_table_id, &encap_access_bitmap[0], &found));

            SHR_BITSET(encap_access_bitmap, logical_phase);

            SHR_IF_ERR_EXIT(flow_db.valid_phases_per_dbal_table.insert(unit, &dbal_table_id, &encap_access_bitmap[0],
                                                                       &success));

            if (success == 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Internal error - failed to add table %s to encap access %d sw state",
                             dbal_logical_table_to_string(unit, dbal_table_id), logical_phase);
            }
        }
    }

exit:
    dbx_xml_top_close(curTop);
    SHR_FUNC_EXIT;
}

/** Flow init procedures */
shr_error_e
dnx_flow_init(
    int unit)
{
    int num_apps;
    uint8 is_init;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    num_apps = dnx_flow_number_of_apps();

    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * Initialize flow SW state.
         */
        SHR_IF_ERR_EXIT(flow_db.is_init(unit, &is_init));

        if (!is_init)
        {
            SHR_IF_ERR_EXIT(flow_db.init(unit));
        }

        SHR_IF_ERR_EXIT(flow_db.flow_application_info.alloc(unit, num_apps));

        SHR_IF_ERR_EXIT(dnx_flow_app_encap_access_init(unit));
    }

    /*
     * Initialize the FLOW LIF Mgmnt module
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
flow_match_entry_from_global_lif_get(
    int unit,
    int global_lif,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    uint32 entry_handle_id;
    int is_end;
    lif_mapping_local_lif_payload_t local_lif_payload;
    dnx_flow_app_config_t *flow_app_info;
    dbal_tables_e dbal_table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_FLOW_HANDLE_VALIDATE(flow_handle_info->flow_handle);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    dbal_table =
        (!(_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD))) ? flow_app_info->flow_table :
        flow_app_info->second_pass_table;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_global_to_local_get
                        (unit, DNX_ALGO_LIF_INGRESS, global_lif, &local_lif_payload));

        if (dbal_table != DBAL_TABLE_INGRESS_PP_PORT)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                                                                 DBAL_CONDITION_EQUAL_TO,
                                                                 local_lif_payload.local_lifs_array.local_lifs[0], 0));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                             local_lif_payload.local_lifs_array.local_lifs[0], 0));
        }
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT_MATCH)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_global_to_local_get
                        (unit, DNX_ALGO_LIF_EGRESS, global_lif, &local_lif_payload));

        SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE,
                                                             DBAL_CONDITION_EQUAL_TO,
                                                             local_lif_payload.local_lifs_array.local_lifs[0], 0));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal flow handle (%s), only match applications supported ",
                     flow_app_info->app_name);
    }

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    if (is_end)
    {
        /** no matching entry found */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else
    {
        int nof_app_special_fields = 0;
        dbal_table_type_e table_type;

        if (flow_app_info->is_entry_related &&
            dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE))
        {
            int is_entry_related;

            SHR_IF_ERR_EXIT(flow_app_info->is_entry_related(unit, entry_handle_id, &is_entry_related));
            if (!is_entry_related)
            {
                /** no matching entry found */
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));

        /** Get number of key special fields, serves as key for match applications Assign each key field id */
        while (flow_app_info->special_fields[nof_app_special_fields] != FLOW_S_F_EMPTY)
        {
            key_special_fields->special_fields[nof_app_special_fields].field_id =
                flow_app_info->special_fields[nof_app_special_fields];
            nof_app_special_fields++;
        }

        key_special_fields->actual_nof_special_fields = nof_app_special_fields;

        SHR_IF_ERR_EXIT(dnx_flow_match_key_fields_get
                        (unit, entry_handle_id, flow_handle_info, flow_app_info, table_type, key_special_fields,
                         nof_app_special_fields));

        /** If a specific application callback is defined - call it which traverse cmd */
        if (flow_app_info->app_specific_operations_cb)
        {
            dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_TRAVERSE, FLOW_COMMAND_CONTINUE };

            SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                            (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, &flow_cmd_control,
                             NULL, NULL, key_special_fields, NULL, NULL));
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
flow_lif_flow_app_from_gport_get(
    int unit,
    bcm_gport_t gport,
    dnx_flow_app_type_e flow_app_type,
    uint32 *flow_handle)
{

    dbal_tables_e table_id;
    int is_virtual = FALSE;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    if (FLOW_IS_FLOW_ID_VIRTUAL(gport))
    {
        is_virtual = TRUE;
    }

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        if (is_virtual)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress Virtual gport is not supported yet");
        }

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

        table_id = gport_hw_resources.inlif_dbal_table_id;
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        uint32 lif_flags = (is_virtual) ? DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL :
            (DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS);

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, lif_flags, &gport_hw_resources));

        table_id = gport_hw_resources.outlif_dbal_table_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value for flow_app_type (%d) ", flow_app_type);
    }

    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_table_id_get(unit, table_id, flow_app_type, flow_handle));
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file
 */
shr_error_e
flow_lif_flow_app_from_table_id_get(
    int unit,
    dbal_tables_e table_id,
    dnx_flow_app_type_e flow_app_type,
    uint32 *flow_handle)
{
    int app_idx;
    int num_apps = dnx_flow_number_of_apps();
    SHR_FUNC_INIT_VARS(unit);
    if ((flow_app_type != FLOW_APP_TYPE_INIT) && flow_app_type != FLOW_APP_TYPE_TERM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flow application type passed\n");
    }
    for (app_idx = FLOW_APP_FIRST_IDX; app_idx < num_apps; app_idx++)
    {
        if (FLOW_APP_IS_NOT_VALID(dnx_flow_objects[app_idx]))
        {
            continue;
        }
        if (dnx_flow_objects[app_idx]->flow_table == table_id)
        {
            (*flow_handle) = app_idx;
            SHR_EXIT();
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

/** Convert a LIF gport to a Flow ID */
uint32
dnx_flow_gport_to_flow_id(
    int unit,
    uint32 gport)
{
    uint32 flow_gport;

    /*
     * A VLAN-Port requires a reset of the subtype
     */
    if (BCM_GPORT_IS_VLAN_PORT(gport))
    {
        uint32 subtype_reset_mask;

        subtype_reset_mask = ~(_SHR_GPORT_SUB_TYPE_MASK << _SHR_GPORT_SUB_TYPE_SHIFT);
        flow_gport = gport & subtype_reset_mask;
        BCM_GPORT_TUNNEL_ID_SET(flow_gport, flow_gport);
    }
    /*
     * The general case for Tunnel gports or gports that don't support subtypes 
     * Other gport types may require a subtype reset as above.
     */
    else
    {
        BCM_GPORT_TUNNEL_ID_SET(flow_gport, gport);
    }

    return flow_gport;
}

/**
 * Returns true if the app indication is set. 
 */
int
dnx_flow_app_is_ind_set(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 ind)
{
    uint32 app_ind[2] = { 0 };

    app_ind[0] = flow_app_info->app_indications_bm;

    return SHR_IS_BITSET(app_ind, ind);
}

/**
 * Returns true if the app indication is set. 
 */
int
dnx_flow_special_field_is_ind_set(
    int unit,
    const flow_special_field_info_t * field,
    uint32 ind)
{
    uint32 field_ind[2] = { 0 };

    field_ind[0] = field->indications_bm;

    return SHR_IS_BITSET(field_ind, ind);
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

/** Function Retrieves the virtual gport by traversing the gport-to-match-key for the keys from this entry this is
 *  needed since for there is no local lif for match virtual applications */
static shr_error_e
dnx_flow_virtual_gport_by_match_lookup(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_gport_t * virtual_lif_gport)
{
    uint32 sw_entry_handle_id;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    int is_end;
    char *app_name;
    dbal_fields_e field_id;
    uint32 vsi, vlan_domain, c_vid;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, sw_entry_handle_id, DBAL_ITER_MODE_ALL));

    /** Retrieve the match keys according to the application */
    

    app_name = FLOW_APP_NAME_VLAN_PORT_ESEM_INITIATOR;
    field_id = DBAL_FIELD_VLAN_DOMAIN;

    if (sal_strncmp(flow_app_info->app_name, app_name, sizeof(flow_app_info->app_name)) == 0)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, &vsi));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, &vlan_domain));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_C_VID, &c_vid));

        /** Set the iterator rules for the SW mapping table according to match key values */
        SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add(unit, sw_entry_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE,
                                                             DBAL_CONDITION_EQUAL_TO, vsi, 0));
        SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                        (unit, sw_entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                         vlan_domain, 0));
        SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                        (unit, sw_entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                         c_vid, 0));
    }

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, sw_entry_handle_id, &is_end));
    if (is_end)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error: For application %s: No virtual gport found with the given match keys = {vsi = %d, vlan_domain = %d, c_vid = %d",
                     app_name, vsi, vlan_domain, c_vid);
    }
    else
    {
        uint32 ret_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, sw_entry_handle_id, DBAL_FIELD_GPORT, ret_field_val));
        *virtual_lif_gport = ret_field_val[0];
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_special_fields_set_one_field(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields,
    int field_idx)
{
    const flow_special_field_info_t *curr_field;
    int is_match_app = FLOW_APP_IS_MATCH(flow_app_info->flow_app_type) ? TRUE : FALSE;
    bcm_flow_special_field_t *bcm_special_fields = &(special_fields->special_fields[field_idx]);
    int is_key, is_tcam = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Determine whether it's TCAM field key */
    if (is_match_app)
    {
        dbal_table_type_e table_type;
        SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, flow_app_info->flow_table, &table_type));
        if (table_type == DBAL_TABLE_TYPE_TCAM)
        {
            is_tcam = TRUE;
        }
    }

    SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, bcm_special_fields->field_id, &curr_field));

    /** A key field is either a match application field or a field that has a key indication (Virtual application) */
    is_key = is_match_app || dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_IS_KEY);

    if (curr_field->set != NULL)
    {
        SHR_IF_ERR_EXIT(curr_field->set
                        (unit, entry_handle_id, flow_handle_info, flow_app_info, gport_hw_resources, field_idx,
                         special_fields));
    }
    else if (is_key && is_tcam
             && (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MASK_SUPPORTED)))
    {
        SHR_IF_ERR_EXIT(flow_special_fields_mask_field_value_set(unit, entry_handle_id, flow_handle_info, flow_app_info,
                                                                 gport_hw_resources, field_idx, special_fields));
    }
    else if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED) &&
             dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE))
    {
        SHR_IF_ERR_EXIT(flow_special_field_src_address_set
                        (unit, entry_handle_id, flow_handle_info, flow_app_info, gport_hw_resources, field_idx,
                         special_fields));
    }
    else if (curr_field->mapped_dbal_field != FLOW_S_FIELD_UNMAPPED)
    {
        /** set directly according to dbal field and payload type */
        switch (curr_field->payload_type)
        {
            case FLOW_PAYLOAD_TYPE_UINT32:
                if (is_key)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, curr_field->mapped_dbal_field,
                                               bcm_special_fields->shr_var_uint32);
                }
                else
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                                 bcm_special_fields->shr_var_uint32);

                }
                break;
            case FLOW_PAYLOAD_TYPE_UINT32_ARR:
                if (is_key)
                {
                    dbal_entry_key_field_arr32_set(unit, entry_handle_id, curr_field->mapped_dbal_field,
                                                   bcm_special_fields->shr_var_uint32_arr);
                }
                else
                {
                    dbal_entry_value_field_arr32_set(unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                                     bcm_special_fields->shr_var_uint32_arr);

                }
                break;
            case FLOW_PAYLOAD_TYPE_UINT8_ARR:
                if (is_key)
                {
                    dbal_entry_key_field_arr8_set(unit, entry_handle_id, curr_field->mapped_dbal_field,
                                                  bcm_special_fields->shr_var_uint8_arr);
                }
                else
                {
                    dbal_entry_value_field_arr8_set(unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                                    bcm_special_fields->shr_var_uint8_arr);

                }
                break;

            case FLOW_PAYLOAD_TYPE_ENUM:
            case FLOW_PAYLOAD_TYPE_BCM_ENUM:

                if (is_key)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, curr_field->mapped_dbal_field,
                                               bcm_special_fields->symbol);
                }
                else
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                                 bcm_special_fields->symbol);

                }
                break;
            case FLOW_PAYLOAD_TYPE_ENABLER:
                if (is_key)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, curr_field->mapped_dbal_field, 0x1);
                }
                else
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                                 0x1);
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "unrecognized payload_type %d\n", curr_field->payload_type);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field has no mapping %s\n", curr_field->name);
    }

exit:
    SHR_FUNC_EXIT;
}

/** copy one field HW value (from get_entry_handle_id) to new handle the field to perform the operations is
 *  flow_app_info->special_fields[field_idx]  */
static shr_error_e
dnx_flow_special_fields_copy_one_field(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx)
{
    flow_special_field_info_t *curr_field;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    curr_field = &flow_special_fields_db[flow_app_info->special_fields[field_idx]];

    if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED)
        && (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE)))
    {
        uint32 src_addr_profile = 0;

        rv = dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE,
                                                 &src_addr_profile);
        if (rv == _SHR_E_NONE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_IDX, INST_SINGLE, src_addr_profile);
        }
    }
    else if (curr_field->mapped_dbal_field != FLOW_S_FIELD_UNMAPPED)
    {
        /** set directly according to dbal field and payload type */
        switch (curr_field->payload_type)
        {
            case FLOW_PAYLOAD_TYPE_UINT32:
            case FLOW_PAYLOAD_TYPE_ENUM:
            case FLOW_PAYLOAD_TYPE_BCM_ENUM:
            case FLOW_PAYLOAD_TYPE_ENABLER:
            {
                uint32 value = 0;

                rv = dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, curr_field->mapped_dbal_field,
                                                         INST_SINGLE, &value);
                if (rv == _SHR_E_NONE)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                                 value);
                }
            }
                break;

            case FLOW_PAYLOAD_TYPE_UINT32_ARR:
            {
                uint32 uint32_arr_value[BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE] = { 0 };

                rv = dbal_entry_handle_value_field_arr32_get
                    (unit, get_entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE, uint32_arr_value);
                if (rv == _SHR_E_NONE)
                {
                    dbal_entry_value_field_arr32_set(unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                                     uint32_arr_value);
                }
            }
                break;

            case FLOW_PAYLOAD_TYPE_UINT8_ARR:
            {
                uint8 uint8_arr_value[BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE] = { 0 };

                rv = dbal_entry_handle_value_field_arr8_get
                    (unit, get_entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE, uint8_arr_value);
                if (rv == _SHR_E_NONE)
                {
                    dbal_entry_value_field_arr8_set(unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                                    uint8_arr_value);
                }

            }
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "unrecognized payload_type %d\n", curr_field->payload_type);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field has no mapping %s\n", curr_field->name);
    }

    if (rv == _SHR_E_NOT_FOUND)
    {
        /** rv not found, means that the special field was not set. this is valid. */
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    }
    else if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_special_fields_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields)
{
    int ii;
    flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < special_fields->actual_nof_special_fields; ii++)
    {
        curr_field = &flow_special_fields_db[special_fields->special_fields[ii].field_id];

        if ((FLOW_APP_IS_MATCH(flow_app_info->flow_app_type) &&
             dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MATCH_IGNORE_OPERATION)) ||
            (FLOW_APP_IS_LIF(flow_app_info->flow_app_type) &&
             dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_IGNORE_OPERATION)))
        {
            continue;
        }
        if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type) &&
            (curr_field->mapped_dbal_field == DBAL_FIELD_VMPI) &&
            !dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_per_port_ilm_lookup_supported))
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_set_one_field(unit, entry_handle_id, flow_handle_info, flow_app_info,
                                                              gport_hw_resources, special_fields, ii));
    }

exit:
    SHR_FUNC_EXIT;
}

/** This function is applicable for Initiator and Terminator applications */
static shr_error_e
dnx_flow_special_fields_destroy(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    int sp_field_idx;
    flow_special_field_info_t *curr_field;
    uint8 is_valid;

    SHR_FUNC_INIT_VARS(unit);

    for (sp_field_idx = 0; flow_app_info->special_fields[sp_field_idx] != FLOW_S_F_EMPTY; sp_field_idx++)
    {
        curr_field = &flow_special_fields_db[flow_app_info->special_fields[sp_field_idx]];

        if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED) &&
            dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE))
        {

            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                            (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_SOURCE_IDX, &is_valid));
            if (is_valid)
            {
                SHR_IF_ERR_EXIT(flow_special_field_src_address_destroy
                                (unit, entry_handle_id, flow_handle_info, flow_app_info, gport_hw_resources,
                                 flow_app_info->special_fields[sp_field_idx]));
            }
        }
        else if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_IGNORE_OPERATION) ||
                 dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_IS_KEY))
        {
            continue;
        }
        if (curr_field->clear != NULL)
        {
            dbal_fields_e mapped_dbal_field = curr_field->mapped_dbal_field;

            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                            (unit, flow_handle_info, gport_hw_resources, mapped_dbal_field, &is_valid));
            if (is_valid)
            {
                SHR_IF_ERR_EXIT(curr_field->clear
                                (unit, entry_handle_id, flow_handle_info, flow_app_info, gport_hw_resources,
                                 flow_app_info->special_fields[sp_field_idx]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** get the value directly according to mapped dbal field and payload type */
static shr_error_e
flow_special_field_direct_mapping_get(
    int unit,
    flow_special_field_info_t * curr_field,
    uint32 entry_handle_id,
    bcm_flow_special_field_t * special_field)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (curr_field->payload_type)
    {
        case FLOW_PAYLOAD_TYPE_UINT32:
            FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                     &special_field->shr_var_uint32));
            break;

        case FLOW_PAYLOAD_TYPE_UINT32_ARR:
            FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                    (unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                     special_field->shr_var_uint32_arr));
            break;

        case FLOW_PAYLOAD_TYPE_UINT8_ARR:
            FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                                    (unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE,
                                     special_field->shr_var_uint8_arr));
            break;

        case FLOW_PAYLOAD_TYPE_ENABLER:
        {
            uint32 value;
            FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE, &value));
                /** for enabler, if there is a value, it means the field exists, no need to update its data */
                /** if there is no value, it means that the field was not set and so the NOT_FOUND marking*/
            if (value == 0)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
            }
        }
            break;

        case FLOW_PAYLOAD_TYPE_ENUM:
        case FLOW_PAYLOAD_TYPE_BCM_ENUM:
        {
            uint32 value;
            FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, curr_field->mapped_dbal_field, INST_SINGLE, &value));
            special_field->symbol = (int) value;
        }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unrecognized payload_type %d\n", curr_field->payload_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** get the value directly according to mapped dbal field and payload type (for key fields) */
static shr_error_e
flow_special_field_key_direct_mapping_get(
    int unit,
    flow_special_field_info_t * curr_field,
    uint32 entry_handle_id,
    bcm_flow_special_field_t * special_field)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (curr_field->payload_type)
    {
        case FLOW_PAYLOAD_TYPE_UINT32:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, curr_field->mapped_dbal_field, &special_field->shr_var_uint32));
            break;

        case FLOW_PAYLOAD_TYPE_UINT32_ARR:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, curr_field->mapped_dbal_field, special_field->shr_var_uint32_arr));
            break;

        case FLOW_PAYLOAD_TYPE_UINT8_ARR:
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                            (unit, entry_handle_id, curr_field->mapped_dbal_field, special_field->shr_var_uint8_arr));
            break;

        case FLOW_PAYLOAD_TYPE_ENABLER:
            break;

        case FLOW_PAYLOAD_TYPE_ENUM:
        case FLOW_PAYLOAD_TYPE_BCM_ENUM:
        {
            uint32 value;

            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, curr_field->mapped_dbal_field, &value));
            special_field->symbol = (int) value;
        }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unrecognized payload_type %d\n", curr_field->payload_type);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_special_fields_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields)
{
    int idx = 0, rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->special_fields[0] != FLOW_S_F_EMPTY)
    {
        int ii;

        sal_memset(special_fields, 0, sizeof(bcm_flow_special_fields_t));

        special_fields->actual_nof_special_fields = 0;

        for (ii = 0; flow_app_info->special_fields[ii] != FLOW_S_F_EMPTY; ii++)
        {
            flow_special_field_info_t *curr_field;

            curr_field = &flow_special_fields_db[flow_app_info->special_fields[ii]];

            if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_IGNORE_OPERATION))
            {
                continue;
            }
            if (curr_field->get != NULL)
            {
                rv = curr_field->get
                    (unit, entry_handle_id, flow_handle_info, flow_app_info, gport_hw_resources,
                     flow_app_info->special_fields[ii], &(special_fields->special_fields[idx]));
                if (rv == _SHR_E_NONE)
                {
                    idx++;
                    continue;
                }
                else
                {
                    if (rv != _SHR_E_NOT_FOUND)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "issue when getting field %s ", curr_field->name);
                    }
                    else
                    {
                        continue;
                    }
                }
            }

            if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED)
                && dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE))
            {
                rv = flow_special_field_src_address_get(unit, entry_handle_id, flow_handle_info, flow_app_info,
                                                        gport_hw_resources, flow_app_info->special_fields[ii],
                                                        &(special_fields->special_fields[idx]));

                if (rv == _SHR_E_NONE)
                {
                    idx++;
                    continue;
                }
                else
                {
                    if (rv != _SHR_E_NOT_FOUND)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "issue when getting field %s ", curr_field->name);
                    }
                    else
                    {
                        continue;
                    }
                }
            }

            if (curr_field->mapped_dbal_field != FLOW_S_FIELD_UNMAPPED)
            {
                int is_virtual_match_key =
                    dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_IS_KEY);
                if (!is_virtual_match_key)
                {
                    rv = flow_special_field_direct_mapping_get(unit, curr_field, entry_handle_id,
                                                               &(special_fields->special_fields[idx]));
                }
                else
                {
                    rv = flow_special_field_key_direct_mapping_get(unit, curr_field, entry_handle_id,
                                                                   &(special_fields->special_fields[idx]));
                }

                if (rv == _SHR_E_NONE)
                {
                    special_fields->special_fields[idx].field_id = flow_app_info->special_fields[ii];
                    idx++;
                }
                else
                {
                    if (rv != _SHR_E_NOT_FOUND)
                    {
                        SHR_ERR_EXIT(rv, "issue when getting field %s ", curr_field->name);
                    }
                }
                continue;
            }

            /** reaching here means that the field was not handled before, this is wrong */
            SHR_ERR_EXIT(_SHR_E_PARAM, "error mapping field %s ", curr_field->name);

        }

        special_fields->actual_nof_special_fields = idx;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_special_fields_replace(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields)
{
    int field = 0, used_field = 0;
    uint32 special_fields_bitmap[1] = { 0 };
    flow_special_field_info_t *curr_field;
    uint32 actual_nof_special_fields;

    SHR_FUNC_INIT_VARS(unit);

    if (special_fields == NULL)
    {
        actual_nof_special_fields = 0;
    }
    else
    {
        actual_nof_special_fields = special_fields->actual_nof_special_fields;
    }
    /*
     * Iterate over app special fields, set bit for each supported field 
     */
    while (flow_app_info->special_fields[field] != FLOW_S_F_EMPTY)
    {
        SHR_BITSET(special_fields_bitmap, field);
        field++;
    }

    for (field = 0; field < actual_nof_special_fields; field++)
    {
        used_field = 0;

        /*
         * Find the field set by the user in the application supported special fields, later to be marked as used
         */
        while ((flow_app_info->special_fields[used_field] != special_fields->special_fields[field].field_id) &&
               (flow_app_info->special_fields[used_field] != FLOW_S_F_EMPTY))
        {
            used_field++;
        }

        if (flow_app_info->special_fields[used_field] == FLOW_S_F_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Couldn't find special field given %s in app",
                         dnx_flow_special_field_to_string(unit, special_fields->special_fields[field].field_id));
        }

        /*
         * Clear the field bit in the bitmap, this way we're marking it as if it was already handled. Later - we
         * iterate over this bitmap, and each set bit - we take the original value (if existed) 
         */
        SHR_BITCLR(special_fields_bitmap, used_field);

        curr_field = &flow_special_fields_db[special_fields->special_fields[field].field_id];

        if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_IGNORE_OPERATION))
        {
            continue;
        }

        /*
         * If is_clear is off and field was set --> set requested value
         * Special fields with is_clear set should be ignored
         */
        if (!special_fields->special_fields[field].is_clear)
        {
            if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE) &&
                dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED))
            {
                SHR_IF_ERR_EXIT(dnx_flow_initiator_src_address_profile_replace
                                (unit, entry_handle_id, get_entry_handle_id, flow_handle_info, flow_app_info,
                                 gport_hw_resources, &special_fields->special_fields[field]));
            }
            else if (curr_field->replace != NULL)
            {
                SHR_IF_ERR_EXIT(curr_field->replace
                                (unit, entry_handle_id, get_entry_handle_id,
                                 flow_handle_info, flow_app_info, gport_hw_resources,
                                 &special_fields->special_fields[field]));

            }
            else
            {
                /** in case of regular mapping, just need to update the new value */
                SHR_IF_ERR_EXIT(dnx_flow_special_fields_set_one_field
                                (unit, entry_handle_id, flow_handle_info, flow_app_info, gport_hw_resources,
                                 special_fields, field));
            }
        }
        else
        {
            if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SRC_ADDR_PROFILE) &&
                dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED))
            {
                uint8 is_valid;

                SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                                (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_SOURCE_IDX, &is_valid));
                if (is_valid)
                {
                    SHR_IF_ERR_EXIT(flow_special_field_src_address_destroy
                                    (unit, get_entry_handle_id, flow_handle_info, flow_app_info, gport_hw_resources,
                                     special_fields->special_fields[field].field_id));
                }

            }
            else if (curr_field->clear != NULL)
            {
                SHR_IF_ERR_EXIT(curr_field->clear
                                (unit, entry_handle_id, flow_handle_info, flow_app_info, gport_hw_resources,
                                 special_fields->special_fields[field].field_id));
                continue;
            }
        }
    }

    /*
     * Iterate on the special fields bitmap. Each set field is a field which the user didn't relate to. This field
     * should keep its original value (if existed) 
     */
    field = 0;
    while (special_fields_bitmap[0])
    {
        if (special_fields_bitmap[0] & 0x1)
        {
            curr_field = &flow_special_fields_db[flow_app_info->special_fields[field]];

            if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_LIF_IGNORE_OPERATION) ||
                dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_IS_KEY))
            {
                special_fields_bitmap[0] = (special_fields_bitmap[0] >> 1);
                field++;
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_flow_special_fields_copy_one_field(unit, entry_handle_id, get_entry_handle_id,
                                                                   flow_handle_info,
                                                                   flow_app_info, gport_hw_resources, field));
        }
        special_fields_bitmap[0] = (special_fields_bitmap[0] >> 1);
        field++;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_common_fields_set(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *bcm_struct_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id,
    lif_table_mngr_outlif_info_t * outlif_info)
{
    int field = 0;
    dnx_flow_common_field_desc_t **common_fields_arr;
    uint32 valid_elements_set;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        common_fields_arr = dnx_flow_term_common_fields;
        valid_elements_set = ((bcm_flow_terminator_info_t *) bcm_struct_info)->valid_elements_set;
    }
    else
    {
        common_fields_arr = dnx_flow_init_common_fields;
        valid_elements_set = ((bcm_flow_initiator_info_t *) bcm_struct_info)->valid_elements_set;
    }

    /*
     * Iterate on the common fields. If a field flag was set by the user - call relevant callback 
     */
    while (valid_elements_set != 0)
    {
        if (valid_elements_set & 0x1)
        {
            if (common_fields_arr[field]->field_cbs.field_set_cb)
            {
                /** generic call to the common field set callback */
                SHR_IF_ERR_EXIT(common_fields_arr[field]->
                                field_cbs.field_set_cb(unit, entry_handle_id, flow_app_type, flow_handle_info,
                                                       gport_hw_resources, flow_app_info, bcm_struct_info));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "common field has no set CB Application %s, common field set, %s",
                             flow_app_info->app_name, common_fields_arr[field]->field_name);
            }
        }
        valid_elements_set = (valid_elements_set >> 1);
        field++;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_common_fields_replace(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *bcm_struct_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id)
{
    int num_common_fields;
    int field;
    dnx_flow_common_field_desc_t **common_fields_arr;
    uint32 valid_elements_set, valid_elements_clear;
    bcm_flow_initiator_info_t get_initiator_info = { 0 };
    bcm_flow_terminator_info_t get_terminator_info = { 0 };
    bcm_flow_terminator_info_t *terminator_info = NULL;
    bcm_flow_initiator_info_t *initiator_info = NULL;

    int rv;
    void *get_info;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        common_fields_arr = dnx_flow_term_common_fields;

        /** get_terminator_info will be used for profiles delete  */
        sal_memcpy(&get_terminator_info, ((bcm_flow_terminator_info_t *) bcm_struct_info),
                   sizeof(bcm_flow_terminator_info_t));
        get_info = &get_terminator_info;
        terminator_info = ((bcm_flow_terminator_info_t *) bcm_struct_info);
        valid_elements_set = terminator_info->valid_elements_set;
        valid_elements_clear = terminator_info->valid_elements_clear;
    }
    else
    {
        common_fields_arr = dnx_flow_init_common_fields;

        /** get_initiator_info will be used for profiles delete  */
        sal_memcpy(&get_initiator_info, ((bcm_flow_initiator_info_t *) bcm_struct_info),
                   sizeof(bcm_flow_initiator_info_t));
        get_info = &get_initiator_info;
        initiator_info = ((bcm_flow_initiator_info_t *) bcm_struct_info);
        valid_elements_set = initiator_info->valid_elements_set;
        valid_elements_clear = initiator_info->valid_elements_clear;
    }

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_type, &num_common_fields));

    /*
     * Iterate over the flow common fields. If requested by the user - call relevant callback
     */
    for (field = 0; field < num_common_fields; field++)
    {
        uint32 field_valid_bit = (1 << field);

        /*
         * If the application does not support this common field, skip all set/get actions
         * Reasoning:
         * If not skipped, the user should be aware which common fields are not supported for the application
         * and set the corresponding "valid_elements_clear" bit in order to skip get during replace
         */
        if (!_SHR_IS_FLAG_SET(flow_app_info->valid_common_fields_bitmap, field_valid_bit))
        {
            continue;
        }

        /*
         * If valid_elements is set --> field should be replaced. Call replace cb
         */
        if (_SHR_IS_FLAG_SET(valid_elements_set, field_valid_bit))
        {
            /*
             * For standard fields - perform simple set
             * For profiles etc - a replace cb can be defined, in case the old profile should be deleted after creating the new one etc
             */
            if (common_fields_arr[field]->field_cbs.field_replace_cb)
            {
                SHR_IF_ERR_EXIT(common_fields_arr[field]->
                                field_cbs.field_replace_cb(unit, entry_handle_id, get_entry_handle_id, flow_app_type,
                                                           flow_handle_info, gport_hw_resources, flow_app_info,
                                                           bcm_struct_info));
            }
            else if (common_fields_arr[field]->field_cbs.field_set_cb)
            {
                SHR_IF_ERR_EXIT(common_fields_arr[field]->
                                field_cbs.field_set_cb(unit, entry_handle_id, flow_app_type, flow_handle_info,
                                                       gport_hw_resources, flow_app_info, bcm_struct_info));
            }
        }
        /*
         * If no flag was set - keep original field value. Perform get on original entry. If exists - set previous
         * value. Otherwise - leave default value
         */
        else if (!_SHR_IS_FLAG_SET(valid_elements_clear, field_valid_bit))
        {
            if (common_fields_arr[field]->field_cbs.field_get_cb)
            {
                rv = common_fields_arr[field]->field_cbs.field_get_cb(unit, get_entry_handle_id, flow_app_type,
                                                                      flow_handle_info, gport_hw_resources,
                                                                      flow_app_info, bcm_struct_info);

                if (rv == _SHR_E_NONE)
                {
                    if (common_fields_arr[field]->field_cbs.field_set_cb)
                    {
                        SHR_IF_ERR_EXIT(common_fields_arr[field]->
                                        field_cbs.field_set_cb(unit, entry_handle_id, flow_app_type, flow_handle_info,
                                                               gport_hw_resources, flow_app_info, bcm_struct_info));
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
         * Valid elements clear - in case of a profile with a delete cb - call this cb to free the profile. No need to set anything
         */
        else
        {
            if ((common_fields_arr[field]->field_cbs.field_delete_cb)
                && (common_fields_arr[field]->field_cbs.field_get_cb))
            {
                rv = common_fields_arr[field]->field_cbs.field_get_cb(unit, get_entry_handle_id, flow_app_type,
                                                                      flow_handle_info, gport_hw_resources,
                                                                      flow_app_info, get_info);
                /*
                 * If profile existed on entry - need to restore it to default value
                 */
                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT_EXCEPT_IF(common_fields_arr[field]->
                                              field_cbs.field_delete_cb(unit, entry_handle_id, flow_app_type,
                                                                        flow_handle_info, gport_hw_resources,
                                                                        flow_app_info, bcm_struct_info),
                                              _SHR_E_NOT_FOUND);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_common_fields_get(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_terminator_info_t * terminator_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id)
{
    int field = 0;
    shr_error_e rv;
    dnx_flow_common_field_desc_t **common_fields_arr;
    uint32 temp_common_bitmap = flow_app_info->valid_common_fields_bitmap;
    void *bcm_struct_info;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        common_fields_arr = dnx_flow_term_common_fields;
        terminator_info->valid_elements_set = 0;
        bcm_struct_info = terminator_info;
    }
    else
    {
        common_fields_arr = dnx_flow_init_common_fields;
        initiator_info->valid_elements_set = 0;
        bcm_struct_info = initiator_info;
    }

    /*
     * Iterate over all common fields, and call relevant get callback.
     * If not found - ignore, field didn't exist on entry.
     * Mark the found fields in the valid_elements_set bitmap.
     */
    while (temp_common_bitmap != 0)
    {
        if ((temp_common_bitmap & 0x1) && (common_fields_arr[field]->field_cbs.field_get_cb))
        {
            rv = common_fields_arr[field]->field_cbs.field_get_cb(unit, entry_handle_id,
                                                                  flow_app_type,
                                                                  flow_handle_info,
                                                                  gport_hw_resources, flow_app_info, bcm_struct_info);
            if (rv == _SHR_E_NONE)
            {
                if (flow_app_type == FLOW_APP_TYPE_TERM)
                {
                    terminator_info->valid_elements_set |= common_fields_arr[field]->term_field_enabler;
                }
                else
                {
                    initiator_info->valid_elements_set |= common_fields_arr[field]->init_field_enabler;
                }
            }
            else if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(rv, "Error, retrieving flow field %s", common_fields_arr[field]->field_name);
            }
        }
        temp_common_bitmap = (temp_common_bitmap >> 1);
        field++;
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
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 entry_handle_id, get_entry_handle_id = 0;
    lif_table_mngr_inlif_info_t inlif_info = { 0 };
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    uint32 selectable_result_types = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

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

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_get
                    (unit, flow_app_info, flow_handle_info, &gport_hw_resources, get_entry_handle_id));

    inlif_info.global_lif = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info->flow_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, inlif_info.global_lif);

    SHR_IF_ERR_EXIT(dnx_flow_common_fields_replace
                    (unit, flow_app_info, FLOW_APP_TYPE_TERM, flow_handle_info, terminator_info, &gport_hw_resources,
                     entry_handle_id, get_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_replace(unit, entry_handle_id, get_entry_handle_id, flow_handle_info,
                                                    flow_app_info, &gport_hw_resources, special_fields));

    /** Check if errors occured on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    /*
     * If a result type selection callback is defined - call it
     */
    if (flow_app_info->result_type_select_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->result_type_select_cb
                        (unit, entry_handle_id, flow_handle_info, &gport_hw_resources, terminator_info,
                         special_fields, &inlif_info, &selectable_result_types));
    }

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, get_entry_handle_id,
                         flow_handle_info, flow_cmd_control, &gport_hw_resources, terminator_info,
                         special_fields, &inlif_info, &selectable_result_types));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    /** Write to HW */
    inlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_replace
                    (unit, flow_handle_info, &gport_hw_resources, entry_handle_id, &inlif_info,
                     selectable_result_types));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Allocate a Flow virtual value (with no encoding), encode it to a gport and store as flow_id
 */
static shr_error_e
dnx_flow_initiator_info_virtual_gport_allocate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info)
{
    int virtual_id = 0;
    int virtual_val;
    uint32 sw_handle_alloc_flags = 0;
    char *app_name;

    SHR_FUNC_INIT_VARS(unit);

    /** Handle the WITH_ID case similarly for all Virtual gports */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID))
    {
        sw_handle_alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        virtual_val = _SHR_GPORT_FLOW_LIF_VAL_GET(flow_handle_info->flow_id);
    }

    app_name = FLOW_APP_NAME_VLAN_PORT_ESEM_INITIATOR;

    if (!sal_strncasecmp(flow_app_info->app_name, app_name, FLOW_STR_MAX))
    {
        SHR_IF_ERR_EXIT(lif_table_mngr_db.flow_egress_virtual_gport_id.allocate_single(unit,
                                                                                       sw_handle_alloc_flags,
                                                                                       NULL, &virtual_val));

        /** Encode the virtual_val as a flow virtual gport with a subtype */
        _SHR_GPORT_FLOW_LIF_SET(virtual_id, _SHR_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, virtual_val);
        _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SET(virtual_id, virtual_id);
    }
    else if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_ESEM_DEFAULT_INITIATOR, FLOW_STR_MAX))
    {
        SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.allocate_single(unit,
                                                                                    sw_handle_alloc_flags,
                                                                                    NULL, &virtual_val));

        /** Encode the virtual_val as a flow virtual gport with a subtype */
        _SHR_GPORT_FLOW_LIF_SET(virtual_id, _SHR_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, virtual_val);
        _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(virtual_id, virtual_id);
    }

    /** Set the virtual id as a gport flow_id */
    _SHR_GPORT_FLOW_LIF_ID_SET(flow_handle_info->flow_id, virtual_id);

exit:
    SHR_FUNC_EXIT;
}

/** create the flow ID - this can be global lif or virtual ID according to the application */
static shr_error_e
dnx_flow_gport_flow_id_allocate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    global_lif_alloc_info_t * alloc_info)
{
    int global_lif = DNX_ALGO_GPM_LIF_INVALID;
    uint32 alloc_flags = 0;
    uint32 direction_flags =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM) ? DNX_ALGO_LIF_INGRESS : DNX_ALGO_LIF_EGRESS;

    SHR_FUNC_INIT_VARS(unit);

    if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
        && dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF))
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_virtual_gport_allocate(unit, flow_handle_info, flow_app_info));
        SHR_EXIT();
    }

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID))
    {
        alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        global_lif = BCM_GPORT_TUNNEL_ID_GET(flow_handle_info->flow_id);
    }

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_L2_GLOBAL_LIF)
        || (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM))
    {
        alloc_flags |= LIF_MNGR_L2_GPORT_GLOBAL_LIF;
    }

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC))
    {
        /** Allocate as symmetric only if it wasn't already allocated or reserved as such */
        uint8 is_symmetric_allocated = FALSE;
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID))
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.
                            symmetric_global_lif.is_allocated(unit, global_lif, &is_symmetric_allocated));
        }

        if (!is_symmetric_allocated)
        {
            alloc_flags |= LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF;
        }
    }

    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                    (unit, alloc_flags, direction_flags, alloc_info, &global_lif));

    BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, global_lif);

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
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    int local_inlif = 0;
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_table_mngr_inlif_info = { 0 };
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    uint32 selectable_result_types = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    SHR_IF_ERR_EXIT(dnx_flow_gport_flow_id_allocate(unit, flow_handle_info, flow_app_info, NULL));

    lif_table_mngr_inlif_info.global_lif = _SHR_GPORT_FLOW_LIF_ID_GET(flow_handle_info->flow_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE,
                                 lif_table_mngr_inlif_info.global_lif);

    SHR_IF_ERR_EXIT(dnx_flow_common_fields_set
                    (unit, flow_app_info, FLOW_APP_TYPE_TERM, flow_handle_info, terminator_info, &gport_hw_resources,
                     entry_handle_id, NULL));

    if (special_fields)
    {
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit,
                                                    entry_handle_id,
                                                    flow_handle_info,
                                                    flow_app_info, &gport_hw_resources, special_fields));
    }

    /** Check if errors occurred on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    /*
     * If a result type selection callback is defined - call it
     */
    if (flow_app_info->result_type_select_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->result_type_select_cb
                        (unit, entry_handle_id, flow_handle_info, &gport_hw_resources, terminator_info,
                         special_fields, &lif_table_mngr_inlif_info, &selectable_result_types));
    }

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control,
                         &gport_hw_resources, terminator_info, special_fields, &lif_table_mngr_inlif_info,
                         &selectable_result_types));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    /** Call lif table manager set */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_create
                    (unit, flow_handle_info, entry_handle_id, &local_inlif, &lif_table_mngr_inlif_info,
                     selectable_result_types));

exit:
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
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 entry_handle_id = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /** Take handle to read from DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_get
                    (unit, flow_app_info, flow_handle_info, &gport_hw_resources, entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_common_fields_get
                    (unit, FLOW_APP_TYPE_TERM, flow_app_info, flow_handle_info, NULL, terminator_info,
                     &gport_hw_resources, entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_get(unit, entry_handle_id, flow_handle_info, flow_app_info,
                                                &gport_hw_resources, special_fields));

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control,
                         &gport_hw_resources, terminator_info, special_fields, NULL, NULL));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * set the alloc info structure in case of concatenate tunnel
 */
static shr_error_e
dnx_flow_initiator_global_lif_alloc_info_set(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_initiator_info_t * initiator_info,
    global_lif_alloc_info_t * alloc_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** if BTA is supported add the bta info to the allocation info */
    if (dnx_data_lif.global_lif.global_lif_bta_max_val_get(unit))
    {
        if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID) &&
            (initiator_info->l3_intf_id != 0) && (initiator_info->l3_intf_id != BCM_IF_INVALID))
        {
            /** ipv4 or ipv6 over UDP is possible + GTP. Take the smaller between them */
            if (sal_strncmp(flow_app_info->app_name, "GTP_INITIATOR", sizeof(flow_app_info->app_name)) == 0)
            {
                alloc_info->bta = dnx_data_lif.out_lif.global_gtp_concat_ip_outlif_bta_sop_get(unit);
            }

            /** BIER */
            if (sal_strncmp(flow_app_info->app_name, "BIER_BASE_INITIATOR", sizeof(flow_app_info->app_name)) == 0)
            {
                alloc_info->bta = dnx_data_lif.out_lif.global_bier_outlif_bta_sop_get(unit);
            }

        }
    }

/** exit: */
    SHR_FUNC_EXIT;
}

/*
 * Free an allocated Flow virtual gport 
 */
static shr_error_e
dnx_flow_initiator_info_virtual_gport_free(
    int unit,
    bcm_gport_t virtual_lif_gport)
{
    uint32 gport_subtype, virtual_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    gport_subtype = BCM_GPORT_SUB_TYPE_GET(virtual_lif_gport);
    virtual_val = _SHR_GPORT_FLOW_LIF_VAL_GET(virtual_lif_gport);

    switch (gport_subtype)
    {
        case BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH:
            SHR_IF_ERR_EXIT(lif_table_mngr_db.flow_egress_virtual_gport_id.free_single(unit, virtual_val, NULL));
            break;
        case BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT:
            SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.free_single(unit, virtual_val, NULL));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported gport sub-type %d for virtual gport 0x%x\n", gport_subtype,
                         virtual_lif_gport);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Internal egress flow lif create
 */
static shr_error_e
dnx_flow_initiator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    int local_outlif;
    uint32 entry_handle_id;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    uint32 selectable_result_types = 0;
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info = { 0 };
    bcm_encap_access_t encap_access;
    int is_virtual;
    global_lif_alloc_info_t alloc_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    is_virtual = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /** Create global lif */
    SHR_IF_ERR_EXIT(dnx_flow_initiator_global_lif_alloc_info_set(unit, flow_app_info, initiator_info, &alloc_info));

    SHR_IF_ERR_EXIT(dnx_flow_gport_flow_id_allocate(unit, flow_handle_info, flow_app_info, &alloc_info));
    lif_table_mngr_outlif_info.global_lif = _SHR_GPORT_FLOW_LIF_ID_GET(flow_handle_info->flow_id);

    /*
     * Special treatment for encap access - mandatory for initiator, set on lif table manager outlif info
     * struct. Applicable only for EEDB entries, not for Virtual entries.
     */
    if (!is_virtual)
    {
        DNX_FLOW_SET_ENCAP_ACCESS(encap_access, initiator_info->encap_access,
                                  flow_app_info->encap_access_default_mapping);
        SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert
                        (unit, encap_access, &lif_table_mngr_outlif_info.logical_phase));
    }

    /*
     * Call the set callbacks for the common and special fields
     */
    SHR_IF_ERR_EXIT(dnx_flow_common_fields_set
                    (unit, flow_app_info, FLOW_APP_TYPE_INIT, flow_handle_info, initiator_info, &gport_hw_resources,
                     entry_handle_id, &lif_table_mngr_outlif_info));

    if (special_fields)
    {
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit,
                                                    entry_handle_id,
                                                    flow_handle_info,
                                                    flow_app_info, &gport_hw_resources, special_fields));
    }

    /** executing logic related to NATIVE flag */
    if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_NATIVE))
    {
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_NATIVE))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, FALSE);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, TRUE);
        }
    }

    /** Check if errors occurred on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID))
    {
        /** update the QoS */
        SHR_IF_ERR_EXIT(dnx_flow_init_dedicated_logic_qos_create
                        (unit, entry_handle_id, initiator_info, special_fields));
    }

    /*
     * If a result type selection callback is defined - call it
     */
    if (flow_app_info->result_type_select_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->result_type_select_cb
                        (unit, entry_handle_id, flow_handle_info, &gport_hw_resources, initiator_info,
                         special_fields, &lif_table_mngr_outlif_info, &selectable_result_types));
    }

    /*
     * If a specific application callback is defined - call it 
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control,
                         &gport_hw_resources, initiator_info, special_fields, &lif_table_mngr_outlif_info,
                         &selectable_result_types));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    /*
     * For virtual LIFs that contain a match, store in SW the gport mapping to the match info
     */
    if (is_virtual)
    {
        SHR_IF_ERR_EXIT(dnx_flow_virtual_match_sw_add(unit, flow_handle_info->flow_id, entry_handle_id, flow_app_info));
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw
                    (unit, flow_handle_info, entry_handle_id, &local_outlif, &lif_table_mngr_outlif_info,
                     selectable_result_types));

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

shr_error_e
dnx_flow_initiator_encap_access_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_encap_access_t * encap_access)
{
    dnx_algo_local_outlif_logical_phase_e logical_phase;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                    (unit, gport_hw_resources->local_out_lif, NULL, NULL, &logical_phase, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_lif_lib_logical_phase_to_encap_access_convert(unit, logical_phase, encap_access));

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
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    int global_lif_id = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    uint32 lif_get_flags;
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info_get = { 0 };
    uint32 entry_handle_id, get_entry_handle_id = 0;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info = { 0 };
    uint32 selectable_result_types = 0;
    bcm_encap_access_t orig_encap_access = initiator_info->encap_access;
    int is_virtual;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    is_virtual = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF);

    lif_get_flags = (is_virtual) ? DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL :
        (DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_get_flags, &gport_hw_resources));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &get_entry_handle_id));

    if (!is_virtual)
    {
        lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

        global_lif_id = _SHR_GPORT_FLOW_LIF_ID_GET(flow_handle_info->flow_id);

        lif_table_mngr_outlif_info.global_lif = global_lif_id;

        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_get_lif_table_manager_outlif
                        (unit, flow_handle_info, initiator_info, get_entry_handle_id, &lif_table_mngr_outlif_info_get));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_virtual_entry_on_handle_get
                        (unit, flow_handle_info->flow_id, get_entry_handle_id));
    }

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));

    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /*
     * Special treatment for encap access, mandatory for initiator, set on lif table manager outlif info struct
     */
    if (!is_virtual)
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_encap_access_get
                        (unit, &gport_hw_resources, initiator_info, &initiator_info->encap_access));

        SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert
                        (unit, initiator_info->encap_access, &lif_table_mngr_outlif_info.logical_phase));
    }

    SHR_IF_ERR_EXIT(dnx_flow_common_fields_replace
                    (unit, flow_app_info, FLOW_APP_TYPE_INIT, flow_handle_info, initiator_info, &gport_hw_resources,
                     entry_handle_id, get_entry_handle_id));

    if (special_fields)
    {
        SHR_IF_ERR_EXIT(dnx_flow_special_fields_replace
                        (unit, entry_handle_id, get_entry_handle_id, flow_handle_info,
                         flow_app_info, &gport_hw_resources, special_fields));
    }

    /** Check if errors occurred on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    /** special treatment for qos egress fields */
    SHR_IF_ERR_EXIT(dnx_flow_init_dedicated_logic_qos_replace
                    (unit, flow_handle_info, flow_app_info, entry_handle_id, get_entry_handle_id, &gport_hw_resources,
                     initiator_info, special_fields));

    /*
     * If a result type selection callback is defined - call it
     */
    if (flow_app_info->result_type_select_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->result_type_select_cb
                        (unit, entry_handle_id, flow_handle_info, &gport_hw_resources, initiator_info,
                         special_fields, &lif_table_mngr_outlif_info, &selectable_result_types));
    }
    /*
     * If a specific application callback is defined - call it 
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, get_entry_handle_id, flow_handle_info, flow_cmd_control,
                         &gport_hw_resources, initiator_info, special_fields, &lif_table_mngr_outlif_info,
                         &selectable_result_types));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw
                    (unit, flow_handle_info, entry_handle_id, &gport_hw_resources.local_out_lif,
                     &lif_table_mngr_outlif_info, selectable_result_types));

    /*
     * Replace virtual LIFs can change the payload but not the match keys, so there's no need to store the match in the SW again. 
     * dnx_flow_virtual_result_type_update is called from dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw() 
     * to store a replaced result-type in SW.
     */

exit:
    initiator_info->encap_access = orig_encap_access;
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
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields)
{
    lif_table_mngr_outlif_info_t outlif_info;
    uint32 entry_handle_id = 0;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    bcm_encap_access_t encap_access;
    int is_virtual, is_local_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));
    is_virtual = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF);

    lif_flags = (is_virtual) ? DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL :
        (DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));

    if (is_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_get_lif_table_manager_outlif
                        (unit, flow_handle_info, initiator_info, entry_handle_id, &outlif_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_virtual_entry_on_handle_get
                        (unit, flow_handle_info->flow_id, entry_handle_id));
    }

    /*
     * Encap access get. In case a default application value exists and it is similar to what was set - return encap
     * access invalid. Otherwise - return the encap access set
     */
    if (!is_virtual)
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_encap_access_get(unit, &gport_hw_resources, initiator_info, &encap_access));
        initiator_info->encap_access =
            (flow_app_info->encap_access_default_mapping == encap_access) ? bcmEncapAccessInvalid : encap_access;
    }

    SHR_IF_ERR_EXIT(dnx_flow_common_fields_get
                    (unit, FLOW_APP_TYPE_INIT, flow_app_info, flow_handle_info, initiator_info, NULL,
                     &gport_hw_resources, entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_get(unit, entry_handle_id, flow_handle_info, flow_app_info,
                                                &gport_hw_resources, special_fields));

    if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_NATIVE))
    {
        uint32 last_layer = 0;
        shr_error_e rv;
        rv = dbal_entry_handle_value_field32_get
            (unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, &last_layer);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if ((rv != _SHR_E_NOT_FOUND) && !last_layer)
        {
            flow_handle_info->flags |= BCM_FLOW_HANDLE_NATIVE;
        }
    }

    SHR_IF_ERR_EXIT(dnx_flow_init_dedicated_logic_qos_get
                    (unit, flow_app_info, entry_handle_id, initiator_info, special_fields));

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control,
                         &gport_hw_resources, initiator_info, special_fields, &outlif_info, NULL));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_local_lif_to_flow_id(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 lif,
    int core_id)
{
    lif_mapping_local_lif_key_t local_lif_info;
    int global_lif;
    uint32 mapping_flags =
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH) ? DNX_ALGO_LIF_INGRESS : DNX_ALGO_LIF_EGRESS;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&local_lif_info, 0, sizeof(local_lif_info));

    /** in in_lif_phy_db_dpc the LIF is created over all cores. When !in_lif_phy_db_dpc, there is a need to check to
     *  which LIF phy DB the lif is related in order to find the mapping */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
    {
        if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
        {
            bcm_flow_handle_t match_payload_flow_handle_id;
            dnx_flow_app_config_t *payload_flow_app_info;

            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get
                            (unit, flow_app_info->match_payload_apps[0], &match_payload_flow_handle_id));
            payload_flow_app_info = dnx_flow_objects[match_payload_flow_handle_id];

            SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                            (unit, payload_flow_app_info->flow_table, DBAL_PHY_DB_DEFAULT_INDEX,
                             &(local_lif_info.phy_table)));
        }
    }

    local_lif_info.local_lif = lif;
    local_lif_info.core_id = core_id;
    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get(unit, mapping_flags, &local_lif_info, &global_lif));

    BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, global_lif);

exit:
    SHR_FUNC_EXIT;
}

/** \brief
 *  Function set the key fields for entry according to the key_special_fields. for TCAM match table there is a dedicated
 *  logic for mask fields, for those fields, first finding all the mask and maskable fields pos in
 *  key_special_fields, than looking for matching pairs and eventually setting them to DBAL */
/*
 * Internal match add entry
 */
static shr_error_e
dnx_flow_match_info_add(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_special_fields_t * key_special_fields)
{
    uint32 entry_handle_id;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    dbal_table_type_e table_type;
    uint32 entry_access_id;
    dbal_tables_e dbal_table =
        (!(_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD))) ? flow_app_info->flow_table :
        flow_app_info->second_pass_table;
    int lif_per_core = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** Create entry access id to TCAM */
    if (table_type == DBAL_TABLE_TYPE_TCAM)
    {
        SHR_IF_ERR_EXIT(dbal_entry_attribute_set
                        (unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, flow_handle_info->flow_priority));
    }

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
    {
        lif_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    }
    else
    {
        lif_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit, entry_handle_id,
                                                flow_handle_info,
                                                flow_app_info, &gport_hw_resources, key_special_fields));

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
    {
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc) &&
            (gport_hw_resources.local_in_lif_per_core.core_id[0] != _SHR_CORE_ALL))
        {
            lif_per_core = TRUE;
        }
        else
        {
            if (dbal_table != DBAL_TABLE_INGRESS_PP_PORT)
            {
                /**  The INGRESS_PP_PORT has a different In-LIF field name - DEFAULT_LIF, this is handled in the specific app cb */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                                             gport_hw_resources.local_in_lif);
            }
        }
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE,
                                     gport_hw_resources.local_out_lif);
    }

    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control,
                         &gport_hw_resources, NULL, key_special_fields, NULL, NULL));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    /** Check if errors occured on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    if (!lif_per_core)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
        {
            if (table_type == DBAL_TABLE_TYPE_TCAM)
            {
                SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
                SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    else
    {
        int core_iter;

        for (core_iter = 0; core_iter < gport_hw_resources.local_in_lif_per_core.nof_in_lifs; core_iter++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_hw_resources.local_in_lif_per_core.core_id[core_iter]);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                                         gport_hw_resources.local_in_lif_per_core.local_in_lif[core_iter]);

            if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
            {
                if (table_type == DBAL_TABLE_TYPE_TCAM)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get
                                    (unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
                }
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

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
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_special_fields_t * key_special_fields)
{
    uint32 entry_handle_id;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    dbal_table_type_e table_type;
    dbal_tables_e dbal_table =
        (!(_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD))) ? flow_app_info->flow_table :
        flow_app_info->second_pass_table;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table, &core_mode));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit,
                                                entry_handle_id,
                                                flow_handle_info, flow_app_info, NULL, key_special_fields));

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control, NULL,
                         NULL, key_special_fields, NULL, NULL));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    {
        int core_iter;
        int nof_cores = (core_mode == DBAL_CORE_MODE_SBC || (dnx_data_lif.in_lif.feature_get(unit,
                                                                                             dnx_data_lif_in_lif_phy_db_dpc)))
            ? 1 : dnx_data_device.general.nof_cores_get(unit);
        int entry_clear_rv = 0;
        int entry_cleared = 0;

        for (core_iter = 0; core_iter < nof_cores; core_iter++)
        {
            if (nof_cores == 1)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_iter);
            }
            if (table_type == DBAL_TABLE_TYPE_TCAM)
            {
                int rv;
                uint32 entry_access_id;
                rv = dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT);

                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
                    /** Delete entry from DBAL table*/
                    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
                    entry_cleared = 1;
                }
            }
            else
            {
                entry_clear_rv |= dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT);
                if (entry_clear_rv == _SHR_E_NONE)
                {

                    entry_cleared = 1;
                }
            }
        }

        if (!entry_cleared)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry not found for related key \n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** Internal match entry get */
static shr_error_e
dnx_flow_match_info_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_special_fields_t * key_special_fields)
{
    uint32 entry_handle_id;
    uint32 local_lif;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    dbal_table_type_e table_type;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;
    dbal_tables_e dbal_table =
        (!(_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD))) ? flow_app_info->flow_table :
        flow_app_info->second_pass_table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** setting the key of the entry */
    SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit, entry_handle_id,
                                                flow_handle_info, flow_app_info, NULL, key_special_fields));

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table, &core_mode));
    if ((core_mode == DBAL_CORE_MODE_DPC) && !(dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)))
    {
        /** setting the core_id to default (get requires to be done on only one of the cores) */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    }

    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control, NULL,
                         NULL, key_special_fields, NULL, NULL));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    if (table_type == DBAL_TABLE_TYPE_TCAM)
    {
        uint32 tcam_handler_id, entry_access_id;
        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table, &tcam_handler_id));
        SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_priority_get
                        (unit, tcam_handler_id, entry_access_id, &flow_handle_info->flow_priority));
    }

    /** in case of entry was added not to all the cores need to loop over all cores and see if entry exists once found,
     *  need to return the related global LIF */
    if ((core_mode == DBAL_CORE_MODE_DPC) && (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH) &&
        dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        int rv, core_iter;
        int nof_cores = dnx_data_device.general.nof_cores_get(unit);
        for (core_iter = 0; core_iter < nof_cores; core_iter++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_iter);
            rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (rv == _SHR_E_NONE)
            {

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &local_lif));

                /** Retrieve the flow id from the lif and encode it as a gport */
                SHR_IF_ERR_EXIT(dnx_flow_local_lif_to_flow_id
                                (unit, flow_app_info, flow_handle_info, local_lif, core_iter));

                SHR_EXIT();
            }
        }

        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry not found for related key \n");
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &local_lif));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, &local_lif));
        }

        /** Retrieve the flow id from the lif and encode it as a gport */
        SHR_IF_ERR_EXIT(dnx_flow_local_lif_to_flow_id
                        (unit, flow_app_info, flow_handle_info, local_lif, DBAL_CORE_DEFAULT));
    }

    if (flow_app_info->is_entry_related &&
        dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE))
    {
        int is_entry_related;

        SHR_IF_ERR_EXIT(flow_app_info->is_entry_related(unit, entry_handle_id, &is_entry_related));
        if (!is_entry_related)
        {
            /** no matching entry found */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** returns is_designated true according to predefine logic (lif on the lowest core_id) */
static shr_error_e
dnx_flow_is_lif_designated(
    int unit,
    int core_id,
    bcm_flow_handle_info_t * flow_handle_info,
    int global_lif,
    int *is_designated)
{
    lif_mapping_local_lif_payload_t local_lif_payload;
    int core_iter;

    SHR_FUNC_INIT_VARS(unit);

    (*is_designated) = 0;

    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_global_to_local_get
                    (unit, DNX_ALGO_LIF_INGRESS, global_lif, &local_lif_payload));

    (*is_designated) = 1;
    /** run over all cores up to the core_id, and check if a lif exists if yes the lif is not designated since a lower
     *  lif exists */
    for (core_iter = 0; core_iter < core_id; core_iter++)
    {
        if (local_lif_payload.local_lifs_array.local_lifs[core_iter] != LIF_MNGR_INLIF_INVALID)
        {
            (*is_designated) = 0;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_terminator_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_traverse_cb cb,
    void *user_data,
    dnx_flow_traverse_operation_e traverse_operation)
{
    uint32 entry_handle_id;
    int is_end;
    bcm_flow_terminator_info_t terminator_info = { 0 };
    dnx_flow_app_config_t *flow_app_info;
    bcm_flow_special_fields_t special_fields = { 0 };
    int global_lif;
    lif_mapping_local_lif_key_t local_lif_info;
    uint32 key_field;
    dbal_physical_tables_e physical_table_id;
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_GET, FLOW_COMMAND_CONTINUE };

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

    if ((!_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM))
        && (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)))
    {
        uint32 core_id = DBAL_CORE_DEFAULT;
        /** in case that LIF tables are per core and the application not supporting per app allocation no need to run over
         *  all cores.  So Add KEY rule to skip duplicated entry  */
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                               DBAL_CONDITION_EQUAL_TO, &core_id, NULL));
    }

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        shr_error_e rv;
        uint32 core_id;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF, &key_field));
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = key_field;
        local_lif_info.phy_table = physical_table_id;

        if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM) &&
            dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_CORE_ID, &core_id));
            local_lif_info.core_id = core_id;
        }
        else
        {
            core_id = 0;
        }

        rv = dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_INGRESS, &local_lif_info, &global_lif);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            BCM_GPORT_TUNNEL_ID_SET(flow_handle_info->flow_id, global_lif);

            if (traverse_operation == FLOW_TRAVERSE_OPERATION_DELETE)
            {
                /** Delete the entry   */
                SHR_IF_ERR_EXIT(bcm_dnx_flow_terminator_info_destroy(unit, flow_handle_info));
            }
            else
            {
                int is_designated;

                if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM) &&
                    dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
                {
                    SHR_IF_ERR_EXIT(dnx_flow_is_lif_designated
                                    (unit, core_id, flow_handle_info, global_lif, &is_designated));
                }
                else
                {
                    /** in case that app is not supporting allocation by core, lif is always the designated */
                    is_designated = TRUE;
                }

                if (is_designated)
                {
                    SHR_IF_ERR_EXIT(dnx_flow_terminator_info_get
                                    (unit, flow_handle_info, &flow_cmd_control, &terminator_info, &special_fields));

                    /** Invoke callback function */
                    SHR_IF_ERR_EXIT((*cb) (unit, flow_handle_info, &terminator_info, &special_fields, user_data));
                }
            }
        }

        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_initiator_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_traverse_cb cb,
    void *user_data,
    dnx_flow_traverse_operation_e traverse_operation)
{
    uint32 entry_handle_id;
    int is_end;
    int global_lif, is_local_lif;
    lif_mapping_local_lif_key_t local_lif_info;
    uint32 key_field;
    bcm_flow_initiator_info_t initiator_info;
    bcm_flow_special_fields_t special_fields;
    dnx_flow_app_config_t *flow_app_info;
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_GET, FLOW_COMMAND_CONTINUE };
    dbal_table_type_e table_type;
    dbal_physical_tables_e physical_table_id = DBAL_PHYSICAL_TABLE_NONE;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;
    uint32 core_id = DBAL_CORE_DEFAULT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, flow_app_info->flow_table, &table_type));
    if (table_type != DBAL_TABLE_TYPE_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                        (unit, flow_app_info->flow_table, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));
        core_mode = dbal_mdb_phy_table_core_mode_get(unit, physical_table_id);
    }

    if (core_mode == DBAL_CORE_MODE_DPC)
    {
        /** Add KEY rule to skip duplicated entry */
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                               DBAL_CONDITION_EQUAL_TO, &core_id, NULL));
    }

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        shr_error_e rv;

        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &key_field));
            local_lif_info.local_lif = key_field;
            local_lif_info.core_id = core_id;
            rv = dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            _SHR_GPORT_FLOW_LIF_ID_SET(flow_handle_info->flow_id, global_lif);
        }
        else
        {
            
            rv = dnx_flow_virtual_gport_by_match_lookup(unit, entry_handle_id, flow_handle_info,
                                                        &flow_handle_info->flow_id);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        }

        if (rv == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_flow_initiator_info_get
                            (unit, flow_handle_info, &flow_cmd_control, &initiator_info, &special_fields));

            if (traverse_operation == FLOW_TRAVERSE_OPERATION_DELETE)
            {
                /** Delete the entry   */
                SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_destroy(unit, flow_handle_info));
            }
            else
            {
                /** Invoke callback function */
                SHR_IF_ERR_EXIT((*cb) (unit, flow_handle_info, &initiator_info, &special_fields, user_data));
            }

        }

        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_match_key_fields_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dbal_table_type_e table_type,
    bcm_flow_special_fields_t * key_special_fields,
    int nof_app_special_fields)
{
    int key_idx;
    flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);

    /** Get each key, if the key has mask keeping the mask value and the field ID. at the end appending them to the array
     *  of special fields returning to the user */
    for (key_idx = 0; key_idx < nof_app_special_fields; key_idx++)
    {
        curr_field = &flow_special_fields_db[key_special_fields->special_fields[key_idx].field_id];

        if ((table_type == DBAL_TABLE_TYPE_TCAM)
            && dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MASK_SUPPORTED))
        {
            SHR_IF_ERR_EXIT(flow_special_fields_mask_field_value_get
                            (unit, entry_handle_id, curr_field, key_special_fields, key_idx));
        }
        else if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_IS_MASK) ||
                 dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MATCH_IGNORE_OPERATION))
        {
            /** do nothing */
            continue;
        }
        else if (curr_field->get != NULL)
        {
            int rv;

            rv = curr_field->get
                (unit, entry_handle_id, flow_handle_info, flow_app_info, NULL,
                 key_special_fields->special_fields[key_idx].field_id, &(key_special_fields->special_fields[key_idx]));
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_UNAVAIL);
            if (rv == _SHR_E_UNAVAIL)
            {
                continue;
            }

        }
        else if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type) &&
                 (curr_field->mapped_dbal_field == DBAL_FIELD_VMPI) &&
                 !dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_per_port_ilm_lookup_supported))
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT(flow_special_field_key_direct_mapping_get
                            (unit, curr_field, entry_handle_id, &(key_special_fields->special_fields[key_idx])));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_match_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_match_info_traverse_cb cb,
    void *user_data,
    dnx_flow_traverse_operation_e traverse_operation)
{
    bcm_flow_special_fields_t key_special_fields = { 0 };
    uint32 entry_handle_id;
    int is_end;
    int global_lif;
    lif_mapping_local_lif_key_t local_lif_info;
    dnx_flow_app_config_t *flow_app_info;
    int nof_app_special_fields = 0;
    dbal_table_type_e table_type;
    dbal_physical_tables_e physical_table_id = DBAL_PHYSICAL_TABLE_NONE;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_TRAVERSE, FLOW_COMMAND_CONTINUE };
    dbal_tables_e dbal_table;
    int local_lif, is_lif_dpc = dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc);
    uint32 core_id = DBAL_CORE_DEFAULT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    dbal_table =
        (!(_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD))) ? flow_app_info->flow_table :
        flow_app_info->second_pass_table;

    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));
    if (table_type != DBAL_TABLE_TYPE_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                        (unit, dbal_table, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));
        core_mode = dbal_mdb_phy_table_core_mode_get(unit, physical_table_id);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    if (!is_lif_dpc)
    {
        if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH) && (core_mode != DBAL_CORE_MODE_SBC)
            && (dbal_table != DBAL_TABLE_INGRESS_PP_PORT))
        {
          /** Add KEY rule to skip duplicated entry */
            SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                                   DBAL_CONDITION_EQUAL_TO, &core_id, NULL));
        }
    }

    /** Get number of key special fields, serves as key for match applications Assign each key field id */
    while (flow_app_info->special_fields[nof_app_special_fields] != FLOW_S_F_EMPTY)
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
        int rv;

        if (flow_app_info->is_entry_related &&
            dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE))
        {
            int is_entry_related;

            SHR_IF_ERR_EXIT(flow_app_info->is_entry_related(unit, entry_handle_id, &is_entry_related));
            if (!is_entry_related)
            {
                /** Receive next entry in table.*/
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                continue;
            }
        }

        SHR_IF_ERR_EXIT(dnx_flow_match_key_fields_get
                        (unit, entry_handle_id, flow_handle_info, flow_app_info, table_type, &key_special_fields,
                         nof_app_special_fields));
        /*
         * If a specific application callback is defined - call it
         */
        if (flow_app_info->app_specific_operations_cb)
        {
            SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                            (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, &flow_cmd_control,
                             NULL, NULL, &key_special_fields, NULL, NULL));
        }

        if (flow_cmd_control.flow_command == FLOW_COMMAND_CONTINUE)
        {
            if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, (uint32 *) &local_lif));

#if defined(BCM_DNX2_SUPPORT)
                if (!is_lif_dpc)
                {
                    local_lif_info.phy_table = (physical_table_id == DBAL_PHYSICAL_TABLE_ISEM_2) ?
                        DBAL_PHYSICAL_TABLE_INLIF_2 : DBAL_PHYSICAL_TABLE_INLIF_1;
                }
#endif
                local_lif_info.local_lif = local_lif;
                local_lif_info.core_id = core_id;
                rv = dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_INGRESS, &local_lif_info, &global_lif);
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, (uint32 *) &local_lif));
                local_lif_info.local_lif = local_lif;
                local_lif_info.core_id = core_id;
                rv = dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif);
            }

            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                _SHR_GPORT_FLOW_LIF_ID_SET(flow_handle_info->flow_id, global_lif);
            }
        }

        if (flow_cmd_control.flow_command != FLOW_COMMAND_SKIP_TO_END)
        {
            if (traverse_operation == FLOW_TRAVERSE_OPERATION_USE_CALLBACK)
            {
                int is_designated = 1;

                /** Invoke the callback function */
                if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH) && (core_mode != DBAL_CORE_MODE_SBC)
                    && (dbal_table != DBAL_TABLE_INGRESS_PP_PORT) && (is_lif_dpc))
                {
                    uint32 core_id;
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_CORE_ID, &core_id));

                    SHR_IF_ERR_EXIT(dnx_flow_is_lif_designated
                                    (unit, core_id, flow_handle_info, global_lif, &is_designated));
                }

                if (is_designated)
                {
                    SHR_IF_ERR_EXIT((*cb) (unit, flow_handle_info, &key_special_fields, user_data));
                }
            }

            if (traverse_operation == FLOW_TRAVERSE_OPERATION_DELETE)
            {
                dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_DELETE, FLOW_COMMAND_CONTINUE };

                SHR_IF_ERR_EXIT(dnx_flow_match_info_delete
                                (unit, flow_handle_info, &flow_cmd_control, &key_special_fields));
            }
        }

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
    int app_idx, num_apps;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    num_apps = dnx_flow_number_of_apps();

    for (app_idx = FLOW_APP_FIRST_IDX; app_idx < num_apps; app_idx++)
    {
        if (FLOW_APP_IS_NOT_VALID(dnx_flow_objects[app_idx]))
        {
            continue;
        }
        if (!sal_strncasecmp(flow_name, dnx_flow_objects[app_idx]->app_name, FLOW_STR_MAX))
        {
            *handle = app_idx;
            break;
        }
    }
    if (app_idx == num_apps)
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
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_SET, FLOW_COMMAND_CONTINUE };
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_create_verify(unit, flow_handle_info, terminator_info, special_fields));

    /*
     * Set
     */
    if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_flow_terminator_info_create
                        (unit, flow_handle_info, &flow_cmd_control, terminator_info, special_fields));
    }
    /*
     * Replace
     */
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_terminator_info_replace
                        (unit, flow_handle_info, &flow_cmd_control, terminator_info, special_fields));
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
    int field = 0;
    uint32 temp_common_bitmap;
    uint32 entry_handle_id;
    bcm_flow_terminator_info_t terminator_info;
    dnx_flow_app_config_t *flow_app_info;
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_DELETE, FLOW_COMMAND_CONTINUE };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_delete_verify(unit, FLOW_APP_TYPE_TERM, flow_handle_info, NULL));

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, flow_app_info->flow_table, &entry_handle_id));

    /** Get global and local inLIF from GPort hw resources */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_get
                    (unit, flow_app_info, flow_handle_info, &gport_hw_resources, entry_handle_id));

    /*
     * Iterate over all common fields. If exists in application and delete callback exists - delete it
     */
    temp_common_bitmap = flow_app_info->valid_common_fields_bitmap;
    while (temp_common_bitmap != 0)
    {
        if ((temp_common_bitmap & 0x1) && (dnx_flow_term_common_fields[field]->field_cbs.field_delete_cb))
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_term_common_fields[field]->
                                      field_cbs.field_delete_cb(unit, entry_handle_id, FLOW_APP_TYPE_TERM,
                                                                flow_handle_info, &gport_hw_resources, flow_app_info,
                                                                &terminator_info), _SHR_E_NOT_FOUND);
        }
        temp_common_bitmap = (temp_common_bitmap >> 1);
        field++;
    }

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_destroy
                    (unit, entry_handle_id, flow_handle_info, flow_app_info, &gport_hw_resources));

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, &flow_cmd_control,
                         &gport_hw_resources, &terminator_info, NULL, NULL, NULL));
        if (flow_cmd_control.flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    /** Delete entry from an In-LIF table - Delete the local In-LIF */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_clear(unit, &gport_hw_resources));

    /** Delete the Global In-LIF */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free
                    (unit, DNX_ALGO_LIF_INGRESS, _SHR_GPORT_FLOW_LIF_ID_GET(flow_handle_info->flow_id)));

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
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_GET, FLOW_COMMAND_CONTINUE };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_get_verify
                           (unit, FLOW_APP_TYPE_TERM, flow_handle_info, terminator_info, special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_terminator_info_get
                    (unit, flow_handle_info, &flow_cmd_control, terminator_info, special_fields));

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
    dnx_flow_traverse_operation_e traverse_operation = _SHR_IS_FLAG_SET(flow_handle_info->flags,
                                                                        BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL) ?
        FLOW_TRAVERSE_OPERATION_DELETE : FLOW_TRAVERSE_OPERATION_USE_CALLBACK;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_flow_traverse_verify(unit, flow_handle_info, (void *) &cb, user_data));

    if (traverse_operation == FLOW_TRAVERSE_OPERATION_DELETE)
    {
        /** removing  BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL flag since traverse will call entry destroy and this flag is
         *  illegal there */
        flow_handle_info->flags = 0;
    }

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
            if (FLOW_APP_IS_NOT_VALID(dnx_flow_objects[app_idx]))
            {
                continue;
            }
            if (dnx_flow_objects[app_idx]->flow_app_type == FLOW_APP_TYPE_TERM)
            {
                traverse_flow_handle_info.flow_handle = app_idx;
                SHR_IF_ERR_EXIT(dnx_flow_terminator_info_traverse
                                (unit, &traverse_flow_handle_info, cb, user_data, traverse_operation));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_terminator_info_traverse(unit, flow_handle_info, cb, user_data, traverse_operation));
    }

    if (traverse_operation == FLOW_TRAVERSE_OPERATION_DELETE)
    {
        /** returning the flag to original value */
        flow_handle_info->flags |= BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL;
    }

exit:
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
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_SET, FLOW_COMMAND_CONTINUE };
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_create_verify(unit, flow_handle_info, initiator_info, special_fields));

    /*
     * Set
     */
    if (!_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_create
                        (unit, flow_handle_info, &flow_cmd_control, initiator_info, special_fields));
    }
    /*
     * Replace
     */
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_replace
                        (unit, flow_handle_info, &flow_cmd_control, initiator_info, special_fields));
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
    int field = 0;
    uint32 temp_common_bitmap;
    uint32 entry_handle_id;
    bcm_flow_initiator_info_t initiator_info;
    dnx_flow_app_config_t *flow_app_info;
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_DELETE, FLOW_COMMAND_CONTINUE };
    int is_virtual, is_local_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_delete_verify(unit, FLOW_APP_TYPE_INIT, flow_handle_info, NULL));

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));
    is_virtual = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, dnx_flow_objects[flow_handle_info->flow_handle]->flow_table, &entry_handle_id));

    /** Get local Out-LIF or virtual gport info from GPort hw resources (and global Out-LIF) */
    lif_flags = (is_virtual) ? DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL :
        (DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    /** Retrieve the entry into the handler and free the allocation */
    if (!is_virtual)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources.outlif_dbal_result_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free
                        (unit, DNX_ALGO_LIF_EGRESS, gport_hw_resources.global_out_lif));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_virtual_entry_on_handle_get
                        (unit, flow_handle_info->flow_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_virtual_gport_free(unit, flow_handle_info->flow_id));
    }

    /*
     * Iterate on all common fields. If a delete callback exists - call it 
     */
    temp_common_bitmap = flow_app_info->valid_common_fields_bitmap;
    while (temp_common_bitmap != 0)
    {
        if ((temp_common_bitmap & 0x1) && (dnx_flow_init_common_fields[field]->field_cbs.field_delete_cb))
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_init_common_fields[field]->
                                      field_cbs.field_delete_cb(unit, entry_handle_id, FLOW_APP_TYPE_INIT,
                                                                flow_handle_info, &gport_hw_resources, flow_app_info,
                                                                &initiator_info), _SHR_E_NOT_FOUND);
        }
        temp_common_bitmap = (temp_common_bitmap >> 1);
        field++;
    }

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_destroy
                    (unit, entry_handle_id, flow_handle_info, flow_app_info, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_flow_init_dedicated_logic_qos_destroy
                    (unit, flow_handle_info, entry_handle_id, &gport_hw_resources));

    /*
     * If a specific application callback is defined - call it
     */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, &flow_cmd_control,
                         &gport_hw_resources, &initiator_info, NULL, NULL, NULL));
        if (flow_cmd_control.flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    /** Remove flow data from EEDB outlif table */
    if (is_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_info_clear(unit, &gport_hw_resources));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_virtual_entry_clear
                        (unit, flow_handle_info->flow_id, flow_app_info->flow_table));
    }

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
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_GET, FLOW_COMMAND_CONTINUE };

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_get_verify
                           (unit, FLOW_APP_TYPE_INIT, flow_handle_info, initiator_info, special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_initiator_info_get
                    (unit, flow_handle_info, &flow_cmd_control, initiator_info, special_fields));

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
    dnx_flow_traverse_operation_e traverse_operation = _SHR_IS_FLAG_SET(flow_handle_info->flags,
                                                                        BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL) ?
        FLOW_TRAVERSE_OPERATION_DELETE : FLOW_TRAVERSE_OPERATION_USE_CALLBACK;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_flow_traverse_verify(unit, flow_handle_info, (void *) &cb, user_data));

    if (traverse_operation == FLOW_TRAVERSE_OPERATION_DELETE)
    {
        /** removing  BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL flag since traverse will call entry destroy and this flag is
         *  illegal there */
        flow_handle_info->flags = 0;
    }
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
            if (FLOW_APP_IS_NOT_VALID(dnx_flow_objects[app_idx]))
            {
                continue;
            }
            if (dnx_flow_objects[app_idx]->flow_app_type == FLOW_APP_TYPE_INIT)
            {
                traverse_flow_handle_info.flow_handle = app_idx;
                SHR_IF_ERR_EXIT(dnx_flow_initiator_info_traverse
                                (unit, &traverse_flow_handle_info, cb, user_data, traverse_operation));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_initiator_info_traverse(unit, flow_handle_info, cb, user_data, traverse_operation));
    }

    if (traverse_operation == FLOW_TRAVERSE_OPERATION_DELETE)
    {
        /** returning the flag to original value */
        flow_handle_info->flags |= BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL;
    }

exit:
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
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_SET, FLOW_COMMAND_CONTINUE };
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_create_verify(unit, flow_handle_info, NULL, key_special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_match_info_add(unit, flow_handle_info, &flow_cmd_control, key_special_fields));

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
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_DELETE, FLOW_COMMAND_CONTINUE };
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_delete_verify(unit, dnx_flow_objects[flow_handle_info->flow_handle]->flow_app_type,
                                                  flow_handle_info, key_special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_match_info_delete(unit, flow_handle_info, &flow_cmd_control, key_special_fields));

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
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_GET, FLOW_COMMAND_CONTINUE };
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_get_verify(unit, dnx_flow_objects[flow_handle_info->flow_handle]->flow_app_type,
                                               flow_handle_info, NULL, key_special_fields));

    SHR_IF_ERR_EXIT(dnx_flow_match_info_get(unit, flow_handle_info, &flow_cmd_control, key_special_fields));

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
    dnx_flow_traverse_operation_e traverse_operation = FLOW_TRAVERSE_OPERATION_USE_CALLBACK;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_flow_traverse_verify(unit, flow_handle_info, (void *) &cb, user_data));

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_TRAVERSE_DELETE_ALL))
    {
        flow_handle_info->flags = 0;
        traverse_operation = FLOW_TRAVERSE_OPERATION_DELETE;
    }
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
            if (FLOW_APP_IS_NOT_VALID(dnx_flow_objects[app_idx]))
            {
                continue;
            }
            if (FLOW_APP_IS_MATCH(dnx_flow_objects[app_idx]->flow_app_type))
            {
                traverse_flow_handle_info.flow_handle = app_idx;
                SHR_IF_ERR_EXIT(dnx_flow_match_info_traverse
                                (unit, &traverse_flow_handle_info, cb, user_data, traverse_operation));
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_match_info_traverse(unit, flow_handle_info, cb, user_data, traverse_operation));
    }

exit:
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
        if (sal_strncmp(flow_special_fields_db[flow_app_info->special_fields[iter]].name, field_name, FLOW_STR_MAX)
            == 0)
        {
            *field_id = flow_app_info->special_fields[iter];
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "Field %s is not supported by the application\n", field_name);

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
    SHR_INVOKE_VERIFY_DNXC(dnx_flow_handle_control_verify(unit, flow_handle, type, value));

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
    SHR_INVOKE_VERIFY_DNXC(dnx_flow_handle_control_verify(unit, flow_handle, type, *value));

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
bcm_flow_special_fields_db_t_init(
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
