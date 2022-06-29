#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm_int/dnx/switch/switch_svtag.h>
#include "../flow_def.h"

/** Supported LL Terminator common fields */
#define VALID_VLAN_PORT_LL_TERMINATOR_COMMON_FIELDS \
            BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FIELD_CLASS_ID_VALID

/** Supported AC NATIVE Terminator common fields */
#define VALID_VLAN_PORT_AC_NATIVE_TERMINATOR_COMMON_FIELDS \
            BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FIELD_CLASS_ID_VALID

/** Supported AC NATIVE Virtual Terminator common fields */
#define VALID_VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR_COMMON_FIELDS \
            BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID | \
            BCM_FLOW_TERMINATOR_ELEMENT_FIELD_CLASS_ID_VALID

/** Supported LL Terminator special fields */
#define VALID_VLAN_PORT_LL_TERMINATOR_SPECIAL_FIELDS { \
            FLOW_S_F_VLAN_EDIT_PROFILE, \
            FLOW_S_F_VLAN_EDIT_PCP_DEI_PROFILE, \
            FLOW_S_F_VLAN_EDIT_VID_1, \
            FLOW_S_F_VLAN_EDIT_VID_2, \
            FLOW_S_F_VSI_ASSIGNMENT_MODE, \
            FLOW_S_F_UNKNOWN_UNICAST_DEFAULT_DESTINATION, \
            FLOW_S_F_UNKNOWN_MULTICAST_DEFAULT_DESTINATION, \
            FLOW_S_F_BROADCAST_DEFAULT_DESTINATION}

/** Supported AC NATIVE Terminator special fields */
#define VALID_VLAN_PORT_AC_NATIVE_TERMINATOR_SPECIAL_FIELDS { \
            FLOW_S_F_VLAN_EDIT_PROFILE, \
            FLOW_S_F_VLAN_EDIT_PCP_DEI_PROFILE, \
            FLOW_S_F_VLAN_EDIT_VID_1, \
            FLOW_S_F_VLAN_EDIT_VID_2, \
            FLOW_S_F_VSI_ASSIGNMENT_MODE, \
            FLOW_S_F_NATIVE_INDEX_MODE}

/** Supported AC NATIVE Virtual Terminator special fields */
#define VALID_VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR_SPECIAL_FIELDS { \
            FLOW_S_F_VLAN_EDIT_PROFILE, \
            FLOW_S_F_VLAN_EDIT_PCP_DEI_PROFILE, \
            FLOW_S_F_VLAN_EDIT_VID_1, \
            FLOW_S_F_VLAN_EDIT_VID_2, \
            FLOW_S_F_VSI_ASSIGNMENT_MODE }

/** Supported LL Initiator common fields */
#define VALID_VLAN_PORT_LL_INITIATOR_COMMON_FIELDS \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_OUTLIF_PROFILE_GROUP_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_FIELD_CLASS_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_FIELD_CLASS_CS_ID_VALID | \
            BCM_FLOW_INITIATOR_ELEMENT_SVTAG_ENABLE_VALID

/** Supported LL Initiator special fields */
#define VALID_VLAN_PORT_LL_INITIATOR_SPECIAL_FIELDS { \
            FLOW_S_F_VLAN_EDIT_PROFILE, \
            FLOW_S_F_VLAN_EDIT_VID_1, \
            FLOW_S_F_VLAN_EDIT_VID_2, \
            FLOW_S_F_QOS_PRI, \
            FLOW_S_F_QOS_CFI, \
            FLOW_S_F_QOS_PCP, \
            FLOW_S_F_NEXT_OUTLIF_POINTER, \
            FLOW_S_F_VLAN_PORT_SVTAG_PORT}

/** Supported initiator indications */
#define VALID_VLAN_PORT_LL_INITIATOR_INDICATIONS (SAL_BIT(FLOW_APP_INIT_IND_L2_GLOBAL_LIF) | SAL_BIT(FLOW_APP_INIT_IND_VIRTUAL_LIF_SUPPORTED))

/** Supported LL Match special fields for all matches */
#define VALID_VLAN_PORT_LL_MATCH_IN_PP_PORT_SPECIAL_FIELDS {FLOW_S_F_PORT}

#define VALID_VLAN_PORT_LL_MATCH_IN_AC_S_VLAN_SPECIAL_FIELDS {FLOW_S_F_S_VID, FLOW_S_F_VLAN_DOMAIN}

#define VALID_VLAN_PORT_LL_MATCH_IN_AC_C_VLAN_SPECIAL_FIELDS {FLOW_S_F_C_VID, FLOW_S_F_VLAN_DOMAIN}

#define VALID_VLAN_PORT_LL_MATCH_IN_AC_S_C_VLAN_SPECIAL_FIELDS {FLOW_S_F_C_VID, FLOW_S_F_S_VID, FLOW_S_F_VLAN_DOMAIN}

#define VALID_VLAN_PORT_LL_MATCH_IN_AC_C_C_VLAN_SPECIAL_FIELDS {FLOW_S_F_C_VID_1, FLOW_S_F_C_VID_2, FLOW_S_F_VLAN_DOMAIN}

#define VALID_VLAN_PORT_LL_MATCH_IN_AC_S_S_VLAN_SPECIAL_FIELDS {FLOW_S_F_S_VID_1, FLOW_S_F_S_VID_2, FLOW_S_F_VLAN_DOMAIN}

#define VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_NW_SCOPED_0_VLAN_SPECIAL_FIELDS {FLOW_S_F_VLAN_DOMAIN}
#define VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_NW_SCOPED_1_VLAN_SPECIAL_FIELDS {FLOW_S_F_VID, FLOW_S_F_VLAN_DOMAIN}
#define VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_NW_SCOPED_2_VLAN_SPECIAL_FIELDS {FLOW_S_F_VID_OUTER_VLAN, FLOW_S_F_VID_INNER_VLAN, FLOW_S_F_VLAN_DOMAIN}
#define VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_LIF_SCOPED_0_VLAN_SPECIAL_FIELDS {FLOW_S_F_GLOB_IN_LIF}
#define VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_LIF_SCOPED_1_VLAN_SPECIAL_FIELDS {FLOW_S_F_VID, FLOW_S_F_GLOB_IN_LIF}
#define VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_LIF_SCOPED_2_VLAN_SPECIAL_FIELDS {FLOW_S_F_VID_OUTER_VLAN, FLOW_S_F_VID_INNER_VLAN, FLOW_S_F_GLOB_IN_LIF}

/*
* IN_AC_UNTAGGED_DB:
* keys: IN_PP_PORT + CORE_ID
*/
#define VALID_VLAN_PORT_LL_MATCH_IN_AC_UNTAGGED_DPC_SPECIAL_FIELDS  {FLOW_S_F_IN_PORT}

#define VALID_VLAN_PORT_LL_MATCH_IN_AC_TCAM_SPECIAL_FIELDS {FLOW_S_F_VLAN_DOMAIN, FLOW_S_F_VID_OUTER_VLAN, FLOW_S_F_VID_INNER_VLAN, FLOW_S_F_PCP_DEI_OUTER_VLAN, FLOW_S_F_LAYER_TYPE}

/** VLAN-Port LL Terminator indications */
#define VLAN_PORT_LL_TERMINATOR_INDICATIONS (SAL_BIT(FLOW_APP_TERM_IND_L2_LEARN_INFO_STANDARD_SET) | \
                                             SAL_BIT(FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE) | \
                                             SAL_BIT(FLOW_APP_TERM_IND_QOS_RM_DEFAULT_PIPE) | \
                                             SAL_BIT(FLOW_APP_TERM_IND_QOS_TTL_DEFAULT_PIPE) | \
                                             SAL_BIT(FLOW_APP_TERM_IND_L2_GLOBAL_LIF) | \
                                             SAL_BIT(FLOW_APP_TERM_IND_VIRTUAL_LIF_SUPPORTED))

/** VLAN-Port AC NATIVE Terminator indications */
#define VLAN_PORT_AC_NATIVE_TERMINATOR_INDICATIONS  (SAL_BIT(FLOW_APP_TERM_IND_L2_GLOBAL_LIF))

/** VLAN-Port AC NATIVE Virtual Terminator indications */
#define VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR_INDICATIONS  (SAL_BIT(FLOW_APP_TERM_IND_L2_GLOBAL_LIF) | \
                                                             SAL_BIT(FLOW_APP_TERM_IND_VIRTUAL_LIF_ONLY))

/** VLAN-Port Terminator Match indications */
#define VLAN_PORT_TERM_MATCH_INDICATIONS SAL_BIT(FLOW_APP_TERM_MATCH_IND_PER_CORE)

/* 
 * enums and functions for L2 Learn info calculation and result-type selection as consequence
 */
typedef enum
{
    LIF_LEARN_INFO_REQ_TYPE_INVALID = -1,
    LIF_LEARN_INFO_REQ_TYPE_FIRST,
    LIF_LEARN_INFO_REQ_TYPE_NO_REQUIREMENT = LIF_LEARN_INFO_REQ_TYPE_FIRST,
    LIF_LEARN_INFO_REQ_TYPE_EXTENDED_LIF,
    LIF_LEARN_INFO_REQ_TYPE_BASIC_LIF,
    LIF_LEARN_INFO_REQ_TYPE_OPTIMIZED_LIF,
    LIF_LEARN_INFO_REQ_TYPE_LAST = LIF_LEARN_INFO_REQ_TYPE_OPTIMIZED_LIF,
    LIF_LEARN_INFO_REQ_TYPE_COUNT
} lif_learn_info_type_e;

int
flow_vlan_port_flow_app_is_valid(
    int unit)
{
    return FALSE;
}


int
flow_vlan_port_initiator_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields,
    uint32 get_entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify that VLAN_PORT_SVTAG_PORT is not used in replace */
    if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
    {
        if ((special_fields != NULL) && _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
        {
            int field_idx;
            shr_error_e rv =
                dnx_flow_special_field_find(unit, special_fields, FLOW_S_F_VLAN_PORT_SVTAG_PORT, &field_idx);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Field VLAN_PORT_SVTAG_PORT is not replaceable\n");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
flow_vlan_port_initiator_app_specific_operations_cb(
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

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_port_traverse_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e dbal_field,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_special_field_t * special_field)
{
    uint32 pp_port;
    bcm_core_t core_id;
    bcm_port_t phy_port_gport;
    int valid;

    SHR_FUNC_INIT_VARS(unit);

    /** we don't use this cb for lif apps, and specific cbs will return actual values */
    if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
    {

    /** Retrieve the PP-Port and the Core-ID */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, dbal_field, &pp_port));

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_CORE_ID, (uint32 *) &core_id));

    /** Convert to a logical Port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, core_id, pp_port, &valid));

        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_gport_get(unit, core_id, pp_port, &phy_port_gport));
            special_field->shr_var_uint32 = phy_port_gport;
        }
        else
        {
            special_field->shr_var_uint32 = DNX_ALGO_PORT_INVALID;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
flow_special_field_in_mapped_port_traverse_get(
    int unit,
    uint32 entry_handle_id,
    const dnx_flow_app_config_t * flow_app_info,
    bcm_flow_special_field_t * special_field)
{
    uint32 mapped_pp_port;
    uint32 pp_port;
    bcm_core_t core_id;
    bcm_port_t phy_port_gport;
    int valid;
    uint32 pp_port_mask;

    SHR_FUNC_INIT_VARS(unit);

    /** we don't use this cb for lif apps, and specific cbs will return actual values */
    if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
    {

    /** Retrieve the PP-Port and the Core-ID from MAPPED_PP_PORT */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, &mapped_pp_port));

        pp_port_mask = (1 << dnx_data_port.general.pp_port_bits_size_get(unit)) - 1;
        core_id = mapped_pp_port >> dnx_data_port.general.pp_port_bits_size_get(unit);
        pp_port = mapped_pp_port & pp_port_mask;

    /** Convert to a logical Port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, core_id, pp_port, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_gport_get(unit, core_id, pp_port, &phy_port_gport));
            special_field->shr_var_uint32 = phy_port_gport;
        }
        else
        {
            special_field->shr_var_uint32 = DNX_ALGO_PORT_INVALID;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** A utility function to determine whether a p2p result-type was requested */
static shr_error_e
dnx_flow_vlan_port_ll_is_p2p(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int *is_p2p)
{
    uint8 is_replace = _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE);

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID))
    {
        /** if service_type is not part of the inlif or service_type is p2p */
        *is_p2p = (terminator_info->l2_ingress_info.service_type == bcmFlowServiceTypeCrossConnect) ? TRUE : FALSE;
        SHR_EXIT();
    }

    /** in case of replace, when nor clearing the field, need to check the previous value */
    if (is_replace)
    {
        if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID))
        {
            if ((gport_hw_resources->inlif_dbal_result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P) ||
                (gport_hw_resources->inlif_dbal_result_type == DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE) ||
                (gport_hw_resources->inlif_dbal_result_type ==
                 DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE_GENERIC_DATA))
            {
                *is_p2p = TRUE;
                SHR_EXIT();
            }
        }
    }

    *is_p2p = FALSE;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flow_vlan_port_ll_terminator_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields,
    uint32 get_entry_handle)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) flow_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    int is_p2p, is_replace;
    int special_field_idx;
    uint32 generic_data = 0, fodo_assignment_mode = 0;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Result types are P2P if they include either a service-type field, which is part of common field l2_ingress_info,
     * or a destination field.
     * If p2p, we don't support learning configuration
     */
    if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
    {
        is_replace = _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE);
        if (is_replace)
        {
            /** Get Local In-LIF ID */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                            (unit, flow_handle_info->flow_id,
                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
        }
        else
        {
            if ((_SHR_IS_FLAG_SET
                 (terminator_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID))
                &&
                ((!_SHR_IS_FLAG_SET
                  (terminator_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID))
                 || (terminator_info->l2_ingress_info.service_type == bcmFlowServiceTypeMultiPoint)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Flag BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID can only be used when BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID is set to a P2P value");
            }
        }

        /** Validate that learn information isn't supplied for a p2p entry */
        SHR_IF_ERR_EXIT(dnx_flow_vlan_port_ll_is_p2p
                        (unit, flow_handle_info, terminator_info, &gport_hw_resources, &is_p2p));

        /** validate that ADDITIONAL DATA is not set if the FODO MODE (which exist if not p2p)
         *  is LOOKUP (which means it is used for RECYCLE flow)
         */
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE) && !is_p2p)
        {
            rv = dbal_entry_handle_value_field32_get
                (unit, get_entry_handle, DBAL_FIELD_LIF_GENERIC_DATA_0, INST_SINGLE, &generic_data);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT(rv);
            }
            if (generic_data == 0)
            {
                rv = dbal_entry_handle_value_field32_get
                    (unit, get_entry_handle, DBAL_FIELD_LIF_GENERIC_DATA_1, INST_SINGLE, &generic_data);
                if (rv != _SHR_E_NOT_FOUND)
                {
                    SHR_IF_ERR_EXIT(rv);
                }
            }
            /** if from some reason, the field does not exist, skip the check */
            rv = dbal_entry_handle_value_field32_get
                (unit, get_entry_handle, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE, &fodo_assignment_mode);
            if (rv != _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        if (_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID))
        {
            generic_data = (COMPILER_64_IS_ZERO(terminator_info->additional_data) == FALSE);
        }
        if (special_fields)
        {
            for (special_field_idx = 0; special_field_idx < special_fields->actual_nof_special_fields;
                 special_field_idx++)
            {
                if ((special_fields->special_fields[special_field_idx].field_id == FLOW_S_F_VSI_ASSIGNMENT_MODE) &&
                    (special_fields->special_fields[special_field_idx].is_clear == 0))
                {
                    fodo_assignment_mode = (uint32) special_fields->special_fields[special_field_idx].symbol;
                }
            }
        }
        if ((fodo_assignment_mode == DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP) &&
            (generic_data != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "ADDITIONAL (WIDE) DATA is not allowed for VLAN_PORT_RECYCLE (VSI_ASSIGNMENT_MODE==FROM_LOOKUP)");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* 
 * Perform LL VLAN-Port Terminator result type selection: 
 *  - A P2P result type when service-type or destination fields are supplied.
 *  - A MP result type is later set by flow_vlan_port_ll_terminator_app_specific_operations_cb
 */
int
flow_vlan_port_ll_terminator_result_type_select_cb(
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    uint32 *selectable_result_types)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) app_entry_data;
    int is_p2p;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_vlan_port_ll_is_p2p(unit, flow_handle_info, terminator_info, gport_hw_resources, &is_p2p));

    /*
     * In case of P2P, select all the P2P supported result types                                      .
     */
    if (is_p2p)
    {
        /** AC2EEI is for interop with JR1*/
        if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
            DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC2EEI);
            SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC2EEI_LARGE_GENERIC_DATA);
            SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC2EEI_WITH_PROTECTION);
        }
        SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P);
        SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE);
        SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE_GENERIC_DATA);
    }
    else
    {
        SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP);
        SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE);
        SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_LARGE_GENERIC_DATA);
        SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD);
        SHR_BITSET(selectable_result_types, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_RCH_CONTROL_LIF);
    }

exit:
    SHR_FUNC_EXIT;
}

/* 
 * Perform VLAN-Port Native AC Terminator specific operations: 
 *  - Reset the value of GLOB_IN_LIF for non-symmetric (ingress only) case
 */
int
flow_vlan_port_ac_native_terminator_app_specific_operations_cb(
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
    uint8 lif_id_is_symmetric;

    SHR_FUNC_INIT_VARS(unit);

    if (!(flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_lif_id_is_symmetric
                    (unit, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id), &lif_id_is_symmetric));

    if (lif_id_is_symmetric == 0)
    {
        dbal_entry_value_field32_set(unit, current_entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, 0);
    }

exit:
    SHR_FUNC_EXIT;
}

/** VLAN-Port Link-Layer applications */
const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_terminator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_IN_AC_INFO_DB),

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
    VALID_VLAN_PORT_LL_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_TERMINATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS | BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC | BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM |
        BCM_FLOW_HANDLE_INFO_VIRTUAL,

    /*
     * Indications
     */
    VLAN_PORT_LL_TERMINATOR_INDICATIONS,

    /*
     * verify cb 
     */
    flow_vlan_port_ll_terminator_verify_cb,

    /*
     * app_specific_operations_cb
     */
    NULL,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    flow_vlan_port_ll_terminator_result_type_select_cb,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

/** VLAN-Port Native applications */
const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_terminator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_TERMINATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION),

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
    VALID_VLAN_PORT_AC_NATIVE_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_AC_NATIVE_TERMINATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS | BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC | BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM |
        BCM_FLOW_HANDLE_NATIVE,

    /*
     * Indications
     */
    VLAN_PORT_AC_NATIVE_TERMINATOR_INDICATIONS,

    /*
     * verify cb 
     */
    NULL,

    /*
     * app_specific_operations_cb
     */
    flow_vlan_port_ac_native_terminator_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

/** VLAN-Port Native virtual applications */
const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_virtual_terminator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT),

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
    VALID_VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS | BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM | BCM_FLOW_HANDLE_INFO_VIRTUAL | BCM_FLOW_HANDLE_NATIVE,

    /*
     * Indications
     */
    VLAN_PORT_AC_NATIVE_VIRTUAL_TERMINATOR_INDICATIONS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_initiator = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_INITIATOR,

    /** related DBAL table */
    FLOW_LIF_DBAL_TABLE_SET(DBAL_TABLE_EEDB_OUT_AC),

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
    VALID_VLAN_PORT_LL_INITIATOR_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_INITIATOR_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS | BCM_FLOW_HANDLE_INFO_SYMMETRIC_ALLOC | BCM_FLOW_HANDLE_NATIVE | BCM_FLOW_HANDLE_INFO_VIRTUAL,

    /*
     * Indications
     */
    VALID_VLAN_PORT_LL_INITIATOR_INDICATIONS,

    /*
     * verify cb 
     */
    flow_vlan_port_initiator_verify_cb,

    /*
     * app_specific_operations_cb
     */
    flow_vlan_port_initiator_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

shr_error_e
flow_vlan_port_ll_pp_port_term_match_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields,
    uint32 get_entry_handle)
{
    bcm_flow_special_field_t special_field_data_dummy = { 0 };
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dnx_flow_special_field_data_get(unit, special_fields, FLOW_S_F_PORT, &special_field_data_dummy);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "For LL VLAN-Port Port match, The Port field must be provided");
    }

exit:
    SHR_FUNC_EXIT;
}

/* 
 * Perform LL VLAN-Port Match PP-Port specific operations: 
 *  - Convert the local port to a list of PP-Port + Core-ID.
 *    For a set/delete cb, perform DBAL commit for each of the PP-Ports.
 *    For a get cb, perform DBAL get.
 */
int
flow_vlan_port_ll_pp_port_term_match_app_specific_operations_cb(
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
    uint32 idx;
    uint32 default_in_lif;
    const dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    flow_cmd_control->flow_command = FLOW_COMMAND_SKIP_TO_END;
    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

    for (idx = 0; idx < special_fields->actual_nof_special_fields; idx++)
    {
        if (special_fields->special_fields[idx].field_id == FLOW_S_F_PORT)
        {
            dnx_algo_gpm_gport_phy_info_t gport_info;
            uint32 local_port, pp_port_index;

            /*
             * For traverse, need to take keys value and convert to back to physical port value 
             */
            if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_TRAVERSE)
            {
                SHR_IF_ERR_EXIT(flow_special_field_port_traverse_get(unit,
                                                                     current_entry_handle_id,
                                                                     DBAL_FIELD_PP_PORT,
                                                                     flow_app_info, special_fields->special_fields));
            }

            /*
             * Convert the local port to a list of PP-Port + Core-ID
             */
            local_port = special_fields->special_fields[idx].shr_var_uint32;
            if (flow_cmd_control->flow_cb_type != FLOW_CB_TYPE_TRAVERSE)
            {
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                                (unit, local_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
            }
            else
            {
                /*
                 * In Traverse, skip entries with a PP-Port key that wasn't mapped to a logical port
                 */
                if (local_port == DNX_ALGO_PORT_INVALID)
                {
                    flow_cmd_control->flow_command = FLOW_COMMAND_SKIP_TO_END;
                    SHR_EXIT();
                }
            }

            /*
             * For a set/delete cb, for each PP-Port, use the PP-Port and the Core-ID,
             * perform DBAL commit and skip the rest of the FLOW sequence.
             */
            if ((flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
                || (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_DELETE))
            {
                /*
                 * Determine the default_in_lif (local in-lif) to set for a set/delete cb 
                 * In case of a delete cb, set to the initial default In-LIF 
                 */
                if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_DELETE)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_default_lif_get(unit, (int *) &default_in_lif));
                }
                else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
                {
                    default_in_lif = gport_hw_resources->local_in_lif;
                }

                /*
                 * Set the default In-LIF to each of the PP-Ports
                 */
                for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
                {
                    dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_PP_PORT,
                                               gport_info.internal_port_pp_info.pp_port[pp_port_index]);
                    dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_CORE_ID,
                                               gport_info.internal_port_pp_info.core_id[pp_port_index]);
                    dbal_entry_value_field32_set(unit, current_entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE,
                                                 default_in_lif);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, current_entry_handle_id, DBAL_COMMIT));
                }
            }
            /*
             * For a get cb, use the first PP-Port as all PP-Ports are expected the same content. 
             * perform DBAL get and skip the rest of the FLOW sequence, as the DBAL field name differs 
             * from the standard naming. 
             */
            else if ((flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_GET)
                     || (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_TRAVERSE))
            {
                uint32 initial_bridge_in_lif, initial_drop_in_lif;
                int core_id;

                if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_GET)
                {
                    /*
                     * Retrieve the first PP-Port's default In-LIF
                     */
                    dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_PP_PORT,
                                               gport_info.internal_port_pp_info.pp_port[0]);
                    dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_CORE_ID,
                                               gport_info.internal_port_pp_info.core_id[0]);
                    dbal_value_field_arr32_request(unit, current_entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE,
                                                   &default_in_lif);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, current_entry_handle_id, DBAL_COMMIT));

                }
                else    /* flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_TRAVERSE */
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, current_entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE,
                                     &default_in_lif));
                }

                /*
                 * In case the default_lif is one of the initial SDK default LIFs (drop-lif or simple-bridge-lif) - return a not-found error
                 */
                initial_drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);
                SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_get(unit, (int *) &initial_bridge_in_lif));
                if ((default_in_lif == initial_drop_in_lif) || (default_in_lif == initial_bridge_in_lif))
                {
                    if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_GET)
                    {
                        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error! Port = %d, No In-LIF assigned\n", local_port);
                    }
                    else        /* Traverse - Skip ports that point to a default In-LIF */
                    {
                        flow_cmd_control->flow_command = FLOW_COMMAND_SKIP_TO_END;
                        SHR_EXIT();
                    }
                }

                /*
                 * Convert the local In-LIF to a FLOW gport
                 */
                core_id =
                    (dnx_data_lif.
                     in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)) ? DBAL_CORE_DEFAULT : _SHR_CORE_ALL;
                SHR_IF_ERR_EXIT(dnx_flow_local_lif_to_flow_id
                                (unit, flow_app_info, flow_handle_info, default_in_lif, core_id));
            }
            /*
             * The entry was mostly handled by this cb, but the entry is valid and may require finalization
             * In traverse - Call the user cb.
             */
            flow_cmd_control->flow_command = FLOW_COMMAND_FINALIZE;
            SHR_EXIT();
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_pp_port_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_PP_PORT_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_INGRESS_PP_PORT),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_MATCH_IN_PP_PORT_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * Indications
     */
    VLAN_PORT_TERM_MATCH_INDICATIONS,

    /*
     * verify cb
     */
    flow_vlan_port_ll_pp_port_term_match_verify_cb,

    /*
     * app_specific_operations_cb
     */
    flow_vlan_port_ll_pp_port_term_match_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_s_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_AC_S_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IN_AC_S_VLAN_DB),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_MATCH_IN_AC_S_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_c_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_AC_C_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IN_AC_C_VLAN_DB),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_MATCH_IN_AC_C_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_s_c_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_AC_S_C_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IN_AC_S_C_VLAN_DB),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_MATCH_IN_AC_S_C_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_c_c_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_AC_C_C_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IN_AC_C_C_VLAN_DB),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_MATCH_IN_AC_C_C_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_s_s_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_AC_S_S_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IN_AC_S_S_VLAN_DB),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_MATCH_IN_AC_S_S_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

shr_error_e
flow_vlan_port_ll_ac_untagged_dpc_term_match_verify_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields,
    uint32 get_entry_handle)
{
    int idx;

    SHR_FUNC_INIT_VARS(unit);

    for (idx = 0; idx < special_fields->actual_nof_special_fields; idx++)
    {
        if (special_fields->special_fields[idx].field_id == FLOW_S_F_IN_PORT)
        {
            break;
        }
    }

    if (idx >= special_fields->actual_nof_special_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "For LL VLAN-Port Untagged match, The Port field must be provided");
    }

exit:
    SHR_FUNC_EXIT;
}

/* 
 * Perform LL VLAN-Port Match Untagged specific operations: 
 *  - Convert the local port to a list of PP-Port and Core-ID.
 *    For a set/delete cb, perform DBAL commit for each of the PP-Ports.
 */
int
flow_vlan_port_ll_ac_untagged_dpc_term_match_app_specific_operations_cb(
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
    uint32 idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (idx = 0; idx < special_fields->actual_nof_special_fields; idx++)
    {
        if (special_fields->special_fields[idx].field_id == FLOW_S_F_IN_PORT)
        {
            /*
             * For traverse, need to take keys value and convert to back to physical port value
             */
            if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_TRAVERSE)
            {
                const dnx_flow_app_config_t *flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

                SHR_IF_ERR_EXIT(flow_special_field_port_traverse_get(unit,
                                                                     current_entry_handle_id,
                                                                     DBAL_FIELD_IN_PP_PORT,
                                                                     flow_app_info, special_fields->special_fields));
            }
            else
            {
                dnx_algo_gpm_gport_phy_info_t gport_info;
                uint32 pp_port_index;
                /*
                 * Convert the local port to a list of PP-Port + Core-ID
                 */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                                (unit, special_fields->special_fields[idx].shr_var_uint32,
                                 DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

                /*
                 * For a set/delete cb, for each PP-Port, encode the PP-Port and the Core-ID,
                 * perform DBAL commit/clear and skip the rest of the FLOW sequence.
                 */
                if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_SET)
                {
                    dbal_entry_action_flags_e dbal_flags =
                        (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE)) ? DBAL_COMMIT_UPDATE :
                        DBAL_COMMIT;

                    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports;
                         pp_port_index++)
                    {
                        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
                        uint32 core_id = gport_info.internal_port_pp_info.core_id[pp_port_index];

                        dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_IN_PP_PORT, pp_port);
                        dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
                        dbal_entry_value_field32_set(unit, current_entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                                                     gport_hw_resources->local_in_lif);
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, current_entry_handle_id, dbal_flags));
                    }
                    flow_cmd_control->flow_command = FLOW_COMMAND_SKIP_TO_END;
                }
                else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_DELETE)
                {
                    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports;
                         pp_port_index++)
                    {
                        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
                        uint32 core_id = gport_info.internal_port_pp_info.core_id[pp_port_index];

                        dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_IN_PP_PORT, pp_port);
                        dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
                        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, current_entry_handle_id, DBAL_COMMIT));
                    }
                    flow_cmd_control->flow_command = FLOW_COMMAND_SKIP_TO_END;
                }

                /*
                 * For a get cb, use the first PP-Port as all PP-Ports are expected the same content. 
                 * Encode the PP-Port and the Core-ID, befoe continuing with the FLOW sequence.
                 */
                else if (flow_cmd_control->flow_cb_type == FLOW_CB_TYPE_GET)
                {
                    uint32 pp_port = gport_info.internal_port_pp_info.pp_port[0];
                    uint32 core_id = gport_info.internal_port_pp_info.core_id[0];

                    dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_IN_PP_PORT, pp_port);
                    dbal_entry_key_field32_set(unit, current_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
flow_vlan_port_untagged_dpc_term_match_is_valid(
    int unit)
{
    return FALSE;
}

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_untagged_dpc_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_AC_UNTAGGED_DPC_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IN_AC_UNTAGGED_DB),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_MATCH_IN_AC_UNTAGGED_DPC_SPECIAL_FIELDS,

    /*
     * Bitmap for supported flow handler flags
     */
    FLOW_APP_BASE_FLAGS,

    /*
     * Indications
     */
    VLAN_PORT_TERM_MATCH_INDICATIONS,

    /*
     * verify cb
     */
    flow_vlan_port_ll_ac_untagged_dpc_term_match_verify_cb,

    /*
     * app_specific_operations_cb
     */
    flow_vlan_port_ll_ac_untagged_dpc_term_match_app_specific_operations_cb,

    /*
     * Generic callback for marking selectable result types - result_type_select_cb
     */
    NULL,

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_untagged_dpc_term_match_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_tcam_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_LL_AC_TCAM_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_IN_AC_TCAM_DB),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_LL_MATCH_IN_AC_TCAM_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_nw_scoped_0_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_NW_SCOPED_0_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_NW_SCOPED_0_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_nw_scoped_1_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_NW_SCOPED_1_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_NW_SCOPED_1_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_nw_scoped_2_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_NW_SCOPED_2_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_NW_SCOPED_2_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_lif_scoped_0_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_LIF_SCOPED_0_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_0_VLANS),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_LIF_SCOPED_0_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_lif_scoped_1_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_LIF_SCOPED_1_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_1_VLANS),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_LIF_SCOPED_1_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};

const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_lif_scoped_2_vlan_term_match = {
    /*
     * Application name
     */
    FLOW_APP_NAME_VLAN_PORT_AC_NATIVE_LIF_SCOPED_2_VLAN_TERM_MATCH,

    /** related DBAL table */
    FLOW_MATCH_DBAL_TABLE_SET(DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_2_VLANS),

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
    {FLOW_APP_NAME_VLAN_PORT_LL_TERMINATOR},

    /*
     * Bitwise for supported common fields
     */
    FLOW_NO_COMMON_FIELDS,

    /*
     * specific table special fields
     */
    VALID_VLAN_PORT_MATCH_IN_AC_NATIVE_LIF_SCOPED_2_VLAN_SPECIAL_FIELDS,

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
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    flow_vlan_port_flow_app_is_valid
};
