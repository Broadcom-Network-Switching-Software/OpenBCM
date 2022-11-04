
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include "../flow_def.h"
#include <bcm_int/dnx/flow/flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>

shr_error_e
flow_oam_reflector_initiator_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields,
    uint32 get_entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
    {
        bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) flow_info;

        if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_SVTAG_ENABLE_VALID)
            && !dnx_data_switch.svtag.supported_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Enabling SVTAG is not supported on this device.\r\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_app_config_t dnx_flow_app_oam_reflector_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_OAM_REFLECTOR_INITIATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_EEDB_OAM_REFLECTOR),

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

    
    0,

    /*
     * specific table special fields
     */
    {FLOW_S_F_OAM_REFLECTOR_RAW_DATA, FLOW_S_F_REFLECTOR_EGRESS_LAST_LAYER},

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
    flow_oam_reflector_initiator_verify_cb,

    /*
     * app_specific_operations_cb
     */
    NULL,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL
};
