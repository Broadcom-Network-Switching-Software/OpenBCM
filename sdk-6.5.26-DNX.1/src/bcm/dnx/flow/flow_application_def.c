#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_flow.h>
#include "flow_def.h"

/*
 * Adding new applications
 * =======================
 * 1) Create a new file in the current (flow) directory - flow_xxx.c (where xxx stands for the application name). Add the new file to FILES.dnx
 * 2) Implement the necessary structure (dnx_flow_app_config_t) with the application's info, as defined in flow_def.h
 *    This structure will hold all of the relevant info for the application:
 *    - application name- must end with TERMINATOR or INITIATOR or MATCH
 *    - application related table
 *    - flow app type - termination/initiator/match
 *    - common application configuration - configuration which is common to some applications, such as verify enable, error_recovery enable
 *    - common fields bitmap - list of fields which are common to most applications, and have a static callbacks for set,get,delete,replace in flow_fields.c.
 *                             All of the common fields are related to a specific enabler, which is reflected in this bitmap
 *    - special fields - list of fields which are unique per application, and are being set/get directly from the flow table
 *    - verify cb - generic validations are done (and can be disabled per application) for all applications. In case unique validation is also required - define this cb
 *    - result type pick cb - result type is chosen by an internal algorithm which considers all of the user's set fields. In case another algorithm is required - define this cb
 * 3) Add the application to the list of applications in this file (below)
 * 4) In order to use the new defined application - call bcm_flow_handle_get() with the new application's name
 * 5) Examples can be found in flow.tcl, gtp tests and flow ctests
 * 6) Application name should be defined in flow.h to give easy documentation for all existing application names according to naming convention.
 * 7) Once the application added user should run the UT using ctest flow ut name=<app_name>
 * 8) In case Match application need to add related LIF applications to app definition
 * 9) In case of a new application (not ported) - add it to cleanup in dnx_flow_lib.tcl (in order for it to be cleared and iterated automatically when calling DnxFlowConfigReset etc
 *
*/

/******************************************************************************************************
 *                                              APPLICATIONS
 ******************************************************************************************************/
extern const dnx_flow_app_config_t dnx_flow_app_gtp_terminator;
extern const dnx_flow_app_config_t dnx_flow_app_gtp_initiator;
extern const dnx_flow_app_config_t dnx_flow_app_gtp_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_terminator;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_terminator;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_virtual_terminator;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_initiator;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_pp_port_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_s_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_c_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_s_c_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_c_c_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_s_s_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_untagged_dpc_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ll_ac_tcam_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_nw_scoped_0_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_nw_scoped_1_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_nw_scoped_2_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_lif_scoped_0_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_lif_scoped_1_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_vlan_port_ac_native_lif_scoped_2_vlan_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_bier_term_match;
extern const dnx_flow_app_config_t dnx_flow_app_bier_terminator;
extern const dnx_flow_app_config_t dnx_flow_app_bier_base_initiator;
extern const dnx_flow_app_config_t dnx_flow_app_bier_bfr_bitstr_initiator;
extern const dnx_flow_app_config_t dnx_flow_app_reflector_initiator;

extern const dnx_flow_app_config_t dnx_flow_app_oam_reflector_initiator;

const dnx_flow_app_config_t *dnx_flow_objects[] = {
    NULL,
    &dnx_flow_app_gtp_terminator,
    &dnx_flow_app_gtp_initiator,
    &dnx_flow_app_gtp_term_match,
    &dnx_flow_app_vlan_port_ll_terminator,
    &dnx_flow_app_vlan_port_ac_native_terminator,
    &dnx_flow_app_vlan_port_ac_native_virtual_terminator,
    &dnx_flow_app_vlan_port_ll_initiator,
    &dnx_flow_app_vlan_port_ll_pp_port_term_match,
    &dnx_flow_app_vlan_port_ll_ac_s_vlan_term_match,
    &dnx_flow_app_vlan_port_ll_ac_c_vlan_term_match,
    &dnx_flow_app_vlan_port_ll_ac_s_c_vlan_term_match,
    &dnx_flow_app_vlan_port_ll_ac_c_c_vlan_term_match,
    &dnx_flow_app_vlan_port_ll_ac_s_s_vlan_term_match,
    &dnx_flow_app_vlan_port_ll_ac_untagged_dpc_term_match,
    &dnx_flow_app_vlan_port_ll_ac_tcam_term_match,
    &dnx_flow_app_vlan_port_ac_native_nw_scoped_0_vlan_term_match,
    &dnx_flow_app_vlan_port_ac_native_nw_scoped_1_vlan_term_match,
    &dnx_flow_app_vlan_port_ac_native_nw_scoped_2_vlan_term_match,
    &dnx_flow_app_vlan_port_ac_native_lif_scoped_0_vlan_term_match,
    &dnx_flow_app_vlan_port_ac_native_lif_scoped_1_vlan_term_match,
    &dnx_flow_app_vlan_port_ac_native_lif_scoped_2_vlan_term_match,
    &dnx_flow_app_bier_term_match,
    &dnx_flow_app_bier_terminator,
    &dnx_flow_app_bier_base_initiator,
    &dnx_flow_app_bier_bfr_bitstr_initiator,
    &dnx_flow_app_reflector_initiator,
    &dnx_flow_app_oam_reflector_initiator,
};

/******************************************************************************************************
 *                                              FIELDS
 ******************************************************************************************************/
/** all existing common fields should be extern to here in order to use the in the DB 
    Note: Any new supported field should be added to the diagnostics.
    see: diag_flow_app_init_entry_print_cb */
/* *INDENT-OFF* */
extern dnx_flow_common_field_desc_t
    dnx_flow_common_field_vsi,
    dnx_flow_common_field_vrf,
    dnx_flow_common_field_stat_id,
    dnx_flow_common_field_l2_ingress_info,
    dnx_flow_common_field_l3_ingress_info,
    dnx_flow_common_field_stat_pp_profile,
    dnx_flow_common_field_qos_map_id,
    dnx_flow_common_field_qos_ingress_model_info,
    dnx_flow_common_field_dest_info,
    dnx_flow_common_field_additional_data,
    dnx_flow_common_field_term_action_gport,
    dnx_flow_common_field_init_action_gport,
    dnx_flow_common_field_l3_intf_id,
    dnx_flow_common_field_qos_egress_model,
    dnx_flow_common_field_mtu_profile,
    dnx_flow_common_field_failover_id,
    dnx_flow_common_field_failover_state,
    dnx_flow_common_field_term_class_id,
    dnx_flow_common_field_init_class_id,
    dnx_flow_common_field_term_field_class_id,
    dnx_flow_common_field_l2_learn_info,
    dnx_flow_common_field_learn_enable,
    dnx_flow_common_field_l2_egress_info,
    dnx_flow_common_field_init_tpid_class_id,
    dnx_flow_common_field_qos_ttl,
    dnx_flow_common_field_outlif_group,
    dnx_flow_common_field_init_field_class_id,
    dnx_flow_common_field_field_class_cs_id,
    dnx_flow_common_field_svtag_en,
dnx_flow_common_field_term_tpid_class_id;
/* *INDENT-ON* */

/** Flow termination common field DB  */
dnx_flow_common_field_desc_t *dnx_flow_term_common_fields[] = {
    &dnx_flow_common_field_vrf,
    &dnx_flow_common_field_stat_id,
    &dnx_flow_common_field_l3_ingress_info,
    &dnx_flow_common_field_stat_pp_profile,
    &dnx_flow_common_field_qos_map_id,
    &dnx_flow_common_field_qos_ingress_model_info,
    &dnx_flow_common_field_dest_info,
    &dnx_flow_common_field_additional_data,
    &dnx_flow_common_field_term_action_gport,
    &dnx_flow_common_field_vsi,
    &dnx_flow_common_field_l2_ingress_info,
    &dnx_flow_common_field_failover_id,
    &dnx_flow_common_field_failover_state,
    &dnx_flow_common_field_term_class_id,
    &dnx_flow_common_field_term_field_class_id,
    &dnx_flow_common_field_l2_learn_info,
    &dnx_flow_common_field_learn_enable,
    &dnx_flow_common_field_term_tpid_class_id,
    &dnx_flow_common_field_field_class_cs_id,
};

/** Flow initiator common field DB  */
dnx_flow_common_field_desc_t *dnx_flow_init_common_fields[] = {
    &dnx_flow_common_field_stat_id,
    &dnx_flow_common_field_stat_pp_profile,
    &dnx_flow_common_field_qos_map_id,
    &dnx_flow_common_field_l3_intf_id,
    &dnx_flow_common_field_init_class_id,
    &dnx_flow_common_field_qos_egress_model,
    &dnx_flow_common_field_init_action_gport,
    &dnx_flow_common_field_mtu_profile,
    &dnx_flow_common_field_l2_egress_info,
    &dnx_flow_common_field_failover_id,
    &dnx_flow_common_field_failover_state,
    &dnx_flow_common_field_init_tpid_class_id,
    &dnx_flow_common_field_qos_ttl,
    &dnx_flow_common_field_outlif_group,
    &dnx_flow_common_field_init_field_class_id,
    &dnx_flow_common_field_field_class_cs_id,
    &dnx_flow_common_field_svtag_en,
};

/******************************************************************************************************
 *                                              UTILS
 ******************************************************************************************************/

/**
 * \brief
 * Retrieve the flow application information for a given flow handle ID
 */
const dnx_flow_app_config_t *
dnx_flow_app_info_get(
    int unit,
    bcm_flow_handle_t flow_handle_id)
{

    if (flow_handle_id >= (sizeof(dnx_flow_objects) / sizeof(dnx_flow_app_config_t *)))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "No app for handle id %d \n"), flow_handle_id));
        return NULL;
    }
    return dnx_flow_objects[flow_handle_id];
}

int
dnx_flow_number_of_apps(
    void)
{
    return sizeof(dnx_flow_objects) / sizeof(dnx_flow_app_config_t *);
}

/**
 * \brief
 * Retrieve the flow handle ID of a given application name and 
 * an application type 
 */
bcm_flow_handle_t
dnx_flow_handle_by_app_name_get(
    int unit,
    const char *flow_app_name,
    dnx_flow_app_type_e flow_app_type)
{
    int app_idx, nof_apps = dnx_flow_number_of_apps();

    for (app_idx = FLOW_APP_FIRST_IDX; app_idx < nof_apps; app_idx++)
    {
        if (dnx_flow_objects[app_idx]->flow_app_type == flow_app_type)
        {
            if (!sal_strncasecmp(flow_app_name, dnx_flow_objects[app_idx]->app_name, FLOW_STR_MAX))
            {
                return app_idx;
            }
        }
    }

    return FLOW_APP_INVALID_IDX;
}

shr_error_e
dnx_flow_number_of_common_fields(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    int *nof_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (flow_app_type)
    {
        case FLOW_APP_TYPE_TERM:
            (*nof_fields) = sizeof(dnx_flow_term_common_fields) / sizeof(dnx_flow_common_field_desc_t *);
            break;

        case FLOW_APP_TYPE_INIT:
            (*nof_fields) = sizeof(dnx_flow_init_common_fields) / sizeof(dnx_flow_common_field_desc_t *);
            break;

        case FLOW_APP_TYPE_TERM_MATCH:
        case FLOW_APP_TYPE_INIT_MATCH:
            (*nof_fields) = 0;
            break;

        default:
            (*nof_fields) = 0;
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flow_app_type, %d", flow_app_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Retrieve a pointer to a common fields descriptor, according 
 * to a supplied application type and field index to a common 
 * fields descriptors array.
 */
shr_error_e
dnx_flow_common_fields_desc_by_app_type_get(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    int field_idx,
    const dnx_flow_common_field_desc_t ** common_field_desc)
{
    int nof_common_fields;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate the supplied application type and field index
     */
    if (flow_app_type >= FLOW_APP_TYPE_NOF_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, FLOW application type Out-Of-Range - %d", flow_app_type);
    }

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_type, &nof_common_fields));
    if (field_idx >= nof_common_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, Field index (%d) larger the common fields array size (%d)", field_idx,
                     nof_common_fields);
    }

    /*
     * Retrieve a pointer to the common field descriptor according to the supplied 
     * application type and field index to a common fields descriptors array.
     */
    switch (flow_app_type)
    {
        case FLOW_APP_TYPE_TERM:
            *common_field_desc = dnx_flow_term_common_fields[field_idx];
            break;
        case FLOW_APP_TYPE_INIT:
            *common_field_desc = dnx_flow_init_common_fields[field_idx];
            break;
        case FLOW_APP_TYPE_TERM_MATCH:
        case FLOW_APP_TYPE_INIT_MATCH:
        default:
            *common_field_desc = NULL;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get the common fields index for term, init according to enabler
 */
shr_error_e
dnx_flow_common_fields_index_by_enabler_get(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    int enabler,
    int *index)
{
    int nof_common_fields, idx, field_enabler;
    dnx_flow_common_field_desc_t **dnx_flow_common_fields;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate the supplied application type and field index
     */
    if (flow_app_type >= FLOW_APP_TYPE_NOF_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, FLOW application type Out-Of-Range - %d", flow_app_type);
    }

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_type, &nof_common_fields));

    /*
     * Retrieve a pointer to the common field descriptor according to the supplied
     * application type.
     */
    switch (flow_app_type)
    {
        case FLOW_APP_TYPE_TERM:
            dnx_flow_common_fields = dnx_flow_term_common_fields;
            break;
        case FLOW_APP_TYPE_INIT:
            dnx_flow_common_fields = dnx_flow_init_common_fields;
            break;
        case FLOW_APP_TYPE_TERM_MATCH:
        case FLOW_APP_TYPE_INIT_MATCH:
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Only term, init applications have common fields");
            break;
    }

    for (idx = 0; idx < nof_common_fields; idx++)
    {
        if (dnx_flow_common_fields[idx] == NULL)
        {
            continue;
        }
        field_enabler =
            (flow_app_type ==
             FLOW_APP_TYPE_TERM) ? dnx_flow_common_fields[idx]->
            term_field_enabler : dnx_flow_common_fields[idx]->init_field_enabler;
        if (field_enabler == enabler)
        {
            *index = idx;
            break;
        }
    }
    if (idx == nof_common_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Couldn't find matching field for enabler %d", enabler);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Returns true if the app indication is set. 
 */
int
dnx_flow_app_is_ind_set(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 ind)
{
    uint32 app_ind[2] = { 0 };

    app_ind[0] = flow_app_info->app_indications_bm;

    return SHR_IS_BITSET(app_ind, ind);
}
