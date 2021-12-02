
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <shared/shrextend/shrextend_debug.h>
#include "flow_def.h"
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/qos/qos.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_rx_access.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_table_mngr.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/flow/flow.h>
#include "app_defs/ip_tunnel/flow_ipvx_tunnel_terminator.h"

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

/******************************************************************************************************
 *                                              VSI - START
 ******************************************************************************************************/

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
     * field_desc
     */
    field_desc_vsi_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_vsi_set, dnx_flow_field_vsi_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              VSI - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              VRF - START
 ******************************************************************************************************/
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_vrf_set, dnx_flow_field_vrf_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              VRF - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              STAT_ID - START
 ******************************************************************************************************/

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
     * field_desc
     */
    field_desc_stat_id_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_stat_id_set, dnx_flow_field_stat_id_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};
/******************************************************************************************************
 *                                              STAT_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              L2_INGRESS_INFO - START
 ******************************************************************************************************/
/*
 * Print call back function to display the value of FLOW field l2_ingress_info
 * Return the formated string length or an error
 */
int
flow_field_l2_ingress_info_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "ingress_network_group_id = %d, service_type = %s",
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

/*
 * Get in lif profile
 */
static shr_error_e
dnx_flow_terminator_in_lif_profile_get(
    int unit,
    uint32 get_entry_handle_id,
    uint32 *in_lif_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    *in_lif_profile = 0;
    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, get_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, in_lif_profile));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_service_type_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_service_type_t service_type)
{
    uint32 service_type_value = 0;
    dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (service_type == bcmFlowServiceTypeCrossConnect)
    {
        vtt_lif_service_type = DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P;
    }
    else
    {
        vtt_lif_service_type = DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP;
    }

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_TYPE,
                                                    &vtt_lif_service_type, &service_type_value));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE, service_type_value);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_service_type_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_service_type_t * service_type)
{
    uint32 dbal_field = 0;
    dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE,
                                                        INST_SINGLE, &dbal_field));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_TYPE,
                                                    &vtt_lif_service_type, &dbal_field));

    if (vtt_lif_service_type == DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P)
    {
        (*service_type) |= bcmFlowServiceTypeCrossConnect;
    }
    else
    {
        (*service_type) |= bcmFlowServiceTypeMultiPoint;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l2_ingress_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    dbal_tables_e dbal_table_id;
    dnx_in_lif_profile_last_info_t last_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** if required, use the dedicated logic to set rather than this code */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
    {
        SHR_EXIT();
    }

    /** set the inlif_profile */
    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    /** Allocate default inLIF profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    in_lif_profile_info.egress_fields.in_lif_orientation = terminator_info->l2_ingress_info.ingress_network_group_id;

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange(unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile,
                                                LIF, dbal_table_id, &last_profile));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_in_lif_profile);

    /** set the inlif_profile */
    SHR_IF_ERR_EXIT(dnx_flow_service_type_set(unit, entry_handle_id, terminator_info->l2_ingress_info.service_type));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l2_ingress_info_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    in_lif_profile_info_t in_lif_profile_info;
    uint32 in_lif_profile = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** if required, use the dedicated logic to get rather than this code */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    /** get the inlif_profile */
    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                                                &in_lif_profile));

    /** get in_lif_profile data and store it in in_lif_profile_info */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                    (unit, in_lif_profile, &in_lif_profile_info, gport_hw_resources->inlif_dbal_table_id, LIF));

    /*
     * for application that have both l3 and l2 ingress info, default value is invalid
     * This was made in order to allow us to determine which common field was set
     */
    if ((flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
        (in_lif_profile_info.egress_fields.in_lif_orientation == DEFAULT_IN_LIF_ORIENTATION))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();

    }

    terminator_info->l2_ingress_info.ingress_network_group_id = in_lif_profile_info.egress_fields.in_lif_orientation;

    /** get the service_type - if reached here, the value was previously set */
    SHR_IF_ERR_EXIT(dnx_flow_service_type_get(unit, entry_handle_id, &(terminator_info->l2_ingress_info.service_type)));

exit:
    SHR_FUNC_EXIT;
}

/* Replace callback for l2 ingress info. Replace profile, if needed - remove the old one */
shr_error_e
dnx_flow_field_l2_ingress_info_replace(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dnx_in_lif_profile_last_info_t last_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** if required, use the dedicated logic to replace rather than this code */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
    {
        SHR_EXIT();
    }

    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    /** Get entry from inLIF table */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get
                              (unit, get_entry_handle_id, (uint32 *) &old_in_lif_profile), _SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                    (unit, old_in_lif_profile, &in_lif_profile_info, gport_hw_resources->inlif_dbal_table_id, LIF));

    /** it is invalid that l3_ingress_info was set previously and not cleared now */
    if ((in_lif_profile_info.ingress_fields.urpf_mode != bcmL3IngressUrpfDisable) &&
        (!(terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "In replace, cannot set L2_INGRESS_INFO_VALID without clearing L3_INGRESS_INFO_VALID");
    }

    in_lif_profile_info.ingress_fields.urpf_mode = bcmL3IngressUrpfDisable;
    in_lif_profile_info.egress_fields.in_lif_orientation = terminator_info->l2_ingress_info.ingress_network_group_id;
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     gport_hw_resources->inlif_dbal_table_id, &last_profile));
    dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_in_lif_profile);

    /** replace the inlif_profile */
    SHR_IF_ERR_EXIT(dnx_flow_service_type_set
                    (unit, set_entry_handle_id, terminator_info->l2_ingress_info.service_type));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l2_ingress_info_delete(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    int new_in_lif_profile;
    uint32 in_lif_profile = 0;
    uint8 is_set = FALSE;
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);

    /** if required, use the dedicated logic to delete rather than this code */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
    {
        SHR_EXIT();
    }

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     * In this field case, need also to verify that this the common field is correct what who set the dbal field
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                                                       &is_set, &in_lif_profile));

    if (is_set)
    {
        /** get in_lif_profile data and store it in in_lif_profile_info */
        SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

        in_lif_profile_info.ingress_fields.lif_gport = flow_handle_info->flow_id;

        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                        (unit, in_lif_profile, &in_lif_profile_info, gport_hw_resources->inlif_dbal_table_id, LIF));

        /** if l2_ingress_info was set - release the profile */
        if ((in_lif_profile_info.egress_fields.in_lif_orientation != DEFAULT_IN_LIF_ORIENTATION) ||
            (!(flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID)))
        {
            /** Get entry from inLIF table */
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get(unit, entry_handle_id, &in_lif_profile),
                                      _SHR_E_NOT_FOUND);
            /** Deallocate in_lif_profile */
            SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc
                            (unit, in_lif_profile, &new_in_lif_profile, &in_lif_profile_info, LIF));
        }
    }

exit:
    SHR_FUNC_EXIT;
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
     * field_desc
     */
    field_desc_l2_ingress_info_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_l2_ingress_info_set, dnx_flow_field_l2_ingress_info_get, dnx_flow_field_l2_ingress_info_replace,
     dnx_flow_field_l2_ingress_info_delete},
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

const char *
field_desc_l2_egress_info_cb(
    int unit)
{
    return "L2 Egress info: Orientation group etc";
}

/*
 * Retrieve the Out-LIF profile from the handler
 */
static shr_error_e
dnx_flow_initiator_out_lif_profile_get(
    int unit,
    uint32 get_entry_handle_id,
    uint32 *out_lif_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    *out_lif_profile = 0;
    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, get_entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, out_lif_profile));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l2_egress_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    int new_out_lif_profile;
    dbal_tables_e dbal_table_id;
    uint8 is_last = 0;
    etpp_out_lif_profile_info_t out_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field not supported yet");
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    /*
     * Format the data for the Out-LIF profile 
     * Note: Future additional subfields should be supported here
     */
    etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
    out_lif_profile_info.out_lif_orientation = initiator_info->l2_egress_info.egress_network_group_id;

    /*
     * Allocate an Out-LIF profile
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange(unit,
                                                      dbal_table_id, 0, &out_lif_profile_info,
                                                      DNX_OUT_LIF_PROFILE_DEFAULT, &new_out_lif_profile, &is_last));

    /*
     * Set the allocated Out-LIF profile
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, new_out_lif_profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l2_egress_info_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    int out_lif_profile;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the Out-LIF Profile value
     */

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE,
                                                                INST_SINGLE, (uint32 *) &out_lif_profile));

    /*
     * Retrieve the data for the Out-LIF Profile
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));
    initiator_info->l2_egress_info.egress_network_group_id = out_lif_profile_info.out_lif_orientation;

exit:
    SHR_FUNC_EXIT;
}

/* 
 * Replace callback for l2 egress info
 */
shr_error_e
dnx_flow_field_l2_egress_info_replace(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    int old_out_lif_profile, new_out_lif_profile = DNX_OUT_LIF_PROFILE_DEFAULT;
    uint8 is_last = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the Out-LIF Profile value
     */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_initiator_out_lif_profile_get
                              (unit, get_entry_handle_id, (uint32 *) &old_out_lif_profile), _SHR_E_NOT_FOUND);

    etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, old_out_lif_profile, &out_lif_profile_info));
    out_lif_profile_info.out_lif_orientation = initiator_info->l2_egress_info.egress_network_group_id;
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange(unit,
                                                      gport_hw_resources->outlif_dbal_table_id, 0,
                                                      &out_lif_profile_info, old_out_lif_profile,
                                                      &new_out_lif_profile, &is_last));
    dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_out_lif_profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l2_egress_info_delete(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    etpp_out_lif_profile_info_t out_lif_profile_info;
    int out_lif_profile, new_out_lif_profile;
    uint8 is_last = FALSE, is_set = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                    (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_OUT_LIF_PROFILE, &is_set));
    if (is_set)
    {
        /** Get entry from inLIF table */
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_initiator_out_lif_profile_get
                                  (unit, entry_handle_id, (uint32 *) &out_lif_profile), _SHR_E_NOT_FOUND);

        /*
         * Free the Out-LIF profile
         * by calling the exchange function with the reset default profile data
         */
        etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
        SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                        (unit, gport_hw_resources->outlif_dbal_table_id, 0, &out_lif_profile_info, out_lif_profile,
                         &new_out_lif_profile, &is_last));
        /** in dbal table EEDB_IPV4_TUNNEL, OUT_LIF_PROFILE has an arr-prefix */
        if ((!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPV4_INITIATOR, FLOW_STR_MAX)) ||
            (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPV6_INITIATOR, FLOW_STR_MAX)))
        {
            if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                             DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE, new_out_lif_profile);
            }
        }
        else
        {
            if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                             DNX_OUT_LIF_PROFILE_DEFAULT, new_out_lif_profile);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
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
     * field_desc
     */
    field_desc_l2_egress_info_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_l2_egress_info_set, dnx_flow_field_l2_egress_info_get, dnx_flow_field_l2_egress_info_replace,
     dnx_flow_field_l2_egress_info_delete},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              L2_EGRESS_INFO - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              L3_INGRESS_INFO - START
 ******************************************************************************************************/
/*
 * Print call back function to display the value of FLOW field l3_ingress_info
 * Return the formated string length or an error
 */
int
flow_field_l3_ingress_info_print_cb(
    int unit,
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

shr_error_e
dnx_flow_field_l3_ingress_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    dbal_tables_e dbal_table_id;
    dnx_in_lif_profile_last_info_t last_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** if required, use the dedicated logic to set rather than this code */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
    {
        SHR_EXIT();
    }

    /** set the inlif_profile */
    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    /** Allocate default inLIF profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    in_lif_profile_info.ingress_fields.urpf_mode = terminator_info->l3_ingress_info.urpf_mode;

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     dbal_table_id, &last_profile));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_in_lif_profile);

    /** set the service_type */
    SHR_IF_ERR_EXIT(dnx_flow_service_type_set(unit, entry_handle_id, terminator_info->l3_ingress_info.service_type));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l3_ingress_info_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    in_lif_profile_info_t in_lif_profile_info;
    uint32 in_lif_profile = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** if required, use the dedicated logic to get rather than this code */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    /** get the inlif_profile */
    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                                                &in_lif_profile));

    /*
     * get in_lif_profile data and store it in in_lif_profile_info
     */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                    (unit, in_lif_profile, &in_lif_profile_info, gport_hw_resources->inlif_dbal_table_id, LIF));

    /*
     * for application that have both l3 and l2 ingress info, default value is invalid
     * This was made in order to allow us to determine which common field was set
     */
    if ((flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID) &&
        (flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID) &&
        (in_lif_profile_info.ingress_fields.urpf_mode == bcmL3IngressUrpfDisable))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();

    }

    terminator_info->l3_ingress_info.urpf_mode = in_lif_profile_info.ingress_fields.urpf_mode;

    /** get the service_type - if reached here, the value was previously set */
    SHR_IF_ERR_EXIT(dnx_flow_service_type_get(unit, entry_handle_id, &(terminator_info->l3_ingress_info.service_type)));

exit:
    SHR_FUNC_EXIT;
}

/* Replace callback for l3 ingress info. Replace profile, if needed - remove the old one */
shr_error_e
dnx_flow_field_l3_ingress_info_replace(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dnx_in_lif_profile_last_info_t last_profile;

    SHR_FUNC_INIT_VARS(unit);

    /** if required, use the dedicated logic to replace rather than this code */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
    {
        SHR_EXIT();
    }

    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    /** Get entry from inLIF table */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get
                              (unit, get_entry_handle_id, (uint32 *) &old_in_lif_profile), _SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                    (unit, old_in_lif_profile, &in_lif_profile_info, gport_hw_resources->inlif_dbal_table_id, LIF));

    /** it is invalid that l2_ingress_info was set previously and not cleared now */
    if ((in_lif_profile_info.egress_fields.in_lif_orientation != DEFAULT_IN_LIF_ORIENTATION) &&
        (!(terminator_info->valid_elements_clear & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "In replace, cannot set L3_INGRESS_INFO_VALID without clearing L2_INGRESS_INFO_VALID");
    }

    in_lif_profile_info.egress_fields.in_lif_orientation = DEFAULT_IN_LIF_ORIENTATION;
    in_lif_profile_info.ingress_fields.urpf_mode = terminator_info->l3_ingress_info.urpf_mode;
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     gport_hw_resources->inlif_dbal_table_id, &last_profile));
    dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_in_lif_profile);

    /** replace the inlif_profile */
    SHR_IF_ERR_EXIT(dnx_flow_service_type_set
                    (unit, set_entry_handle_id, terminator_info->l3_ingress_info.service_type));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l3_ingress_info_delete(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    int new_in_lif_profile;
    uint32 in_lif_profile = 0;
    uint8 is_set = FALSE;
    in_lif_profile_info_t in_lif_profile_info;

    SHR_FUNC_INIT_VARS(unit);

    /** if required, use the dedicated logic to delete rather than this code */
    if (dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled))
    {
        SHR_EXIT();
    }

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     * In this field case, need also to verify that this the common field is correct what who set the dbal field
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE,
                                                                       &is_set, &in_lif_profile));

    if (is_set)
    {
        /** get in_lif_profile data and store it in in_lif_profile_info */
        SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

        in_lif_profile_info.ingress_fields.lif_gport = flow_handle_info->flow_id;

        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data
                        (unit, in_lif_profile, &in_lif_profile_info, gport_hw_resources->inlif_dbal_table_id, LIF));

        /** if l3_ingress_info was set - release the profile */
        if ((in_lif_profile_info.ingress_fields.urpf_mode != bcmL3IngressUrpfDisable) ||
            (!(flow_app_info->valid_common_fields_bitmap & BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID)))
        {
            /** Get entry from inLIF table */
            SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get(unit, entry_handle_id, &in_lif_profile),
                                      _SHR_E_NOT_FOUND);
            /** Deallocate in_lif_profile */
            SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc
                            (unit, in_lif_profile, &new_in_lif_profile, &in_lif_profile_info, LIF));
        }
    }

exit:
    SHR_FUNC_EXIT;
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
     * field_desc
     */
    flow_field_l3_ingress_info_desc,
    /*
     * field_cb
     */
    {dnx_flow_field_l3_ingress_info_set, dnx_flow_field_l3_ingress_info_get, dnx_flow_field_l3_ingress_info_replace,
     dnx_flow_field_l3_ingress_info_delete},
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
    NULL
};

/******************************************************************************************************
 *                                              STAT_PP_PROFILE - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              QOS_INGRESS_MODEL - START
 ******************************************************************************************************/
/*
 * Print call back function to display the vale of FLOW field qos_ingress_model 
 * Return the formated string length or an error
 */
int
flow_field_qos_ingress_model_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_qos_ingress_model_t *qos_model = (bcm_qos_ingress_model_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "PHB: %s, Remark: %s, TTL: %s",
                        flow_field_string_qos_ingress_model_type[qos_model->ingress_phb],
                        flow_field_string_qos_ingress_model_type[qos_model->ingress_remark],
                        flow_field_string_qos_ingress_model_type[qos_model->ingress_ttl]);

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

/*
 * Get in propag profile
 */
static shr_error_e
dnx_flow_terminator_propag_profile_get(
    int unit,
    uint32 get_entry_handle_id,
    uint32 *propag_profile)
{
    uint32 dbal_field = 0;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, get_entry_handle_id, DBAL_FIELD_PROPAGATION_PROFILE, INST_SINGLE, &dbal_field));
    *propag_profile = dbal_field;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_qos_ingress_model_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    dnx_qos_propagation_type_e ttl_propagation_type = 0;
    dnx_qos_propagation_type_e phb_propagation_type = 0;
    dnx_qos_propagation_type_e remark_propagation_type = 0;
    dnx_qos_propagation_type_e ecn_propagation_type = 0;
    int propag_profile = 0;
    int old_propag_profile = 0;
    uint8 qos_propagation_prof_first_ref = 0;
    uint8 qos_propagation_prof_last_ref;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    /** Set QOS */
    ecn_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE))
    {
        phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }
    else
    {
        phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    }
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_QOS_RM_DEFAULT_PIPE))
    {
        remark_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }
    else
    {
        remark_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    }
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_QOS_TTL_DEFAULT_PIPE))
    {
        ttl_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }
    else
    {
        ttl_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    }

    if (terminator_info->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, terminator_info->ingress_qos_model.ingress_phb, &phb_propagation_type));
    }

    if (terminator_info->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, terminator_info->ingress_qos_model.ingress_remark, &remark_propagation_type));
    }

    if (terminator_info->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, terminator_info->ingress_qos_model.ingress_ttl, &ttl_propagation_type));
    }

    /** Alloc propagation profile ID corresponding to chosen QOS model */
    old_propag_profile = DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE;
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc(unit, phb_propagation_type,
                                                              remark_propagation_type, ecn_propagation_type,
                                                              ttl_propagation_type, old_propag_profile,
                                                              &propag_profile,
                                                              &qos_propagation_prof_first_ref,
                                                              &qos_propagation_prof_last_ref));

    /*
     * Update H/W
     */
    if (qos_propagation_prof_first_ref == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set(unit, propag_profile, phb_propagation_type,
                                                                   remark_propagation_type,
                                                                   ecn_propagation_type, ttl_propagation_type));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROFILE, INST_SINGLE,
                                 (uint32) propag_profile);

    if (terminator_info->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE,
                                     DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_qos_ingress_model_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    dnx_qos_propagation_type_e ttl_propagation_type;
    uint32 dbal_field = 0;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    int propagation_prof;
    uint8 is_valid;
    dnx_qos_propagation_type_e phb_default_type;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROFILE, INST_SINGLE, &dbal_field));

    propagation_prof = (int) dbal_field;

    /** Update QOS flags according to returned propagation_prof */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get
                    (unit, propagation_prof, &phb_propagation_type, &remark_propagation_type, &ecn_propagation_type,
                     &ttl_propagation_type));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE))
    {
        phb_default_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }
    else
    {
        phb_default_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    }

    if (phb_propagation_type != phb_default_type)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, phb_propagation_type, &(terminator_info->ingress_qos_model.ingress_phb)));
    }

    if (remark_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, remark_propagation_type, &(terminator_info->ingress_qos_model.ingress_remark)));
    }

    if (ttl_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, ttl_propagation_type, &(terminator_info->ingress_qos_model.ingress_ttl)));
    }

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_lif_mgmt_field_is_set_in_hw
                           (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_ECN_MAPPING_PROFILE, &is_valid));

    if (is_valid)
    {
        uint32 ecn_mapping_profile;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE, &ecn_mapping_profile));

        if (ecn_mapping_profile == DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE)
        {
            terminator_info->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelEligible;
        }
        else
        {
            terminator_info->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* Replace callback for qos ingress model. Replace profile, if needed - remove the old one */
shr_error_e
dnx_flow_field_qos_ingress_model_replace(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    dnx_qos_propagation_type_e ttl_propagation_type = 0;
    dnx_qos_propagation_type_e phb_propagation_type = 0;
    dnx_qos_propagation_type_e remark_propagation_type = 0;
    dnx_qos_propagation_type_e ecn_propagation_type = 0;
    int propag_profile = 0;
    int old_propag_profile = 0;
    uint8 qos_propagation_prof_first_ref = 0;
    uint8 qos_propagation_prof_last_ref;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    /** Set QOS */
    ecn_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE))
    {
        phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }
    else
    {
        phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    }
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_QOS_RM_DEFAULT_PIPE))
    {
        remark_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }
    else
    {
        remark_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    }
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_QOS_TTL_DEFAULT_PIPE))
    {
        ttl_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }
    else
    {
        ttl_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    }

    /*
     * If this field is to be restored to default - leave the profile properties with their default values, so that the 
     * original profile will be replaced with the default one 
     */
    if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID))
    {
        if (terminator_info->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                            (unit, terminator_info->ingress_qos_model.ingress_phb, &phb_propagation_type));
        }

        if (terminator_info->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                            (unit, terminator_info->ingress_qos_model.ingress_remark, &remark_propagation_type));
        }

        if (terminator_info->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                            (unit, terminator_info->ingress_qos_model.ingress_ttl, &ttl_propagation_type));
        }
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_propag_profile_get
                              (unit, get_entry_handle_id, (uint32 *) &old_propag_profile), _SHR_E_NOT_FOUND);

    /** Update propagation profile ID. */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc
                    (unit, phb_propagation_type, remark_propagation_type,
                     ecn_propagation_type, ttl_propagation_type, old_propag_profile, &propag_profile,
                     &qos_propagation_prof_first_ref, &qos_propagation_prof_last_ref));

    /** Update the profile table if it is first used*/
    if (qos_propagation_prof_first_ref == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set(unit, propag_profile, phb_propagation_type,
                                                                   remark_propagation_type,
                                                                   ecn_propagation_type, ttl_propagation_type));
    }

    /*
     * In case this is a cleared-field - no need to set the default profile to HW, only manage the profiles 
     */
    if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID))
    {
        dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_PROPAGATION_PROFILE, INST_SINGLE,
                                     (uint32) propag_profile);
    }

    /** Clear the unused profile which is indicated by last-referrence*/
    if (qos_propagation_prof_last_ref == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_clear(unit, old_propag_profile));
    }

    if (terminator_info->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
    {
        dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE,
                                     DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_qos_ingress_model_destroy(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    int propagation_prof = 0, rv = 0;
    dbal_entry_handle_t *entry_handle_ptr = NULL;
    uint8 is_set = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                    (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_PROPAGATION_PROFILE, &is_set));
    if (is_set)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle_ptr));

        /** Get entry from inLIF table */
        rv = dnx_flow_terminator_propag_profile_get(unit, entry_handle_id, (uint32 *) &propagation_prof);

        /*
         * In case propagation profile is found - delete it 
         */
        if (rv == _SHR_E_NONE)
        {
            /** Free propagation profile template */
            SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_free_and_hw_clear(unit, propagation_prof, NULL));
        }
        /*
         * Not found or another error 
         */
        else
        {
            SHR_SET_CURRENT_ERR(rv);
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
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
     * field_desc
     */
    dnx_flow_field_qos_ingress_model_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_qos_ingress_model_set, dnx_flow_field_qos_ingress_model_get,
     dnx_flow_field_qos_ingress_model_replace,
     dnx_flow_field_qos_ingress_model_destroy},
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
/*
 * Print call back function to display the vale of FLOW field dest_info 
 * Return the formated string length or an error
 */
int
flow_field_dest_info_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_flow_dest_info_t *dest_info = (bcm_flow_dest_info_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "Port: %d, Encap: %d", dest_info->dest_port, dest_info->dest_encap);

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
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert from dest gport and encap-id to a forward info that consists of
     * an encoded destination.
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, terminator_info->dest_info.dest_port,
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_dest_info_set, dnx_flow_field_dest_info_get, NULL, NULL},
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
/*
 * Print call back function to display the vale of FLOW field l3_intf_id 
 * Return the formated string length or an error
 */
int
flow_field_l3_intf_id_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_if_t *l3_intf_id = (bcm_if_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", *l3_intf_id);

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
        gport = dnx_flow_gport_to_flow_id(unit, gport);
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_l3_intf_id_set, dnx_flow_field_l3_intf_id_get, NULL, NULL},
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
/*
 * Print call back function to display the vale of FLOW field action_gport
 * Return the formated string length or an error
 */
int
flow_field_action_gport_terminator_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_gport_t *action_gport = (bcm_gport_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", *action_gport);

}

static shr_error_e
dnx_flow_field_action_gport_terminator_verify(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_terminator_info_t * terminator_info)
{
    uint32 action_gport = terminator_info->action_gport;
    int trap_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_MIRROR_MIRROR(action_gport))
    {
        /** Mirror valid case, no need for additional checks */
        SHR_EXIT();
    }

    if (!BCM_GPORT_IS_TRAP(action_gport) && (action_gport != BCM_GPORT_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The Trap-Gport is not valid");
    }

    if ((action_gport != BCM_GPORT_INVALID)
        && (BCM_GPORT_TRAP_GET_STRENGTH(action_gport) != 0) && (BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(action_gport) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The Trap-Gport is not valid, "
                     "Snoop and Forwarding strengths cannot be both set to non-zero values.");
    }

    if (action_gport != BCM_GPORT_INVALID)
    {
        dnx_rx_trap_block_e trap_block = DNX_RX_TRAP_BLOCK_INVALID;
        trap_id = BCM_GPORT_TRAP_GET_ID(action_gport);
        trap_block = DNX_RX_TRAP_ID_BLOCK_GET(trap_id);

        if (trap_block != DNX_RX_TRAP_BLOCK_INGRESS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For lif types InLif and InRif, an IRPP trap_id must be provided! "
                         "The supplied trap_id 0x%08x is on trap_block=%d!", trap_id, trap_block);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_terminator_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 new_lif_trap_profile = 0;
    bcm_flow_terminator_info_t *terminator_info = ((bcm_flow_terminator_info_t *) field_data);

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_field_action_gport_terminator_verify(unit, flow_app_type, terminator_info));

    /** in case of replace, this means, we are in re-setting old value and thus no need to allocate profile */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_get(unit, &terminator_info->action_gport,
                                                                (int *) &new_lif_trap_profile));
    }
    else
    {
        uint8 is_first = 0, is_last = 0;
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.exchange
                        (unit, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE, &terminator_info->action_gport,
                         0, NULL, (int *) &new_lif_trap_profile, &is_first, &is_last));

        /** If this is the first time the lif trap profile is allocated, set it in HW */
        if (is_first)
        {
            dnx_rx_trap_action_profile_t trap_action_profile;

            trap_action_profile.profile = BCM_GPORT_TRAP_GET_ID(terminator_info->action_gport);
            trap_action_profile.fwd_strength = BCM_GPORT_TRAP_GET_STRENGTH(terminator_info->action_gport);
            trap_action_profile.snp_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(terminator_info->action_gport);

            SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_action_profile_hw_set
                            (unit, new_lif_trap_profile, &trap_action_profile));
        }
    }

    /** Set the LIF trap profile */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE,
                                 new_lif_trap_profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_terminator_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = ((bcm_flow_terminator_info_t *) field_data);
    uint8 is_set = FALSE;
    uint32 lif_trap_profile;
    int ref_count;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get(unit, entry_handle_id,
                                                                       DBAL_FIELD_ACTION_PROFILE_IDX,
                                                                       &is_set, &lif_trap_profile));

    if (is_set == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.profile_data_get(unit, lif_trap_profile, &ref_count,
                                                                 &terminator_info->action_gport));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_terminator_replace(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = ((bcm_flow_terminator_info_t *) field_data);
    uint8 is_set, is_first = 0, is_last = 0;
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;
    dnx_rx_trap_action_profile_t trap_action_profile;
    bcm_gport_t action_gport;

    SHR_FUNC_INIT_VARS(unit);

    /** Get previous profile */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get(unit, get_entry_handle_id,
                                                                       DBAL_FIELD_ACTION_PROFILE_IDX,
                                                                       &is_set, &old_lif_trap_profile));

    /** In case action gport is marked as valid_elements_clear - clear it */
    action_gport =
        (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID)) ?
        terminator_info->action_gport : BCM_GPORT_INVALID;

    /** Exchange profiles */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.exchange
                    (unit, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE, &action_gport,
                     old_lif_trap_profile, NULL, (int *) &new_lif_trap_profile, &is_first, &is_last));

    /** If this is the last trap pointing to this lif trap profile, clear profile from HW */
    if (is_last)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_action_profile_hw_clear(unit, old_lif_trap_profile));
    }

    /** If this is the first time the lif trap profile is allocated, set it in HW */
    if (is_first)
    {
        trap_action_profile.profile = BCM_GPORT_TRAP_GET_ID(action_gport);
        trap_action_profile.fwd_strength = BCM_GPORT_TRAP_GET_STRENGTH(action_gport);
        trap_action_profile.snp_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(action_gport);

        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_action_profile_hw_set(unit, new_lif_trap_profile,
                                                                      &trap_action_profile));
    }

    /** In case field is not marked as valid_elements_clear - write to HW */
    if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        /** Set the LIF trap profile */
        dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE,
                                     new_lif_trap_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_terminator_delete(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint8 is_set;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated).
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw(unit, flow_handle_info, gport_hw_resources,
                                                         DBAL_FIELD_ACTION_PROFILE_IDX, &is_set));
    if (is_set)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_profile_set
                        (unit, flow_handle_info->flow_id, gport_hw_resources, BCM_GPORT_INVALID));
    }

exit:
    SHR_FUNC_EXIT;
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_action_gport_terminator_set, dnx_flow_field_action_gport_terminator_get,
     dnx_flow_field_action_gport_terminator_replace, dnx_flow_field_action_gport_terminator_delete},
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
/*
 * Print call back function to display the vale of FLOW field action_gport
 * Return the formated string length or an error
 */
int
flow_field_action_gport_initiator_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_gport_t *action_gport = (bcm_gport_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", *action_gport);

}

static shr_error_e
dnx_flow_field_action_gport_initiator_verify(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_initiator_info_t * initiator_info)
{
    uint32 action_gport = initiator_info->action_gport;
    int trap_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_MIRROR_MIRROR(action_gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Mirror configuration can only be done for ETPP lif traps");
    }

    if (!BCM_GPORT_IS_TRAP(action_gport) && (action_gport != BCM_GPORT_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The Trap-Gport is not valid");
    }

    if ((action_gport != BCM_GPORT_INVALID)
        && (BCM_GPORT_TRAP_GET_STRENGTH(action_gport) != 0) && (BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(action_gport) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The Trap-Gport is not valid, "
                     "Snoop and Forwarding strengths cannot be both set to non-zero values.");
    }

    if (action_gport != BCM_GPORT_INVALID)
    {
        dnx_rx_trap_block_e trap_block = DNX_RX_TRAP_BLOCK_INVALID;
        trap_id = BCM_GPORT_TRAP_GET_ID(action_gport);
        trap_block = DNX_RX_TRAP_ID_BLOCK_GET(trap_id);

        if (trap_block != DNX_RX_TRAP_BLOCK_ETPP)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For lif types OutLif and OutRif, an ETPP trap_id must be provided! "
                         "The supplied trap_id 0x%08x is on trap_block=%d!", trap_id, trap_block);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_initiator_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 lif_trap_profile = 0;
    bcm_flow_initiator_info_t *initiator_info = ((bcm_flow_initiator_info_t *) field_data);

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_field_action_gport_initiator_verify(unit, flow_app_type, initiator_info));

    /** in case of replace, this means, we are in re-setting old value and thus no need to allocate profile */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_get(unit, &initiator_info->action_gport,
                                                             (int *) &lif_trap_profile));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update
                        (unit, 0, initiator_info->action_gport, &lif_trap_profile));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, lif_trap_profile);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_initiator_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = ((bcm_flow_initiator_info_t *) field_data);
    uint8 is_set = FALSE;
    uint32 lif_trap_profile;
    int ref_count;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get(unit, entry_handle_id,
                                                                       DBAL_FIELD_ACTION_PROFILE,
                                                                       &is_set, &lif_trap_profile));

    if (is_set == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.profile_data_get(unit, lif_trap_profile, &ref_count,
                                                              &initiator_info->action_gport));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_initiator_replace(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = ((bcm_flow_initiator_info_t *) field_data);
    bcm_gport_t action_gport;
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;
    uint8 is_set;

    SHR_FUNC_INIT_VARS(unit);

    /** In case action gport is marked as valid_elements_clear - set it as invalid */
    action_gport =
        (!_SHR_IS_FLAG_SET(initiator_info->valid_elements_clear, BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID)) ?
        initiator_info->action_gport : BCM_GPORT_INVALID;

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get(unit, get_entry_handle_id,
                                                                       DBAL_FIELD_ACTION_PROFILE,
                                                                       &is_set, &old_lif_trap_profile));

    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update
                    (unit, old_lif_trap_profile, action_gport, &new_lif_trap_profile));

    /** Update hw only if relevant */
    if (!_SHR_IS_FLAG_SET(initiator_info->valid_elements_clear, BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE,
                                     new_lif_trap_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_initiator_delete(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint8 is_set;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated).
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw(unit, flow_handle_info, gport_hw_resources,
                                                         DBAL_FIELD_ACTION_PROFILE, &is_set));

    if (is_set)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_profile_lif_set
                        (unit, flow_handle_info->flow_id, gport_hw_resources, BCM_GPORT_INVALID));
    }

exit:
    SHR_FUNC_EXIT;
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_action_gport_initiator_set, dnx_flow_field_action_gport_initiator_get,
     dnx_flow_field_action_gport_initiator_replace, dnx_flow_field_action_gport_initiator_delete},
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_mtu_profile_set, dnx_flow_field_mtu_profile_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              MTU_PROFILE - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              QOS_MAP_ID - START
 ******************************************************************************************************/
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
        qos_map_id = DNX_QOS_INITIAL_MAP_ID;

        FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, &qos_map_id));

        if (qos_map_id != DNX_QOS_INITIAL_MAP_ID)
        {
            DNX_QOS_REMARK_MAP_SET(qos_map_id);
            DNX_QOS_EGRESS_MAP_SET(qos_map_id);
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
    "REMARK_PROFILE",
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_remark_profile_set, dnx_flow_field_remark_profile_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
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

/*
 * Print call back function to display the vale of FLOW field qos_egress_model
 * Return the formated string length or an error
 */
int
flow_field_qos_egress_model_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_qos_egress_model_t *qos_model = (bcm_qos_egress_model_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "QOS: %s, TTL: %s, ECN: %s",
                        flow_field_string_qos_egress_model_type[qos_model->egress_qos],
                        flow_field_string_qos_egress_model_type[qos_model->egress_ttl],
                        flow_field_string_qos_egress_model_type[qos_model->egress_ecn]);

}

shr_error_e
dnx_flow_field_qos_egress_model_set(
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
dnx_flow_field_qos_egress_model_get(
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
     * field_desc
     */
    dnx_flow_field_qos_egress_model_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_qos_egress_model_set, dnx_flow_field_qos_egress_model_get, NULL, NULL},
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
    void *val,
    char *print_string)
{
    uint64 *additional_data = (uint64 *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", COMPILER_64_LO(*additional_data));

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
     * field_desc
     */
    dnx_flow_field_additional_data_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_additional_data_set, dnx_flow_field_additional_data_get, NULL, NULL},
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
     * field_desc
     */
    dnx_flow_field_term_class_id_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_term_class_id_set, dnx_flow_field_term_class_id_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              TERMINATOR_CLASS_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              INITIATOR_CLASS_ID - START
 ******************************************************************************************************/
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
     * field_desc
     */
    dnx_flow_field_init_class_id_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_init_class_id_set, dnx_flow_field_init_class_id_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              INITIATOR_CLASS_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              FIELD_CLASS_ID - START
 ******************************************************************************************************/

const dnx_flow_common_field_desc_t dnx_flow_common_field_field_class_id = {
    /*
     * field_name
     */
    "FIELD_CLASS_ID",
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
    FALSE,
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {NULL, NULL, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              FIELD_CLASS_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              FAILOVER_ID - START
 ******************************************************************************************************/

const char *
field_desc_failover_id_cb(
    int unit)
{
    return "Failover object index";
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

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        /*
         * Convert from a failover ID to a HW Protection-Pointer encoded value
         */
        failover_id = ((bcm_flow_terminator_info_t *) field_data)->failover_id;
        BCM_FAILOVER_ID_GET(failover_id, failover_id);
        DNX_FAILOVER_ID_PROTECTION_POINTER_ENCODE(protection_pointer_encoded[0], failover_id);
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        failover_id = ((bcm_flow_initiator_info_t *) field_data)->failover_id;
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
     * field_desc
     */
    field_desc_failover_id_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_failover_id_set, dnx_flow_field_failover_id_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              FAILOVER_ID - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              FAILOVER_STATE - START
 ******************************************************************************************************/
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
     * field_desc
     */
    field_desc_failover_state_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_failover_state_set, dnx_flow_field_failover_state_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              FAILOVER_STATE - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              LEARN_INFO - START
 ******************************************************************************************************/
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

    if (!(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_IGNORE_L2_LEARN_INFO_SET)))
    {
        if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPVX_TERMINATOR, FLOW_STR_MAX))
        {
            SHR_IF_ERR_EXIT(dnx_flow_field_ipvx_term_l2_learn_info_set(unit, entry_handle_id, l2_learn_info));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Generic support for the l2_learn_info field was not implemented, see FLOW_APP_TERM_IND_IGNORE_L2_LEARN_INFO_SET indication");
        }
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
    uint8 is_field_on_handle, is_dest_decoded = FALSE, is_dest_only = FALSE;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dbal_enum_value_field_learn_payload_context_e learn_context;
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_FIELD_GET_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE, &learn_context));
    /*
     * Retrieve the learn dest-port and flush-group information according to the 
     * existing learn-info struct on the handle. 
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, &is_field_on_handle,
                     &dbal_struct_data));

    if (is_field_on_handle == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_DESTINATION,
                         &forward_info.destination, &dbal_struct_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_FLUSH_GROUP,
                         &(terminator_info->l2_learn_info.flush_group), &dbal_struct_data));
        is_dest_decoded = TRUE;
        is_dest_only = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, &is_field_on_handle,
                         &dbal_struct_data));

        if (is_field_on_handle == TRUE)
        {
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, DBAL_FIELD_DESTINATION,
                             &forward_info.destination, &dbal_struct_data));
            is_dest_decoded = TRUE;
            is_dest_only = TRUE;
        }
        else
        {
            uint8 is_set = FALSE;

            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_field_is_set_in_hw
                            (unit, flow_handle_info, gport_hw_resources, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT,
                             &is_set));

            if (is_set == TRUE)
            {
                uint32 dbal_struct_arr[BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE] = { 0 };
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, INST_SINGLE,
                                 dbal_struct_arr));

                /** even if the field id is ASYM_LIF_BASIC_EXT, it might be encoded as symmetric lif  depend of the context */
                if (learn_context == DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC_SVL)
                {
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                    (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, DBAL_FIELD_DESTINATION,
                                     &forward_info.destination, dbal_struct_arr));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                    (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_DESTINATION,
                                     &forward_info.destination, dbal_struct_arr));
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                    (unit, DBAL_FIELD_LEARN_INFO_ASYM_LIF_BASIC_EXT, DBAL_FIELD_GLOB_OUT_LIF,
                                     (uint32 *) &(terminator_info->l2_learn_info.encap_id), dbal_struct_arr));
                }

                is_dest_decoded = TRUE;
            }
        }
    }

    /*
     * Convert the HW encoded destination to a physical gport that represents the destination 
     */
    if (is_dest_decoded)
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, forward_info.destination,
                         &(terminator_info->l2_learn_info.dest_port)));
        /** flag will be added just in case of FEC, and need to distinguish between FEC with LIF and without LIF */
        if (_SHR_GPORT_IS_FORWARD_PORT(terminator_info->l2_learn_info.dest_port) && is_dest_only)
        {
            terminator_info->l2_learn_info.l2_learn_info_flags |= BCM_FLOW_L2_LEARN_INFO_DEST_ONLY;
        }
    }

    /*
     * Retrieve the Optimized flag according to the selected learn-payload-context
     */
    if ((learn_context != DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED_SVL) &&
        (learn_context != DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED))
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

    /** for FLOW_APP_NAME_IPVX_TERMINATOR, we don't need to do anything */
    if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_IPVX_TERMINATOR, FLOW_STR_MAX))
    {
        SHR_EXIT();
    }

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
    FALSE,
    /*
     * field_desc
     */
    field_desc_l2_learn_info_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_l2_learn_info_set, dnx_flow_field_l2_learn_info_get, NULL, dnx_flow_field_l2_learn_info_delete},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              LEARN_INFO - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              LEARN_ENABLE - START
 ******************************************************************************************************/
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
     * field_desc
     */
    field_desc_learn_enable_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_learn_enable_set, dnx_flow_field_learn_enable_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              LEARN_ENABLE - END
 ******************************************************************************************************/

/******************************************************************************************************
 *                                              TPID_CLASS_ID - START
 ******************************************************************************************************/

shr_error_e
dnx_flow_field_tpid_class_id_set(
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

    tpid_class_id = ((bcm_flow_initiator_info_t *) (field_data))->tpid_class_id;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LLVP_PROFILE, INST_SINGLE, tpid_class_id);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_tpid_class_id_get(
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

const dnx_flow_common_field_desc_t dnx_flow_common_field_tpid_class_id = {
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
    FALSE,
    /*
     * field_desc
     */
    field_desc_tpid_class_id_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_tpid_class_id_set, dnx_flow_field_tpid_class_id_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};
/******************************************************************************************************
 *                                              TPID_CLASS_ID - END
 ******************************************************************************************************/
