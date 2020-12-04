/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/shrextend/shrextend_debug.h>
#include "flow_def.h"

/** Supported initiator common fields */
#define VALID_MPLS_TUNNEL_INITIATOR_COMMON_FIELDS \
        BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID | \
        BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID

/** Supported initiator speciel fields */
#define VALID_MPLS_TUNNEL_INITIATOR_SPECIAL_FIELDS {DBAL_FIELD_MPLS_LABEL, DBAL_FIELD_MPLS_LABEL_2}

int
flow_mpls_tunnel_app_specific_operations_cb(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    void *lif_info)
{
    int field, nof_labels = 0;
    lif_table_mngr_outlif_info_t *outlif_info = (lif_table_mngr_outlif_info_t *) lif_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check how many labels were set
     */
    if (special_fields)
    {
        for (field = 0; field < special_fields->actual_nof_special_fields; field++)
        {
            if ((special_fields->special_fields[field].field_id == DBAL_FIELD_MPLS_LABEL)
                || (special_fields->special_fields[field].field_id == DBAL_FIELD_MPLS_LABEL_2))
            {
                nof_labels++;
            }
        }
    }

    if (nof_labels == 2)
    {
        outlif_info->table_specific_flags |= DNX_ALGO_LIF_TABLE_MPLS_2_LABELS;
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_app_config_t dnx_flow_app_mpls_tunnel_initiator = {
    /*
     * Application name
     */
    "MPLS_TUNNEL_INITIATOR",
    /** related DBAL table */
    DBAL_TABLE_EEDB_MPLS_TUNNEL,

    /*
     * Flow app type 
     */
    FLOW_APP_TYPE_INIT,

    /*
     * Bitwise for supported common fields
     */
    VALID_MPLS_TUNNEL_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_MPLS_TUNNEL_INITIATOR_SPECIAL_FIELDS,

    /*
     * verify cb
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_mpls_tunnel_app_specific_operations_cb
        /*
         * TBD mechanism to pick the result type (override generic method)
         */
        /*
         * NULL
         */
};
