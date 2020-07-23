/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_bfd.c
 *
 * Resource and templates needed for the BFD feature.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <bcm/types.h>
#include <bcm_int/dnx/algo/bfd/algo_bfd.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_bfd_access.h>

/** Callbacks needed for the template used in BFD   */

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_mep_action_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_bfd_mep_profile_t *bfd_action = (dnx_bfd_mep_profile_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "trpcd", bfd_action->trap_code, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "fwdstrng", bfd_action->forwarding_stregth,
                                   NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "snpstrn", bfd_action->snoop_strength, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_server_trap_codes_print_cb(
    int unit,
    const void *data)
{
    uint16 *trap_code = (uint16 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "bfd_server_trap_code", trap_code[0], NULL,
                                   "0x%04X");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_dip_profile_print_cb(
    int unit,
    const void *data)
{
    bcm_ip6_t *ip = (bcm_ip6_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_IPV6, "destination IP", *ip, NULL, NULL);
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_your_discr_range_profile_print_cb(
    int unit,
    const void *data)
{
    uint32 *min_max = (uint32 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "minimum", min_max[0], NULL, "0x%08X");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "maximum", min_max[1], NULL, "0x%08X");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_tx_rate_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_bfd_tx_period_t *tx_rate_data = (dnx_bfd_tx_period_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "tx rate", tx_rate_data->tx_period, NULL,
                                   "0x%08X");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "cluster profile", tx_rate_data->cluster_profile,
                                   NULL, "0x%08X");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_req_int_profile_print_cb(
    int unit,
    const void *data)
{
    uint32 *req_int = (uint32 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "required interval", req_int[0], NULL, "0x%08X");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_discr_range_start_profile_print_cb(
    int unit,
    const void *data)
{
    uint32 *range_start = (uint32 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "range start", range_start[0], NULL, "0x%08X");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_tos_ttl_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_bfd_tos_ttl_t *tos_ttl = (dnx_bfd_tos_ttl_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "Type of Service", tos_ttl->tos, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "Time to Live", tos_ttl->ttl, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_ipv4_src_addr_profile_print_cb(
    int unit,
    const void *data)
{
    bcm_ip_t *ip = (bcm_ip_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_IPV4, "source IP", ip[0], NULL, NULL);
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_udp_src_port_profile_print_cb(
    int unit,
    const void *data)
{
    uint16 *sport = (uint16 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "source port", sport[0], NULL, "0x%04X");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_mpls_src_port_profile_print_cb(
    int unit,
    const void *data)
{
    uint16 *sport = (uint16 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "source port", sport[0], NULL, "0x%04X");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_oamp_single_hop_tos_profile_print_cb(
    int unit,
    const void *data)
{
    uint8 *tos = (uint8 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "type of service", tos[0], NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_oamp_bfd_local_min_echo_print_cb(
    int unit,
    const void *data)
{
    uint32 *local_min_echo = (uint32 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "local min echo", local_min_echo[0], NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_oamp_bfd_tx_period_cluster_print_cb(
    int unit,
    const void *data)
{
    uint8 *tx_period_cluster = (uint8 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8,
                                   "Tx period cluster", tx_period_cluster[0], NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_bfd.h
 */
void
dnx_bfd_seamless_udp_src_port_profile_print_cb(
    int unit,
    const void *data)
{
    uint16 *sport = (uint16 *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "seamless BFD source port", sport[0], NULL,
                                   "0x%04X");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/**
 * \brief - This function creates the templates needed for the 
 *        BFD feature
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * Four templates are created:\n
 *      BFD Non-Accelerated MEP profiles: mirror the entries in
 *      the OAMA available for BFD profiles\n
 *      BFD Accelerated MEP profiles: mirror the entries in
 *      the OAMB available for BFD profiles\n
 *      BFD DIPs - mirror the entries in the IPPB_MY_BFD_DIP
 *      hardware table\n
 *      BFD Discriminator range - used to make sure that new
 *      endpoints that receive packets that are classified by
 *      discriminator has a local discriminator that falls
 *      within the allowed range, or to define such a range if
 *      not defined yet or if all reference to the existing
 *      range are deleted.
 *  
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_bfd_init_templates(
    int unit)
{
    dnx_algo_template_create_data_t bfd_dip_mep_profile_data, bfd_dip_template_data,
        bfd_server_trap_codes_template_data, bfd_your_dis_range_data, bfd_local_min_echo_data;
    uint32 nof_non_acc_profiles, nof_acc_profiles, nof_references;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_non_acc_profiles = dnx_data_oam.general.oam_nof_acc_action_profiles_get(unit);
    nof_acc_profiles = dnx_data_oam.general.oam_nof_acc_action_profiles_get(unit);
    nof_references = dnx_data_bfd.general.nof_endpoints_get(unit);

    /** Create template for BFD OAMA Profiles */
    sal_memset(&bfd_dip_mep_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_dip_mep_profile_data.flags = 0;
    bfd_dip_mep_profile_data.first_profile = 1;
    bfd_dip_mep_profile_data.nof_profiles = nof_non_acc_profiles;
    bfd_dip_mep_profile_data.max_references = nof_references;
    bfd_dip_mep_profile_data.default_profile = 0;
    bfd_dip_mep_profile_data.data_size = sizeof(dnx_bfd_mep_profile_t);
    bfd_dip_mep_profile_data.default_data = NULL;
    sal_strncpy(bfd_dip_mep_profile_data.name, DNX_BFD_TEMPLATE_OAMA_ACTION,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oama_action.create(unit, &bfd_dip_mep_profile_data, NULL));

    /** Create template for BFD OAMB Profiles */
    bfd_dip_mep_profile_data.flags = 0;
    bfd_dip_mep_profile_data.first_profile = 0;
    bfd_dip_mep_profile_data.nof_profiles = nof_acc_profiles;
    bfd_dip_mep_profile_data.max_references = nof_references;
    bfd_dip_mep_profile_data.default_profile = 0;
    bfd_dip_mep_profile_data.data_size = sizeof(dnx_bfd_mep_profile_t);
    bfd_dip_mep_profile_data.default_data = NULL;
    sal_strncpy(bfd_dip_mep_profile_data.name, DNX_BFD_TEMPLATE_OAMB_ACTION,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamb_action.create(unit, &bfd_dip_mep_profile_data, NULL));

    /** Create template for BFD DIPs */
    sal_memset(&bfd_dip_template_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_dip_template_data.flags = 0;
    bfd_dip_template_data.first_profile = 0;
    bfd_dip_template_data.nof_profiles = dnx_data_bfd.general.nof_dips_for_bfd_multihop_get(unit);
    bfd_dip_template_data.max_references = nof_references;
    bfd_dip_template_data.default_profile = 0;
    bfd_dip_template_data.data_size = sizeof(bcm_ip6_t);
    bfd_dip_template_data.default_data = NULL;
    sal_strncpy(bfd_dip_template_data.name, DNX_BFD_TEMPLATE_DIP, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_dip.create(unit, &bfd_dip_template_data, NULL));

    /** Create template for BFD server trap codes */
    sal_memset(&bfd_server_trap_codes_template_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_server_trap_codes_template_data.flags = 0;
    bfd_server_trap_codes_template_data.first_profile = 0;
    bfd_server_trap_codes_template_data.nof_profiles = dnx_data_bfd.general.nof_bfd_server_trap_codes_get(unit);
    bfd_server_trap_codes_template_data.max_references = nof_references;
    bfd_server_trap_codes_template_data.default_profile = 0;
    bfd_server_trap_codes_template_data.data_size = sizeof(uint16);
    bfd_server_trap_codes_template_data.default_data = NULL;
    sal_strncpy(bfd_server_trap_codes_template_data.name, DNX_BFD_SERVER_TRAP_CODES,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_server_trap_codes.create(unit, &bfd_server_trap_codes_template_data, NULL));

    /** Create template for BFD discriminator range */
    sal_memset(&bfd_your_dis_range_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_your_dis_range_data.flags = 0;
    bfd_your_dis_range_data.first_profile = 0;
    bfd_your_dis_range_data.nof_profiles = 1;
    bfd_your_dis_range_data.max_references = nof_references;
    bfd_your_dis_range_data.default_profile = 0;
    bfd_your_dis_range_data.data_size = sizeof(dnx_bfd_discr_range_t);
    bfd_your_dis_range_data.default_data = NULL;
    sal_strncpy(bfd_your_dis_range_data.name, DNX_BFD_TEMPLATE_YOUR_DISCR_RANGE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_tyour_discr_range.create(unit, &bfd_your_dis_range_data, NULL));

    /** Create template for BFD OAMP one-hop TOS */
    sal_memset(&bfd_your_dis_range_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_your_dis_range_data.flags = 0;
    bfd_your_dis_range_data.first_profile = 0;
    bfd_your_dis_range_data.nof_profiles = 1;
    bfd_your_dis_range_data.max_references = nof_references;
    bfd_your_dis_range_data.default_profile = 0;
    bfd_your_dis_range_data.data_size = sizeof(uint8);
    bfd_your_dis_range_data.default_data = NULL;
    sal_strncpy(bfd_your_dis_range_data.name, DNX_BFD_TEMPLATE_OAMP_SINGLE_HOP_TOS,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_single_hop_tos.create(unit, &bfd_your_dis_range_data, NULL));

    /** Create template for BFD local min echo */
    sal_memset(&bfd_local_min_echo_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_local_min_echo_data.flags = 0;
    bfd_local_min_echo_data.first_profile = 0;
    bfd_local_min_echo_data.nof_profiles = 1;
    bfd_local_min_echo_data.max_references = nof_references;
    bfd_local_min_echo_data.default_profile = 0;
    bfd_local_min_echo_data.data_size = sizeof(uint32);
    bfd_local_min_echo_data.default_data = NULL;
    sal_strncpy(bfd_local_min_echo_data.name, DNX_BFD_TEMPLATE_LOCAL_MIN_ECHO,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_local_min_echo.create(unit, &bfd_local_min_echo_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the templates needed for the
 *        accelerated BFD feature
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Five templates are created:\n
 *      BFD OAMP Tx rate: mirror the entries
 *      available for accelerated BFD Tx profiles\n
 *      BFD OAMP Req Int: mirror the entries available for
 *      accelerated BFD intervals (Rx/Tx packet intervals)\n
 *      BFD OAMP Discriminator range start data - used to calculate
 *      my discriminator value for transmitted BFD packets.  Most
 *      significant 16 bits come from this value.  Least significant
 *      16 bits are endpoint OAM_ID.\n
 *      BFD OAMP TOS/TTL: mirror the entries available for accelerated
 *      BFD Type of Service and Time To Live value pairs.
 *      BFD OAMP IPv4 Src Addr: mirror the entries available for accelerated
 *      BFD IPv4 source addresses.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_bfd_oamp_init_templates(
    int unit)
{
    dnx_algo_template_create_data_t bfd_tx_rate_profile_data, bfd_req_int_profile_data, bfd_my_dis_range_start_data;
    dnx_algo_template_create_data_t bfd_tos_ttl_data, bfd_src_addr_data, bfd_udp_src_port_data, bfd_mpls_src_port_data;
    dnx_algo_template_create_data_t bfd_tx_period_cluster_data, seamless_bfd_udp_src_port_data;
    uint32 nof_acc_profiles, nof_references, nof_req_int_profiles;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_acc_profiles = dnx_data_oam.general.oam_nof_acc_action_profiles_get(unit);
    nof_references = dnx_data_bfd.general.nof_endpoints_get(unit);
    nof_req_int_profiles = dnx_data_bfd.general.nof_req_int_profiles_get(unit);

    /** Create template for BFD Tx rate   */
    sal_memset(&bfd_tx_rate_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_tx_rate_profile_data.flags = 0;
    bfd_tx_rate_profile_data.first_profile = 0;
    bfd_tx_rate_profile_data.nof_profiles = dnx_data_bfd.general.nof_profiles_for_tx_rate_get(unit);
    bfd_tx_rate_profile_data.max_references = nof_references;
    bfd_tx_rate_profile_data.default_profile = 0;
    bfd_tx_rate_profile_data.data_size = sizeof(dnx_bfd_tx_period_t);
    bfd_tx_rate_profile_data.default_data = NULL;
    sal_strncpy(bfd_tx_rate_profile_data.name, DNX_BFD_OAMP_TEMPLATE_TX_RATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tx_rate.create(unit, &bfd_tx_rate_profile_data, NULL));

    /** Create template for BFD required intervals  */
    sal_memset(&bfd_req_int_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_req_int_profile_data.flags = 0;
    bfd_req_int_profile_data.first_profile = 0;
    bfd_req_int_profile_data.nof_profiles = nof_req_int_profiles;
    bfd_req_int_profile_data.max_references = nof_references;
    bfd_req_int_profile_data.default_profile = 0;
    bfd_req_int_profile_data.data_size = sizeof(uint32);
    bfd_req_int_profile_data.default_data = NULL;
    sal_strncpy(bfd_req_int_profile_data.name, DNX_BFD_OAMP_TEMPLATE_REQ_INTERVAL,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_req_interval.create(unit, &bfd_req_int_profile_data, NULL));

    /** Create template for BFD required intervals  */
    sal_memset(&bfd_my_dis_range_start_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_my_dis_range_start_data.flags = 0;
    bfd_my_dis_range_start_data.first_profile = 0;
    bfd_my_dis_range_start_data.nof_profiles = 1;
    bfd_my_dis_range_start_data.max_references = nof_references;
    bfd_my_dis_range_start_data.default_profile = 0;
    bfd_my_dis_range_start_data.data_size = sizeof(uint32);
    bfd_my_dis_range_start_data.default_data = NULL;
    sal_strncpy(bfd_my_dis_range_start_data.name, DNX_BFD_OAMP_TEMPLATE_MY_DISCR_RANGE_START,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_my_discr_range_start.create(unit, &bfd_my_dis_range_start_data, NULL));

    /** Create template for BFD ToS and TTL   */
    sal_memset(&bfd_tos_ttl_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_tos_ttl_data.flags = 0;
    bfd_tos_ttl_data.first_profile = 0;
    bfd_tos_ttl_data.nof_profiles = nof_acc_profiles;
    bfd_tos_ttl_data.max_references = nof_references;
    bfd_tos_ttl_data.default_profile = 0;
    bfd_tos_ttl_data.data_size = sizeof(uint16);
    bfd_tos_ttl_data.default_data = NULL;
    sal_strncpy(bfd_tos_ttl_data.name, DNX_BFD_OAMP_TEMPLATE_TOS_TTL, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_tos_ttl.create(unit, &bfd_tos_ttl_data, NULL));

    /** Create template for BFD IPv4 source address   */
    sal_memset(&bfd_src_addr_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_src_addr_data.flags = 0;
    bfd_src_addr_data.first_profile = 0;
    bfd_src_addr_data.nof_profiles = 256;
    bfd_src_addr_data.max_references = nof_references;
    bfd_src_addr_data.default_profile = 0;
    bfd_src_addr_data.data_size = sizeof(bcm_ip_t);
    bfd_src_addr_data.default_data = NULL;
    sal_strncpy(bfd_src_addr_data.name, DNX_BFD_OAMP_TEMPLATE_IPV4_SRC_ADDR,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_ipv4_src_addr.create(unit, &bfd_src_addr_data, NULL));

    /** Create template for BFD UDP source port   */
    sal_memset(&bfd_udp_src_port_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_udp_src_port_data.flags = 0;
    bfd_udp_src_port_data.first_profile = 0;
    bfd_udp_src_port_data.nof_profiles = 1;
    bfd_udp_src_port_data.max_references = nof_references;
    bfd_udp_src_port_data.default_profile = 0;
    bfd_udp_src_port_data.data_size = sizeof(uint16);
    bfd_udp_src_port_data.default_data = NULL;
    sal_strncpy(bfd_udp_src_port_data.name, DNX_BFD_OAMP_TEMPLATE_UDP_SPORT,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_udp_sport.create(unit, &bfd_udp_src_port_data, NULL));

    /** Create template for BFD MPLS source port   */
    sal_memset(&bfd_mpls_src_port_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_mpls_src_port_data.flags = 0;
    bfd_mpls_src_port_data.first_profile = 0;
    bfd_mpls_src_port_data.nof_profiles = 1;
    bfd_mpls_src_port_data.max_references = nof_references;
    bfd_mpls_src_port_data.default_profile = 0;
    bfd_mpls_src_port_data.data_size = sizeof(uint16);
    bfd_mpls_src_port_data.default_data = NULL;
    sal_strncpy(bfd_mpls_src_port_data.name, DNX_BFD_OAMP_TEMPLATE_MPLS_SPORT,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_mpls_sport.create(unit, &bfd_mpls_src_port_data, NULL));

    sal_memset(&bfd_tx_period_cluster_data, 0, sizeof(dnx_algo_template_create_data_t));
    bfd_tx_period_cluster_data.flags = 0;
    bfd_tx_period_cluster_data.first_profile = 0;
    bfd_tx_period_cluster_data.nof_profiles = 8;
    bfd_tx_period_cluster_data.max_references = nof_references;
    bfd_tx_period_cluster_data.default_profile = 0;
    bfd_tx_period_cluster_data.data_size = sizeof(uint8);
    bfd_tx_period_cluster_data.default_data = NULL;
    sal_strncpy(bfd_tx_period_cluster_data.name, DNX_BFD_TEMPLATE_TX_PERIOD_CLUSTER,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_tx_period_cluster.create(unit, &bfd_tx_period_cluster_data, NULL));

    /** Create template for seamless BFD UDP source port   */
    sal_memset(&seamless_bfd_udp_src_port_data, 0, sizeof(dnx_algo_template_create_data_t));
    seamless_bfd_udp_src_port_data.flags = 0;
    seamless_bfd_udp_src_port_data.first_profile = 0;
    seamless_bfd_udp_src_port_data.nof_profiles = 1;
    seamless_bfd_udp_src_port_data.max_references = nof_references;
    seamless_bfd_udp_src_port_data.default_profile = 0;
    seamless_bfd_udp_src_port_data.data_size = sizeof(uint16);
    seamless_bfd_udp_src_port_data.default_data = NULL;
    sal_strncpy(seamless_bfd_udp_src_port_data.name, DNX_BFD_OAMP_TEMPLATE_SEAMLESS_UDP_SPORT,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_oamp_seamless_udp_sport.create(unit, &seamless_bfd_udp_src_port_data, NULL));

    /** Create an almost identical template for non-accelerated endpoints */
    sal_strncpy(seamless_bfd_udp_src_port_data.name, DNX_BFD_TEMPLATE_SEAMLESS_UDP_SPORT,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_bfd_db.bfd_seamless_udp_sport.create(unit, &seamless_bfd_udp_src_port_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_bfd_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize algo_bfd sw state module.
     */
    SHR_IF_ERR_EXIT(algo_bfd_db.init(unit));
    /*
     * Initialize bfd templates.
     */
    SHR_IF_ERR_EXIT(dnx_algo_bfd_init_templates(unit));

    /*
     * Initialize bfd OAMP templates.
     */
    SHR_IF_ERR_EXIT(dnx_algo_bfd_oamp_init_templates(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
