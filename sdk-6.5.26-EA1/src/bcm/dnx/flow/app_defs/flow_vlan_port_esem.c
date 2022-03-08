#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/flow/flow.h>
#include "../flow_def.h"

/** Supported ESEM Initiator common fields */

#define VALID_VLAN_PORT_ESEM_INITIATOR_COMMON_FIELDS \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID


/** Supported ESEM-Default Initiator special fields */
#define VALID_VLAN_PORT_ESEM_DEFAULT_INITIATOR_SPECIAL_FIELDS { \
            FLOW_S_F_VLAN_EDIT_PROFILE, \
            FLOW_S_F_VLAN_EDIT_VID_1, \
            FLOW_S_F_VLAN_EDIT_VID_2, \
            FLOW_S_F_QOS_PRI, \
            FLOW_S_F_QOS_CFI}

/** Supported ESEM initiator indications */
#define VALID_VLAN_PORT_ESEM_INITIATOR_INDICATIONS (SAL_BIT(FLOW_APP_INIT_IND_VIRTUAL_LIF_ONLY) | SAL_BIT(FLOW_APP_INIT_IND_NO_LOCAL_LIF))

/* 
 * Perform VLAN-Port ESEM Initiator specific operations: 
 *  - Set the Out-LIF phase
 *  - Mark the entry as a last layer
 */
int
flow_vlan_port_esem_initiator_app_specific_operations_cb(
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
    lif_table_mngr_outlif_info_t *lif_table_mngr_outlif_info = (lif_table_mngr_outlif_info_t *) lif_info;
    SHR_FUNC_INIT_VARS(unit);

    if (!(flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET))
    {
        SHR_EXIT();
    }

    /*
     * Set the Out-LIF phase to 
     */
    lif_table_mngr_outlif_info->logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_8;

    /*
     * Mark the entry as a last layer 
     */
    dbal_entry_value_field32_set(unit, current_entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, TRUE);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

