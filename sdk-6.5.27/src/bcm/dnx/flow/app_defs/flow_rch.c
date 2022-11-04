#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/flow/flow.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <bcm_int/dnx/switch/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>

/** Supported initiator common fields */
#define VALID_RCH_INITIATOR_COMMON_FIELDS BCM_FLOW_INITIATOR_ELEMENT_OUTLIF_PROFILE_GROUP_VALID | \
                                           BCM_FLOW_INITIATOR_ELEMENT_FIELD_CLASS_CS_ID_VALID

/** Supported initiator special fields */
#define VALID_RCH_INITIATOR_SPECIAL_FIELDS {FLOW_S_F_RCH_VRF, \
                                            FLOW_S_F_RCH_OUTLIF_0_PUSHED_TO_STACK, \
                                            FLOW_S_F_RCH_P2P, \
                                            FLOW_S_F_RCH_DESTINATION, \
                                            FLOW_S_F_RCH_CONTROL_VLAN_PORT}

int
flow_rch_result_type_set(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    uint32 *selectable_result_types)
{
    int special_field_idx;
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, get_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, selectable_result_types));
    }

    if (special_fields != NULL)
    {
        for (special_field_idx = 0; special_field_idx < special_fields->actual_nof_special_fields; special_field_idx++)
        {
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
flow_rch_app_is_valid_cb(
    int unit)
{
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
    {
        return FALSE;
    }

    return TRUE;
}

const dnx_flow_app_config_t dnx_flow_app_rch_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_RCH_INITIATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_EEDB_RCH),

    /** Second Pass related Param (dbal table / dbal field) */
    DBAL_TABLE_EMPTY,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_INIT,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessAc,

    /*
     * Match payload valid applications
     */
    FLOW_NO_MATCH_PAYLOAD_APPS,

    /*
     * Bitwise for supported common fields
     */
    VALID_RCH_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_RCH_INITIATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * Indications
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

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    flow_rch_result_type_set,
    /*
     *  Generic callback, used to indicate if the app is valid. Usually the app should be
     *  valid according to dnx_data
     */
    flow_rch_app_is_valid_cb
};
