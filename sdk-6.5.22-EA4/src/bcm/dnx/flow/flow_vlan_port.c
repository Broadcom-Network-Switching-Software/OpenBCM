#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include "flow_def.h"

extern shr_error_e dnx_flow_field_l2_learn_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data);

/** Supported LL Terminator common fields */
#define VALID_VLAN_PORT_LL_TERMINATOR_COMMON_FIELDS \
            BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FLOW_SERVICE_TYPE_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID

/** Supported LL Terminator speciel fields */
#define VALID_VLAN_PORT_LL_TERMINATOR_SPECIAL_FIELDS { \
            DBAL_FIELD_VLAN_EDIT_PROFILE, \
            DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, \
            DBAL_FIELD_VLAN_EDIT_VID_1, \
            DBAL_FIELD_VLAN_EDIT_VID_2, \
            DBAL_FIELD_FODO_ASSIGNMENT_MODE }

/** Supported LL Initiator common fields */
#define VALID_VLAN_PORT_LL_INITIATOR_COMMON_FIELDS \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_PKT_PRI_CFI_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID

/** Supported LL Initiator speciel fields */
#define VALID_VLAN_PORT_LL_INITIATOR_SPECIAL_FIELDS { \
            DBAL_FIELD_VLAN_EDIT_PROFILE, \
            DBAL_FIELD_VLAN_EDIT_VID_1, \
            DBAL_FIELD_VLAN_EDIT_VID_2 }

/* 
 * Perform LL VLAN-Port Terminator specific operations: 
 *  - Reset the value of GLOB_IN_LIF for Non-Symmetric LIFs
 *  - Reconfigure the learn-info in case of Replace
 */
int
flow_vlan_port_ll_terminator_app_specific_operations_cb(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    void *lif_info)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) app_entry_data;
    uint8 is_exist;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Reset the value of GLOB_IN_LIF for Non-Symmetric LIFs
     */
    if (!(_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC)))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, 0);
    }

    /*
     * Reconfigure the Learn info in case of Replace as other fields may impact the learn context 
     * and the selected learn info structure. 
     * In case the learn was set during Replace, there's no need for an additional calculation.
     */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_lif_table_mngr_is_valid_field
                               (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif,
                                gport_hw_resources->inlif_dbal_table_id, TRUE, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT,
                                &is_exist));
        if (is_exist
            &&
            (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID))
            &&
            (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID)))
        {
            SHR_IF_ERR_EXIT(dnx_flow_field_l2_learn_info_set
                            (unit, entry_handle_id, FLOW_APP_TYPE_TERM, flow_handle_info, *gport_hw_resources,
                             app_entry_data));
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/** VLAN-Port Link-Layer applications */
const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_terminator = {
    /*
     * Application name
     */
    "VLAN_PORT_LL_TERMINATOR",
    /** related DBAL table */
    DBAL_TABLE_IN_AC_INFO_DB,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_TERM,

    /*
     * Bitwise for supported common fields
     */
    VALID_VLAN_PORT_LL_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_TERMINATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    BCM_FLOW_HANDLE_INFO_WITH_ID | BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC,

    /*
     * verify cb 
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_vlan_port_ll_terminator_app_specific_operations_cb
        /*
         * TBD mechanism to pick the result type (override generic method)
         */
        /*
         * NULL
         */
};

/* 
 * Perform LL VLAN-Port Initiator specific operations: 
 *  - Set the Out-LIF phase
 *  - Mark the entry as a last layer
 */
int
flow_vlan_port_ll_initiator_app_specific_operations_cb(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    void *lif_info)
{
    lif_table_mngr_outlif_info_t *lif_table_mngr_outlif_info = (lif_table_mngr_outlif_info_t *) lif_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set the Out-LIF phase to 
     */
    lif_table_mngr_outlif_info->logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_8;

    /*
     * Mark the entry as a last layer 
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, TRUE);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_initiator = {
    /*
     * Application name
     */
    "VLAN_PORT_LL_INITIATOR",
    /** related DBAL table */
    DBAL_TABLE_EEDB_OUT_AC,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_INIT,

    /*
     * Bitwise for supported common fields
     */
    VALID_VLAN_PORT_LL_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_INITIATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    BCM_FLOW_HANDLE_INFO_WITH_ID | BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC,

    /*
     * verify cb 
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_vlan_port_ll_initiator_app_specific_operations_cb
        /*
         * TBD mechanism to pick the result type (override generic method)
         */
        /*
         * NULL
         */
};
