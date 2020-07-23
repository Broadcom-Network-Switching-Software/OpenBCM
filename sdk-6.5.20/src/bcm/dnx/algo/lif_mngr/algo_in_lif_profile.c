/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_in_lif_profile.c
 *
 * Wrapper functions for utilex_multi_set.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_in_lif_profile.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_lif_profile_access.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>

/**
 * \see
 *   algo_in_lif_profile.h file
 */
void
dnx_algo_in_lif_profile_l3_enablers_profile_print_entry_cb(
    int unit,
    const void *data)
{
    uint32 *enablers_vector = (uint32 *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "enabler vector MSB",
                                   enablers_vector[1], NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "enablers vector LSB",
                                   enablers_vector[0], NULL, "0x%08x");

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \see
 *   algo_in_lif_profile.h file
 */
void
dnx_algo_in_lif_profile_in_lif_lb_profile_print_entry_cb(
    int unit,
    const void *data)
{
    uint32 *layers_disable = (uint32 *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Layers Disable", *layers_disable, NULL,
                                   "0x%08x");

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \see
 *   algo_in_lif_profile.h file
 */
void
dnx_algo_in_lif_profile_print_entry_cb(
    int unit,
    const void *data)
{
    in_lif_template_data_t *template_data = (in_lif_template_data_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "routing enablers profile",
                                   template_data->routing_enablers_profile, NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "rpf mode", template_data->rpf_mode,
                                   NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "cs_in_lif_profile",
                                   template_data->cs_in_lif_profile, NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "cs_in_lif_fwd_profile",
                                   template_data->cs_in_lif_fwd_profile, NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "da_not_found_dst_profile",
                                   template_data->da_not_found_dst_profile, NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "mact_mode",
                                   template_data->mact_mode, NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "l2_v4_mc_fwd_type",
                                   template_data->l2_v4_mc_fwd_type, NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "l2_v6_mc_fwd_type",
                                   template_data->l2_v6_mc_fwd_type, NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "oam_default_profile",
                                   template_data->oam_default_profile, NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "in_lif_lb_profile",
                                   template_data->in_lif_lb_profile, NULL, NULL);
    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \see
 *   algo_in_lif_profile.h file
 */
void
dnx_algo_in_lif_flood_profile_print_entry_cb(
    int unit,
    const void *data)
{
    dnx_default_frwrd_dst_t *template_data = NULL;
    dnx_default_frwrd_dst_t *default_dst_data = (dnx_default_frwrd_dst_t *) data;

    template_data = &default_dst_data[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_UC];

    DNX_SW_STATE_PRINT(unit,
                       "Default forward destinations for unknown unicast packets: destination = 0x%08x, offset = 0x%08x, add_vsi_dst = 0x%08x",
                       template_data->destination, template_data->offset, template_data->add_vsi_dst);

    template_data = &default_dst_data[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC];

    DNX_SW_STATE_PRINT(unit,
                       "Default forward destinations for unknown multicast packets: destination = 0x%08x, offset = 0x%08x, add_vsi_dst = 0x%08x",
                       template_data->destination, template_data->offset, template_data->add_vsi_dst);

    template_data = &default_dst_data[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_BC];

    DNX_SW_STATE_PRINT(unit,
                       "Default forward destinations for broadcast packets: destination = 0x%08x, offset = 0x%08x, add_vsi_dst = 0x%08x",
                       template_data->destination, template_data->offset, template_data->add_vsi_dst);

    return;
}

/**
 * \brief
 * Create Template Manager for L3 enablers_vector, with the following properties:
 * * entries : each entry is an enablers vector, more than one RIF profile can point to it.
 * * key : the key represents ETH_RIF_Profile
 * * the template has a default profile which will always exist in index 0.
 * * max references per profile is number of keys + 1,this is in order to always keep the default profile.
 *   this way no matter how many template exchanges we will do, the default profile won't reach 0 references.
 * * according to the user's flags BCM_L3_INGRESS_ROUTE_DISABLE* the profile will be decided and allocated
 *   (in case it doesn't exist already).
 * * a profile will be deleted when the API bcm_l3_intf_delete will be called and no more references
 *   exist for this profile
 * \par DIRECT_INPUT:
 *   \param [in] unit - the unit number
 * \par INDIRECT INPUT:
 *   * The definition DNX_ALGO_L3_ENABLERS_VECTOR_TABLE_RESOURCE
 *   * The definition DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE
 *   * The definition NOF_ENTRIES_ENABLERS_VECTOR
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT OUTPUT
 *   * The new created template
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_l3_enablers_vector_template_create(
    int unit)
{
    dnx_algo_template_create_data_t data;
    int nof_inlif1_lifs, nof_inlif2_lifs, nof_cores, nof_references;
    uint32 initial_enablers_vector[2] = { INITIAL_ENABLERS_VECTOR_LSB, INITIAL_ENABLERS_VECTOR_MSB };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * enablers vector table contains 32 entries, first entry will be used as default entry with value:
     * 0x0000000000000004 (can't be deleted), the rest of the entries will be assigned dynamically
 */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &nof_inlif1_lifs, &nof_inlif2_lifs));
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_references = nof_inlif1_lifs + nof_inlif2_lifs * nof_cores + dnx_data_l3.rif.nof_rifs_get(unit);
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.data_size = 2 * sizeof(uint32);
    data.default_data = initial_enablers_vector;
    data.default_profile = 0;
    data.first_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.nof_profiles = NOF_ENTRIES_ENABLERS_VECTOR;
    data.max_references = nof_references;
    sal_strncpy(data.name, DNX_ALGO_IN_LIF_PROFILE_L3_ENABLERS, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile_l3_enablers.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create Template Manager for LB LIF Profile enablers:
 */
static shr_error_e
dnx_algo_lif_lb_profile_template_create(
    int unit)
{
    dnx_algo_template_create_data_t data;
    int default_data;
    SHR_FUNC_INIT_VARS(unit);

    default_data = INITIAL_IN_LIF_LB_ENABLERS_VECTOR;
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.data_size = sizeof(uint32);
    data.default_data = &default_data;
    data.default_profile = 0;
    data.first_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.nof_profiles = 4;
    data.max_references = dnx_data_l3.rif.nof_rifs_get(unit);
    sal_strncpy(data.name, DNX_ALGO_IN_LIF_LB_PROFILE_ENABLERS, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_lb_profile_enablers.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}
/** } **/

/**
 * \brief
 *   Create Template Manager for in_lif_profile, the template is created with the following properties:
 * * entries : each entry represents LIF properties, more than one LIF profile can point to it.
 * * key : the key represents IN_LIF_PROFILE
 * * the template has a default profile which will always exist in index 0.
 * * max references per profile = number of keys
 * * according to the user's flags the profile will be decided and allocated
 *   (in case it doesn't exist already).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     the unit number
 * \par INDIRECT INPUT
 *   * The definition DEFAULT_ROUTING_ENABLERS_PROFILE
 *   * The definition DEFAULT_RPF_MODE
 *   * The definition DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE
 *   * The definition DNX_ALGO_IN_LIF_PROFILE
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * The new created template
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_in_lif_profile_template_create(
    int unit)
{
    uint8 fwd_type_sel;
    dnx_algo_template_create_data_t data;
    in_lif_template_data_t default_template_data;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize the default template data with default values
     */
    sal_memset(&default_template_data, 0, sizeof(default_template_data));
    default_template_data.routing_enablers_profile = DEFAULT_ROUTING_ENABLERS_PROFILE;
    default_template_data.rpf_mode = DEFAULT_RPF_MODE;
    default_template_data.cs_in_lif_profile = DNX_IN_LIF_PROFILE_CS_DEFAULT;
    default_template_data.cs_in_lif_fwd_profile = DNX_IN_LIF_FWD_PROFILE_CS_DEFAULT;
    default_template_data.mact_mode = DBAL_ENUM_FVAL_VSI_PROFILE_BRIDGE_SVL;
    SHR_IF_ERR_EXIT(switch_db.l3mcastl2_fwd_type.get(unit, &fwd_type_sel));
    default_template_data.l2_v4_mc_fwd_type = fwd_type_sel;
    default_template_data.l2_v6_mc_fwd_type = fwd_type_sel;
    default_template_data.oam_default_profile = DEFAULT_OAM_PROFILE;
    default_template_data.protocol_trap_profiles.arp_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.dhcp_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.icmpv4_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.icmpv6_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.igmp_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.l2cp_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.non_auth_8021x_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.in_lif_lb_profile = DEFAULT_IN_LIF_LB_PROFILE;
    /*
     * initialize the templates data/properties
     */
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.data_size = sizeof(default_template_data);
    data.default_data = &default_template_data;
    data.default_profile = 0;
    data.first_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.nof_profiles = NOF_IN_LIF_PROFILES;
    if ((data.nof_profiles <= 0) || (data.nof_profiles > (1 << NOF_IN_LIF_PROFILE_BITS)))
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%d: Outer DP profile: %d, Inner DP profile: %d, Orientation: %d\n", unit,
                    IVE_OUTER_QOS_DP_PROFILE_NOF_BITS, IVE_INNER_QOS_DP_PROFILE_NOF_BITS, IN_LIF_ORIENTATION_NOF_BITS);
        LOG_INFO_EX(BSL_LOG_MODULE, "%d: Same interface: %d,   Index mode: %d,       PMF reserved: %d\n", unit,
                    SAME_INTERFACE_MODE_NOF_BITS, NATIVE_INDEXED_MODE_NOF_BITS, IN_LIF_PROFILE_PMF_RESERVED_NOF_BITS);
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Too more SOC property enabled\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &data.max_references, NULL));
    sal_strncpy(data.name, DNX_ALGO_IN_LIF_PROFILE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create Template Manager for eth_rif_profile, the template is created with the following properties:
 * * entries : each entry represents ETH_RIF properties, more than one ETH_RIF profile can point to it.
 * * key : the key represents ETH_RIF_PROFILE
 * * the template has a default profile which will always exist in index 0.
 * * max references per profile = number of keys
 * * according to the user's flags the profile will be decided and allocated
 *   (in case it doesnt exist already).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     the unit number
 * \par INDIRECT INPUT
 *   * The definition DEFAULT_ROUTING_ENABLERS_PROFILE
 *   * The definition DEFAULT_RPF_MODE
 *   * The definition DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE
 *   * The definition DNX_ALGO_ETH_RIF_PROFILE
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * The new created template
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_eth_rif_profile_template_create(
    int unit)
{
    uint8 fwd_type_sel;
    dnx_algo_template_create_data_t data;
    in_lif_template_data_t default_template_data;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize the default template data with default values
     */
    sal_memset(&default_template_data, 0, sizeof(default_template_data));
    default_template_data.routing_enablers_profile = DEFAULT_ROUTING_ENABLERS_PROFILE;
    default_template_data.rpf_mode = DEFAULT_RPF_MODE;
    default_template_data.cs_in_lif_profile = DNX_IN_LIF_PROFILE_CS_DEFAULT;
    default_template_data.cs_in_lif_fwd_profile = DNX_IN_LIF_FWD_PROFILE_CS_DEFAULT;
    default_template_data.mact_mode = DBAL_ENUM_FVAL_VSI_PROFILE_BRIDGE_SVL;
    SHR_IF_ERR_EXIT(switch_db.l3mcastl2_fwd_type.get(unit, &fwd_type_sel));
    default_template_data.l2_v4_mc_fwd_type = fwd_type_sel;
    default_template_data.l2_v6_mc_fwd_type = fwd_type_sel;
    default_template_data.oam_default_profile = DEFAULT_OAM_PROFILE;
    default_template_data.protocol_trap_profiles.arp_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.dhcp_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.icmpv4_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.icmpv6_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.igmp_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.l2cp_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.protocol_trap_profiles.non_auth_8021x_trap_profile = DEFAULT_TRAP_PROFILES_DATA;
    default_template_data.in_lif_lb_profile = DEFAULT_IN_LIF_LB_PROFILE;
    /*
     * initialize the templates data/properties
     */
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.data_size = sizeof(default_template_data);
    data.default_data = &default_template_data;
    data.default_profile = 0;
    data.first_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.nof_profiles = NOF_ETH_RIF_PROFILES;
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &data.max_references, NULL));
    sal_strncpy(data.name, DNX_ALGO_ETH_RIF_PROFILE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_lif_profile_db.eth_rif_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Create the template for inlif flood profile.
 *
 * \param [in] unit - The unit ID
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   Create the template with number of profiles available defined in DNX.
 *   Meanwhile, the default in-lif-flood-profile is allocated and will be used
 *   everywhere.
 *
 * \see
 *   None.
 */
static shr_error_e
dnx_algo_in_lif_flood_profile_template_create(
    int unit)
{
    dnx_algo_template_create_data_t data;
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES];
    int nof_inlif1_lifs, nof_inlif2_lifs, nof_cores, nof_references;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize the default template data with default values
     */
    sal_memset(&default_frwrd_dst, 0, sizeof(default_frwrd_dst));
    default_frwrd_dst[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_MC].add_vsi_dst = TRUE;
    default_frwrd_dst[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_BC].add_vsi_dst = TRUE;
    default_frwrd_dst[DBAL_ENUM_FVAL_DESTINATION_DISTRIBUTION_TYPE_UC].add_vsi_dst = TRUE;

    /*
     * initialize the templates data/properties
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &nof_inlif1_lifs, &nof_inlif2_lifs));
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_references = nof_inlif1_lifs + nof_inlif2_lifs * nof_cores;

    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.data_size = sizeof(default_frwrd_dst);
    data.default_data = default_frwrd_dst;
    data.default_profile = dnx_data_lif.in_lif.default_da_not_found_destination_profile_get(unit);
    data.first_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.nof_profiles = dnx_data_lif.in_lif.nof_da_not_found_destination_profiles_get(unit);

    data.max_references = nof_references;
    sal_strncpy(data.name, DNX_ALGO_IN_LIF_DA_NOT_FOUND_DESTINATION, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_lif_profile_db.in_lif_da_not_found_destination.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   algo_in_lif_profile.h file
 */
shr_error_e
dnx_algo_in_lif_profile_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_lif_profile_db.init(unit));
    /*
     * Initialize in_lif_profile_high template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_in_lif_profile_template_create(unit));

    /*
     * Initialize l3 enablers vector template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_enablers_vector_template_create(unit));
    /*
     * Initialize lif_lb enablers vector template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_lb_profile_template_create(unit));
    /*
     * Initialize in_lif_profile_low template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_eth_rif_profile_template_create(unit));

    /*
     * Initialize da_not_found_dst_profile template.
     */
    SHR_IF_ERR_EXIT(dnx_algo_in_lif_flood_profile_template_create(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
