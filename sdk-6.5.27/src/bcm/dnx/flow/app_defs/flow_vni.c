#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/flow/flow.h>
#include "../flow_def.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>

/** Supported terminator common fields */
#define VALID_VNI_TERMINATOR_COMMON_FIELDS \
        BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID | \
        BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID

/** Supported terminator special fields */
#define VALID_VNI_TERMINATOR_SPECIAL_FIELDS {FLOW_S_F_VSI_ASSIGNMENT_MODE}

/** Supported match speciel fields */
#define VALID_VNI_MATCH_SPECIAL_FIELDS {FLOW_S_F_NEXT_LAYER_NETWORK_DOMAIN, \
                                        FLOW_S_F_VNI}

/** VNI Virtual Terminator indications */
#define VNI_VIRTUAL_TERMINATOR_INDICATIONS  SAL_BIT(FLOW_APP_TERM_IND_VIRTUAL_LIF_ONLY)

static int
flow_match_is_valid(
    int unit)
{
    return dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_vxlan_vni2lif_supported);
}

int
flow_vni_app_specific_operations_cb(
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
        dbal_entry_value_field32_set(unit, current_entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF);
    }

    SHR_FUNC_EXIT;
}

const dnx_flow_app_config_t dnx_flow_app_vni_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VNI_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_VNI2LIF),

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
    {FLOW_APP_NAME_VNI_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VNI_MATCH_SPECIAL_FIELDS,

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
    flow_match_is_valid,

    NULL
};

const dnx_flow_app_config_t dnx_flow_app_vni_terminator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VNI_TERMINATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_IN_LIF_FORMAT_VNI),

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
    VALID_VNI_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VNI_TERMINATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS | BCM_FLOW_HANDLE_INFO_VIRTUAL,

    /*
     * terminator indication
     */
    VNI_VIRTUAL_TERMINATOR_INDICATIONS,

    /*
     * verify cb 
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_vni_app_specific_operations_cb,

    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_match_is_valid
};
