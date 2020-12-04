/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


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
 *                                              VRF - START
 */
shr_error_e
dnx_flow_field_vrf_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    uint32 fodo;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                    (unit, DBAL_FIELD_FODO, DBAL_FIELD_VRF, (uint32 *) &terminator_info->vrf, &fodo));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE, fodo);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_vrf_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    uint8 is_field_valid;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_FODO, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        uint32 fodo;
        uint32 vrf;
        dbal_fields_e sub_field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FODO, INST_SINGLE, &fodo));

        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_FODO, fodo, &sub_field_id, &vrf));

        if (sub_field_id != DBAL_FIELD_VRF)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }

        terminator_info->vrf = (bcm_vlan_t) vrf;
    }
    else
    {
        terminator_info->vrf = 0;
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
 *                                              L3_INGRESS_INFO - START
 */
/*
 * Print call back function to display the vale of FLOW field l3_ingress_info 
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

/*
 * Get in lif profile
 */
static shr_error_e
dnx_flow_terminator_in_lif_profile_get(
    int unit,
    uint32 get_entry_handle_id,
    uint32 *in_lif_profile)
{
    uint32 dbal_field = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                           (unit, get_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, &dbal_field));
    *in_lif_profile = dbal_field;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l3_ingress_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dbal_entry_handle_t *entry_handle_ptr = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    /** Allocate default inLIF profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle_ptr));

    in_lif_profile_info.ingress_fields.urpf_mode = terminator_info->l3_ingress_info.urpf_mode;

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     entry_handle_ptr->table_id));

    old_in_lif_profile = new_in_lif_profile;

    if (terminator_info->service_type == bcmFlowServiceTypeCrossConnect)
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                         entry_handle_ptr->table_id));
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dbal_entry_handle_t *entry_handle_ptr = NULL;
    uint32 internal_entry_handle_id;
    lif_table_mngr_inlif_info_t inlif_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    /** Allocate default inLIF profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle_ptr));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &internal_entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, internal_entry_handle_id, &inlif_info));
    /** Get entry from inLIF table */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get
                              (unit, internal_entry_handle_id, (uint32 *) &old_in_lif_profile), _SHR_E_NOT_FOUND);

    /*
     * In case of in lif profile field marked with valid elements clear - restore to default profile 
     */
    if (!_SHR_IS_FLAG_SET(terminator_info->valid_elements_clear, BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID))
    {
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, old_in_lif_profile, &in_lif_profile_info, LIF));
        in_lif_profile_info.ingress_fields.urpf_mode = terminator_info->l3_ingress_info.urpf_mode;
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                        (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                         entry_handle_ptr->table_id));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    int new_in_lif_profile;
    uint32 in_lif_profile = 0;
    dbal_entry_handle_t *entry_handle_ptr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle_ptr));

    /** Get entry from inLIF table */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_in_lif_profile_get(unit, entry_handle_id, &in_lif_profile),
                              _SHR_E_NOT_FOUND);

    /** Deallocate in_lif_profile */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, LIF));

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dbal_table_field_info_t field_info = { 0 };
    const dnx_flow_app_config_t *flow_app_info;
    int rv, nof_result_types, res_type;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    /** Set QOS */
    phb_propagation_type = remark_propagation_type = ecn_propagation_type = ttl_propagation_type =
        DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;

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

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

    /*
     * Check if ecn mapping profile field exists in any of the result types
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, flow_app_info->flow_table, &table));
    nof_result_types = table->nof_result_types;
    for (res_type = 0; res_type < nof_result_types; res_type++)
    {
        rv = dbal_tables_field_info_get_no_err(unit, flow_app_info->flow_table, DBAL_FIELD_ECN_MAPPING_PROFILE,
                                               FALSE, res_type, 0, &field_info);
        if (rv == _SHR_E_NONE)
        {
            break;
        }
    }

    /*
     * Field exists in table
     */
    if (res_type < nof_result_types)
    {
        uint32 ecn_mapping_profile, ingress_ecn;

        ingress_ecn = terminator_info->ingress_qos_model.ingress_ecn;

        ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_DEFAULT;
        if (ingress_ecn == bcmQosIngressEcnModelEligible)
        {
            ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE;
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE,
                                     ecn_mapping_profile);
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, &dbal_field));

    propagation_prof = (int) dbal_field;

    /** Update QOS flags according to returned propagation_prof */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get
                    (unit, propagation_prof, &phb_propagation_type, &remark_propagation_type, &ecn_propagation_type,
                     &ttl_propagation_type));

    if (phb_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
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
                           (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif,
                            gport_hw_resources.inlif_dbal_table_id, TRUE, DBAL_FIELD_ECN_MAPPING_PROFILE, &is_valid));

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
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    uint32 internal_entry_handle_id;
    lif_table_mngr_inlif_info_t inlif_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set QOS */
    phb_propagation_type = remark_propagation_type = ecn_propagation_type = ttl_propagation_type =
        DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;

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

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &internal_entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, internal_entry_handle_id, &inlif_info));

    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_terminator_propag_profile_get
                              (unit, internal_entry_handle_id, (uint32 *) &old_propag_profile), _SHR_E_NOT_FOUND);

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
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                     (uint32) propag_profile);
    }

    /** Clear the unused profile which is indicated by last-referrence*/
    if (qos_propagation_prof_last_ref == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_clear(unit, old_propag_profile));
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    int propagation_prof = 0, rv = 0;
    dbal_entry_handle_t *entry_handle_ptr = NULL;

    SHR_FUNC_INIT_VARS(unit);

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    lif_table_mngr_outlif_info_t outlif_info = { 0 };
    uint32 internal_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &internal_entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                    (unit, gport_hw_resources.local_out_lif, internal_entry_handle_id, &outlif_info));

    SHR_IF_ERR_EXIT(dnx_lif_lib_logical_phase_to_encap_access_convert(unit,
                                                                      outlif_info.logical_phase,
                                                                      &(initiator_info->encap_access)));

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_profile_get(unit, &gport_hw_resources, &terminator_info->action_gport));
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_flow_field_value32_get
                               (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, &action_gport));
        initiator_info = (bcm_flow_initiator_info_t *) field_data;
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_profile_lif_get
                        (unit, &gport_hw_resources, &initiator_info->action_gport, gport_hw_resources.local_out_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_action_gport_terminator_replace(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
                                                           gport_hw_resources.inlif_dbal_table_id,
                                                           gport_hw_resources.inlif_dbal_result_type,
                                                           gport_hw_resources.local_in_lif, &old_lif_trap_profile));

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
                    (unit, &gport_hw_resources, &old_lif_trap_profile, gport_hw_resources.local_out_lif));

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
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(dnx_flow_field_action_gport_terminator_replace
                        (unit, entry_handle_id, gport_hw_resources, (bcm_flow_terminator_info_t *) field_data));
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_IF_ERR_EXIT(dnx_flow_field_action_gport_initiator_replace
                        (unit, entry_handle_id, gport_hw_resources, (bcm_flow_initiator_info_t *) field_data));
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_ingress_profile_set(unit, &gport_hw_resources, BCM_GPORT_INVALID));
    }
    else if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_lif_etpp_profile_lif_set
                        (unit, &gport_hw_resources, BCM_GPORT_INVALID, gport_hw_resources.local_out_lif));
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
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
    return "Profile of the qos model";
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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

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
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    void *field_data)
{
    uint8 is_field_valid;
    uint32 ttl_mode;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;
    bcm_qos_egress_model_t model;
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_TTL_MODE, &is_field_valid, &ttl_mode));
    if (is_field_valid == TRUE)
    {
        /** uniform is default, do nothing*/
        if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
        {
            initiator_info->egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        }
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, &is_field_valid, &encap_qos_model));
    if (is_field_valid == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, encap_qos_model, &model));
        initiator_info->egress_qos_model.egress_qos = model.egress_qos;
        initiator_info->egress_qos_model.egress_ecn = model.egress_ecn;
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
    FALSE,
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
 */

/******************************************************************************************************
 *                                              TBD FIELDS
 */

const dnx_flow_common_field_desc_t dnx_flow_common_field_additional_data =
    { {0}, FLOW_ENABLER_INVALID, FLOW_ENABLER_INVALID, 0, 0, {0}, NULL };
