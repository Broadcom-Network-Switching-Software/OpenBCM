/**
 * \file mpls_tunnel_term.c
 * MPLS functionality for DNX tunnel termination.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <bcm_int/dnx/field/field_entry.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm/mpls.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/mpls/mpls_tunnel_term.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/algo/mpls/algo_mpls.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_mpls_access.h>
#include <bcm_int/dnx_dispatch.h>
#include "mpls_evpn.h"
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
 /*
  * }
  */
#if defined(INCLUDE_L3)
/* { */
/*
 * Procedures related to special labels.
 * {
 */
shr_error_e
dnx_bcm_mpls_special_label_t_init(
    int unit,
    bcm_mpls_special_label_t * bcm_mpls_special_label_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_mpls_special_label_p, _SHR_E_PARAM, "bcm_mpls_special_label_p");
    sal_memset(bcm_mpls_special_label_p, 0, sizeof(*bcm_mpls_special_label_p));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/*
 * Procedures related to FRR feature.
 * {
 */
/*
 * See header in mpls_tunnel_term.h
 */
shr_error_e
dnx_mpls_frr_label_identifier_add(
    int unit,
    bcm_mpls_special_label_t * label_info_p)
{
    uint32 entry_handle_id;
    uint32 entry_access_id;
    uint32 field_val;
    int core = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Here add validity checks.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_FRR_TCAM_DB, &entry_handle_id));
    /*
     * Create TCAM access id
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, DBAL_TABLE_MPLS_FRR_TCAM_DB, 1, &entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    field_val = label_info_p->label_value;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_val);
    field_val = 1;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FOUND, INST_SINGLE, field_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See header in mpls_tunnel_term.h
 */
shr_error_e
dnx_mpls_frr_label_identifier_get(
    int unit,
    bcm_mpls_special_label_t * label_info_p)
{
    uint32 entry_handle_id;
    uint32 entry_access_id;
    shr_error_e shr_error;
    uint32 field_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Here add validity checks.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_FRR_TCAM_DB, &entry_handle_id));
    field_val = label_info_p->label_value;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_val);

    shr_error = dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT);
    if (shr_error != _SHR_E_NONE)
    {
        if (shr_error == _SHR_E_NOT_FOUND)
        {
            SHR_SET_CURRENT_ERR(shr_error);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(shr_error);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FOUND, INST_SINGLE, &field_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See header in mpls_tunnel_term.h
 */
shr_error_e
dnx_mpls_frr_label_identifier_delete(
    int unit,
    bcm_mpls_special_label_t * label_info_p)
{
    uint32 entry_handle_id;
    uint32 entry_access_id;
    uint32 field_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Here add validity checks.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_FRR_TCAM_DB, &entry_handle_id));
    field_val = label_info_p->label_value;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_val);

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, DBAL_TABLE_MPLS_FRR_TCAM_DB, entry_access_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_mpls_frr_label_identifier_delete_all(
    int unit)
{
    return _SHR_E_UNAVAIL;
}

/*
 * See header in mpls_tunnel_term.h
 */
shr_error_e
dnx_mpls_frr_label_identifier_traverse(
    int unit,
    bcm_mpls_special_label_identifier_traverse_cb cb,
    void *user_data_p)
{
    uint32 entry_handle_id;
    uint32 dbal_tables[] = { DBAL_TABLE_MPLS_FRR_TCAM_DB };
    uint32 nof_tables;
    uint32 table_id;
    int is_end;
    uint32 field_val;
    bcm_mpls_special_label_type_t label_type;
    bcm_mpls_special_label_t label_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "No callback function has been provided for this 'traverse'.");
    nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    is_end = 0;
    label_type = bcmMplsSpecialLabelTypeFrr;
    /*
     * Iterate over all tables and all their entries
     * Currently, there is only one table.
     */
    for (table_id = 0; table_id < nof_tables; table_id++)
    {
        /*
         * Allocate handle to the table of the iteration and initialise an iterator entity. * The iterator is in mode
         * ALL, which means that it will consider all entries regardless * of them being default entries or not.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table_id], &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, &field_val));
            SHR_IF_ERR_EXIT(dnx_bcm_mpls_special_label_t_init(unit, &label_info));
            label_info.label_value = (bcm_mpls_label_t) field_val;
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, &label_type, &label_info, user_data_p));
            /*
             * Receive next entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/* } */
#endif /* INCLUDE_L3 */

/**
 * \brief
 * Fill the dnx_mpls_termination_profile_t structure
 * According to the information in bcm_mpls_tunnel_switch_t
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to a struct containing relevant
 *        information for the LIF TABLE entry.
 * \param [out] term_profile_info  -  A pointer to the struct representing the tunnel term profile entry.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_mpls_tunnel_switch_to_termination_profile(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    dnx_mpls_termination_profile_t * term_profile_info)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(term_profile_info, 0, sizeof(dnx_mpls_termination_profile_t));

    term_profile_info->expect_bos = _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_EXPECT_BOS) ? 1 : 0;
    /** Check bos if it is expected.*/
    term_profile_info->check_bos = _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_EXPECT_BOS) ? TRUE : FALSE;

    term_profile_info->reject_ttl_0 = _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_TRAP_TTL_0) ? 1 : 0;
    term_profile_info->reject_ttl_1 = _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_TRAP_TTL_1) ? 1 : 0;
    term_profile_info->has_cw = 0;
    term_profile_info->labels_to_terminate = _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL) ?
        TWO_MPLS_LABELS_TO_TERMINATE : ONE_MPLS_LABEL_TO_TERMINATE;
    term_profile_info->ttl_exp_label_index = TTL_EXP_CURRENT_MPLS_LABEL;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Fill the dnx_mpls_termination_profile_t structure
 * According to the information in bcm_mpls_tunnel_switch_t
 *
 * \param [in] unit - Relevant unit.
 * \param [in] term_profile_info - A pointer to the struct representing the tunnel term profile entry.
 * \param [out] info - A pointer to a struct containing relevant
 *        information for the LIF TABLE entry.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_mpls_termination_profile_to_tunnel_switch(
    int unit,
    dnx_mpls_termination_profile_t * term_profile_info,
    bcm_mpls_tunnel_switch_t * info)
{

    SHR_FUNC_INIT_VARS(unit);

    if (term_profile_info->expect_bos)
    {
        info->flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
    }
    if (term_profile_info->reject_ttl_0)
    {
        info->flags |= BCM_MPLS_SWITCH_TRAP_TTL_0;
    }
    if (term_profile_info->reject_ttl_1)
    {
        info->flags |= BCM_MPLS_SWITCH_TRAP_TTL_1;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_mpls_tunnel_switch_term_add_verify(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If REPLACE flag is set, WITH_ID must also be set
     */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_REPLACE)
        && !_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "If REPLACE flag is set, WITH_ID flag must also be set.");
    }

    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_OUTER_EXP)
        || _SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_OUTER_TTL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_MPLS_SWITCH_OUTER_EXP and BCM_MPLS_SWITCH_OUTER_TTL flags were obsoleted for mpls tunnel term in JR2, use ingress_qos_model instead");
    }

    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_WITH_ID) && !(BCM_GPORT_IS_TUNNEL(info->tunnel_id)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel is set WITH_ID but 'info->tunnel_id' (%d) is not a valid tunnel",
                     info->tunnel_id);
    }

    /**tunnel LIF no longer points to RIF*/
    if (info->tunnel_if != BCM_IF_INVALID)
    {
        int feature = dnx_data_lif.global_lif.feature_get(unit, dnx_data_lif_global_lif_prevent_tunnel_update_rif);
        if (!(feature && (info->tunnel_if == 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "tunnel_if parameter cannot be used to create RIF. Please use bcm_l3_ingress_create instead with intf_id as mpls tunnel.");
        }
    }

    /*
     * Verify ingress qos model.
     */
    if ((info->ingress_qos_model.ingress_remark > bcmQosIngressModelUniform) ||
        (info->ingress_qos_model.ingress_phb > bcmQosIngressModelUniform) ||
        (info->ingress_qos_model.ingress_ttl > bcmQosIngressModelUniform))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Available Ingress qos model for remark(%d), phb(%d), ttl(%d) are Invalid(SDK default), ShortPipe, Pipe and Uniform only.",
                     info->ingress_qos_model.ingress_remark, info->ingress_qos_model.ingress_phb,
                     info->ingress_qos_model.ingress_ttl);
    }

    if ((info->stat_id != 0) || (info->stat_pp_profile != STAT_PP_PROFILE_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "stat_id, stat_pp_profile are invalid for mpls action POP (mpls tunnel termination)");
    }

    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_REPLACE))
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, info->tunnel_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                         &gport_hw_resources));

        if ((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_LSP) && DNX_MPLS_SWITCH_IS_EVPN(info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot replace MPLS tunnel with EVPN tunnel\n");
        }

        if ((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_EVPN) && !DNX_MPLS_SWITCH_IS_EVPN(info))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "CAnnot replace EVPN tunnel with MPLS tunnel\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in mpls_tunnel_term.h for function description
 */
shr_error_e
dnx_mpls_termination_profile_allocation(
    int unit,
    dnx_mpls_termination_profile_t term_profile_info,
    int *term_profile,
    int old_term_profile,
    uint8 *is_first_term_profile_reference,
    uint8 *is_last_term_profile_reference)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_first_term_profile_reference, _SHR_E_PARAM, "is_first_term_profile_reference");

    if (is_last_term_profile_reference == NULL)
    {
        SHR_IF_ERR_EXIT(algo_mpls_db.termination_profile.allocate_single
                        (unit, 0, &term_profile_info, NULL, term_profile, is_first_term_profile_reference));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_mpls_db.termination_profile.exchange
                        (unit, 0, &term_profile_info, old_term_profile, NULL, term_profile,
                         is_first_term_profile_reference, is_last_term_profile_reference));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in mpls_tunnel_term.h for function description
 */
shr_error_e
dnx_mpls_term_match_criteria_from_tunnel_switch_get(
    int unit,
    bcm_mpls_tunnel_switch_t * tunnel,
    dnx_mpls_term_match_t * match)
{
    uint8 second_pass;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tunnel, _SHR_E_INTERNAL, "tunnel");
    SHR_NULL_CHECK(match, _SHR_E_INTERNAL, "match");

    second_pass = _SHR_IS_FLAG_SET(tunnel->flags2, BCM_MPLS_SWITCH2_TERM_BUD);

    if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF))
    {
        /*
         * Per-Intf termination
         */
        match->match_criteria = second_pass ?
            TERM_MATCH_CRITERIA_INTF_LABEL_2ND_PASS_DB : TERM_MATCH_CRITERIA_INTF_LABEL;
        match->label = tunnel->label;
        match->context_intf = tunnel->ingress_if;
    }
    else if (_SHR_IS_FLAG_SET(tunnel->flags, BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL))
    {
        /*
         * Upstream assigned (match 2 labels)
         */
        match->match_criteria = second_pass ?
            TERM_MATCH_CRITERIA_TWO_LABELS_2ND_PASS_DB : TERM_MATCH_CRITERIA_TWO_LABELS;
        match->label = tunnel->second_label;
        match->context_label = tunnel->label;
    }
    else if (BCM_GPORT_IS_SET(tunnel->port))
    {
        /*
         * Per port label termination
         */

        uint32 vlan_domain = 0;
        SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, tunnel->port, &vlan_domain));
        match->match_criteria = TERM_MATCH_CRITERIA_VLAN_DOMAIN_LABEL;
        match->label = tunnel->label;
        match->vlan_domain = vlan_domain;
    }
    else
    {
        /*
         * "Vanilla" single label match
         */
        match->match_criteria = second_pass ? TERM_MATCH_CRITERIA_LABEL_2ND_PASS_DB : TERM_MATCH_CRITERIA_LABEL;
        match->label = tunnel->label;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in mpls_tunnel_term.h for function description
 */
shr_error_e
dnx_mpls_termination_lookup(
    int unit,
    dnx_mpls_termination_lookup_action_e action,
    dnx_mpls_term_match_t * match_info,
    int *local_in_lif)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(match_info, _SHR_E_INTERNAL, "match_info");
    if (action != TERM_LOOKUP_DELETE)
    {
        SHR_NULL_CHECK(local_in_lif, _SHR_E_INTERNAL, "local_in_lif");
    }

    /*
     * Set the app DB type according to type and key of lookup requested
     *  (one or two label lookup or label + lif, 1st/2nd pass).
     */
    switch (match_info->match_criteria)
    {
        case TERM_MATCH_CRITERIA_LABEL:
            table_id = DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB;
            break;
        case TERM_MATCH_CRITERIA_LABEL_2ND_PASS_DB:
            table_id = DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB_2ND_PASS;
            break;
        case TERM_MATCH_CRITERIA_LABEL_BOS:
            table_id = DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_WITH_BOS_DB;
            break;
        case TERM_MATCH_CRITERIA_INTF_LABEL:
        case TERM_MATCH_CRITERIA_TWO_LABELS:
            table_id = DBAL_TABLE_MPLS_TERMINATION_CONTEXT_SPECIFIC_LABEL_DB;
            break;
        case TERM_MATCH_CRITERIA_INTF_LABEL_2ND_PASS_DB:
        case TERM_MATCH_CRITERIA_TWO_LABELS_2ND_PASS_DB:
            table_id = DBAL_TABLE_MPLS_TERMINATION_CONTEXT_SPECIFIC_LABEL_DB_2ND_PASS;
            break;
        case TERM_MATCH_CRITERIA_VLAN_DOMAIN_LABEL:
            table_id = DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_WITH_NETWORK_DOMAIN_DB;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid match criteria for MPLS termination.");
    }

    /*
     * Allocate table handle and set the label
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, match_info->label);

    /*
     * Add additional key fields if necessary
     */
    switch (match_info->match_criteria)
    {
        case TERM_MATCH_CRITERIA_LABEL_BOS:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_BOS, match_info->bos_val);
            break;
        case TERM_MATCH_CRITERIA_INTF_LABEL:
        case TERM_MATCH_CRITERIA_INTF_LABEL_2ND_PASS_DB:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF,
                                       BCM_L3_ITF_VAL_GET(match_info->context_intf));
            break;
        case TERM_MATCH_CRITERIA_TWO_LABELS:
        case TERM_MATCH_CRITERIA_TWO_LABELS_2ND_PASS_DB:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_LABEL, match_info->context_label);
            break;
        case TERM_MATCH_CRITERIA_VLAN_DOMAIN_LABEL:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, match_info->vlan_domain);
        default:
            /*
             * Impossible to get here, but required for compilation.
             */
            break;
    }

    /*
     * Perform action
     */
    switch (action)
    {
        case TERM_LOOKUP_SET:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, *local_in_lif);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case TERM_LOOKUP_FORCE_SET:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, *local_in_lif);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
            break;
        case TERM_LOOKUP_GET:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, (uint32 *) local_in_lif);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case TERM_LOOKUP_DELETE:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid action(%d) requested.", action);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


/**
* \brief
*  Add termination profile and global lif to local in_lif into lif format table.
* \param [in] unit                 - Relevant unit.
* \param [in] global_in_lif        - Global lif to be saved.
* \param [in] in_lif_profile       - Ingress lif applications.
* \param [in] term_profile         - Termination profile to be saved.
* \param [in] propagation_profile  - propagation profile to be used
* \param [in] forwarding_domain    - forwarding domain to be used (vsi)
* \param [in] protection_pointer -protection pointer.
* \param [in] protection_path - protection path.
* \param [in] stat_enable - indication if statistics are enabled on this lif.
* \param [in] qos_profile - qos profile.
* \param [in] entry_handle_id - DBAL handle id of type superset.
* \param [in] lif_info - Local Inlif Information to be filled.
* \return
*   shr_error_e - Non-zero in case of an error.
* \remark
*   All parameters must be valid
*/
static shr_error_e
dnx_mpls_term_to_lif_table_manager_info(
    int unit,
    int global_in_lif,
    int in_lif_profile,
    int term_profile,
    int propagation_profile,
    bcm_vpn_t forwarding_domain,
    int protection_pointer,
    int protection_path,
    int8 stat_enable,
    int qos_profile,
    uint32 entry_handle_id,
    lif_table_mngr_inlif_info_t * lif_info)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");

    sal_memset(lif_info, 0, sizeof(lif_table_mngr_inlif_info_t));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, global_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, in_lif_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE, term_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, forwarding_domain);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, propagation_profile);
    /** Check if Protection is enabled */
    if (protection_pointer != dnx_data_failover.path_select.ing_no_protection_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                     protection_pointer);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE, protection_path);
    }
    if (stat_enable)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, qos_profile);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add termination profile and global lif to local in_lif into lif format table for BIER_MPLS
* \param [in] unit                 - Relevant unit.
* \param [in] local_in_lif         - Local lif to be saved.
* \param [in] global_in_lif        - Global lif to be saved.
* \param [in] in_lif_profile       - Ingress lif applications.
* \param [in] term_profile         - Termination profile to be saved.
* \param [in] mc_id                - BIER Multicast Group ID
* \param [in] propagation_profile  - propagation profile to be used
* \param [in] bier_mpls_domain     - BIER domain to be used as forwarding domain
* \param [in] qos_profile          - qos profile.
* \return
*   shr_error_e - Non-zero in case of an error.
* \remark
*   All parameters must be valid
*/
shr_error_e
dnx_mpls_in_lif_format_bier_set(
    int unit,
    int local_in_lif,
    int global_in_lif,
    int in_lif_profile,
    int term_profile,
    int mc_id,
    int propagation_profile,
    int bier_mpls_domain,
    int qos_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_BIER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_BIER_IN_LIF_BIER_MPLS);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, global_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE, term_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, in_lif_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE, mc_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, propagation_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, qos_profile);
    /*
     * DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERMPLS_B1 replaced by
     * DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERMPLS256_B1
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_PARSER_CONTEXT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIERMPLS256_B1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get termination profile and global lif using local in_lif from lif format table.
* \par DIRECT INPUT:
*   \param [in] unit          -  Relevant unit.
*   \param [in] entry_handle_id - DBAL handle id of type superset.
*   \param [in] lif_info - Local Inlif Information to be filled.
*   \param [in] tunnel_id     -  Global lif to be returned. In
*          case of NULL, no value will be returned
*   \param [in] term_profile  -  Termination profile to be
*          returned. In case of NULL, no value will be returned
*   \param [in] propagation_profile  -  Termination profile to
*          be returned. In case of NULL, no value will be
*          returned
*   \param [out] forwarding_domain - forwarding domain.
*   \param [out] protection_pointer -protection pointer.
*   \param [out] protection_path - protection path.
*   \param [out] stat_enable - indication if statistics are enabled.
*   \param [out] qos_profile - qos profile.
*   \param [out] in_lif_profile - in_lif profile.
* \par INDIRECT INPUT:
*   * DBAL_TABLE_IN_LIF_FORMAT_LSP table.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * \b *tunnel_id \n
*     See 'tunnel_id' in DIRECT INPUT above
*   * \b *term_profile \n
*     See 'term_profile' in DIRECT INPUT above
*/
static shr_error_e
dnx_lif_table_manager_info_to_mpls_term(
    int unit,
    uint32 entry_handle_id,
    lif_table_mngr_inlif_info_t * lif_info,
    bcm_gport_t * tunnel_id,
    uint32 *term_profile,
    uint32 *propagation_profile,
    bcm_vpn_t * forwarding_domain,
    int *protection_pointer,
    int *protection_path,
    uint8 *stat_enable,
    int *qos_profile,
    int *in_lif_profile)
{
    uint8 field_exists;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (tunnel_id != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, (uint32 *) tunnel_id));
    }

    if (term_profile != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TERMINATION_TYPE, INST_SINGLE, term_profile));
    }

    if (propagation_profile != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, propagation_profile));
    }

    if (protection_pointer != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, &field_exists,
                         (uint32 *) protection_pointer));
        if (!field_exists)
        {
            *protection_pointer = 0;
        }
    }

    if (protection_path != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, &field_exists, (uint32 *) protection_path));
        if (!field_exists)
        {
            *protection_path = 0;
        }
    }

    if (stat_enable != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, stat_enable));
    }

    if (forwarding_domain != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_VRF, INST_SINGLE, (uint16 *) forwarding_domain));
    }

    if (qos_profile != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, (uint32 *) qos_profile));
    }

    if (in_lif_profile != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, (uint32 *) in_lif_profile));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in mpls_tunnel_term.h for function description
 */
shr_error_e
dnx_mpls_termination_profile_hw_set(
    int unit,
    dnx_mpls_termination_profile_t * term_profile_info,
    int term_profile)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_INGRESS_MPLS_TERMINATION_PROFILE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_MPLS_TERMINATION_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_TERMINATION_PROFILE, term_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LABELS_TO_TERMINATE, INST_SINGLE,
                                 term_profile_info->labels_to_terminate);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_EXP_LABEL_INDEX, INST_SINGLE,
                                 term_profile_info->ttl_exp_label_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REJECT_TTL_0, INST_SINGLE,
                                 term_profile_info->reject_ttl_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REJECT_TTL_1, INST_SINGLE,
                                 term_profile_info->reject_ttl_1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HAS_CW, INST_SINGLE, term_profile_info->has_cw);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXPECT_BOS, INST_SINGLE,
                                 term_profile_info->expect_bos);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHECK_BOS, INST_SINGLE,
                                 term_profile_info->check_bos);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * Get termination profile using the template manager.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] term_profile - Termination profile number to be used for the loookup
 * \param [out] info -  A pointer to a struct to be filled.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_mpls_termination_profile_get(
    int unit,
    int term_profile,
    bcm_mpls_tunnel_switch_t * info)
{
    int ref_count;
    dnx_mpls_termination_profile_t term_profile_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&term_profile_info, 0, sizeof(term_profile_info));

    SHR_IF_ERR_EXIT(algo_mpls_db.termination_profile.profile_data_get
                    (unit, term_profile, &ref_count, &term_profile_info));

    SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_to_tunnel_switch(unit, &term_profile_info, info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in mpls_tunnel_term.h for function description
 */
shr_error_e
dnx_mpls_hw_termination_profile_delete(
    int unit,
    uint32 term_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Clear the values written in DBAL_TABLE_INGRESS_MPLS_TERMINATION_PROFILE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_MPLS_TERMINATION_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_TERMINATION_PROFILE, term_profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get termination profile using the template manager.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] term_profile - Termination profile number to be used for the loookup
 * \param [out] is_last - Indicated this is the last entry
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_mpls_termination_profile_delete(
    int unit,
    uint32 term_profile,
    uint8 *is_last)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_mpls_db.termination_profile.free_single(unit, term_profile, is_last));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * CB function used by travert API for term_delete_all case
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for the ingress MPLS entry to be deleted
 * \param [out] user_data - not used
 *
 * \return shr_error_e
 */
static shr_error_e
dnx_mpls_tunnel_switch_term_delete_all_cb(
    int unit,
    bcm_mpls_tunnel_switch_t * info,
    void *user_data)
{
    int local_in_lif;
    uint8 is_evpn = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /*
     * Check if EVPN handling is needed according to the local inlif
     */
    SHR_IF_ERR_EXIT(dnx_mpls_evpn_local_in_lif_is_evpn(unit, info, &local_in_lif, &is_evpn));
    if (is_evpn)
    {
        /*
         * EVPN handling
         */
        SHR_IF_ERR_EXIT(dnx_mpls_evpn_term_delete(unit, local_in_lif));
    }
    else
    {
        /*
         * MPLS termination
         */
        SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_term_delete(unit, local_in_lif, info));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in mpls_tunnel_term.h for function description
 */
shr_error_e
dnx_mpls_tunnel_switch_term_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    int term_profile, old_term_profile;
    int in_global_lif = 0, old_in_global_lif, sys_in_lif;
    dnx_mpls_termination_profile_t term_profile_info;
    uint8 is_first_term_profile_reference = 0, is_last_term_profile_reference = 0;
    uint8 is_first_ingress_propag_profile_reference = 0, is_last_ingress_propag_profile_reference = 0;
    uint32 lif_flags;
    uint32 propag_profile, old_propag_profile;
    dnx_qos_propagation_type_e ttl_propag_type, phb_propag_type, remark_propag_type, ecn_propag_type;
    int old_local_inlif, local_inlif;
    int qos_profile;
    int protection_path = 0;
    int old_in_lif_profile = 0;
    int new_in_lif_profile;
    in_lif_profile_info_t in_lif_profile_info;
    lif_table_mngr_inlif_info_t lif_info;
    uint32 entry_handle_id;
    shr_error_e rv = _SHR_E_NONE;

    /** Needed to prevent Coverity issue of Out-of-bounds access (ARRAY_VS_SINGLETON) */
    int protection_pointer[1] = { 0 };
    int failover_id[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    old_propag_profile = DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE;

    /*
     * SW allocations
     */
    /** configure requested termination profile properties */
    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_to_termination_profile(unit, info, &term_profile_info));

    if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_REPLACE))
    {
        dnx_mpls_term_match_t match;
        uint32 old_entry_handle_id;

        /** Get entry from ISEM to get local lif id from label */
        SHR_IF_ERR_EXIT(dnx_mpls_term_match_criteria_from_tunnel_switch_get(unit, info, &match));
        rv = dnx_mpls_termination_lookup(unit, TERM_LOOKUP_GET, &match, &old_local_inlif);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, " REPLACE must be done using existing match information\n");
        }

        /** Get entry from MPLS IN-LIF table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_LSP, &old_entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, _SHR_CORE_ALL, old_local_inlif, old_entry_handle_id, &lif_info));

        SHR_IF_ERR_EXIT(dnx_lif_table_manager_info_to_mpls_term
                        (unit, old_entry_handle_id, &lif_info, &old_in_global_lif, (uint32 *) &old_term_profile,
                         &old_propag_profile, NULL, NULL, NULL, NULL, NULL, NULL));

        /** in some case, global inlif is 0, and should get it from sw state*/
        if (old_in_global_lif == 0)
        {
            bcm_gport_t gport;

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                            (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL,
                             old_local_inlif, &gport));
            old_in_global_lif = BCM_GPORT_TUNNEL_ID_GET(gport);
        }
        if (BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id) != old_in_global_lif)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tunnel_id (%d) != replaced tunnel_id (%d)",
                         BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id), old_in_global_lif);
        }
        in_global_lif = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);

        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_allocation
                        (unit, term_profile_info, &term_profile, old_term_profile,
                         &is_first_term_profile_reference, &is_last_term_profile_reference));
    }
    else
    {
        /** Allocate Global LIF */
        lif_flags = 0;
        if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_WITH_ID))
        {
            in_global_lif = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
            lif_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        }
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, lif_flags, DNX_ALGO_LIF_INGRESS, &in_global_lif));

        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_allocation
                        (unit, term_profile_info, &term_profile, 0, &is_first_term_profile_reference, NULL));
    }

    /** get propagation profile, if REPLACE used profile will be updated */
    phb_propag_type = DNX_QOS_PROPAGATION_TYPE_PIPE;
    remark_propag_type = ecn_propag_type = ttl_propag_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;

    if (info->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, info->ingress_qos_model.ingress_phb, &phb_propag_type));
    }

    if (info->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, info->ingress_qos_model.ingress_remark, &remark_propag_type));
    }

    if (info->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_qos_model_to_propag_type
                        (unit, info->ingress_qos_model.ingress_ttl, &ttl_propag_type));
    }

    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc
                    (unit, phb_propag_type, remark_propag_type, ecn_propag_type, ttl_propag_type,
                     old_propag_profile, (int *) &propag_profile, &is_first_ingress_propag_profile_reference,
                     &is_last_ingress_propag_profile_reference));

    /*
     * writing to HW tables
     */
    if (is_first_term_profile_reference)
    {
        /** Set profile termination in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_hw_set(unit, &term_profile_info, term_profile));
    }

    if (is_first_ingress_propag_profile_reference)
    {
        /** Set propagation profile in case it is the first time. */
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set(unit, propag_profile, phb_propag_type,
                                                                   remark_propag_type, ecn_propag_type,
                                                                   ttl_propag_type));
    }

    /** Set entry to MPLS IN-LIF table */
    

    /** Check if Protection is enabled */
    if (DNX_FAILOVER_IS_PROTECTION_ENABLED(info->failover_id))
    {
        DNX_FAILOVER_ID_GET(failover_id[0], info->failover_id);
        /** Encode IN-LIF protection pointer format from Failover ID */
        DNX_FAILOVER_ID_PROTECTION_POINTER_ENCODE(protection_pointer[0], failover_id[0]);
        protection_path = info->failover_tunnel_id ? 0 : 1;
    }
    else
    {
        protection_pointer[0] = dnx_data_failover.path_select.ing_no_protection_get(unit);
    }

    if (DNX_QOS_MAP_IS_PHB(info->qos_map_id) && DNX_QOS_MAP_IS_REMARK(info->qos_map_id))
    {
        qos_profile = DNX_QOS_MAP_PROFILE_GET(info->qos_map_id);
    }
    else
    {
        qos_profile = 0;
    }

    /*
     * allocate inlif profile
     */
    SHR_IF_ERR_EXIT(in_lif_profile_info_t_init(unit, &in_lif_profile_info));
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, old_in_lif_profile, &new_in_lif_profile, LIF,
                     DBAL_TABLE_IN_LIF_FORMAT_LSP));

    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_LSP, &entry_handle_id));
        /** if tunnel_if is 0, set HW sys_inlif field to NULL*/
        if (info->tunnel_if == 0)
        {
            sys_in_lif = 0;
        }
        else
        {
            sys_in_lif = in_global_lif;
        }
        SHR_IF_ERR_EXIT(dnx_mpls_term_to_lif_table_manager_info(unit, sys_in_lif,
                                                                new_in_lif_profile, term_profile,
                                                                propag_profile, info->vpn, protection_pointer[0],
                                                                protection_path, _SHR_IS_FLAG_SET(info->flags2,
                                                                                                  BCM_MPLS_SWITCH2_STAT_ENABLE),
                                                                qos_profile, entry_handle_id, &lif_info));

        /*
         * Find result type, allocate lif and write to HW
         */
        lif_info.global_lif = in_global_lif;
        if (_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_REPLACE))
        {
            lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
            local_inlif = old_local_inlif;
        }
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                        (unit, _SHR_CORE_ALL, entry_handle_id, &local_inlif, &lif_info));

        /** Add entry to ISEM to get local lif id from label */
        if (!_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_REPLACE))
        {
            dnx_mpls_term_match_t match;

            /** updating match key not supported */
            SHR_IF_ERR_EXIT(dnx_mpls_term_match_criteria_from_tunnel_switch_get(unit, info, &match));
            SHR_IF_ERR_EXIT(dnx_mpls_termination_lookup(unit, TERM_LOOKUP_SET, &match, &local_inlif));
        }
    }

    /** Update tunnel-ID according to global LIF value */
    if (!_SHR_IS_FLAG_SET(info->flags, BCM_MPLS_SWITCH_WITH_ID))
    {
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, in_global_lif);
    }

    /*
     * After new entry values are saved to HW delete redundant old values from HW
     */

    if (is_last_ingress_propag_profile_reference)
    {
        /** Delete profile propagation in case it is the last reference. */
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_clear(unit, old_propag_profile));
    }

    if (is_last_term_profile_reference)
    {
        /** Delete profile termination in case it is the last reference. */
        SHR_IF_ERR_EXIT(dnx_mpls_hw_termination_profile_delete(unit, old_term_profile));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in mpls_tunnel_term.h for function description
 */
shr_error_e
dnx_mpls_tunnel_switch_term_get(
    int unit,
    int local_inlif,
    bcm_mpls_tunnel_switch_t * info)
{
    int in_global_lif, qos_profile, failover_status;
    uint32 term_profile;
    uint32 propag_profile;
    dnx_qos_propagation_type_e phb_propagation_type, remark_propagation_type, ecn_propagation_type,
        ttl_propagation_type;
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_info;
    uint8 is_stat_enable;

    /** Needed to prevent Coverity issue of Out-of-bounds access (ARRAY_VS_SINGLETON) */
    int protection_pointer[1] = { 0 };
    int failover_id[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get entry from MPLS IN-LIF table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_LSP, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info(unit, _SHR_CORE_ALL, local_inlif, entry_handle_id, &lif_info));

    SHR_IF_ERR_EXIT(dnx_lif_table_manager_info_to_mpls_term
                    (unit, entry_handle_id, &lif_info, &in_global_lif, &term_profile, &propag_profile, &info->vpn,
                     &protection_pointer[0], &info->failover_tunnel_id, &is_stat_enable, &qos_profile, NULL));

    if (is_stat_enable)
    {
        info->flags2 |= BCM_MPLS_SWITCH2_STAT_ENABLE;
    }

    /** set QoS map type with PHB and REMARK */
    if (qos_profile != 0)
    {
        DNX_QOS_PHB_MAP_SET(qos_profile);
        DNX_QOS_REMARK_MAP_SET(qos_profile);
        DNX_QOS_INGRESS_MAP_SET(qos_profile);
        info->qos_map_id = qos_profile;
    }

    /** Get Failovr ID from IN-LIF protection pointer format */
    DNX_FAILOVER_ID_PROTECTION_POINTER_DECODE(failover_id[0], protection_pointer[0]);
    info->failover_id = failover_id[0];
    if (info->failover_id != dnx_data_failover.path_select.ing_no_protection_get(unit))
    {
        info->failover_tunnel_id = info->failover_tunnel_id ? 0 : 1;
        DNX_FAILOVER_SET(info->failover_id, info->failover_id, DNX_FAILOVER_TYPE_INGRESS);
        /** As per legacy, info->failover_tunnel_id indicates not the actual failover ID of the tunnel,
         * but whether it is currently the active tunnel or not */
        SHR_IF_ERR_EXIT(bcm_dnx_failover_get(unit, info->failover_id, &failover_status));
        info->failover_tunnel_id = (info->failover_tunnel_id == failover_status) ? 1 : 0;
    }

    /** Get propagation profile properties */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_attr_get
                    (unit, propag_profile, &phb_propagation_type, &remark_propagation_type, &ecn_propagation_type,
                     &ttl_propagation_type));

    if (ttl_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, ttl_propagation_type, &(info->ingress_qos_model.ingress_ttl)));
    }

    if (phb_propagation_type != DNX_QOS_PROPAGATION_TYPE_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, phb_propagation_type, &(info->ingress_qos_model.ingress_phb)));
    }
    if (remark_propagation_type != DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propag_type_to_qos_model
                        (unit, remark_propagation_type, &(info->ingress_qos_model.ingress_remark)));
    }

    /** Get profile termination using template manager */
    SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_get(unit, term_profile, info));

    /** Update tunnel-ID according to global LIF value */
    /** in some case, global inlif is 0, and should get it from sw state*/
    if (in_global_lif == 0)
    {
        bcm_gport_t gport;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL, local_inlif,
                         &gport));
        info->tunnel_id = gport;
        info->tunnel_if = 0;
    }
    else
    {
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, in_global_lif);
    }

    /** BCM_MPLS_SWITCH_TTL_DECREMENT is set even if not provided originaly */
    info->flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;

    /*
     * this is done for the _traverse API as all other APIs expect correct 'action' to be provided
     */
    info->action = BCM_MPLS_SWITCH_ACTION_POP;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in mpls_tunnel_term.h for function description
 */
shr_error_e
dnx_mpls_tunnel_switch_term_delete(
    int unit,
    int local_inlif,
    bcm_mpls_tunnel_switch_t * info)
{
    uint32 term_profile;
    int in_global_lif;
    uint8 is_last;
    int protection_path, protection_pointer;
    uint32 propag_profile;
    dnx_mpls_term_match_t match;
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_info;
    int in_lif_profile, new_in_lif_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get entry from MPLS IN-LIF table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_LSP, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info(unit, _SHR_CORE_ALL, local_inlif, entry_handle_id, &lif_info));

    SHR_IF_ERR_EXIT(dnx_lif_table_manager_info_to_mpls_term
                    (unit, entry_handle_id, &lif_info, &in_global_lif, &term_profile, &propag_profile, NULL,
                     &protection_pointer, &protection_path, NULL, NULL, &in_lif_profile));
    /** in some case, global inlif is 0, and should get it from sw state*/
    if (in_global_lif == 0)
    {
        bcm_gport_t gport;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL,
                         local_inlif, &gport));
        in_global_lif = BCM_GPORT_TUNNEL_ID_GET(gport);
    }

    /** Delete entry from ISEM*/
    SHR_IF_ERR_EXIT(dnx_mpls_term_match_criteria_from_tunnel_switch_get(unit, info, &match));
    SHR_IF_ERR_EXIT(dnx_mpls_termination_lookup(unit, TERM_LOOKUP_DELETE, &match, NULL));

    /** Delete profile termination using template manager */
    SHR_IF_ERR_EXIT(dnx_mpls_termination_profile_delete(unit, term_profile, &is_last));

    /** Delete profile termination entry from the DBAL */
    if (is_last)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_hw_termination_profile_delete(unit, term_profile));
    }

    /** Delete propagation profile template and hw */
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_free_and_hw_clear(unit, propag_profile, NULL));

    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, LIF));

    /*
     * Free Lif table and local LIF allocation
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                    (unit, _SHR_CORE_ALL, local_inlif, DBAL_TABLE_IN_LIF_FORMAT_LSP, 0));

    /** delete global lif */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_INGRESS, in_global_lif));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Delete all MPLS tunnels for mpls termination.
 *
 * \param [in] unit       -  device unit number.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 * \remark
 *   None.
 * \see
 *   * None
 */
shr_error_e
dnx_mpls_tunnel_switch_term_delete_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_term_traverse(unit, dnx_mpls_tunnel_switch_term_delete_all_cb, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse all MPLS tunnels in the ingress for mpls termination and run a callback function
 * provided by the user for each one.
 * \param [in] unit - The unit number.
 * \param [in] cb   - A pointer to callback function,
 *          it receives the value of the user_data variable and
 *          all MPLS tunnel objects that were iterated
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
shr_error_e
dnx_mpls_tunnel_switch_term_traverse(
    int unit,
    bcm_mpls_tunnel_switch_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    /*
     * Note: MPLS_TERMINATION_SINGLE_LABEL_WITH_BOS_DB is used to EVPN, don't add it
     */
    uint32 dbal_tables[] = { DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB,
        DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB_2ND_PASS,
        DBAL_TABLE_MPLS_TERMINATION_CONTEXT_SPECIFIC_LABEL_DB,
        DBAL_TABLE_MPLS_TERMINATION_CONTEXT_SPECIFIC_LABEL_DB_2ND_PASS,
        DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_WITH_BOS_DB
    };
    uint32 field_value[1];
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table;
    int is_end;
    bcm_mpls_tunnel_switch_t info;
    uint32 max_rsv_label = MPLS_MAX_RESERVED_LABEL;
    uint32 core_id = DBAL_CORE_DEFAULT;
    dbal_fields_e sub_field_id;
    uint32 sub_field_val = 0;
    uint32 local_inlif = 0;
    dbal_tables_e dbal_table_id;
    uint32 dbal_result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cb, BCM_E_PARAM, "bcm_mpls_tunnel_switch_traverse_cb");
    /*
     * Iterate over all tables and all their entries
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[0], &entry_handle_id));
    for (table = 0; table < nof_tables; table++)
    {
        /*
         * Allocate handle to the table of the iteration and initialize an iterator entity.
         * The iterator is in mode ALL without default, which means that it will consider all entries except
         * default entries.
         */
        if (table > 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_tables[table], entry_handle_id));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        /** Add KEY rule to skip MPLS reserved label */
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, DBAL_CONDITION_BIGGER_THAN, &max_rsv_label,
                         NULL));
        /** Add KEY rule to skip duplicated entry in core 1 */
        
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                               DBAL_CONDITION_EQUAL_TO, &core_id, NULL));
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            bcm_mpls_tunnel_switch_t_init(&info);
            field_value[0] = 0;

            switch (dbal_tables[table])
            {
                case DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB:
                case DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB_2ND_PASS:
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_value));
                    info.label = field_value[0];
                    break;
                case DBAL_TABLE_MPLS_TERMINATION_CONTEXT_SPECIFIC_LABEL_DB:
                case DBAL_TABLE_MPLS_TERMINATION_CONTEXT_SPECIFIC_LABEL_DB_2ND_PASS:
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_MPLS_EXTENDED_FODO, field_value));
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                                    (unit, DBAL_FIELD_MPLS_EXTENDED_FODO, field_value, &sub_field_id, &sub_field_val));
                    if (sub_field_id == DBAL_FIELD_GLOB_IN_LIF)
                    {
                        /** Special match - {Context-LIF, Label}, BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF set */
                        info.ingress_if = sub_field_val;
                        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                        (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_value));
                        info.label = field_value[0];
                        info.flags |= BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF;
                    }
                    else if (sub_field_id == DBAL_FIELD_CONTEXT_LABEL)
                    {
                        /** Special match - {Context-label, Mpls-label} */
                        info.label = sub_field_val;
                        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                        (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_value));
                        info.second_label = field_value[0];
                        info.flags |= BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid sub field id %d in MPLS termination entry.",
                                     sub_field_id);
                    }
                    break;
                case DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_WITH_BOS_DB:     /* EVPN term DB */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_MPLS_BOS, field_value));
                    if (field_value[0])
                    {
                        info.flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
                    }
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL, field_value));
                    info.label = field_value[0];
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid match criteria for MPLS termination.");
            }
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, (uint32 *) &local_inlif));
            /*
             * LSP, PWE and EVPN INLIF always use INLIF-2.
             */
            SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                            (unit, local_inlif, dnx_drv_soc_core_default(), DBAL_PHYSICAL_TABLE_INLIF_2, &dbal_table_id,
                             &dbal_result_type, NULL));
            if (dbal_tables[table] == DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB_2ND_PASS
                || dbal_tables[table] == DBAL_TABLE_MPLS_TERMINATION_CONTEXT_SPECIFIC_LABEL_DB_2ND_PASS)
            {
                info.flags2 |= BCM_MPLS_SWITCH2_TERM_BUD;
            }
            /*
             * PWE lifs are handles with mpls_port_delete APIs
             */
            if (dbal_table_id != DBAL_TABLE_IN_LIF_FORMAT_PWE)
            {
                if (dbal_table_id == DBAL_TABLE_IN_LIF_FORMAT_EVPN)
                {
                    SHR_IF_ERR_EXIT(dnx_mpls_evpn_term_get(unit, local_inlif, &info));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_mpls_tunnel_switch_term_get(unit, local_inlif, &info));
                }
                SHR_IF_ERR_EXIT((*cb) (unit, &info, user_data));
            }

            /*
             * Receive next entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
