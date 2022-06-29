
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/sand/shrextend/shrextend_debug.h>
#include "flow_def.h"
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_table_mngr.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <bcm_int/dnx/qos/qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <bcm_int/dnx/failover/failover.h>

#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/flow/flow.h>
#include <bcm/failover.h>
#include <bcm_int/dnx/algo/failover/algo_failover.h>
#include <bcm_int/dnx/switch/switch_svtag.h>

/**
 * \brief
 * Strings mappings for field enums
 */
static const char *flow_field_string_urpf_mode[] = {
    "Disable",  /* bcmL3IngressUrpfDisable */
    "Loose",    /* bcmL3IngressUrpfLoose */
    "Strict",   /* bcmL3IngressUrpfStrict */
    ""
};

static const char *flow_field_string_service_type[] = {
    "Multi-Point",      /* bcmFlowServiceTypeMultiPoint */
    "Cross-Connect",    /* bcmFlowServiceTypeCrossConnect */
    "Count",    /* bcmFlowServiceTypeCount */
    "",
};

static const char *flow_field_string_qos_ingress_model_type[] = {
    "Invalid",  /* bcmQosIngressModelInvalid */
    "Shortpipe",        /* bcmQosIngressModelShortpipe */
    "Pipe",     /* bcmQosIngressModelPipe */
    "Uniform",  /* bcmQosIngressModelUniform */
    "Stuck",    /* bcmQosIngressModelStuck */
    ""
};

static const char *flow_field_string_qos_egress_model_type[] = {
    "Uniform",  /* bcmQosEgressModelUniform */
    "PipeNextNameSpace",        /* bcmQosEgressModelPipeNextNameSpace */
    "PipeMyNameSpace",  /* bcmQosEgressModelPipeMyNameSpace */
    "Initial",  /* bcmQosEgressModelInitial */
    ""
};

static const char *flow_field_string_failover_state_types[] = {
    "Primary",  /* bcmbcmFlowFailoverStatePrimary */
    "Secondary",        /* bcmbcmFlowFailoverStateSecondary */
    "Count",    /* bcmbcmFlowFailoverStateCount */
    ""
};

extern shr_error_e dbal_tables_table_get(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t ** table);

/******************************************************************************************************
 *                                            COMMON - start
 ******************************************************************************************************/
/** the dnx_flow_common_field_empty_XXXXX functions are used for common fields which does not use the set and \ or get */
shr_error_e
dnx_flow_common_field_empty_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    return _SHR_E_NONE;
}

shr_error_e
dnx_flow_common_field_empty_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    return _SHR_E_NOT_FOUND;
}
/******************************************************************************************************
 *                                            COMMON - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              VSI - START
 ******************************************************************************************************/
int
flow_field_vsi_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;
    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", terminator_info->vsi);
}

const char *
field_desc_vsi_cb(
    int unit)
{
    return "Virtual switch instance";
}

shr_error_e
dnx_flow_field_vsi_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, terminator_info->vsi);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_vsi_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 vsi;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    terminator_info->vsi = 0;

    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, &vsi);

    if (rv == _SHR_E_NOT_FOUND || rv == _SHR_E_CONFIG)
    {
        /** in case error is _SHR_E_CONFIG it means that there is no match between with this specific child field so it means
         *  that this field is not found */
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    terminator_info->vsi = vsi;

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_vsi = {
    /*
     * field_name
     */
    "VSI",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_VSI,
    /*
     * field_desc
     */
    field_desc_vsi_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_vsi_set, dnx_flow_field_vsi_get, NULL},
    /*
     * field_print
     */
    flow_field_vsi_print_cb
};

/******************************************************************************************************
 *                                              VSI - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              VRF - START
 ******************************************************************************************************/
int
flow_field_vrf_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;
    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", terminator_info->vrf);
}

shr_error_e
dnx_flow_field_vrf_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, terminator_info->vrf);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_vrf_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 vrf;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    terminator_info->vrf = 0;

    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, &vrf);

    if (rv == _SHR_E_NOT_FOUND || rv == _SHR_E_CONFIG)
    {
        /** in case error is _SHR_E_CONFIG it means that there is no match between with this specific child field so it means
         *  that this field is not found */
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    terminator_info->vrf = vrf;

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_vrf = {
    /*
     * field_name
     */
    "VRF",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_VRF,
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_vrf_set, dnx_flow_field_vrf_get, NULL},
    /*
     * field_print
     */
    flow_field_vrf_print_cb
};

/******************************************************************************************************
 *                                              VRF - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              STAT_ID - START
 ******************************************************************************************************/
int
flow_field_stat_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    uint32 stat_id = (flow_app_type == FLOW_APP_TYPE_TERM) ?
        ((bcm_flow_terminator_info_t *) val)->stat_id : ((bcm_flow_initiator_info_t *) val)->stat_id;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", stat_id);
}

shr_error_e
dnx_flow_field_stat_id_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 stat_id;

    SHR_FUNC_INIT_VARS(unit);

    stat_id =
        (flow_app_type ==
         FLOW_APP_TYPE_TERM) ? ((bcm_flow_terminator_info_t *) (field_data))->stat_id : ((bcm_flow_initiator_info_t
                                                                                          *) (field_data))->stat_id;

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, stat_id);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_stat_id_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 stat_id;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, &stat_id));

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) field_data)->stat_id = stat_id;
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        ((bcm_flow_initiator_info_t *) field_data)->stat_id = stat_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Field description call back function
 */
const char *
field_desc_stat_id_cb(
    int unit)
{
    return "Statistics object identifier. Stat id is not validated to be in range related to the stat profile";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_stat_id = {
    /*
     * field_name
     */
    "STAT_ID",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_STAT_OBJECT_ID,
    /*
     * field_desc
     */
    field_desc_stat_id_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_stat_id_set, dnx_flow_field_stat_id_get, NULL},
    /*
     * field_print
     */
    flow_field_stat_id_print_cb
};
/******************************************************************************************************
 *                                              STAT_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              L2_INGRESS_INFO - START
 ******************************************************************************************************/
int
flow_field_l2_ingress_info_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "ingress_network_group_id = 0x%x, service_type = %s",
                        terminator_info->l2_ingress_info.ingress_network_group_id,
                        flow_field_string_service_type[terminator_info->l2_ingress_info.service_type]);
}

const char *
field_desc_l2_ingress_info_cb(
    int unit)
{
    return
        "L2 Ingress Info. Cannot be set with L3 Ingress Info. Also set the service_type with default value is multi-point.";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_l2_ingress_info = {
    /*
     * field_name
     */
    "L2_INGRESS_INFO",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_IN_LIF_PROFILE,
    /*
     * field_desc
     */
    field_desc_l2_ingress_info_cb,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_l2_ingress_info_print_cb
};

/******************************************************************************************************
 *                                              L2_INGRESS_INFO - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              L2_EGRESS_INFO - START
 ******************************************************************************************************/
int
flow_field_l2_egress_info_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;
    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", initiator_info->l2_egress_info.egress_network_group_id);
}

const char *
field_desc_l2_egress_info_cb(
    int unit)
{
    return "L2 Egress info: Orientation group etc";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_l2_egress_info = {
    /*
     * field_name
     */
    "L2_EGRESS_INFO",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_OUT_LIF_PROFILE,
    /*
     * field_desc
     */
    field_desc_l2_egress_info_cb,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_l2_egress_info_print_cb
};

/******************************************************************************************************
 *                                              L2_EGRESS_INFO - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              L3_INGRESS_INFO - START
 ******************************************************************************************************/
int
flow_field_l3_ingress_info_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "urpf_mode = %s, service_type = %s",
                        flow_field_string_urpf_mode[terminator_info->l3_ingress_info.urpf_mode],
                        flow_field_string_service_type[terminator_info->l3_ingress_info.service_type]);
}

/*
 * Desc cb
 */
const char *
flow_field_l3_ingress_info_desc(
    int unit)
{
    return
        "L3 ingress info. Profile existence is not validated. Cannot be set with L2 Ingress Info. Also set the service_type with default value is multi-point.";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_l3_ingress_info = {
    /*
     * field_name
     */
    "L3_INGRESS_INFO",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_IN_LIF_PROFILE,
    /*
     * field_desc
     */
    flow_field_l3_ingress_info_desc,
    /*
     * field_cb
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_l3_ingress_info_print_cb
};

/******************************************************************************************************
 *                                              L3_INGRESS_INFO - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              STAT_PP_PROFILE - START
 ******************************************************************************************************/
int
flow_field_stat_pp_profile_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    uint32 stat_pp_profile = (flow_app_type == FLOW_APP_TYPE_TERM) ?
        ((bcm_flow_terminator_info_t *) val)->stat_pp_profile : ((bcm_flow_initiator_info_t *) val)->stat_pp_profile;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", stat_pp_profile);
}

/*
 * Desc cb, stat_pp_profile limitations
 */
const char *
dnx_flow_field_stat_pp_profile_desc(
    int unit)
{
    return "Statistic pp profile. Should not be encoded, profile create before adding is not mandatory";
}

shr_error_e
dnx_flow_field_stat_pp_profile_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 stat_pp_profile;

    SHR_FUNC_INIT_VARS(unit);

    stat_pp_profile =
        (flow_app_type ==
         FLOW_APP_TYPE_TERM) ? ((bcm_flow_terminator_info_t *) (field_data))->stat_pp_profile
        : ((bcm_flow_initiator_info_t *) (field_data))->stat_pp_profile;

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, stat_pp_profile);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_stat_pp_profile_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 stat_pp_profile;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));

    /*
     * If profile was previously set - get it 
     */
    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) field_data)->stat_pp_profile = stat_pp_profile;
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        ((bcm_flow_initiator_info_t *) field_data)->stat_pp_profile = stat_pp_profile;
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_stat_pp_profile = {
    /*
     * field_name
     */
    "STAT_PP_PROFILE",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_STAT_OBJECT_CMD,
    /*
     * field_desc
     */
    dnx_flow_field_stat_pp_profile_desc,
    /*
     * field_cb
     */
    {dnx_flow_field_stat_pp_profile_set, dnx_flow_field_stat_pp_profile_get, NULL},
    /*
     * field_print
     */
    flow_field_stat_pp_profile_print_cb
};

/******************************************************************************************************
 *                                              STAT_PP_PROFILE - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              QOS_INGRESS_MODEL - START
 ******************************************************************************************************/
int
flow_field_qos_ingress_model_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "PHB: %s, Remark: %s, TTL: %s",
                        flow_field_string_qos_ingress_model_type[terminator_info->ingress_qos_model.ingress_phb],
                        flow_field_string_qos_ingress_model_type[terminator_info->ingress_qos_model.ingress_remark],
                        flow_field_string_qos_ingress_model_type[terminator_info->ingress_qos_model.ingress_ttl]);
}

/*
 * Desc cb, qos ingress model limitations
 */
const char *
dnx_flow_field_qos_ingress_model_desc(
    int unit)
{
    return "Profile of the qos model, bcmQosIngressModelStuck is illegal for this field";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_qos_ingress_model_info = {
    /*
     * field_name
     */
    "QOS_INGRESS_MODEL",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_PROPAGATION_PROFILE,
    /*
     * field_desc
     */
    dnx_flow_field_qos_ingress_model_desc,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_qos_ingress_model_print_cb
};
/******************************************************************************************************
 *                                              QOS_INGRESS_MODEL - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              DEST_INFO - START
 ******************************************************************************************************/
int
flow_field_dest_info_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "Port: 0x%x, Encap: 0x%x",
                        terminator_info->dest_info.dest_port, terminator_info->dest_info.dest_encap);
}

shr_error_e
dnx_flow_field_dest_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dnx_algo_gpm_forward_info_t forward_info;
    bcm_gport_t gport = terminator_info->dest_info.dest_port;
    uint8 is_valid_dest;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert from dest gport and encap-id to a forward info that consists of
     * an encoded destination.
     */
    SHR_IF_ERR_EXIT(algo_gpm_is_gport_valid_destination(unit, terminator_info->dest_info.dest_port, &is_valid_dest));
    /** convert to flow_id if it is not valid_dest, which means it is probably LIF */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications)
        && (is_valid_dest == FALSE))
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                        (unit, terminator_info->dest_info.dest_port, BCM_GPORT_TYPE_FLOW_LIF,
                         DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &gport));
    }
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, gport,
                     terminator_info->dest_info.dest_encap, &forward_info));

    /*
     * Set the encoded destination to DBAL
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, forward_info.destination);

    /*
     * If the conversion yielded an Out-LIF, set it to DBAL
     */
    if (forward_info.outlif)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, forward_info.outlif);
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_dest_info_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dnx_algo_gpm_forward_info_t forward_info = { 0 };
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve an encoded destination from DBAL
     */
    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &forward_info.destination));

    /*
     * Retrieve an Out-LIF from DBAL, if exists
     */
    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                             &forward_info.outlif);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    forward_info.fwd_info_result_type =
        (forward_info.outlif) ? DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF :
        DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;

    /*
     * Convert from an encoded destination and Out-LIF to a dest gport
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_from_forward_information
                    (unit, &(terminator_info->dest_info.dest_port), (uint32 *) &(terminator_info->dest_info.dest_encap),
                     &forward_info, 0));

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_dest_info = {
    /*
     * field_name
     */
    "DEST_INFO",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_EMPTY, /** can be DBAL_FIELD_DESTINATION or DBAL_FIELD_GLOB_OUT_LIF*/
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_dest_info_set, dnx_flow_field_dest_info_get, NULL},
    /*
     * field_print
     */
    flow_field_dest_info_print_cb
};

/******************************************************************************************************
 *                                              DEST_INFO - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              L3_INTF_ID - START
 ******************************************************************************************************/
int
flow_field_l3_intf_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", initiator_info->l3_intf_id);
}

shr_error_e
dnx_flow_field_l3_intf_id_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    bcm_gport_t gport_tunnel;
    dnx_algo_gpm_gport_hw_resources_t tunnel_gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    /** if indication to ignore is set - skip */
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_IGNORE_L3_INTF))
    {
        SHR_EXIT();
    }

    /** Get next outlif if exist */
    if (initiator_info->l3_intf_id != BCM_IF_INVALID)
    {
        if (initiator_info->l3_intf_id != 0)
        {
            BCM_L3_ITF_LIF_TO_GPORT_FLOW_LIF(gport_tunnel, initiator_info->l3_intf_id);
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                            (unit, gport_tunnel, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                             &tunnel_gport_hw_resources));
            /** Next outlif */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                         tunnel_gport_hw_resources.local_out_lif);
        }
        else
        {
            /** Next outlif */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, 0);
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l3_intf_id_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    int gport = 0, next_outlif = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** if indication to ignore is set - skip */
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_IGNORE_L3_INTF))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    /** Get next outLIF pointer */
    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                             (uint32 *) &next_outlif));

    if (next_outlif != 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL,
                                                    next_outlif, &gport));
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, gport, BCM_GPORT_TYPE_FLOW_LIF,
                                                    DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_INITIATOR, &gport));
        BCM_GPORT_FLOW_LIF_TO_L3_ITF_LIF(initiator_info->l3_intf_id, gport);
    }
    else
    {
        initiator_info->l3_intf_id = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_l3_intf_id = {
    /*
     * field_name
     */
    "L3_INTF_ID",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_NEXT_OUTLIF_POINTER,
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_l3_intf_id_set, dnx_flow_field_l3_intf_id_get, NULL},
    /*
     * field_print
     */
    flow_field_l3_intf_id_print_cb
};

/******************************************************************************************************
 *                                              L3_INTF_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                             TERMINATOR ACTION_GPORT - START
 ******************************************************************************************************/
int
flow_field_action_gport_terminator_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", terminator_info->action_gport);

}

const dnx_flow_common_field_desc_t dnx_flow_common_field_term_action_gport = {
    /*
     * field_name
     */
    "ACTION_GPORT",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_ACTION_PROFILE_IDX,
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_action_gport_terminator_print_cb
};

/******************************************************************************************************
 *                                          TERMINATOR_ACTION_GPORT - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                          INITIATOR_ACTION_GPORT - START
 ******************************************************************************************************/
int
flow_field_action_gport_initiator_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", initiator_info->action_gport);
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_init_action_gport = {
    /*
     * field_name
     */
    "ACTION_GPORT",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_ACTION_PROFILE,
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_action_gport_initiator_print_cb
};

/******************************************************************************************************
 *                                      INITIATOR_ACTION_GPORT - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              MTU_PROFILE - START
 ******************************************************************************************************/
int
flow_field_mtu_profile_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", initiator_info->mtu_profile);
}

shr_error_e
dnx_flow_field_mtu_profile_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, INST_SINGLE,
                                initiator_info->mtu_profile);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_mtu_profile_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, INST_SINGLE, &initiator_info->mtu_profile));

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_mtu_profile = {
    /*
     * field_name
     */
    "MTU_PROFILE",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_MTU_PROFILE,
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_mtu_profile_set, dnx_flow_field_mtu_profile_get, NULL},
    /*
     * field_print
     */
    flow_field_mtu_profile_print_cb
};

/******************************************************************************************************
 *                                              MTU_PROFILE - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              QOS_MAP_ID - START
 ******************************************************************************************************/
int
flow_field_remark_profile_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    int qos_map_id = (flow_app_type == FLOW_APP_TYPE_TERM) ?
        ((bcm_flow_terminator_info_t *) val)->qos_map_id : ((bcm_flow_initiator_info_t *) val)->qos_map_id;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", qos_map_id);
}

shr_error_e
dnx_flow_field_remark_profile_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 remark_profile;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        bcm_flow_terminator_info_t *termination_info = (bcm_flow_terminator_info_t *) field_data;
        remark_profile = DNX_QOS_MAP_PROFILE_GET(termination_info->qos_map_id);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, remark_profile);
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

        remark_profile = DNX_QOS_MAP_PROFILE_GET(initiator_info->qos_map_id);
        if (DNX_QOS_MAP_IS_MPLS_PHP(initiator_info->qos_map_id))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_REMARK_PROFILE, INST_SINGLE,
                                         remark_profile);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, remark_profile);
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_remark_profile_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 qos_map_id;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        bcm_flow_terminator_info_t *termination_info = (bcm_flow_terminator_info_t *) field_data;
        qos_map_id = DNX_QOS_INITIAL_MAP_ID;

        FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, &qos_map_id));
        if (qos_map_id != DNX_QOS_INITIAL_MAP_ID)
        {
            DNX_QOS_REMARK_MAP_SET(qos_map_id);
            DNX_QOS_PHB_MAP_SET(qos_map_id);
            DNX_QOS_INGRESS_MAP_SET(qos_map_id);
        }

        termination_info->qos_map_id = qos_map_id;
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
        uint8 is_field_valid;
        qos_map_id = DNX_QOS_INITIAL_MAP_ID;

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_TERMINATION_REMARK_PROFILE, &is_field_valid, &qos_map_id));
        if ((is_field_valid == TRUE) && (qos_map_id != DNX_QOS_INITIAL_MAP_ID))
        {
            DNX_QOS_MPLS_PHP_MAP_SET(qos_map_id);
            DNX_QOS_EGRESS_MAP_SET(qos_map_id);
        }
        else
        {
            FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, &qos_map_id));

            if (qos_map_id != DNX_QOS_INITIAL_MAP_ID)
            {
                DNX_QOS_REMARK_MAP_SET(qos_map_id);
                DNX_QOS_EGRESS_MAP_SET(qos_map_id);
            }
        }
        initiator_info->qos_map_id = qos_map_id;
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_qos_map_id = {
    /*
     * field_name
     */
    "QOS_MAP_ID",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_EMPTY, /** in term can be DBAL_FIELD_QOS_PROFILE in init can be DBAL_FIELD_TERMINATION_REMARK_PROFILE or DBAL_FIELD_REMARK_PROFILE */
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_remark_profile_set, dnx_flow_field_remark_profile_get, NULL},
    /*
     * field_print
     */
    flow_field_remark_profile_print_cb
};

/******************************************************************************************************
 *                                              QOS_MAP_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              QOS_EGRESS_MODEL - START
 ******************************************************************************************************/
/*
 * Desc cb, qos egress model limitations
 */
const char *
dnx_flow_field_qos_egress_model_desc(
    int unit)
{
    return
        "Profile of the qos model.\n\nFor ttl profile - also set:\nFLOW_S_F_TTL.\n\nFor network profile -\nneed to set one of:\nFLOW_S_F_DSCP\nFLOW_S_F_EXP\nFLOW_S_F_PRI and FLOW_S_F_CFI\naccording to the specific application.";
}

int
flow_field_qos_egress_model_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "QOS: %s, TTL: %s, ECN: %s",
                        flow_field_string_qos_egress_model_type[initiator_info->egress_qos_model.egress_qos],
                        flow_field_string_qos_egress_model_type[initiator_info->egress_qos_model.egress_ttl],
                        flow_field_string_qos_egress_model_type[initiator_info->egress_qos_model.egress_ecn]);

}

/** common field is handled by dnx_flow_init_dedicated_logic_qos_XXXX (where XXXX is create \ get \ replace \ destroy) */
const dnx_flow_common_field_desc_t dnx_flow_common_field_qos_egress_model = {
    /*
     * field_name
     */
    "QOS_EGRESS_MODEL",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_ENCAP_QOS_MODEL,
    /*
     * field_desc
     */
    dnx_flow_field_qos_egress_model_desc,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_qos_egress_model_print_cb
};
/******************************************************************************************************
 *                                              QOS_EGRESS_MODEL - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              ADDITIONAL_DATA - START
 ******************************************************************************************************/
const char *
dnx_flow_field_additional_data_desc(
    int unit)
{
    return "General data associated with a specific IN-LIF object. \
            Controlled by the user and may be used by the ingress ACL as a qualifier.";
}

int
flow_field_additional_data_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", COMPILER_64_LO(terminator_info->additional_data));
}

shr_error_e
dnx_flow_field_additional_data_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    SHR_BITDCL additional_data_bits[2], max_field_size[2];
    uint32 max_field_bits, data_actual_bits = 0, field_mask, field_val;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), &table));

    /*
     * Calculate the size of the additional data in bits - Up to the non-zero MSB
     */
    additional_data_bits[0] = COMPILER_64_LO(terminator_info->additional_data);
    additional_data_bits[1] = COMPILER_64_HI(terminator_info->additional_data);
    data_actual_bits = shr_bitop_last_bit_get(&additional_data_bits[0], SHR_BITWID) + 1;
    if (data_actual_bits == -1)
    {
        data_actual_bits = 0;
    }

    /*
     * From this point on, only the 32LSBs are handled in order to improve performance. 
     * When more than 32bits of additional_data will be supported, this code should be updated. 
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                    (unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), DBAL_FIELD_LIF_GENERIC_DATA_0, FALSE,
                     table->nof_result_types, 0, (int *) &max_field_size[0]));
    max_field_bits = shr_bitop_last_bit_get(&max_field_size[0], SHR_BITWID) + 1;

    if (data_actual_bits > max_field_bits)
    {
        field_mask = (1U << (data_actual_bits - max_field_bits)) - 1;
        field_val = ((terminator_info->additional_data) >> max_field_bits) & field_mask;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_1, INST_SINGLE, field_val);
    }

    field_mask = (1U << max_field_bits) - 1;
    field_val = (terminator_info->additional_data) & field_mask;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_0, INST_SINGLE, field_val);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_additional_data_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    SHR_BITDCL max_field_size[2];
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    uint32 tmp_data_0 = 0, tmp_data_1 = 0, additional_data = 0,
        field_mask_0, field_mask_1, field_max_bits_0, field_max_bits_1;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_0, INST_SINGLE, &tmp_data_0));
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                    (unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), DBAL_FIELD_LIF_GENERIC_DATA_0, FALSE,
                     gport_hw_resources->inlif_dbal_result_type, 0, (int *) &max_field_size[0]));
    field_max_bits_0 = shr_bitop_last_bit_get(&max_field_size[0], SHR_BITWID) + 1;

    field_mask_0 = (1U << field_max_bits_0) - 1;
    additional_data = tmp_data_0 & field_mask_0;

    /*
     * Retrieve data also from LIF_GENERIC_DATA_1, if exists
     */
    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_1, INST_SINGLE,
                                             &tmp_data_1);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (SHR_SUCCESS(rv))
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                        (unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), DBAL_FIELD_LIF_GENERIC_DATA_1, FALSE,
                         gport_hw_resources->inlif_dbal_result_type, 0, (int *) &max_field_size[0]));
        field_max_bits_1 = shr_bitop_last_bit_get(&max_field_size[0], SHR_BITWID) + 1;
        field_mask_1 = ((1U << (field_max_bits_0 + field_max_bits_1)) - 1) & (~(field_mask_0));
        additional_data |= ((tmp_data_1 << field_max_bits_0) & field_mask_1);
    }

    COMPILER_64_ZERO(terminator_info->additional_data);
    COMPILER_64_ADD_32(terminator_info->additional_data, additional_data);

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_additional_data = {
    /*
     * field_name
     */
    "ADDITIONAL_DATA",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_EMPTY, /** DBAL_FIELD_LIF_GENERIC_DATA_0, DBAL_FIELD_LIF_GENERIC_DATA_1*/
    /*
     * field_desc
     */
    dnx_flow_field_additional_data_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_additional_data_set, dnx_flow_field_additional_data_get, NULL},
    /*
     * field_print
     */
    flow_field_additional_data_print_cb
};

/******************************************************************************************************
 *                                              ADDITIONAL_DATA - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              TERMINATOR_CLASS_ID - START
 ******************************************************************************************************/
int
flow_field_term_class_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", terminator_info->class_id);
}

const char *
dnx_flow_field_term_class_id_desc(
    int unit)
{
    return "VLAN Domain for the next layer";
}

shr_error_e
dnx_flow_field_term_class_id_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE,
                                 terminator_info->class_id);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_term_class_id_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, INST_SINGLE,
                             &(terminator_info->class_id)));

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_term_class_id = {
    /*
     * field_name
     */
    "TERMINATOR_CLASS_ID",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN,
    /*
     * field_desc
     */
    dnx_flow_field_term_class_id_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_term_class_id_set, dnx_flow_field_term_class_id_get, NULL},
    /*
     * field_print
     */
    flow_field_term_class_id_print_cb
};

/******************************************************************************************************
 *                                              TERMINATOR_CLASS_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              INITIATOR_CLASS_ID - START
 ******************************************************************************************************/
int
flow_field_init_class_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", initiator_info->class_id);
}

const char *
dnx_flow_field_init_class_id_desc(
    int unit)
{
    return "VLAN Domain for the next layer";
}

shr_error_e
dnx_flow_field_init_class_id_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE,
                                 initiator_info->class_id);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_init_class_id_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE,
                             &(initiator_info->class_id)));

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_init_class_id = {
    /*
     * field_name
     */
    "INITIATOR_CLASS_ID",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_CLASS_ID_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_ESEM_NAME_SPACE,
    /*
     * field_desc
     */
    dnx_flow_field_init_class_id_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_init_class_id_set, dnx_flow_field_init_class_id_get, NULL},
    /*
     * field_print
     */
    flow_field_init_class_id_print_cb
};

/******************************************************************************************************
 *                                              INITIATOR_CLASS_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                            TERMINATOR_FIELD_CLASS_ID - START
 ******************************************************************************************************/
int
flow_field_term_field_class_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", terminator_info->field_class_id);
}

const char *
field_desc_term_field_class_id_cb(
    int unit)
{
    return "field class id parameter";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_term_field_class_id = {
    /*
     * field_name
     */
    "TERM_FIELD_CLASS_ID",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_FIELD_CLASS_ID_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_IN_LIF_PROFILE,
    /*
     * field_desc
     */
    field_desc_term_field_class_id_cb,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_term_field_class_id_print_cb
};

/******************************************************************************************************
 *                                            TERMINATOR_FIELD_CLASS_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                                 TTL - START
 ******************************************************************************************************/
int
flow_field_qos_ttl_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", initiator_info->ttl);
}

const char *
field_desc_qos_ttl_cb(
    int unit)
{
    return "TTL param for egress QOS";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_qos_ttl = {
    /*
     * field_name
     */
    "QOS_TTL",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_TTL_VALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_TTL_MODE,
    /*
     * field_desc
     */
    field_desc_qos_ttl_cb,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_qos_ttl_print_cb
};

/******************************************************************************************************
 *                                                  TTL - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              OUTLIF_GROUP - START
 ******************************************************************************************************/
int
flow_field_outlif_group_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", initiator_info->outlif_profile_group);
}

const char *
field_desc_outlif_group_cb(
    int unit)
{
    return "Global lif property - Outlif group, can only be set in create, and cannot be cleared or updated";
}

shr_error_e
dnx_flow_field_outlif_group_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    global_lif_alloc_info_t alloc_info = { 0 };
    uint32 global_lif = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id);

    SHR_FUNC_INIT_VARS(unit);

    /** outlif group cannot be set when using virtual gport */
    if (FLOW_GPORT_IS_VIRTUAL(flow_handle_info->flow_id))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    /** no need to continue if outlif_group is not supported */
    if (dnx_data_lif.global_lif.global_lif_group_max_val_get(unit) == 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    /** get the allocation info parameters */
    SHR_IF_ERR_EXIT(dnx_algo_global_out_lif_alloc_info_get(unit, global_lif, &alloc_info));

    /** if value is not default */
    if (alloc_info.outlif_group != 0)
    {
        ((bcm_flow_initiator_info_t *) field_data)->outlif_profile_group = alloc_info.outlif_group;
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_outlif_group = {
    /*
     * field_name
     */
    "OUTLIF_GROUP",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_OUTLIF_PROFILE_GROUP_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_EMPTY, /** not part of the LIF fields */
    /*
     * field_desc
     */
    field_desc_outlif_group_cb,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_field_outlif_group_get, NULL},
    /*
     * field_print
     */
    flow_field_outlif_group_print_cb
};

/******************************************************************************************************
 *                                              OUTLIF_GROUP - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              FAILOVER_ID - START
 ******************************************************************************************************/
int
flow_field_failover_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_failover_t failover_id = (flow_app_type == FLOW_APP_TYPE_TERM) ?
        ((bcm_flow_terminator_info_t *) val)->failover_id : ((bcm_flow_initiator_info_t *) val)->failover_id;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", failover_id);
}

const char *
field_desc_failover_id_cb(
    int unit)
{
    return "Failover object index";
}

static shr_error_e
dnx_flow_field_failover_id_range_verify(
    int unit,
    int failover_type,
    int protection_pointer)
{
    SHR_FUNC_INIT_VARS(unit);

    if (failover_type == BCM_FAILOVER_TYPE_INGRESS)
    {
        SHR_RANGE_VERIFY(protection_pointer, 0, dnx_data_failover.path_select.ingress_size_get(unit) - 1, _SHR_E_PARAM,
                         "ingress failover-id out of bound.\n");
        if (protection_pointer == dnx_data_failover.path_select.ing_no_protection_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "failover id %d pre-allocated for ingress no protection.\n", protection_pointer);
        }
    }
    else if (failover_type == BCM_FAILOVER_TYPE_ENCAP)
    {
        SHR_RANGE_VERIFY(protection_pointer, 0, dnx_data_failover.path_select.egress_size_get(unit) - 1, _SHR_E_PARAM,
                         "egress failover-id out of bound.\n");
        if (protection_pointer == dnx_data_failover.path_select.egr_no_protection_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "failover id %d pre-allocated for egress no protection.\n", protection_pointer);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected failover type %d.\n", failover_type);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_field_failover_id_verify(
    int unit,
    bcm_failover_t failover_id)
{
    int protection_ptr, failover_type;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** decode failover */
    BCM_FAILOVER_TYPE_GET(failover_type, failover_id);
    BCM_FAILOVER_ID_GET(protection_ptr, failover_id);

    /** Verify protection pointer ranges */
    SHR_IF_ERR_EXIT(dnx_flow_field_failover_id_range_verify(unit, failover_type, protection_ptr));

    /** Verify protection pointer allocated */
    if (failover_type == BCM_FAILOVER_TYPE_INGRESS)
    {
        SHR_IF_ERR_EXIT(algo_failover_db.ing_path_select.is_allocated(unit, protection_ptr, &is_allocated));
    }
    else if (failover_type == BCM_FAILOVER_TYPE_INGRESS_MC)
    {
        SHR_IF_ERR_EXIT(algo_failover_db.ing_mc_path_select.is_allocated(unit, protection_ptr, &is_allocated));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_failover_db.egr_path_select.is_allocated(unit, protection_ptr, &is_allocated));
    }

    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "protection pointer wasn't allocated %d. use bcm_failover_create() to allocate\n",
                     protection_ptr);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_failover_id_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_failover_t failover_id = 0;
    int protection_pointer_encoded[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    failover_id = (flow_app_type == FLOW_APP_TYPE_TERM) ? ((bcm_flow_terminator_info_t *) field_data)->failover_id :
        ((bcm_flow_initiator_info_t *) field_data)->failover_id;

    SHR_IF_ERR_EXIT(dnx_flow_field_failover_id_verify(unit, failover_id));

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        /*
         * Convert from a failover ID to a HW Protection-Pointer encoded value
         */
        BCM_FAILOVER_ID_GET(failover_id, failover_id);
        DNX_FAILOVER_ID_PROTECTION_POINTER_ENCODE(protection_pointer_encoded[0], failover_id);
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        BCM_FAILOVER_ID_GET(protection_pointer_encoded[0], failover_id);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                 (uint32) protection_pointer_encoded[0]);
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_failover_id_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_failover_t failover_id_decoded[1] = { 0 };
    uint32 protection_pointer;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE, &protection_pointer));

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        /*
         * Convert the HW encoded Protection-Pointer to a failover ID
         */
        DNX_FAILOVER_ID_PROTECTION_POINTER_DECODE(failover_id_decoded[0], protection_pointer);
        BCM_FAILOVER_SET(failover_id_decoded[0], failover_id_decoded[0], BCM_FAILOVER_TYPE_INGRESS);
        ((bcm_flow_terminator_info_t *) field_data)->failover_id = failover_id_decoded[0];
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        if (protection_pointer != dnx_data_failover.path_select.egr_no_protection_get(unit))
        {
            BCM_FAILOVER_SET(failover_id_decoded[0], protection_pointer, BCM_FAILOVER_TYPE_ENCAP);
            ((bcm_flow_initiator_info_t *) field_data)->failover_id = failover_id_decoded[0];
        }
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_failover_id = {
    /*
     * field_name
     */
    "FAILOVER_ID",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_PROTECTION_POINTER,
    /*
     * field_desc
     */
    field_desc_failover_id_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_failover_id_set, dnx_flow_field_failover_id_get, NULL},
    /*
     * field_print
     */
    flow_field_failover_id_print_cb
};

/******************************************************************************************************
 *                                              FAILOVER_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              FAILOVER_STATE - START
 ******************************************************************************************************/
int
flow_field_failover_state_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    uint8 failover_state = (flow_app_type == FLOW_APP_TYPE_TERM) ?
        ((bcm_flow_terminator_info_t *) val)->failover_state : ((bcm_flow_initiator_info_t *) val)->failover_state;

    return sal_snprintf(print_string, FLOW_STR_MAX, "%s", flow_field_string_failover_state_types[failover_state]);
}

const char *
field_desc_failover_state_cb(
    int unit)
{
    return "Failover state: Primary / Secondary";
}

shr_error_e
dnx_flow_field_failover_state_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_failover_state_t failover_state = bcmbcmFlowFailoverStatePrimary;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        failover_state = ((bcm_flow_terminator_info_t *) field_data)->failover_state;
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        failover_state = ((bcm_flow_initiator_info_t *) field_data)->failover_state;
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE, failover_state);
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_failover_state_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_failover_state_t failover_state;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE, &failover_state));

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) field_data)->failover_state = failover_state;
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        ((bcm_flow_initiator_info_t *) field_data)->failover_state = failover_state;
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_failover_state = {
    /*
     * field_name
     */
    "FAILOVER_STATE",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_PROTECTION_PATH,
    /*
     * field_desc
     */
    field_desc_failover_state_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_failover_state_set, dnx_flow_field_failover_state_get, NULL},
    /*
     * field_print
     */
    flow_field_failover_state_print_cb
};

/******************************************************************************************************
 *                                              FAILOVER_STATE - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              LEARN_INFO - START
 ******************************************************************************************************/
int
flow_field_l2_learn_info_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX,
                        "dest port = 0x%x, encap_id = 0x%x, flush_group = 0x%x, flags = 0x%x, learn_strength = 0x%x",
                        terminator_info->l2_learn_info.dest_port, terminator_info->l2_learn_info.encap_id,
                        terminator_info->l2_learn_info.flush_group, terminator_info->l2_learn_info.l2_learn_info_flags,
                        terminator_info->l2_learn_info.learn_strength);
}

const char *
field_desc_l2_learn_info_cb(
    int unit)
{
    return "Learn information. May include Destination, Out-LIF and a MACT flush group.\n"
        "The flag BCM_FLOW_L2_LEARN_INFO_NOT_OPTIMIZED determines that no Optimized Learn-Payload-Context is used.\n"
        "The flag BCM_FLOW_L2_LEARN_INFO_DEST_ONLY determines the learning will be for a Destination with no Out-LIF.";
}

static shr_error_e
dnx_flow_field_l2_learn_info_validate(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_l2_learn_info_t * l2_learn_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** In case of Non-Optimized, a destination must be set */
    if (_SHR_IS_FLAG_SET(l2_learn_info->l2_learn_info_flags, BCM_FLOW_L2_LEARN_INFO_NOT_OPTIMIZED))
    {
        if (l2_learn_info->dest_port == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! l2_learn_info dest_port must be set when BCM_FLOW_L2_LEARN_INFO_NOT_OPTIMIZED is set");
        }
    }

    /** In case of Dest-Only, an Out-LIF shouldn't be set */
    if (_SHR_IS_FLAG_SET(l2_learn_info->l2_learn_info_flags, BCM_FLOW_L2_LEARN_INFO_DEST_ONLY))
    {
        if (l2_learn_info->encap_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! l2_learn_info encap_id should be reset when BCM_FLOW_L2_LEARN_INFO_DEST_ONLY is set");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l2_learn_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_l2_learn_info_t *l2_learn_info = &(((bcm_flow_terminator_info_t *) field_data)->l2_learn_info);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_field_l2_learn_info_validate(unit, entry_handle_id, l2_learn_info));

    if ((dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_IGNORE_L2_LEARN_INFO_SET)) ||
        (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_L2_LEARN_INFO_STANDARD_SET)))
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l2_learn_info_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 dbal_struct_data;
    uint8 is_field_on_handle;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dbal_enum_value_field_learn_payload_context_e learn_context;
    dnx_algo_gpm_forward_info_t forward_info = { 0 };
    int is_symmetric = FALSE, is_extended = FALSE, is_optimized = FALSE, is_dest_only = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE, &learn_context));

    /** Analyze the learn-info according to the learn-payload-context */
    if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_learn_payload_res_optimize))
    {
        is_extended = TRUE;
    }

    /*
     * Retrieve the learn dest-port and flush-group information according to the 
     * existing learn-info struct on the handle. 
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, &is_field_on_handle,
                     &dbal_struct_data));

    if (is_field_on_handle == TRUE)
    {
        if (!is_optimized)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_DESTINATION,
                             &forward_info.destination, &dbal_struct_data));
        }
        if (is_extended)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_FLUSH_GROUP,
                             &(terminator_info->l2_learn_info.flush_group), &dbal_struct_data));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, &is_field_on_handle,
                         &dbal_struct_data));

        if (is_field_on_handle == TRUE)
        {
            if (!is_optimized)
            {
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, DBAL_FIELD_DESTINATION,
                                 &forward_info.destination, &dbal_struct_data));
            }
        }
        else
        {
            uint8 is_set = FALSE;

            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                            (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT,
                             &is_set));

            if (is_set == TRUE)
            {
                uint32 dbal_struct_arr[DNX_FLOW_LEARN_MGMT_INFO_UIN32_ARR_MAX_SIZE] = { 0 };

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, INST_SINGLE,
                                 dbal_struct_arr));

                if (!is_optimized)
                {
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                    (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                                     &forward_info.destination, dbal_struct_arr));
                }
                if (is_extended)
                {
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                    (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_FLUSH_GROUP,
                                     &(terminator_info->l2_learn_info.flush_group), dbal_struct_arr));
                }
                if ((!is_symmetric) && (!is_dest_only))
                {
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                    (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_GLOB_OUT_LIF,
                                     (uint32 *) &(terminator_info->l2_learn_info.encap_id), dbal_struct_arr));
                    BCM_ENCAP_ID_SET(terminator_info->l2_learn_info.encap_id, terminator_info->l2_learn_info.encap_id);
                }
            }
        }
    }

    /** Convert the HW encoded destination to a physical gport that represents the destination */
    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, forward_info.destination,
                     &(terminator_info->l2_learn_info.dest_port)));

    /*
     * Retrieve the Optimized flag according to the selected learn-payload-context
     */
    if (!(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_L2_LEARN_NO_OPTIMIZATION)) && (!is_optimized))
    {
        terminator_info->l2_learn_info.l2_learn_info_flags |= BCM_FLOW_L2_LEARN_INFO_NOT_OPTIMIZED;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l2_learn_info_delete(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint8 is_set = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                    (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, &is_set));
    if (is_set)
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, flow_handle_info->flow_id));
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_l2_learn_info = {
    /*
     * field_name
     */
    "L2_LEARN_INFO",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    TRUE, /** this field is not exactly a profile, but there is a dedicated code in the lif mgmnt that handles it*/
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_EMPTY,
    /*
     * field_desc
     */
    field_desc_l2_learn_info_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_l2_learn_info_set, dnx_flow_field_l2_learn_info_get, dnx_flow_field_l2_learn_info_delete},
    /*
     * field_print
     */
    flow_field_l2_learn_info_print_cb
};

/******************************************************************************************************
 *                                              LEARN_INFO - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              LEARN_ENABLE - START
 ******************************************************************************************************/
int
flow_field_learn_enable_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", terminator_info->learn_enable);
}

const char *
field_desc_learn_enable_cb(
    int unit)
{
    return "Learn enable/disable indication";
}

shr_error_e
dnx_flow_field_learn_enable_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    int learn_enable = ((bcm_flow_terminator_info_t *) field_data)->learn_enable;

    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, learn_enable);
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_learn_enable_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 learn_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, &learn_enable));
    ((bcm_flow_terminator_info_t *) field_data)->learn_enable = learn_enable;

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_learn_enable = {
    /*
     * field_name
     */
    "LEARN_ENABLE",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_LEARN_ENABLE,
    /*
     * field_desc
     */
    field_desc_learn_enable_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_learn_enable_set, dnx_flow_field_learn_enable_get, NULL},
    /*
     * field_print
     */
    flow_field_learn_enable_print_cb
};

/******************************************************************************************************
 *                                              LEARN_ENABLE - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                            INIT_TPID_CLASS_ID - START
 ******************************************************************************************************/
int
flow_field_init_tpid_class_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", initiator_info->tpid_class_id);
}

shr_error_e
dnx_flow_field_init_tpid_class_id_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 tpid_class_id;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_prp_profile_supported))
    {
        SHR_EXIT();
    }

    tpid_class_id = ((bcm_flow_initiator_info_t *) (field_data))->tpid_class_id;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, INST_SINGLE, tpid_class_id);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_init_tpid_class_id_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 tpid_class_id;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_prp_profile_supported))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, INST_SINGLE, &tpid_class_id));
    ((bcm_flow_initiator_info_t *) field_data)->tpid_class_id = tpid_class_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Field description call back function
 */
const char *
field_desc_tpid_class_id_cb(
    int unit)
{
    return "TPID class ID. Not validated to be in range, created by bcm_port_tpid_class_set()";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_init_tpid_class_id = {
    /*
     * field_name
     */
    "TPID_CLASS_ID",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_TPID_CLASS_ID_VALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_EMPTY, /** PRP_PRFOILE*/
    /*
     * field_desc
     */
    field_desc_tpid_class_id_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_init_tpid_class_id_set, dnx_flow_field_init_tpid_class_id_get, NULL},
    /*
     * field_print
     */
    flow_field_init_tpid_class_id_print_cb
};
/******************************************************************************************************
 *                                            INIT_TPID_CLASS_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                        INITIATOR_FIELD_CLASS_ID - START
 ******************************************************************************************************/
int
flow_field_init_field_class_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", initiator_info->field_class_id);
}

const char *
field_desc_init_field_class_id_cb(
    int unit)
{
    return "field class id parameter";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_init_field_class_id = {
    /*
     * field_name
     */
    "INIT_FIELD_CLASS_ID",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_FIELD_CLASS_ID_VALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_EMPTY, /** PRP_PRFOILE*/
    /*
     * field_desc
     */
    field_desc_init_field_class_id_cb,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_init_field_class_id_print_cb
};
/******************************************************************************************************
 *                                        INITIATOR_FIELD_CLASS_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                        INITIATOR_FIELD_CLASS_CS_ID - START
 ******************************************************************************************************/
int
flow_field_field_class_cs_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    uint32 field_class_cs_id = (flow_app_type == FLOW_APP_TYPE_TERM) ?
        ((bcm_flow_terminator_info_t *) val)->field_class_cs_id :
        ((bcm_flow_initiator_info_t *) val)->field_class_cs_id;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", field_class_cs_id);
}

const char *
field_desc_field_class_cs_id_cb(
    int unit)
{
    return "field class cs id parameter";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_field_class_cs_id = {
    /*
     * field_name
     */
    "FIELD_CLASS_CS_ID",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_FIELD_CLASS_CS_ID_VALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_FIELD_CLASS_CS_ID_VALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_EMPTY, /** PRP_PRFOILE*/
    /*
     * field_desc
     */
    field_desc_field_class_cs_id_cb,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_field_class_cs_id_print_cb
};
/******************************************************************************************************
 *                                        INITIATOR_FIELD_CLASS_CS_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                           SVTAG_ENABLE - START
 ******************************************************************************************************/
int
flow_field_svtag_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    return sal_snprintf(print_string, FLOW_STR_MAX, "Enabled");
}

const char *
field_desc_svtag_en_cb(
    int unit)
{
    return "svtag enable";
}

shr_error_e
dnx_flow_field_svtag_en_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    SHR_FUNC_INIT_VARS(unit);

    {
        SHR_IF_ERR_EXIT(BCM_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_svtag_en_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{

    SHR_FUNC_INIT_VARS(unit);
    {
        SHR_IF_ERR_EXIT(BCM_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_svtag_en = {
    /*
     * field_name
     */
    "SVTAG_ENABLE",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_SVTAG_ENABLE_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_ESEM_COMMAND,
    /*
     * field_desc
     */
    field_desc_svtag_en_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_svtag_en_set, dnx_flow_field_svtag_en_get, NULL},
    /*
     * field_print
     */
    flow_field_svtag_print_cb
};
/******************************************************************************************************
 *                                             SVTAG_ENABLE - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                        TERM_TPID_CLASS_ID - START
 ******************************************************************************************************/
int
flow_field_term_tpid_class_id_print_cb(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", terminator_info->tpid_class_id);
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_term_tpid_class_id = {
    /*
     * field_name
     */
    "TPID_CLASS_ID",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_TPID_CLASS_ID_VALID,
    /*
     * init_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * is_profile
     */
    TRUE,
    /*
     * mapped_dbal_field
     */
    DBAL_FIELD_IN_LIF_PROFILE,
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_common_field_empty_set, dnx_flow_common_field_empty_get, NULL},
    /*
     * field_print
     */
    flow_field_term_tpid_class_id_print_cb
};
/******************************************************************************************************
 *                                        TERM_TPID_CLASS_ID - END
 ******************************************************************************************************/
