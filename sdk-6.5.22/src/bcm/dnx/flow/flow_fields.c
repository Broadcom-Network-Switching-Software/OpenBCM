
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
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>

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
    ""
};

static const char *flow_field_string_qos_ingress_model_type[] = {
    "Invalid",  /* bcmQosIngressModelInvalid */
    "Shortpipe",        /* bcmQosIngressModelShortpipe */
    "Pipe",     /* bcmQosIngressModelPipe */
    "Uniform",  /* bcmQosIngressModelUniform */
    "Stuck",    /* bcmQosIngressModelStuck */
    ""
};

static const char *flow_field_string_encap_access[] = {
    "Invalid",  /* bcmEncapAccessInvalid */
    "Tunnel-1", /* bcmEncapAccessTunnel1 */
    "Tunnel-2", /* bcmEncapAccessTunnel2 */
    "Tunnel-3", /* bcmEncapAccessTunnel3 */
    "Tunnel-4", /* bcmEncapAccessTunnel4 */
    "Native-Arp",       /* bcmEncapAccessNativeArp */
    "Native-Ac",        /* bcmEncapAccessNativeAc */
    "Arp",      /* bcmEncapAccessArp */
    "Rif",      /* bcmEncapAccessRif */
    ""
};

static const char *flow_field_string_qos_egress_model_type[] = {
    "Uniform",  /* bcmQosEgressModelUniform */
    "PipeNextNameSpace",        /* bcmQosEgressModelPipeNextNameSpace */
    "PipeMyNameSpace",  /* bcmQosEgressModelPipeMyNameSpace */
    "Initial",  /* bcmQosEgressModelInitial */
    ""
};

/*
 * Get 32 bits field val, set 0 if not set on handle
 */
shr_error_e
dnx_flow_field_value32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    *field_val = 0;
    /** Try to read value, if succeeded return found */
    rv = dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    SHR_SET_CURRENT_ERR(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Get 8 bits field val, set 0 if not set on handle
 */
shr_error_e
dnx_flow_field_value8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    *field_val = 0;
    /** Try to read value, if succeeded return found */
    rv = dbal_entry_handle_value_field8_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    SHR_SET_CURRENT_ERR(rv);

exit:
    SHR_FUNC_EXIT;
}

/******************************************************************************************************
 *                                              VSI - START
 */

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

    SHR_FUNC_INIT_VARS(unit);

    terminator_info->vsi = 0;
    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_VSI, &vsi));

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
 */

/******************************************************************************************************
 *                                              VRF - START
 */
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

    SHR_FUNC_INIT_VARS(unit);

    terminator_info->vrf = 0;
    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_VRF, &vrf));

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
 */

/******************************************************************************************************
 *                                              STAT_ID - START
 */

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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &stat_id));

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
 */

/******************************************************************************************************
 *                                              L2_INGRESS_INFO - START
 */

const char *
field_desc_l2_ingress_info_cb(
    int unit)
{
    return "L2 Ingress info";
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
    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, get_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, in_lif_profile));

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
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dbal_tables_e dbal_table_id;
    dnx_in_lif_profile_last_info_t last_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    /** Allocate default inLIF profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    in_lif_profile_info.egress_fields.in_lif_orientation = terminator_info->l2_ingress_info.ingress_network_group_id;

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     dbal_table_id, &last_profile));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_in_lif_profile);

exit:
    DBAL_FUNC_FREE_VARS;
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
    in_lif_profile_info_t in_lif_profile_info;
    uint32 in_lif_profile = 0;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit,
                                                      entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, &in_lif_profile));

    /*
     * get in_lif_profile data and store it in in_lif_profile_info
     */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));

    terminator_info->l2_ingress_info.ingress_network_group_id = in_lif_profile_info.egress_fields.in_lif_orientation;

exit:
    SHR_FUNC_EXIT;
}

/* Replcae callback for l2 ingress info. Replace profile, if needed - remove the old one */
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
    DBAL_FUNC_INIT_VARS(unit);
    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    /** Get entry from inLIF table */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get
                              (unit, get_entry_handle_id, (uint32 *) &old_in_lif_profile), _SHR_E_NOT_FOUND);

    /*
     * In case of in lif profile field marked with valid elements clear - restore to default profile 
     */
    if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID))
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info, LIF));
        in_lif_profile_info.egress_fields.in_lif_orientation =
            terminator_info->l2_ingress_info.ingress_network_group_id;
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                         gport_hw_resources->inlif_dbal_table_id, &last_profile));
    }

    dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_in_lif_profile);

exit:
    DBAL_FUNC_FREE_VARS;
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

    SHR_FUNC_INIT_VARS(unit);

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif,
                     gport_hw_resources->inlif_dbal_table_id, TRUE, DBAL_FIELD_IN_LIF_PROFILE, &is_set));
    if (is_set)
    {
        /** Get entry from inLIF table */
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get(unit, entry_handle_id, &in_lif_profile),
                                  _SHR_E_NOT_FOUND);
        /** Deallocate in_lif_profile */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, LIF));
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
     dnx_flow_field_l2_ingress_info_delete},    /* 
                                                 * field_print
                                                 */
    NULL
};

/******************************************************************************************************
 *                                              L2_INGRESS_INFO - END
 */

/******************************************************************************************************
 *                                              L2_EGRESS_INFO - START
 */

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
    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, get_entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, out_lif_profile));

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
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    /*
     * Format the data for the Out-LIF profile 
     * Note: Future additional subfields should be supprted here
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
    DBAL_FUNC_FREE_VARS;
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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit,
                                                      entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE,
                                                      (uint32 *) &out_lif_profile));

    /*
     * Retrieve the data for the Out-LIF Profile
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));
    initiator_info->l2_egress_info.egress_network_group_id = out_lif_profile_info.out_lif_orientation;

exit:
    SHR_FUNC_EXIT;
}

/* 
 * Replcae callback for l2 egress info
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
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the Out-LIF Profile value
     */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_initiator_out_lif_profile_get(unit,
                                                                     get_entry_handle_id,
                                                                     (uint32 *) &old_out_lif_profile),
                              _SHR_E_NOT_FOUND);

    /*
     * In case the Out-LIF profile field is marked with valid elements clear - restore to default profile 
     */
    if (!_SHR_IS_FLAG_SET(initiator_info->valid_elements_clear, BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID))
    {
        etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
        SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, old_out_lif_profile, &out_lif_profile_info));
        out_lif_profile_info.out_lif_orientation = initiator_info->l2_egress_info.egress_network_group_id;
        SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange(unit,
                                                          gport_hw_resources->outlif_dbal_table_id, 0,
                                                          &out_lif_profile_info, old_out_lif_profile,
                                                          &new_out_lif_profile, &is_last));
    }

    dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_out_lif_profile);

exit:
    DBAL_FUNC_FREE_VARS;
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
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_out_lif,
                     gport_hw_resources->outlif_dbal_table_id, FALSE, DBAL_FIELD_OUT_LIF_PROFILE, &is_set));
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
        if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                         DNX_OUT_LIF_PROFILE_DEFAULT, new_out_lif_profile);
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
     dnx_flow_field_l2_egress_info_delete},     /* 
                                                 * field_print
                                                 */
    NULL
};

/******************************************************************************************************
 *                                              L2_EGRESS_INFO - END
 */

/******************************************************************************************************
 *                                              L3_INGRESS_INFO - START
 */
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
    bcm_l3_ingress_urpf_mode_t *urpf_mode = (bcm_l3_ingress_urpf_mode_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "%s", flow_field_string_urpf_mode[*urpf_mode]);
}

/*
 * Desc cb
 */
const char *
flow_field_l3_ingress_info_desc(
    int unit)
{
    return "L3 ingress info. Profile existance is not validated";
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
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dbal_tables_e dbal_table_id;
    dnx_in_lif_profile_last_info_t last_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));

    /** Allocate default inLIF profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    in_lif_profile_info.ingress_fields.urpf_mode = terminator_info->l3_ingress_info.urpf_mode;

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     dbal_table_id, &last_profile));

    old_in_lif_profile = new_in_lif_profile;

    if (terminator_info->service_type == bcmFlowServiceTypeCrossConnect)
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                         dbal_table_id, &last_profile));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_in_lif_profile);

exit:
    DBAL_FUNC_FREE_VARS;
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
    in_lif_profile_info_t in_lif_profile_info;
    uint32 in_lif_profile = 0;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit,
                                                      entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, &in_lif_profile));

    /*
     * get in_lif_profile data and store it in in_lif_profile_info
     */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));

    terminator_info->l3_ingress_info.urpf_mode = in_lif_profile_info.ingress_fields.urpf_mode;

exit:
    SHR_FUNC_EXIT;
}

/* Replcae callback for l3 ingress info. Replace profile, if needed - remove the old one */
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
    DBAL_FUNC_INIT_VARS(unit);
    dnx_in_lif_profile_last_info_t_init(&last_profile);
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    /** Get entry from inLIF table */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get
                              (unit, get_entry_handle_id, (uint32 *) &old_in_lif_profile), _SHR_E_NOT_FOUND);

    /*
     * In case of in lif profile field marked with valid elements clear - restore to default profile 
     */
    if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID))
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info, LIF));
        in_lif_profile_info.ingress_fields.urpf_mode = terminator_info->l3_ingress_info.urpf_mode;
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                         gport_hw_resources->inlif_dbal_table_id, &last_profile));
    }

    dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                 (uint32) new_in_lif_profile);

exit:
    DBAL_FUNC_FREE_VARS;
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

    SHR_FUNC_INIT_VARS(unit);

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif,
                     gport_hw_resources->inlif_dbal_table_id, TRUE, DBAL_FIELD_IN_LIF_PROFILE, &is_set));
    if (is_set)
    {
        /** Get entry from inLIF table */
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get(unit, entry_handle_id, &in_lif_profile),
                                  _SHR_E_NOT_FOUND);
        /** Deallocate in_lif_profile */
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, LIF));
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
 */

/******************************************************************************************************
 *                                              STAT_PP_PROFILE - START
 */

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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, &stat_pp_profile));

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
 */

/******************************************************************************************************
 *                                              QOS_INGRESS_MODEL - START
 */
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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, get_entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, &dbal_field));
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
    remark_propagation_type = ecn_propagation_type = ttl_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE))
    {
        phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }
    else
    {
        phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
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

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, &dbal_field));

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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_lif_table_mngr_is_valid_field
                           (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif,
                            gport_hw_resources->inlif_dbal_table_id, TRUE, DBAL_FIELD_ECN_MAPPING_PROFILE, &is_valid));

    if (is_valid)
    {
        uint32 ecn_mapping_profile;

        SHR_IF_ERR_EXIT(dnx_flow_field_value32_get
                        (unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, &ecn_mapping_profile));

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

/* Replcae callback for qos ingress model. Replace profile, if needed - remove the old one */
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
    DBAL_FUNC_INIT_VARS(unit);

    /** Set QOS */
    remark_propagation_type = ecn_propagation_type = ttl_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE))
    {
        phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    }
    else
    {
        phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
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
        dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
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
    DBAL_FUNC_FREE_VARS;
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
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif,
                     gport_hw_resources->inlif_dbal_table_id, TRUE, DBAL_FIELD_PROPAGATION_PROF, &is_set));
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
 */

/******************************************************************************************************
 *                                              SERVICE_TYPE - START
 */
/*
 * Print call back function to display the vale of FLOW field service_type 
 * Return the formated string length or an error
 */
int
flow_field_service_type_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_flow_service_type_t *service_type = (bcm_flow_service_type_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "%s", flow_field_string_service_type[*service_type]);

}

shr_error_e
dnx_flow_field_service_type_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 service_type_value = 0;
    dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type = 0;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    if (terminator_info->service_type >= bcmFlowServiceTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Service type configured %d, max value is %d", terminator_info->service_type,
                     bcmFlowServiceTypeCount);
    }

    if (terminator_info->service_type == bcmFlowServiceTypeCrossConnect)
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

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_service_type_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 dbal_field = 0;
    dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type = 0;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, &dbal_field));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_TYPE, &vtt_lif_service_type, &dbal_field));
    if (vtt_lif_service_type == DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P)
    {
        terminator_info->service_type |= bcmFlowServiceTypeCrossConnect;
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_flow_service_type_info = {
    /*
     * field_name
     */
    "SERVICE_TYPE",
    /*
     * term_field_enabler
     */
    BCM_FLOW_TERMINATOR_ELEMENT_FLOW_SERVICE_TYPE_VALID,
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
    {dnx_flow_field_service_type_set, dnx_flow_field_service_type_get, NULL, NULL},
    /*
     * field_print
     */
    flow_field_service_type_print_cb
};

/******************************************************************************************************
 *                                              SERVICE_TYPE - END
 */

/******************************************************************************************************
 *                                              DEST_INFO - START
 */
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
    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                 BCM_GPORT_FORWARD_PORT_GET(terminator_info->dest_info.dest_port));
    if ((terminator_info->dest_info.dest_encap != BCM_FORWARD_ENCAP_ID_INVALID)
        && (terminator_info->dest_info.dest_encap != 0))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                     terminator_info->dest_info.dest_encap);
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
    uint32 dest, default_dest;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, &dest));

   /** Get field default value */
    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get
                    (unit, entry_handle_id, DBAL_FIELD_DESTINATION, 0, DBAL_PREDEF_VAL_DEFAULT_VALUE, &default_dest));

    /*
     * If default value received
     */
    if (dest == default_dest)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_EXIT();
    }

    BCM_GPORT_FORWARD_PORT_SET(terminator_info->dest_info.dest_port, dest);

    SHR_IF_ERR_EXIT(dnx_flow_field_value32_get
                    (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF,
                     (uint32 *) &terminator_info->dest_info.dest_encap));

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
 */

/******************************************************************************************************
 *                                              L3_INTF_ID - START
 */
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

    /** Get next outlif if exist */
    if ((initiator_info->l3_intf_id != 0) && (initiator_info->l3_intf_id != BCM_IF_INVALID))
    {
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport_tunnel, initiator_info->l3_intf_id);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gport_tunnel, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                         &tunnel_gport_hw_resources));
        /** Next outlif */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     tunnel_gport_hw_resources.local_out_lif);
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

    /** Get next outLIF pointer */
    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, (uint32 *) &next_outlif));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL,
                                                next_outlif, &gport));
    gport = dnx_flow_gport_to_flow_id(unit, gport);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(initiator_info->l3_intf_id, gport);

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
 */

/******************************************************************************************************
 *                                              ENCAP_ACCESS - START
 */
/*
 * Print call back function to display the value of FLOW field encap_access 
 * Return the formated string length or an error 
 */
shr_error_e
flow_field_encap_access_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_encap_access_t *encap_access = (bcm_encap_access_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "%s", flow_field_string_encap_access[*encap_access]);
}

/*
 * Desc cb, encap access limitations
 */
const char *
dnx_flow_field_encap_access_desc(
    int unit)
{
    return
        "Encap access is a mandatory field for initiator, cannot be replaced once set. Set cb is not called, done from initiator API internal functions";
}

/* Empty, encap access set is done in flow.c directly */
shr_error_e
dnx_flow_field_encap_access_set(
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
dnx_flow_field_encap_access_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    dnx_algo_local_outlif_logical_phase_e logical_phase;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                    (unit, gport_hw_resources->local_out_lif, NULL, NULL, &logical_phase, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_lif_lib_logical_phase_to_encap_access_convert(unit,
                                                                      logical_phase, &(initiator_info->encap_access)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_encap_access = {
    /*
     * field_name
     */
    "ENCAP_ACCESS",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * field_desc
     */
    dnx_flow_field_encap_access_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_encap_access_set, dnx_flow_field_encap_access_get, NULL, NULL},
    /*
     * field_print
     */
    flow_field_encap_access_print_cb
};

/******************************************************************************************************
 *                                              ENCAP_ACCESS - END
 */

/******************************************************************************************************
 *                                              ACTION_GPORT - START
 */
/*
 * Print call back function to display the vale of FLOW field action_gport
 * Return the formated string length or an error
 */
int
flow_field_action_gport_print_cb(
    int unit,
    void *val,
    char *print_string)
{
    bcm_gport_t *action_gport = (bcm_gport_t *) val;

    return sal_snprintf(print_string, FLOW_STR_MAX, "0x%x", *action_gport);

}

static shr_error_e
dnx_flow_field_action_gport_verify(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *field_data)
{
    uint32 action_gport = 0;
    int trap_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        action_gport = ((bcm_flow_terminator_info_t *) field_data)->action_gport;
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        action_gport = ((bcm_flow_initiator_info_t *) field_data)->action_gport;
    }

    if (BCM_GPORT_IS_MIRROR_MIRROR(action_gport))
    {
        if (flow_app_type == FLOW_APP_TYPE_INIT)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Mirror configuration can only be done for ETPP lif traps");
        }
        /** Mirror valid case, no need fo adittional checks */
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

        if ((flow_app_type == FLOW_APP_TYPE_TERM) && (trap_block != DNX_RX_TRAP_BLOCK_INGRESS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For lif types InLif and InRif, an IRPP trap_id must be provided! "
                         "The supplied trap_id 0x%08x is on trap_block=%d!", trap_id, trap_block);
        }

        if ((flow_app_type == FLOW_APP_TYPE_INIT) && (trap_block != DNX_RX_TRAP_BLOCK_ETPP))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For lif types OutLif and OutRif, an ETPP trap_id must be provided! "
                         "The supplied trap_id 0x%08x is on trap_block=%d!", trap_id, trap_block);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 lif_trap_profile = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_flow_field_action_gport_verify(unit, flow_app_type, field_data));

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        uint8 is_first = 0, is_last = 0;
        uint32 new_lif_trap_profile = 0;
        dnx_rx_trap_action_profile_t trap_action_profile;
        bcm_flow_terminator_info_t *termination_info = (bcm_flow_terminator_info_t *) field_data;
        SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.exchange
                        (unit, DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE, &termination_info->action_gport,
                         0, NULL, (int *) &new_lif_trap_profile, &is_first, &is_last));

        /** If this is the first time the lif trap profile is allocated, set it in HW */
        if (is_first)
        {
            trap_action_profile.profile = BCM_GPORT_TRAP_GET_ID(termination_info->action_gport);
            trap_action_profile.fwd_strength = BCM_GPORT_TRAP_GET_STRENGTH(termination_info->action_gport);
            trap_action_profile.snp_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(termination_info->action_gport);

            SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_action_profile_hw_set(unit, new_lif_trap_profile,
                                                                          &trap_action_profile));
        }

        /** Set the LIF trap profile */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE,
                                     new_lif_trap_profile);
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update
                        (unit, 0, initiator_info->action_gport, &lif_trap_profile));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, lif_trap_profile);
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info;
    bcm_flow_initiator_info_t *initiator_info;
    uint32 action_gport = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                               (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, &action_gport));
        terminator_info = (bcm_flow_terminator_info_t *) field_data;
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_profile_get(unit, gport_hw_resources, &terminator_info->action_gport));
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                               (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, &action_gport));
        initiator_info = (bcm_flow_initiator_info_t *) field_data;
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_profile_lif_get
                        (unit, gport_hw_resources, &initiator_info->action_gport, gport_hw_resources->local_out_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_terminator_replace(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_terminator_info_t * terminator_info)
{
    uint8 is_first = 0, is_last = 0;
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;
    dnx_rx_trap_action_profile_t trap_action_profile;
    bcm_gport_t action_gport;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get previous profile 
     */
    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_profile_hw_get(unit,
                                                           gport_hw_resources->inlif_dbal_table_id,
                                                           gport_hw_resources->inlif_dbal_result_type,
                                                           gport_hw_resources->local_in_lif, &old_lif_trap_profile));

    /*
     * In case action gport is marked as valid_elements_clear - clear it
     */
    action_gport =
        (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID)) ?
        terminator_info->action_gport : BCM_GPORT_INVALID;
    /*
     * Exchange profiles 
     */
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

    /*
     * In case field is not marked as valid_elements_clear - write to HW 
     */
    if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        /** Set the LIF trap profile */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE_IDX, INST_SINGLE,
                                     new_lif_trap_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_initiator_replace(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_initiator_info_t * initiator_info)
{
    bcm_gport_t action_gport;
    uint32 old_lif_trap_profile = 0, new_lif_trap_profile = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case action gport is marked as valid_elements_clear - set it as invalid
     */
    action_gport =
        (!_SHR_IS_FLAG_SET(initiator_info->valid_elements_clear, BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID)) ?
        initiator_info->action_gport : BCM_GPORT_INVALID;

    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_profile_lif_hw_get
                    (unit, gport_hw_resources, &old_lif_trap_profile, gport_hw_resources->local_out_lif));

    SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_trap_profile_allocate_and_hw_update
                    (unit, old_lif_trap_profile, action_gport, &new_lif_trap_profile));

    /*
     * Update hw only if relevant 
     */
    if (!_SHR_IS_FLAG_SET(initiator_info->valid_elements_clear, BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE,
                                     new_lif_trap_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_replace(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(dnx_flow_field_action_gport_terminator_replace
                        (unit, set_entry_handle_id, gport_hw_resources, (bcm_flow_terminator_info_t *) field_data));
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_IF_ERR_EXIT(dnx_flow_field_action_gport_initiator_replace
                        (unit, set_entry_handle_id, gport_hw_resources, (bcm_flow_initiator_info_t *) field_data));
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_delete(
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

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        /*
         * A delete CB should be run only in case the field was set at create (resources were allocated). 
         * Check only the necessary fields.
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                        (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif,
                         gport_hw_resources->inlif_dbal_table_id, TRUE, DBAL_FIELD_ACTION_PROFILE_IDX, &is_set));
        if (is_set)
        {
            SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_profile_set(unit, gport_hw_resources, BCM_GPORT_INVALID));
        }
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        /*
         * A delete CB should be run only in case the field was set at create (resources were allocated). 
         * Check only the necessary fields.
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                        (unit, _SHR_CORE_ALL, gport_hw_resources->local_out_lif,
                         gport_hw_resources->outlif_dbal_table_id, FALSE, DBAL_FIELD_ACTION_PROFILE, &is_set));
        if (is_set)
        {
            SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_profile_lif_set
                            (unit, gport_hw_resources, BCM_GPORT_INVALID, gport_hw_resources->local_out_lif));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_action_gport = {
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
    {dnx_flow_field_action_gport_set, dnx_flow_field_action_gport_get, dnx_flow_field_action_gport_replace,
     dnx_flow_field_action_gport_delete},
    /*
     * field_print
     */
    flow_field_action_gport_print_cb
};

/******************************************************************************************************
 *                                              ACTION_GPORT - END
 */

/******************************************************************************************************
 *                                              MTU_PROFILE - START
 */
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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value8_get
                           (unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, &initiator_info->mtu_profile));

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
 */

/******************************************************************************************************
 *                                              QOS_MAP_ID - START
 */
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
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, remark_profile);
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

        SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, &qos_map_id));
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

        SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                               (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, &qos_map_id));

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
 */

/******************************************************************************************************
 *                                              QOS_EGRESS_MODEL - START
 */

/*
 * Desc cb, qos egress model limitations
 */
const char *
dnx_flow_field_qos_egress_model_desc(
    int unit)
{
    return
        "Profile of the qos model.\n\nFor ttl profile - also set:\nBCM_FLOW_INITIATOR_ELEMENT_TTL_VALID.\n\nFor network profile -\nneed to set one of:\nBCM_FLOW_INITIATOR_ELEMENT_DSCP_VALID\nBCM_FLOW_INITIATOR_ELEMENT_EXP_VALID\nBCM_FLOW_INITIATOR_ELEMENT_PKT_PRI_CFI_VALID\naccording to the specific application.\n\nEach of these enablers must be set with:\nBCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID";
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
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    uint8 nwk_qos_idx;
    uint8 nwk_qos_enabled = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_TTL_VALID))
    {
        if (initiator_info->egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_TTL_MODE_PIPE);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_TTL_MODE_UNIFORM);
        }

        /** Allocate TTL model */
        if (initiator_info->egress_qos_model.egress_ttl != bcmQosEgressModelUniform)
        {
            uint8 is_first_ttl_profile_ref;
            uint32 ttl_pipe_profile;

            /** allocate TTL PIPE profile */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.allocate_single
                            (unit, 0, &initiator_info->ttl, NULL, (int *) &ttl_pipe_profile,
                             &is_first_ttl_profile_ref));

            /** Write to HW if new TTL profile was allocated */
            if (is_first_ttl_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set
                                (unit, ttl_pipe_profile, (uint32) initiator_info->ttl));
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE,
                                         ttl_pipe_profile);
        }
    }

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_DSCP_VALID))
    {
        nwk_qos_idx = initiator_info->dscp;
    }
    else if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_EXP_VALID))
    {
        nwk_qos_idx = initiator_info->exp;
    }
    else if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_PKT_PRI_CFI_VALID))
    {
        nwk_qos_idx = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(initiator_info->pkt_pri, initiator_info->pkt_cfi);
    }
    else
    {
        nwk_qos_enabled = FALSE;
    }

    if (nwk_qos_enabled)
    {
        /** QOS PIPE Model */
        if ((initiator_info->egress_qos_model.egress_qos != bcmQosEgressModelUniform))
        {
            uint32 nwk_qos_profile;
            uint8 is_first_pipe_propag_profile_ref;

            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate
                            (unit, nwk_qos_idx, nwk_qos_idx, 0, (int *) &nwk_qos_profile,
                             &is_first_pipe_propag_profile_ref));

            if (is_first_pipe_propag_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                                (unit, nwk_qos_profile, nwk_qos_idx, nwk_qos_idx, 0));
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, nwk_qos_profile);
        }
    }

    SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal(unit, &initiator_info->egress_qos_model, &encap_qos_model));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, encap_qos_model);

exit:
    SHR_FUNC_EXIT;
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
    uint8 is_field_valid;
    uint32 ttl_mode;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    bcm_qos_egress_model_t model;
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    const dnx_flow_app_config_t *flow_app_config;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_TTL_MODE, &is_field_valid, &ttl_mode));
    if (is_field_valid == TRUE)
    {
        /** uniform is default, do nothing*/
        if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
        {
            uint32 ttl_pipe_profile;
            int ref_count = 0;

            initiator_info->egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

            /** Get TTL PIPE profile */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE,
                                                                INST_SINGLE, &ttl_pipe_profile));
            /** get TTL value from template manager */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                            (unit, ttl_pipe_profile, &ref_count, (void *) &(initiator_info->ttl)));
        }
        else
        {
            initiator_info->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        }
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, &is_field_valid, &encap_qos_model));
    if (is_field_valid == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, encap_qos_model, &model));
        initiator_info->egress_qos_model.egress_qos = model.egress_qos;
        initiator_info->egress_qos_model.egress_ecn = model.egress_ecn;

        if ((encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN))
        {
            uint32 qos_model_idx;
            uint8 qos_profile_get_dummy_var;
            uint8 nwk_idx;

            /** Get QOS pipe index */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX,
                                                                INST_SINGLE, &qos_model_idx));

            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_attr_get
                            (unit, qos_model_idx, &nwk_idx, &qos_profile_get_dummy_var, &qos_profile_get_dummy_var));
            flow_app_config = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

            if (_SHR_IS_FLAG_SET(flow_app_config->valid_common_fields_bitmap, BCM_FLOW_INITIATOR_ELEMENT_DSCP_VALID))
            {
                initiator_info->dscp = nwk_idx;
            }
            else if (_SHR_IS_FLAG_SET
                     (flow_app_config->valid_common_fields_bitmap, BCM_FLOW_INITIATOR_ELEMENT_EXP_VALID))
            {
                initiator_info->exp = nwk_idx;
            }
            else if (_SHR_IS_FLAG_SET
                     (flow_app_config->valid_common_fields_bitmap, BCM_FLOW_INITIATOR_ELEMENT_PKT_PRI_CFI_VALID))
            {
                initiator_info->pkt_pri = DNX_QOS_IDX_NWK_QOS_PCP_GET(nwk_idx);
                initiator_info->pkt_cfi = DNX_QOS_IDX_NWK_QOS_CFI_GET(nwk_idx);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "No network qos field supported for app");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_qos_egress_model_replace(
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint8 is_field_valid;
    uint32 ttl_mode;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    bcm_qos_egress_model_t model;
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    uint8 is_pipe_mode_old = FALSE;
    uint8 is_pipe_mode_new = TRUE;
    uint8 remove_old_profile;
    uint32 old_profile, new_profile;
    uint8 is_first_profile_ref = FALSE, is_last_profile_ref = FALSE;
    uint8 nwk_qos_idx = 0;
    uint8 nwk_qos_enabled = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, get_entry_handle_id, DBAL_FIELD_TTL_MODE, &is_field_valid, &ttl_mode));
    if (is_field_valid == TRUE)
    {
        if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
        {
            is_pipe_mode_old = 1;

            /** Get TTL PIPE profile */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, get_entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, &old_profile));
            new_profile = old_profile;
        }
    }

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_TTL_VALID))
    {
        if (initiator_info->egress_qos_model.egress_ttl == bcmQosEgressModelUniform)
        {
            is_pipe_mode_new = FALSE;
        }

        /** Allocate TTL template */
        if (is_pipe_mode_old && is_pipe_mode_new)
        {
            /** In this case the old and new TTL model is PIPE - exchange templates */

            /** Exchange TTL PIPE profile */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.exchange
                            (unit, 0, &initiator_info->ttl, old_profile, NULL,
                             (int *) &new_profile, &is_first_profile_ref, &is_last_profile_ref));

            /** If last profile was deallocated remove the old entry from HW */
            if (is_last_profile_ref)
            {
                remove_old_profile = 1;
            }
        }
        else if (!is_pipe_mode_old && is_pipe_mode_new)
        {
            /** In this case the old model is UNIFORM and new TTL model is PIPE - allocate new profile */

            /** allocate TTL PIPE profile */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.allocate_single
                            (unit, 0, &initiator_info->ttl, NULL, (int *) &new_profile, &is_first_profile_ref));
        }
        else if (is_pipe_mode_old && !is_pipe_mode_new)
        {
            /** In this case the old model is PIPE and new TTL model is UNIFORM - remove old template */

            /** Free old TTL profile template */
            SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.free_single
                            (unit, old_profile, &is_last_profile_ref));

            /** If last profile was deallocated remove the entry from HW */
            if (is_last_profile_ref)
            {
                remove_old_profile = 1;
            }
        }

        /** Write to HW if new TTL profile was allocated */
        if (is_first_profile_ref)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set(unit, new_profile, initiator_info->ttl));
        }
       /** Remove TTL entry from HW */
        if (remove_old_profile)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, old_profile));
        }
        dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, new_profile);
        if (is_pipe_mode_new)
        {
            dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_TTL_MODE_PIPE);
        }
    }
    else
    {
        if (is_field_valid)
        {
            /** Set old TTL PIPE profile */
            dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE,
                                         old_profile);
        }
    }

    if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_DSCP_VALID))
    {
        nwk_qos_idx = initiator_info->dscp;
    }
    else if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_EXP_VALID))
    {
        nwk_qos_idx = initiator_info->exp;
    }
    else if (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, BCM_FLOW_INITIATOR_ELEMENT_PKT_PRI_CFI_VALID))
    {
        nwk_qos_idx = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(initiator_info->pkt_pri, initiator_info->pkt_cfi);
    }
    else
    {
        nwk_qos_enabled = FALSE;
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, get_entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, &is_field_valid, &encap_qos_model));
    if (is_field_valid == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, encap_qos_model, &model));
        initiator_info->egress_qos_model.egress_qos = model.egress_qos;
        initiator_info->egress_qos_model.egress_ecn = model.egress_ecn;

        if ((encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE) ||
            (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN))
        {
            /** Get QOS pipe index */
            is_pipe_mode_old = TRUE;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, get_entry_handle_id, DBAL_FIELD_NWK_QOS_IDX,
                                                                INST_SINGLE, &old_profile));
        }
    }

    if (nwk_qos_enabled)
    {
        /** Exchange QOS idx */
        if (is_pipe_mode_old && is_pipe_mode_new)
        {
            /** In this case the old and new model is PIPE - exchange templates */

            /** Exchange QOS PIPE profile */
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_update
                            (unit, (uint16) nwk_qos_idx, (uint16) nwk_qos_idx, 0, (int *) &new_profile,
                             &is_first_profile_ref, &is_last_profile_ref));
            if (is_first_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                                (unit, new_profile, (uint8) nwk_qos_idx, (uint8) nwk_qos_idx, 0));
            }
        }
        else if (!is_pipe_mode_old && is_pipe_mode_new)
        {
            /** In this case the old model is UNIFORM and new model is PIPE - allocate new profile */

            /** allocate QOS PIPE profile */
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate
                            (unit, (uint8) nwk_qos_idx, (uint8) nwk_qos_idx, 0, (int *) &new_profile,
                             &is_first_profile_ref));
            if (is_first_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                                (unit, new_profile, (uint8) nwk_qos_idx, (uint8) nwk_qos_idx, 0));
            }
        }
        else if (is_pipe_mode_old && !is_pipe_mode_new)
        {
            /** In this case the old model is PIPE and new model is UNIFORM - remove old template */

            /** Free old QOS profile template */
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free
                            (unit, (int) old_profile, &is_last_profile_ref));
        }

        if (is_last_profile_ref)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, old_profile));
        }
        dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, new_profile);
        if (is_pipe_mode_new)
        {
            dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                         encap_qos_model);
        }
    }
    else
    {
        if (is_field_valid)
        {
            dbal_entry_value_field32_set(unit, set_entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, old_profile);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_qos_egress_model_delete(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data)
{
    uint32 ttl_mode;
    uint32 profile;
    uint8 is_last_profile_ref = FALSE, is_set = FALSE;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    bcm_qos_egress_model_t model;
    dbal_table_field_info_t field_info;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_out_lif,
                     gport_hw_resources->outlif_dbal_table_id, FALSE, DBAL_FIELD_TTL_MODE, &is_set));
    if (is_set)
    {
        rv = dbal_tables_field_info_get_no_err(unit, gport_hw_resources->outlif_dbal_table_id, DBAL_FIELD_TTL_MODE, 0,
                                               gport_hw_resources->outlif_dbal_result_type, 0, &field_info);

        if (rv == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, &ttl_mode));

            if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
            {
                /** Get TTL PIPE profile */
                SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                                       (unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, &profile));

                /** Free old TTL profile template */
                SHR_IF_ERR_EXIT(algo_qos_db.
                                qos_egress_ttl_pipe_profile.free_single(unit, profile, &is_last_profile_ref));
            }
            if (is_last_profile_ref)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, profile));
            }
        }
    }

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_out_lif,
                     gport_hw_resources->outlif_dbal_table_id, FALSE, DBAL_FIELD_ENCAP_QOS_MODEL, &is_set));
    if (is_set)
    {
        rv = dbal_tables_field_info_get_no_err(unit, gport_hw_resources->outlif_dbal_table_id,
                                               DBAL_FIELD_ENCAP_QOS_MODEL, 0,
                                               gport_hw_resources->outlif_dbal_result_type, 0, &field_info);

        if (rv == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                                   (unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, &encap_qos_model));
            SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, encap_qos_model, &model));
            if ((encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE) ||
                (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE) ||
                (encap_qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN))
            {
                SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                                       (unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, &profile));

                /** Free old QOS profile template */
                SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free
                                (unit, (int) profile, &is_last_profile_ref));
                if (is_last_profile_ref)
                {
                    SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, profile));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

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
    {dnx_flow_field_qos_egress_model_set, dnx_flow_field_qos_egress_model_get, dnx_flow_field_qos_egress_model_replace,
     dnx_flow_field_qos_egress_model_delete},
    /*
     * field_print
     */
    flow_field_qos_egress_model_print_cb
};
/******************************************************************************************************
 *                                              QOS_EGRESS_MODEL - END
 */

/******************************************************************************************************
 *                                              ADDITIONAL_DATA - START
 */

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
 *                                              ADDITIONAL_DATA - END
 */

/******************************************************************************************************
 *                                              CLASS_ID - START
 */
const char *
dnx_flow_field_class_id_desc(
    int unit)
{
    return "VLAN Domain for the next layer";
}

shr_error_e
dnx_flow_field_class_id_set(
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
dnx_flow_field_class_id_get(
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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, &(terminator_info->class_id)));

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_class_id = {
    /*
     * field_name
     */
    "CLASS_ID",
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
    dnx_flow_field_class_id_desc,
    /*
     * field_cbs
     */
    {dnx_flow_field_class_id_set, dnx_flow_field_class_id_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/******************************************************************************************************
 *                                              CLASS_ID - END
 */

/******************************************************************************************************
 *                                              FIELD_CLASS_ID - START
 */

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
 */

/******************************************************************************************************
 *                                              FAILOVER_ID - START
 */

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

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        failover_id = ((bcm_flow_terminator_info_t *) field_data)->failover_id;
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {

        failover_id = ((bcm_flow_initiator_info_t *) field_data)->failover_id;
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                 (uint32) failover_id);
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

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
    bcm_failover_t failover_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, (uint32 *) &failover_id));

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        ((bcm_flow_terminator_info_t *) field_data)->failover_id = failover_id;
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        ((bcm_flow_initiator_info_t *) field_data)->failover_id = failover_id;
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
 */

/******************************************************************************************************
 *                                              FAILOVER_STATE - START
 */
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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, &failover_state));

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
 */

/******************************************************************************************************
 *                                              LEARN_INFO - START
 */
const char *
field_desc_l2_learn_info_cb(
    int unit)
{
    return "Learn information. May include Destination, Out-LIF and a MACT flush group";
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
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If the indication ignore_l2_learn_info_set is set, the l2_learn is later set by the application specific cb
     */
    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                 "Error! Generic support for the l2_learn_info field was not implemented, see FLOW_APP_TERM_IND_IGNORE_L2_LEARN_INFO_SET indication");

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

    SHR_FUNC_INIT_VARS(unit);

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
                         (uint32 *) &(terminator_info->l2_learn_info.dest_port), &dbal_struct_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_FLUSH_GROUP,
                         &(terminator_info->l2_learn_info.flush_group), &dbal_struct_data));

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
                             (uint32 *) &(terminator_info->l2_learn_info.dest_port), &dbal_struct_data));
        }
    }

    /*
     * Retrieve the Optimized flag according to the selected learn-payload-context
     */
    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, &learn_context));
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

    /*
     * A delete CB should be run only in case the field was set at create (resources were allocated). 
     * Check only the necessary fields.
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif,
                     gport_hw_resources->inlif_dbal_table_id, TRUE, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, &is_set));
    if (is_set)
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, gport_hw_resources->global_out_lif));
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
 */

/******************************************************************************************************
 *                                              LEARN_ENABLE - START
 */
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

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, &learn_enable));
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
 */

/******************************************************************************************************
 *                                              EGRESS_QOS_MODEL_SUBFIELDS - START
 */
/* Empty callback. Linked to TTL, DSCP, EXP, PKT_PRI_CFI initiator fields. Those fields are handled in QOS_EGRESS_MODEL callback, if their enablers are set.
 * Like all callbacks which are enabled through their callback, it is called from the main flow sequence, and so must be defined */
shr_error_e
dnx_flow_field_egress_qos_model_subfield_set(
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

/* Empty callback. Linked to TTL, DSCP, EXP, PKT_PRI_CFI initiator fields. Those fields are handled in QOS_EGRESS_MODEL callback, if their enablers are set.
 * Like all callbacks which are enabled through their callback, it is called from the main flow sequence, and so must be defined */
shr_error_e
dnx_flow_field_egress_qos_model_subfield_get(
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

/*
 * Field description call back function
 */
const char *
field_desc_ttl_cb(
    int unit)
{
    return "Egress qos model ttl enabler. Must be set alongside BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_ttl = {
    /*
     * field_name
     */
    "TTL",
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
    FALSE,
    /*
     * field_desc
     */
    field_desc_ttl_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_egress_qos_model_subfield_set, dnx_flow_field_egress_qos_model_subfield_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/*
 * Field description call back function
 */
const char *
field_desc_dscp_cb(
    int unit)
{
    return
        "Egress qos model network profile enabler, using dscp value. Must be set alongside BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_dscp = {
    /*
     * field_name
     */
    "DSCP",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_DSCP_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * field_desc
     */
    field_desc_dscp_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_egress_qos_model_subfield_set, dnx_flow_field_egress_qos_model_subfield_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/*
 * Field description call back function
 */
const char *
field_desc_exp_cb(
    int unit)
{
    return
        "Egress qos model network profile enabler, using exp value. Must be set alongside BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_exp = {
    /*
     * field_name
     */
    "EXP",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_EXP_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * field_desc
     */
    field_desc_exp_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_egress_qos_model_subfield_set, dnx_flow_field_egress_qos_model_subfield_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};

/*
 * Field description call back function
 */
const char *
field_desc_pkt_pri_cfi_cb(
    int unit)
{
    return
        "Egress qos model network profile enabler, using pkt_pri, pkt_cfi values. Must be set alongside BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID";
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_pkt_pri_cfi = {
    /*
     * field_name
     */
    "PKT_PRI_CFI",
    /*
     * term_field_enabler
     */
    FLOW_ENABLER_INVALID,
    /*
     * init_field_enabler
     */
    BCM_FLOW_INITIATOR_ELEMENT_PKT_PRI_CFI_VALID,
    /*
     * is_profile
     */
    FALSE,
    /*
     * field_desc
     */
    field_desc_pkt_pri_cfi_cb,
    /*
     * field_cbs
     */
    {dnx_flow_field_egress_qos_model_subfield_set, dnx_flow_field_egress_qos_model_subfield_get, NULL, NULL},
    /*
     * field_print
     */
    NULL
};
/******************************************************************************************************
 *                                              EGRESS_QOS_MODEL_SUBFIELDS - END
 */
