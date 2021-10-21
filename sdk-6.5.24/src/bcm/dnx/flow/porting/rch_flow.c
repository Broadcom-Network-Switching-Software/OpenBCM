/** \file rch_flow.c
 *  * l2_egress APIs to flows APIs conversion.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm/flow.h>
#include <bcm/l2.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

/** see description in header file */
shr_error_e
dnx_rch_create_flow(
    int unit,
    bcm_l2_egress_t * egr)
{
    bcm_flow_initiator_info_t flow_init_info;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);
    bcm_flow_initiator_info_t_init(&flow_init_info);

    /** flow handle info */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_RCH_INITIATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;

    if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_REPLACE))
    {
        flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE | BCM_FLOW_HANDLE_INFO_WITH_ID;
    }
    /** WITH_ID flag is used */
    if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_WITH_ID))
    {
        flow_handle_info.flags |= BCM_FLOW_HANDLE_INFO_WITH_ID;
    }
    if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_WITH_ID | BCM_L2_EGRESS_REPLACE))
    {
        _SHR_GPORT_FLOW_LIF_ID_SET(flow_handle_info.flow_id, BCM_L3_ITF_VAL_GET(egr->encap_id));
    }

    /** handle special fields */
    special_fields.actual_nof_special_fields = 0;
    if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_EXTENDED_COPY_DEST_ENCAP))
    {
        SPECIAL_FIELD_ENABLER_ADD((&special_fields), FLOW_S_F_RCH_OUTLIF_0_PUSHED_TO_STACK);
    }
    else if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_REPLACE))
    {
        SPECIAL_FIELD_ENABLER_ADD((&special_fields), FLOW_S_F_RCH_OUTLIF_0_PUSHED_TO_STACK);
        special_fields.special_fields[special_fields.actual_nof_special_fields - 1].is_clear = TRUE;
    }
    if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_DEST_PORT))
    {
        SPECIAL_FIELD_ENABLER_ADD((&special_fields), FLOW_S_F_RCH_P2P);
        SPECIAL_FIELD_UINT32_DATA_ADD((&special_fields), FLOW_S_F_RCH_DESTINATION, egr->dest_port);
    }
    else if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_REPLACE))
    {
        SPECIAL_FIELD_ENABLER_ADD((&special_fields), FLOW_S_F_RCH_P2P);
        special_fields.special_fields[special_fields.actual_nof_special_fields - 1].is_clear = TRUE;
        special_fields.special_fields[special_fields.actual_nof_special_fields].field_id = FLOW_S_F_RCH_DESTINATION;
        special_fields.special_fields[special_fields.actual_nof_special_fields].is_clear = TRUE;
        special_fields.actual_nof_special_fields++;
    }
    if (egr->recycle_app == bcmL2EgressRecycleAppRedirectVrf)
    {
        SPECIAL_FIELD_UINT32_DATA_ADD((&special_fields), FLOW_S_F_RCH_VRF, egr->vrf);
    }
    else if (_SHR_IS_FLAG_SET(egr->flags, BCM_L2_EGRESS_REPLACE) &&
             (egr->recycle_app != bcmL2EgressRecycleAppRedirectVrf))
    {
        special_fields.special_fields[special_fields.actual_nof_special_fields].field_id = FLOW_S_F_RCH_VRF;
        special_fields.special_fields[special_fields.actual_nof_special_fields].is_clear = TRUE;
        special_fields.actual_nof_special_fields++;
    }
    SPECIAL_FIELD_UINT32_DATA_ADD((&special_fields), FLOW_S_F_RCH_CONTROL_VLAN_PORT, egr->vlan_port_id);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_create(unit, &flow_handle_info, &flow_init_info, &special_fields));

    /** set the gport on egr structure */
    BCM_L3_ITF_SET(egr->encap_id, BCM_L3_ITF_TYPE_LIF, _SHR_GPORT_FLOW_LIF_ID_GET(flow_handle_info.flow_id));

exit:
    SHR_FUNC_EXIT;
}

/** see description in header file */
shr_error_e
dnx_rch_get_flow(
    int unit,
    bcm_if_t encap_id,
    bcm_l2_egress_t * egr)
{
    bcm_flow_initiator_info_t flow_init_info;
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    bcm_flow_special_fields_t_init(&special_fields);
    bcm_flow_initiator_info_t_init(&flow_init_info);

    /** flow handle info */
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_RCH_INITIATOR, &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    _SHR_GPORT_FLOW_LIF_ID_SET(flow_handle_info.flow_id, BCM_L3_ITF_VAL_GET(encap_id));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_get(unit, &flow_handle_info, &flow_init_info, &special_fields));

    bcm_l2_egress_t_init(egr);
    egr->encap_id = encap_id;
    egr->flags |= BCM_L2_EGRESS_RECYCLE_HEADER;

    /** handle special fields */
    for (int i = 0; i < special_fields.actual_nof_special_fields; i++)
    {
        if (special_fields.special_fields[i].field_id == FLOW_S_F_RCH_VRF)
        {
            egr->vrf = special_fields.special_fields[i].shr_var_uint32;
        }
        if (special_fields.special_fields[i].field_id == FLOW_S_F_RCH_OUTLIF_0_PUSHED_TO_STACK)
        {
            egr->flags |= BCM_L2_EGRESS_EXTENDED_COPY_DEST_ENCAP;
        }
        if (special_fields.special_fields[i].field_id == FLOW_S_F_RCH_P2P)
        {
            egr->flags |= BCM_L2_EGRESS_DEST_PORT;
        }
        if (special_fields.special_fields[i].field_id == FLOW_S_F_RCH_DESTINATION)
        {
            egr->dest_port |= special_fields.special_fields[i].shr_var_uint32;
        }
        if (special_fields.special_fields[i].field_id == FLOW_S_F_RCH_CONTROL_VLAN_PORT)
        {
            if (BCM_GPORT_IS_SET(special_fields.special_fields[i].shr_var_uint32))
            {
                BCM_GPORT_SUB_TYPE_LIF_SET(egr->vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY,
                                           _SHR_GPORT_FLOW_LIF_ID_GET(special_fields.special_fields[i].shr_var_uint32));
                BCM_GPORT_VLAN_PORT_ID_SET(egr->vlan_port_id, egr->vlan_port_id);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see description in header file */
shr_error_e
dnx_rch_destroy_flow(
    int unit,
    bcm_if_t encap_id)
{
    bcm_flow_handle_info_t flow_handle_info;
    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);
    SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, FLOW_APP_NAME_RCH_INITIATOR, &flow_handle_info.flow_handle));
    _SHR_GPORT_FLOW_LIF_ID_SET(flow_handle_info.flow_id, BCM_L3_ITF_VAL_GET(encap_id));
    SHR_IF_ERR_EXIT(bcm_dnx_flow_initiator_info_destroy(unit, &flow_handle_info));

exit:
    SHR_FUNC_EXIT;
}
