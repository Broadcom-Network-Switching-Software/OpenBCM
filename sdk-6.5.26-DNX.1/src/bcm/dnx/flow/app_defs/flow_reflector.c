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
#define VALID_REFLECTOR_INITIATOR_COMMON_FIELDS BCM_FLOW_INITIATOR_ELEMENT_OUTLIF_PROFILE_GROUP_VALID

/** Supported initiator special fields */
#define VALID_REFLECTOR_INITIATOR_SPECIAL_FIELDS {FLOW_S_F_REFLECTOR_PROCESS_TYPE, \
                                            FLOW_S_F_REFLECTOR_SRC_MAC, \
                                            FLOW_S_F_REFLECTOR_EGRESS_LAST_LAYER}

static int
flow_reflector_app_is_valid_cb(
    int unit)
{
    if (!dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
    {
        return FALSE;
    }

    return TRUE;
}

const dnx_flow_app_config_t dnx_flow_app_reflector_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_REFLECTOR_INITIATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_EEDB_REFLECTOR),

    /** Second Pass related Param (dbal table / dbal field) */
    DBAL_TABLE_EMPTY,

    /*
     * Flow app type
     */
    FLOW_APP_TYPE_INIT,

    /*
     * encap_access_default_mapping
     */
    bcmEncapAccessNativeArp,

    /*
     * Match payload valid applications
     */
    FLOW_NO_MATCH_PAYLOAD_APPS,

    /*
     * Bitwise for supported common fields
     */
    VALID_REFLECTOR_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_REFLECTOR_INITIATOR_SPECIAL_FIELDS,

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
    NULL,
    /*
     *  Generic callback, used to indicate if the app is valid. Usually the app should be
     *  valid according to dnx_data
     */
    flow_reflector_app_is_valid_cb
};
