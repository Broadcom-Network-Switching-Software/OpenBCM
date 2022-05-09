#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/flow/flow.h>
#include "../flow_def.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>

/** Supported initiator common fields */

/** Supported match speciel fields */
#define VALID_BIER_2ND_PASS_MATCH_SPECIAL_FIELDS {FLOW_S_F_BIER_BIFT_ID, FLOW_S_F_NEXT_LAYER_NETWORK_DOMAIN}

/** Supported terminator common fields */
#define VALID_BIER_TERMINATOR_COMMON_FIELDS BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID | \
                                            BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
                                            BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
                                            BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
                                            BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID

#define VALID_BIER_INITIATOR_COMMON_FIELDS BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | \
                                           BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
                                           BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
                                           BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID | \
                                           BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID | \
                                           BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID | \
                                           BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID | \
                                           BCM_FLOW_INITIATOR_ELEMENT_TTL_VALID

#define VALID_BIER_BITSTR_INITIATOR_COMMON_FIELDS BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID

#define VALID_BIER_INITIATOR_SPECIAL_FIELDS {FLOW_S_F_BIER_BSL, \
                                             FLOW_S_F_BIER_BFIR_ID, \
                                             FLOW_S_F_QOS_DSCP, \
                                             FLOW_S_F_BIER_RSV, \
                                             FLOW_S_F_BIER_OAM, \
                                             FLOW_S_F_BFR_BIT_STR}

/** Supported initiator indications */
#define VALID_BIER_BITSTR_INITIATOR_INDICATIONS (SAL_BIT(FLOW_APP_INIT_IND_IGNORE_L3_INTF)  | \
                                                 SAL_BIT(FLOW_APP_INIT_IND_MANDATORY_L3_INTF))

int
flow_bier_is_valid(
    int unit)
{
    return TRUE;
}

int
flow_gbier_is_valid(
    int unit)
{
    return dnx_data_bier.params.feature_get(unit, dnx_data_bier_params_ingress_only);
}

/*
 * \brief
 * Verfication for BIER Terminator
 */
shr_error_e
flow_bier_termiator_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields,
    uint32 get_entry_handle)
{
    bcm_flow_terminator_info_t *termiator_info = (bcm_flow_terminator_info_t *) flow_info;

    SHR_FUNC_INIT_VARS(unit);

    /** relevant only for create and replace */
    if ((flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_GET) || (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_DELETE))
    {
        SHR_EXIT();
    }

    if (termiator_info->l3_ingress_info.service_type == bcmFlowServiceTypeCrossConnect)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BIER terminator doest not support P2P service");
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_app_config_t dnx_flow_app_bier_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_BIER_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_BFER_2ND_PASS_TERMINATION_WO_BFIR),

    /** Second Pass related Param (dbal table / dbal field) */
    DBAL_TABLE_EMPTY,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_TERM_MATCH,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessInvalid,

    /*
     * Match payload valid applications
     */
    {FLOW_APP_NAME_BIER_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_BIER_2ND_PASS_MATCH_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * initiator indication
     */
    FLOW_APP_INDICATIONS_NONE,

    /*
     * verify cb
     */
    NULL,
    /*
     * app_specific_operations_cb
     */
    NULL,

    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    NULL
};

const dnx_flow_app_config_t dnx_flow_app_bier_terminator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_BIER_TERMINATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_IN_LIF_FORMAT_BIER),

    /** Second Pass related Param (dbal table / dbal field) */
    DBAL_TABLE_EMPTY,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_TERM,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessInvalid,

    /*
     * Match payload valid applications
     */
    FLOW_NO_MATCH_PAYLOAD_APPS,

    /*
     * Bitwise for supported common fields
     */
    VALID_BIER_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    {0},

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * initiator indication
     */
    FLOW_APP_INDICATIONS_NONE,

    /*
     * verify cb 
     */
    flow_bier_termiator_verify_cb,

    /*
     * app_specific_operations_cb
     */
    NULL,

    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    NULL
};

const dnx_flow_app_config_t dnx_flow_app_bier_base_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_BIER_BASE_INITIATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_EEDB_BIER),

    /** Second Pass related Param (dbal table / dbal field) */
    DBAL_TABLE_EMPTY,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_INIT,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessInvalid,

    /*
     * Match payload valid applications
     */
    FLOW_NO_MATCH_PAYLOAD_APPS,

    /*
     * Bitwise for supported common fields
     */
    VALID_BIER_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_BIER_INITIATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * initiator indication
     */
    FLOW_APP_INDICATIONS_NONE,

    /*
     * verify cb
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    NULL,

    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    NULL
};

/*
 * \brief
 * fill EEDB_BIER_BFR_BITSTR is requested
 */
static shr_error_e
dnx_flow_bier_bitstr_entry_set(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * in_gport_hw_resources,
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_initiator_info_t *init_info = (bcm_flow_initiator_info_t *) app_entry_data;
    uint32 entry_handle_id;
    int local_outlif = 0;
    bcm_gport_t next_pointer = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int special_f_idx = 0;
    bcm_encap_access_t next_entry_encap_access[] = {
        bcmEncapAccessInvalid,
        bcmEncapAccessTunnel2,
        bcmEncapAccessTunnel3,
        bcmEncapAccessTunnel4,
        bcmEncapAccessArp,
        bcmEncapAccessTunnel1,
        bcmEncapAccessInvalid,
        bcmEncapAccessAc,
        bcmEncapAccessNativeArp,
        bcmEncapAccessInvalid
    };
    uint8 is_replace = _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE);
    bcm_encap_access_t curr_phase = init_info->encap_access;
    uint8 bitstr_field_exist = 0;
    dnx_algo_local_outlif_logical_phase_e logical_phase;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** check if bitstr field is needed to be set */
    for (special_f_idx = 0; special_f_idx < special_fields->actual_nof_special_fields; special_f_idx++)
    {
        if (special_fields->special_fields[special_f_idx].field_id == FLOW_S_F_BFR_BIT_STR)
        {
            bitstr_field_exist = 1;
            break;
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_BIER_DATA_BFR_BITSTR, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_BIER_DATA_BFR_BITSTR_ETPS_DATA_BFR_BIT_STR);

    if (bitstr_field_exist)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_BFR_BIT_STR, INST_SINGLE,
                                         &special_fields->special_fields[special_f_idx].shr_var_uint32_arr[4]);
    }
    else
    {
        if (is_replace)
        {
            int rv;
            uint32 existing_next_pointer;
            /** need to copy the content of the field to the entry handle   */
            rv = dbal_entry_handle_value_field32_get(unit, prev_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                                     INST_SINGLE, &existing_next_pointer);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

            if (rv == _SHR_E_NONE)
            {
                dbal_entry_value_field32_set(unit, current_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                             existing_next_pointer);
            }
        }
    }

    /*
     * Update next pointer of 2nd bitstr entry if needed
     */
    if (_SHR_IS_FLAG_SET(init_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID))
    {
        /** update next pointer of data entry with the l3_intf_id if exists */
        if (init_info->l3_intf_id != 0)
        {
            BCM_L3_ITF_LIF_TO_GPORT_FLOW_LIF(next_pointer, init_info->l3_intf_id);
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                            (unit, next_pointer, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                             &gport_hw_resources));
            if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                             gport_hw_resources.local_out_lif);
            }
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, 0);
        }
    }

    /*
     * Always update the next pointer of 1st bitstr entry
     */
    if (!is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert
                        (unit, next_entry_encap_access[curr_phase], &logical_phase));

        SHR_IF_ERR_EXIT(dnx_flow_egress_pointed_data_entry_lif_create
                        (unit, entry_handle_id, logical_phase, &local_outlif));

    }
    else
    {
        uint32 existing_next_pointer;
        int rv;
        rv = dbal_entry_handle_value_field32_get(unit, prev_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                                 INST_SINGLE, &existing_next_pointer);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

        if (rv == _SHR_E_NONE)
        {
            local_outlif = existing_next_pointer;
            SHR_IF_ERR_EXIT(dnx_flow_egress_pointed_data_entry_lif_update(unit, entry_handle_id, &local_outlif));
        }
        else
        {
            /** in replace logical_phase should be taken from hw resources, since it is not passed in the init_info */
            bcm_encap_access_t encap_access;
            SHR_IF_ERR_EXIT(dnx_lif_lib_logical_phase_to_encap_access_convert
                            (unit, in_gport_hw_resources->logical_phase, &encap_access));

            SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert
                            (unit, next_entry_encap_access[encap_access], &logical_phase));

            SHR_IF_ERR_EXIT(dnx_flow_egress_pointed_data_entry_lif_create
                            (unit, entry_handle_id, logical_phase, &local_outlif));
        }
    }

    /** update next_pointer of the first bitstr entry */
    dbal_entry_value_field32_set(unit, current_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                 local_outlif);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * specific bier initiator appl operation which set bitstr based on special fields inputs
 */
static shr_error_e
dnx_flow_bier_bitstr_entry_get(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 entry_handle_id;
    uint32 next_pointer = 0;
    bcm_flow_initiator_info_t *init_info;
    int special_f_idx = 0, rv;
    bcm_gport_t next_gport = BCM_GPORT_INVALID;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    init_info = (bcm_flow_initiator_info_t *) app_entry_data;

    /*
     * find the bit_str field
     */
    for (special_f_idx = 0; special_f_idx < special_fields->actual_nof_special_fields; special_f_idx++)
    {
        if (special_fields->special_fields[special_f_idx].field_id == FLOW_S_F_BFR_BIT_STR)
        {
            break;
        }
    }

    rv = dbal_entry_handle_value_field32_get(unit, current_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                             &next_pointer);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv != _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_BIER_DATA_BFR_BITSTR, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, next_pointer);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_BIER_DATA_BFR_BITSTR_ETPS_DATA_BFR_BIT_STR);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** FLOW_S_F_BFR_BIT_STR */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFR_BIT_STR, INST_SINGLE,
                     &special_fields->special_fields[special_f_idx].shr_var_uint32_arr[4]));

    /** Update the next pointer of the returned structure to be the one from the 2nd BIER BITSTR entry */
    init_info->l3_intf_id = 0;
    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                             &next_pointer);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NONE)
    {
        init_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID;
        if (next_pointer != 0)
        {
            /** In case local lif is not found, return next pointer 0 */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                        DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                        next_pointer, &next_gport));
            if (next_gport != BCM_GPORT_INVALID)
            {
                BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(init_info->l3_intf_id, next_gport);
                init_info->valid_elements_set |= BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID;
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * specific bier initiator appl operation which set bitstr based on special fields inputs
 */
static shr_error_e
dnx_flow_bier_bitstr_entry_delete(
    int unit,
    uint32 current_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields)
{
    uint32 next_pointer;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_entry_handle_value_field32_get(unit, current_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                             &next_pointer);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dnx_flow_egress_pointed_data_entry_lif_delete(unit, next_pointer));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
flow_bier_initiator_app_specific_operations_cb(
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    uint32 *selectable_result_types)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
    {
        SHR_IF_ERR_EXIT(dnx_flow_bier_bitstr_entry_set
                        (unit, current_entry_handle_id, prev_entry_handle_id, gport_hw_resources, flow_handle_info,
                         app_entry_data, special_fields));
    }
    else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_GET)
    {
        SHR_IF_ERR_EXIT(dnx_flow_bier_bitstr_entry_get
                        (unit, current_entry_handle_id, prev_entry_handle_id, flow_handle_info, app_entry_data,
                         special_fields));
    }
    else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_DELETE)
    {
        SHR_IF_ERR_EXIT(dnx_flow_bier_bitstr_entry_delete
                        (unit, current_entry_handle_id, flow_handle_info, app_entry_data, special_fields));
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_app_config_t dnx_flow_app_bier_bfr_bitstr_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_BIER_BITSTR_INITIATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_EEDB_BIER_DATA_BFR_BITSTR),

    /** Second Pass related Param (dbal table / dbal field) */
    DBAL_TABLE_EMPTY,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_INIT,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessInvalid,

    /*
     * Match payload valid applications
     */
    FLOW_NO_MATCH_PAYLOAD_APPS,

    /*
     * Bitwise for supported common fields
     */
    VALID_BIER_BITSTR_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_BIER_INITIATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * initiator indication
     */
    VALID_BIER_BITSTR_INITIATOR_INDICATIONS,

    /*
     * verify cb
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_bier_initiator_app_specific_operations_cb,

    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_bier_is_valid
};

