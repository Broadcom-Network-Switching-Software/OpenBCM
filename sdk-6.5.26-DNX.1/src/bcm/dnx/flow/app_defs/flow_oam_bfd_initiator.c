
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include "../flow_def.h"
#include <bcm_int/dnx/flow/flow.h>

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

    /*
     * Bitwise for supported common fields
     */
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
    NULL,

    /*
     * app_specific_operations_cb
     */
    NULL,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL
};
