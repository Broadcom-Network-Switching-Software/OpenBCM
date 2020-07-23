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
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/qos/qos.h>
#include <soc/dnx/dbal/dbal.h>
#include "src/soc/dnx/dbal/dbal_internal.h"

extern lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info;

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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_vrf_set, dnx_flow_field_vrf_get, NULL}
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
    void *field_data)
{
    uint32 stat_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_field_value32_get
                              (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &stat_id), _SHR_E_NOT_FOUND);

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

const dnx_flow_common_field_desc_t dnx_flow_common_field_stat_id = {
    /*
     * field_name
     */
    "STAT_ID",
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_stat_id_set, dnx_flow_field_stat_id_get, NULL}
};
/******************************************************************************************************
 *                                              STAT_ID - END
 */

/******************************************************************************************************
 *                                              L3_INGRESS_INFO - START
 */
/*
 * Get terminator table fields
 */
static shr_error_e
dnx_flow_terminator_in_lif_table_get(
    int unit,
    uint32 get_entry_handle_id,
    dbal_tables_e table_id,
    uint32 local_inlif,
    uint32 *in_lif_profile,
    int *propagation_prof)
{
    uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);

    /** Get inLIF profile */
    if (in_lif_profile)
    {
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, get_entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) dbal_field));
        *in_lif_profile = dbal_field[0];
    }

    /** Get QoS propagation profile */
    if (propagation_prof)
    {
        uint8 is_field_valid;
        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, get_entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, &is_field_valid,
                         (uint32 *) dbal_field));
        if (is_field_valid)
        {
            *propagation_prof = (int) (dbal_field[0]);
        }
        else
        {
            *propagation_prof = 0;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l3_ingress_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    in_lif_profile_info_t in_lif_profile_info;
    int old_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    int new_in_lif_profile = DNX_IN_LIF_PROFILE_DEFAULT;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    dbal_entry_handle_t *entry_handle_ptr = NULL;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 internal_entry_handle_id;
    lif_table_mngr_inlif_info_t inlif_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    /** Allocate default inLIF profile */
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle_ptr));

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, flow_handle_info->flow_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &internal_entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, internal_entry_handle_id, &inlif_info));
        /** Get entry from inLIF table */
        SHR_IF_ERR_EXIT(dnx_flow_terminator_in_lif_table_get
                        (unit, internal_entry_handle_id, entry_handle_ptr->table_id, gport_hw_resources.local_in_lif,
                         (uint32 *) &old_in_lif_profile, NULL));
    }

    in_lif_profile_info.ingress_fields.urpf_mode = terminator_info->l3_ingress_info.urpf_mode;

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     entry_handle_ptr->table_id));

    old_in_lif_profile = new_in_lif_profile;

    if (terminator_info->service_type == bcmFlowServiceTypeCrossConnect)
    {
        /** Allocate inLIF profile */
        
        /*
         * in_lif_profile_info.ingress_fields.l3_ingress_flags |= ENABLERS_DISABLE_SUPPORTED_FLAGS;
         */
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
    void *field_data)
{
    in_lif_profile_info_t in_lif_profile_info;
    uint32 in_lif_profile = 0;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_field_value32_get
                              (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, &in_lif_profile), _SHR_E_NOT_FOUND);

    /*
     * get in_lif_profile data and store it in in_lif_profile_info
     */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));

    terminator_info->l3_ingress_info.urpf_mode = in_lif_profile_info.ingress_fields.urpf_mode;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_l3_ingress_info_delete(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    int new_in_lif_profile;
    uint32 in_lif_profile = 0;
    dbal_entry_handle_t *entry_handle_ptr = NULL;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle_ptr));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /** Get entry from inLIF table */
    SHR_IF_ERR_EXIT(dnx_flow_terminator_in_lif_table_get
                    (unit, entry_handle_id, entry_handle_ptr->table_id, gport_hw_resources.local_in_lif,
                     &in_lif_profile, NULL));

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
     * field_desc
     */
    NULL,
    /*
     * field_cb
     */
    {dnx_flow_field_l3_ingress_info_set, dnx_flow_field_l3_ingress_info_get, dnx_flow_field_l3_ingress_info_delete}
};

/******************************************************************************************************
 *                                              L3_INGRESS_INFO - END
 */

/******************************************************************************************************
 *                                              STAT_PP_PROFILE - START
 */

shr_error_e
dnx_flow_field_stat_pp_profile_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
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
    void *field_data)
{
    uint32 stat_pp_profile;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_field_value32_get
                              (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, &stat_pp_profile), _SHR_E_NOT_FOUND);

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
     * field_desc
     */
    NULL,
    /*
     * field_cb
     */
    {dnx_flow_field_stat_pp_profile_set, dnx_flow_field_stat_pp_profile_get, NULL}
};

/******************************************************************************************************
 *                                              STAT_PP_PROFILE - END
 */

/******************************************************************************************************
 *                                              QOS_INGRESS_MODEL - START
 */
shr_error_e
dnx_flow_field_qos_ingress_model_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
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

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_qos_ingress_model_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    dnx_qos_propagation_type_e phb_propagation_type;
    dnx_qos_propagation_type_e remark_propagation_type;
    dnx_qos_propagation_type_e ecn_propagation_type;
    dnx_qos_propagation_type_e ttl_propagation_type;
    uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint8 is_field_valid;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    int propagation_prof;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(dbal_field, 0, sizeof(dbal_field));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, &is_field_valid, (uint32 *) dbal_field));
    if (is_field_valid)
    {
        propagation_prof = (int) (dbal_field[0]);
    }
    else
    {
        propagation_prof = 0;
    }

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

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_qos_ingress_model_destroy(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    uint32 in_lif_profile = 0;
    int propagation_prof = 0;
    dbal_entry_handle_t *entry_handle_ptr = NULL;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle_ptr));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /** Get entry from inLIF table */
    SHR_IF_ERR_EXIT(dnx_flow_terminator_in_lif_table_get
                    (unit, entry_handle_id, entry_handle_ptr->table_id, gport_hw_resources.local_in_lif,
                     &in_lif_profile, &propagation_prof));

    /** Free propagation profile template */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_free_and_hw_clear(unit, propagation_prof, NULL));

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_qos_ingress_model_info = {
    /*
     * field_name
     */
    "QOS_INGRESS_MODEL",
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cb
     */
    {dnx_flow_field_qos_ingress_model_set, dnx_flow_field_qos_ingress_model_get,
     dnx_flow_field_qos_ingress_model_destroy}
};
/******************************************************************************************************
 *                                              QOS_INGRESS_MODEL - END
 */

/******************************************************************************************************
 *                                              SERVICE_TYPE - START
 */

shr_error_e
dnx_flow_field_service_type_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    uint32 service_type_value = 0;
    dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type = 0;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    if (terminator_info->service_type == bcmFlowServiceTypeCrossConnect)
    {
        vtt_lif_service_type = DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P;
        /** DESTINATION is supposed to be configured by API bcm_vswitch_cross_connect_add.*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0);
    }
    else
    {
        vtt_lif_service_type = DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2MP;
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
    void *field_data)
{
    uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint8 is_field_valid;
    dbal_enum_value_field_vtt_lif_service_type_e vtt_lif_service_type = 0;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(dbal_field, 0, sizeof(dbal_field));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, &is_field_valid, (uint32 *) dbal_field));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_SERVICE_TYPE, DBAL_FIELD_TYPE, &vtt_lif_service_type, dbal_field));
    if (vtt_lif_service_type == DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P)
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_service_type_set, dnx_flow_field_service_type_set, NULL}
};

/******************************************************************************************************
 *                                              SERVICE_TYPE - END
 */

/******************************************************************************************************
 *                                              DEST_INFO - START
 */

shr_error_e
dnx_flow_field_dest_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;
    SHR_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                 BCM_GPORT_FORWARD_PORT_GET(terminator_info->dest_info.dest_port));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                 terminator_info->dest_info.dest_encap);

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
    void *field_data)
{
    uint32 dest;
    int rv;
    bcm_flow_terminator_info_t *terminator_info = (bcm_flow_terminator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    rv = dnx_flow_field_value32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, &dest);

    if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_dest_info_set, dnx_flow_field_dest_info_get, NULL}
};

/******************************************************************************************************
 *                                              DEST_INFO - END
 */

/******************************************************************************************************
 *                                              L3_INTF_ID - START
 */
shr_error_e
dnx_flow_field_l3_intf_id_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    bcm_gport_t gport_tunnel;

    SHR_FUNC_INIT_VARS(unit);

    /** Get next outlif if exist */
    if ((initiator_info->l3_intf_id != 0) && (initiator_info->l3_intf_id != BCM_IF_INVALID))
    {
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport_tunnel, initiator_info->l3_intf_id);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gport_tunnel, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));
        /** Next outlif */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     gport_hw_resources.local_out_lif);
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
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    int gport = 0, next_outlif = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Get next outLIF pointer */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_field_value32_get
                              (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                               (uint32 *) &next_outlif), _SHR_E_NOT_FOUND);
    /** In case local lif is not found, return next pointer 0 */
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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_l3_intf_id_set, dnx_flow_field_l3_intf_id_get, NULL}
};

/******************************************************************************************************
 *                                              L3_INTF_ID - END
 */

/******************************************************************************************************
 *                                              ENCAP_ACCESS - START
 */
shr_error_e
dnx_flow_field_encap_access_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    if (initiator_info->encap_access == bcmEncapAccessRif)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1;
    }
    else if (initiator_info->encap_access == bcmEncapAccessNativeArp)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2;
    }
    else if (initiator_info->encap_access == bcmEncapAccessTunnel1)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_3;
    }
    else if (initiator_info->encap_access == bcmEncapAccessTunnel2)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_4;
    }
    else if (initiator_info->encap_access == bcmEncapAccessTunnel3)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_5;
    }
    else if (initiator_info->encap_access == bcmEncapAccessTunnel4)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_6;
    }
    else if (initiator_info->encap_access == bcmEncapAccessArp)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_7;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_field_encap_access_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    lif_table_mngr_outlif_info_t outlif_info = { 0 };
    int lif_flags;
    lif_mngr_outlif_phase_e outlif_phase;
    uint32 internal_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get GPort hw resources (global and local tunnel outlif) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &internal_entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                    (unit, gport_hw_resources.local_out_lif, internal_entry_handle_id, &outlif_info));

    outlif_phase = outlif_info.outlif_phase;

    /** Get outLIF phase */
    if (outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1)
    {
        initiator_info->encap_access = bcmEncapAccessRif;
    }
    else if (outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2)
    {
        initiator_info->encap_access = bcmEncapAccessNativeArp;
    }
    else if (outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_3)
    {
        initiator_info->encap_access = bcmEncapAccessTunnel1;
    }
    else if (outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_4)
    {
        initiator_info->encap_access = bcmEncapAccessTunnel2;
    }
    else if (outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_5)
    {
        initiator_info->encap_access = bcmEncapAccessTunnel3;
    }
    else if (outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_6)
    {
        initiator_info->encap_access = bcmEncapAccessTunnel4;
    }
    else if (outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_7)
    {
        initiator_info->encap_access = bcmEncapAccessArp;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognized outlif phase");
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_EXIT();

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
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_encap_access_set, dnx_flow_field_encap_access_get, NULL}
};

/******************************************************************************************************
 *                                              ENCAP_ACCESS - END
 */

/******************************************************************************************************
 *                                              ACTION_GPORT - START
 */
shr_error_e
dnx_flow_field_action_gport_set(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type == FLOW_APP_TYPE_INIT)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE,
                                     initiator_info->action_gport);
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
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_type != FLOW_APP_TYPE_INIT)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_EXIT();
    }

    /** action gport */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_field_value32_get
                              (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE,
                               (uint32 *) &initiator_info->action_gport), _SHR_E_NOT_FOUND);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_action_gport = {
    /*
     * field_name
     */
    "ACTION_GPORT",
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_action_gport_set, dnx_flow_field_action_gport_get, NULL}
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
    void *field_data)
{
    bcm_flow_initiator_info_t *initiator_info = (bcm_flow_initiator_info_t *) field_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_flow_field_value8_get
                              (unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE,
                               &initiator_info->mtu_profile), _SHR_E_NOT_FOUND);

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

const dnx_flow_common_field_desc_t dnx_flow_common_field_mtu_profile = {
    /*
     * field_name
     */
    "MTU_PROFILE",
    /*
     * field_desc
     */
    NULL,
    /*
     * field_cbs
     */
    {dnx_flow_field_mtu_profile_set, dnx_flow_field_mtu_profile_get, NULL}
};

/******************************************************************************************************
 *                                              MTU_PROFILE - END
 */

/******************************************************************************************************
 *                                              TBD FIELDS
 */
const dnx_flow_common_field_desc_t dnx_flow_common_field_qos_map_id = { {0}, 0, {0} };
const dnx_flow_common_field_desc_t dnx_flow_common_field_additional_data = { {0}, 0, {0} };
const dnx_flow_common_field_desc_t dnx_flow_common_field_qos_egress_model = { {0}, 0, {0} };
