/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_qos.c
 *
 *  qos algorithms initialization and deinitialization.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_QOS

/*************
 * INCLUDES  *
 */
/*
 * { 
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>

/*
 * } 
 */
/*************
 * DEFINES   *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * MACROS    *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * TYPE DEFS *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * GLOBALS   *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * FUNCTIONS *
 */
/*
 * { 
 */
 /**
* \brief
*   Print an entry of egress network_qos profile template.
*/
void
dnx_algo_qos_egress_network_qos_profile_template_print_cb(
    int unit,
    const void *data)
{
    qos_pipe_profile_template_t *template_data = (qos_pipe_profile_template_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "NETWORK_QOS_PIPE",
                                   template_data->network_qos_pipe, "NETWORK_QOS_PIPE", NULL);

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "NETWORK_QOS_PIPE_MAPPED",
                                   template_data->network_qos_pipe_mapped, "NETWORK_QOS_PIPE_MAPPED", NULL);

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "DP_PIPE_MAPPED",
                                   template_data->dp_pipe_mapped, "DP_PIPE_MAPPED", NULL);

    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/**
* \brief
*   Print an entry of qos propagation profile template.
*/
void
dnx_algo_qos_propagation_profile_template_print_cb(
    int unit,
    const void *data)
{
    qos_propagation_profile_template_t *template_data = (qos_propagation_profile_template_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "PHB", template_data->phb, "PHB", NULL);

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "REMARK", template_data->remark, "REMARK", NULL);

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "ECN", template_data->ecn, "ECN", NULL);

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "TTL", template_data->ttl, "TTL", NULL);

    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/**
* \brief
*   Print an entry of egress ttl pipe profile template.
*/
void
dnx_algo_qos_egress_ttl_pipe_profile_template_print_cb(
    int unit,
    const void *data)
{
    int *ttl_ptr = (int *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "ttl", *ttl_ptr, NULL, "0d%x");

    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/**
 * \brief 
 * allocate resource pool for all QOS profiles types
 */
shr_error_e
dnx_algo_qos_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    dnx_algo_template_create_data_t template_data;
    qos_propagation_profile_template_t propagation_profile_data;
    int max_dpc_reference;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init Ingress resource pool
     */
    SHR_IF_ERR_EXIT(algo_qos_db.init(unit));

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;

    data.nof_elements = dnx_data_qos.qos.nof_ingress_profiles_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_INGRESS_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.create(unit, &data, NULL));

    /*
     * Init Ingress phb opcode resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;

    data.nof_elements = dnx_data_qos.qos.nof_ingress_opcode_profiles_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_INGRESS_OPCODE_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_phb_opcode.create(unit, &data, NULL));

    /*
     * Init Ingress remark opcode resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;

    data.nof_elements = dnx_data_qos.qos.nof_ingress_opcode_profiles_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_INGRESS_OPCODE_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_remark_opcode.create(unit, &data, NULL));

    /*
     * Init Ingress ecn opcode resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    /*
     * 0 is reserved for ECN non-eligible, first is reserved for ECN eligible logic according to IP header.
     */
    data.first_element = 1;

    data.nof_elements = dnx_data_qos.qos.nof_ingress_ecn_opcode_profiles_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_INGRESS_OPCODE_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn_opcode.create(unit, &data, NULL));

    /*
     * Init Ingress ecn resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    /*
     * 0 is reserved for ECN non-eligible, first is reserved for ECN eligible logic according to IP header.
     */
    data.first_element = 1;

    data.nof_elements = dnx_data_qos.qos.nof_ingress_ecn_profiles_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_INGRESS_ECN_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn.create(unit, &data, NULL));

    /*
     * Init Ingress policer resource pool, don't allow customer to change profile 0
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 1;
    sal_strncpy(data.name, DNX_QOS_INGRESS_POLICER_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    data.nof_elements = dnx_data_qos.qos.nof_ingress_policer_profiles_get(unit) - 1;
    data.flags = 0;

    SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_policer.create(unit, &data, NULL));

    /*
     * Init Ingress vlan pcp resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    sal_strncpy(data.name, DNX_QOS_INGRESS_VLAN_PCP_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    data.nof_elements = dnx_data_qos.qos.nof_ingress_vlan_pcp_profiles_get(unit);
    data.flags = 0;

    SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_vlan_pcp.create(unit, &data, NULL));

    /*
     * Init Egress resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_qos.qos.nof_egress_profiles_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_EGRESS_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.egress_qos.create(unit, &data, NULL));

    /*
     * Init Egress opcode resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_qos.qos.nof_egress_opcode_profiles_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_EGRESS_OPCODE_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_opcode.create(unit, &data, NULL));

    /*
     * Init Egress mpls php resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 1;
    data.nof_elements = dnx_data_qos.qos.nof_egress_mpls_php_profiles_get(unit) - 1;
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_EGRESS_MPLS_PHP_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_mpls_php.create(unit, &data, NULL));

    /*
     * Init Egress php opcode resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 1;
    data.nof_elements = dnx_data_qos.qos.nof_egress_opcode_profiles_get(unit) - 1;
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_EGRESS_PHP_OPCODE_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_php_opcode.create(unit, &data, NULL));

    /*
     * Init Egress policer resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_qos.qos.nof_egress_policer_profiles_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_EGRESS_POLICER_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_policer.create(unit, &data, NULL));

    /*
     * Init Egress OAM PCP MAP resource pool
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_oam.general.oam_nof_priority_map_profiles_get(unit);;
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_EGRESS_OAM_PCP_MAP_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_oam_pcp_map.create(unit, &data, NULL));

    /*
     * Init Egress cos profile resource pool, user can not change cos profile 0 mapping
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 1;
    data.nof_elements = dnx_data_qos.qos.nof_egress_cos_profiles_get(unit) - 1;
    data.flags = 0;
    sal_strncpy(data.name, DNX_QOS_EGRESS_COS_PROFILES_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.egress_cos_profile.create(unit, &data, NULL));

    /*
     * Init Ingress Propagation profile template resource pool
     */
    /** Init the data for default profile with all 'PIPE' first */
    sal_memset(&propagation_profile_data, 0, sizeof(qos_propagation_profile_template_t));
    /** Init the profile resource then*/
    sal_memset(&template_data, 0, sizeof(dnx_algo_template_create_data_t));
    propagation_profile_data.phb = DNX_QOS_PROPAGATION_TYPE_PIPE;
    propagation_profile_data.remark = DNX_QOS_PROPAGATION_TYPE_PIPE;
    /*
     * ECN is short-pipe as default
     */
    propagation_profile_data.ecn = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;
    propagation_profile_data.ttl = DNX_QOS_PROPAGATION_TYPE_PIPE;
    template_data.data_size = sizeof(qos_propagation_profile_template_t);
    template_data.default_data = &propagation_profile_data;
    template_data.default_profile = DNX_QOS_INGRESS_PROPAGATION_PROFILE_PIPE;
    template_data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    template_data.first_profile = 0;
    /**propatation profile will be use over all LIFs and RIFs*/
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &template_data.max_references, &max_dpc_reference));
    template_data.max_references += max_dpc_reference;
    template_data.max_references += dnx_data_l3.rif.nof_rifs_get(unit);
    template_data.nof_profiles = dnx_data_qos.qos.nof_ingress_propagation_profiles_get(unit);
    sal_strncpy(template_data.name, DNX_ALGO_QOS_INGRESS_PROPAGATION_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.qos_ingress_propagation_profile.create(unit, &template_data, NULL));

    /*
     * Init Egress network qos pipe profile template resource pool
     */
    sal_memset(&template_data, 0, sizeof(dnx_algo_template_create_data_t));
    template_data.flags = 0;
    template_data.data_size = sizeof(qos_pipe_profile_template_t);
    template_data.first_profile = 0;
    template_data.max_references = dnx_data_lif.out_lif.nof_local_out_lifs_get(unit);
    template_data.nof_profiles = dnx_data_qos.qos.nof_egress_network_qos_pipe_profiles_get(unit);
    sal_strncpy(template_data.name, DNX_ALGO_QOS_EGRESS_NETWORK_QOS_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.create(unit, &template_data, NULL));

    /*
     * Init Egress TTL-IDX template resource pool
     */
    sal_memset(&template_data, 0, sizeof(dnx_algo_template_create_data_t));
    template_data.flags = 0;
    template_data.data_size = sizeof(int);
    template_data.first_profile = 0;
    template_data.max_references = dnx_data_lif.out_lif.nof_local_out_lifs_get(unit);
    template_data.nof_profiles = dnx_data_qos.qos.nof_egress_ttl_pipe_profiles_get(unit);
    sal_strncpy(template_data.name, DNX_ALGO_QOS_EGRESS_TTL_PIPE_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.create(unit, &template_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * } 
 */
