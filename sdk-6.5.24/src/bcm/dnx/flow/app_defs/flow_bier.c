#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/flow/flow.h>
#include "../flow_def.h"

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
                                           BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID

#define VALID_BIER_INITIATOR_SPECIAL_FIELDS {FLOW_S_F_BIER_BSL, FLOW_S_F_BIER_BFIR_ID, FLOW_S_F_BIER_RSV, FLOW_S_F_BIER_OAM, FLOW_S_F_BFR_BIT_STR, FLOW_S_F_QOS_DSCP}

int
flow_bier_is_valid(
    int unit)
{
    return TRUE;
}

const dnx_flow_app_config_t dnx_flow_app_bier_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_BIER_TERM_MATCH,

    /** related DBAL table */
    DBAL_TABLE_BFER_2ND_PASS_TERMINATION_WO_BFIR,

    /** Second Pass related DBAL table */
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
    DBAL_TABLE_IN_LIF_FORMAT_BIER,

    /** Second Pass related DBAL table */
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

const dnx_flow_app_config_t dnx_flow_app_bier_base_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_BIER_BASE_INITIATOR,

    /** related DBAL table */
    DBAL_TABLE_EEDB_BIER,

    /** Second Pass related DBAL table */
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
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields)
{
    bcm_flow_initiator_info_t *init_info;
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t outlif_info;
    int local_outlif = 0;
    bcm_gport_t next_pointer = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int special_f_idx = 0;
    bcm_encap_access_t entry_encap_access;
    uint8 is_bitstr_entry = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    init_info = (bcm_flow_initiator_info_t *) app_entry_data;

    /** check if it's bitstr entry */
    for (special_f_idx = 0; special_f_idx < special_fields->actual_nof_special_fields; special_f_idx++)
    {
        if (special_fields->special_fields[special_f_idx].field_id == FLOW_S_F_BFR_BIT_STR)
        {
            is_bitstr_entry = 1;
            break;
        }
    }

    if (!is_bitstr_entry)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_BIER_BFR_BITSTR, &entry_handle_id));
    /** Fill superset handle with all the required fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_BFR_BIT_STR, INST_SINGLE,
                                     &special_fields->special_fields[special_f_idx].shr_var_uint32_arr[4]);

    if (init_info->l3_intf_id != 0)
    {
        /** update next pointer of data entry with original l3_intf_id */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_pointer, init_info->l3_intf_id);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, next_pointer, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));
        if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                         gport_hw_resources.local_out_lif);
        }
    }

    entry_encap_access = init_info->encap_access + 1;
    SHR_IF_ERR_EXIT(dnx_lif_lib_encap_access_to_logical_phase_convert
                    (unit, entry_encap_access, &outlif_info.logical_phase));
    outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, &local_outlif, &outlif_info));

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
    bcm_flow_handle_info_t * flow_handle_info,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields)
{
    lif_table_mngr_outlif_info_t lif_info;
    uint32 entry_handle_id;
    uint8 is_field_valid;
    uint32 next_pointer = 0;
    bcm_flow_initiator_info_t *init_info;
    int special_f_idx = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    init_info = (bcm_flow_initiator_info_t *) app_entry_data;

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, current_entry_handle_id, DBAL_FIELD_BFR_BIT_STR, &is_field_valid));
    if (is_field_valid == FALSE)
    {
        SHR_EXIT();
    }

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

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, current_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &is_field_valid, &next_pointer));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_BIER_BFR_BITSTR, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, next_pointer, entry_handle_id, &lif_info));

    /** FLOW_S_F_BFR_BIT_STR */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_BFR_BIT_STR, INST_SINGLE,
                     &special_fields->special_fields[special_f_idx].shr_var_uint32_arr[4]));

    /** Update the next pointer of the returned structure to be the one from the 2nd BIER BITSTR entry */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &is_field_valid, &next_pointer));
    if (next_pointer != 0)
    {
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(init_info->l3_intf_id, next_pointer);
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
    uint32 entry_handle_id;
    uint8 is_field_valid;
    uint32 next_pointer = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, current_entry_handle_id, DBAL_FIELD_BFR_BIT_STR, &is_field_valid));
    if (is_field_valid == FALSE)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, current_entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &is_field_valid, &next_pointer));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_BIER_BFR_BITSTR, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear
                    (unit, next_pointer, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING));
exit:
    DBAL_FUNC_FREE_VARS;
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
    void *lif_info,
    uint32 *selectable_result_types)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
    {
        SHR_IF_ERR_EXIT(dnx_flow_bier_bitstr_entry_set
                        (unit, current_entry_handle_id, flow_handle_info, app_entry_data, special_fields));
    }
    else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_GET)
    {
        SHR_IF_ERR_EXIT(dnx_flow_bier_bitstr_entry_get
                        (unit, current_entry_handle_id, flow_handle_info, app_entry_data, special_fields));
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
    DBAL_TABLE_EEDB_BIER_BFR_BITSTR,

    /** Second Pass related DBAL table */
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
    flow_bier_initiator_app_specific_operations_cb,

    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_bier_is_valid
};
