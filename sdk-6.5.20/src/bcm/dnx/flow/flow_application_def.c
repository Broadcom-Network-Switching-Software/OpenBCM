/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include "flow_def.h"



/******************************************************************************************************
 *                                              APPLICATIONS
 */
extern const dnx_flow_app_config_t dnx_flow_app_gtp_terminator;
extern const dnx_flow_app_config_t dnx_flow_app_gtp_initiator;
extern const dnx_flow_app_config_t dnx_flow_app_gtp_match;

const dnx_flow_app_config_t *dnx_flow_objects[] = {
    NULL,
    &dnx_flow_app_gtp_terminator,
    &dnx_flow_app_gtp_initiator,
    &dnx_flow_app_gtp_match
};

/******************************************************************************************************
 *                                              FIELDS
 */
/** all existing common fields should be extern to here in order touse the in the DB */
extern dnx_flow_common_field_desc_t
    dnx_flow_common_field_vrf,
    dnx_flow_common_field_stat_id,
    dnx_flow_common_field_l3_ingress_info,
    dnx_flow_common_field_stat_pp_profile,
    dnx_flow_common_field_qos_map_id,
    dnx_flow_common_field_qos_ingress_model_info,
    dnx_flow_common_field_flow_service_type_info,
    dnx_flow_common_field_dest_info,
    dnx_flow_common_field_additional_data,
    dnx_flow_common_field_action_gport,
    dnx_flow_common_field_l3_intf_id,
    dnx_flow_common_field_encap_access, dnx_flow_common_field_qos_egress_model, dnx_flow_common_field_mtu_profile;


/** Flow termination common field DB  */
dnx_flow_common_field_desc_t *dnx_flow_term_common_fields[] = {
    &dnx_flow_common_field_vrf,
    &dnx_flow_common_field_stat_id,
    &dnx_flow_common_field_l3_ingress_info,
    &dnx_flow_common_field_stat_pp_profile,
    &dnx_flow_common_field_qos_map_id,
    &dnx_flow_common_field_qos_ingress_model_info,
    &dnx_flow_common_field_flow_service_type_info,
    &dnx_flow_common_field_dest_info,
    &dnx_flow_common_field_additional_data,
    &dnx_flow_common_field_action_gport
};

/** Flow initiator common field DB  */
dnx_flow_common_field_desc_t *dnx_flow_init_common_fields[] = {
    &dnx_flow_common_field_stat_id,
    &dnx_flow_common_field_stat_pp_profile,
    &dnx_flow_common_field_qos_map_id,
    &dnx_flow_common_field_l3_intf_id,
    &dnx_flow_common_field_encap_access,
    &dnx_flow_common_field_qos_egress_model,
    &dnx_flow_common_field_action_gport,
    &dnx_flow_common_field_mtu_profile
};

/******************************************************************************************************
 *                                              UTILS
 */

shr_error_e
dnx_flow_number_of_apps(
    void)
{
    return sizeof(dnx_flow_objects) / sizeof(dnx_flow_app_config_t *);
}


shr_error_e
dnx_flow_number_of_terminator_common_fields(
    void)
{
    return sizeof(dnx_flow_term_common_fields) / sizeof(dnx_flow_common_field_desc_t *);
}


shr_error_e
dnx_flow_number_of_initiator_common_fields(
    void)
{
    return sizeof(dnx_flow_init_common_fields) / sizeof(dnx_flow_common_field_desc_t *);
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

        case FLOW_APP_TYPE_MATCH:
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
