/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_out_lif_profile.c
 *
 * Wrapper functions for utilex_multi_set.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/algo/lif_mngr/algo_out_lif_profile.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_lif_profile_access.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>

/**
 * \see
 *   algo_out_lif_profile.h file
 */
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * DEFINES
 * {
 */
/*
 * }
 */

void
dnx_algo_etpp_out_lif_profile_print_entry_cb(
    int unit,
    const void *data)
{
    etpp_out_lif_template_data_t *template_data = (etpp_out_lif_template_data_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "out lif orientation",
                                   template_data->out_lif_orientation, NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "split horizon enable",
                                   template_data->etpp_split_horizon_enable, NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "oam lif profile",
                                   template_data->oam_lif_profile, NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_FINISH();
}

void
dnx_algo_erpp_out_lif_profile_print_entry_cb(
    int unit,
    const void *data)
{
    erpp_out_lif_template_data_t *template_data = (erpp_out_lif_template_data_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "TTL Scope Threshold",
                                   template_data->ttl_scope_threshold, NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_FINISH();
}
/**
 * \brief
 *  Create Template Manager for out_lif_profile, the template is created with the following properties:
 *  entries : each entry represents OUT-LIF properties, more than one out_lif_profile can point to it.
 *  key : the key represents OUT_LIF_PROFILE
 *  the template has a default profile which will always exist in index 0.
 *  max references per profile > number of keys,this is in order to always keep the default profile.
 *  this way no matter how many template exchanges we will do, the default profile won't reach 0 references.
 *  according to the user's flags the profile will be decided and allocated
 *  (in case it doesnt exist already).
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_etpp_out_lif_profile_template_create(
    int unit)
{
    dnx_algo_template_create_data_t data;
    etpp_out_lif_template_data_t etpp_default_template_data;
    smart_template_create_info_t extra_info;
    resource_tag_bitmap_tag_info_t tag_info;
    uint32 ip_orientation_max_val = 0;
    uint32 ip_orientation_min_val = 0;
    smart_template_alloc_info_t exchange_tag_info;
    int new_etpp_out_lif_profile = 0;
    uint8 is_first = 0, is_last = 0;
    etpp_out_lif_template_data_t etpp_out_lif_profile_template_data;
    uint32 max_nof_refs;
    SHR_FUNC_INIT_VARS(unit);

    max_nof_refs = dnx_data_lif.out_lif.nof_local_out_lifs_get(unit);
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_EEDB_IPV4_TUNNEL,
                                                          DBAL_FIELD_OUT_LIF_PROFILE,
                                                          FALSE,
                                                          DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL,
                                                          0, DBAL_PREDEF_VAL_MAX_VALUE, &ip_orientation_max_val));
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_EEDB_IPV4_TUNNEL,
                                                          DBAL_FIELD_OUT_LIF_PROFILE,
                                                          FALSE,
                                                          DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL,
                                                          0, DBAL_PREDEF_VAL_MIN_VALUE, &ip_orientation_min_val));
    /*
     * initialize the default template data with default values
     */
    sal_memset(&extra_info, 0, sizeof(smart_template_create_info_t));
    sal_memset(&tag_info, 0, sizeof(resource_tag_bitmap_tag_info_t));
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    sal_memset(&exchange_tag_info, 0, sizeof(smart_template_alloc_info_t));
    sal_memset(&etpp_out_lif_profile_template_data, 0, sizeof(etpp_out_lif_template_data_t));
    sal_memset(&etpp_default_template_data, 0, sizeof(etpp_out_lif_template_data_t));
    etpp_default_template_data.out_lif_orientation = DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION;
    etpp_default_template_data.etpp_split_horizon_enable = DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_DISABLE;
    etpp_default_template_data.oam_lif_profile = DNX_OUT_LIF_PROFILE_OAM_PROFILE;
    etpp_default_template_data.keep_internal_default_profile = 0;
    data.data_size = sizeof(etpp_out_lif_template_data_t);
    data.default_data = &etpp_default_template_data;
    data.first_profile = DNX_ALGO_ETPP_OUT_LIF_DEFAULT_PROFILE;
    data.default_profile = DNX_ALGO_ETPP_OUT_LIF_DEFAULT_PROFILE;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE | DNX_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM;
    data.nof_profiles = 64;
    /*
     * Multiply max_references by number of etpp outlif number of ranges in smart template manager
     * This is needed because the smart template can support only one default profile
     * instead of several default profiles per range
     */
    data.max_references = max_nof_refs * DNX_ALGO_ETPP_OUT_LIF_PROFILE_SMART_TEMPLATE_NOF_RANGES;
    data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE;
    sal_strncpy(data.name, DNX_ALGO_OUT_LIF_PROFILE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    /** Use the smart tempate algorithm to allocate template and resource bitmap for range allocation*/
    extra_info.resource_flags = 0;
    extra_info.resource_create_info.max_tag_value = 2;
    extra_info.resource_create_info.grain_size = 1;
    SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.create(unit, &data, (void *) &extra_info));

    tag_info.tag = DNX_ALGO_OUT_LIF_TAG_GENERAL_PROFILE;
    tag_info.force_tag = FALSE;
    tag_info.element = 0;
    tag_info.nof_elements = 52;
    SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.advanced_algorithm_info_set(unit, &tag_info));

    tag_info.tag = DNX_ALGO_OUT_LIF_TAG_MPLS_PROFILE;
    tag_info.force_tag = FALSE;
    tag_info.element = 52;
    tag_info.nof_elements = 4;
    SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.advanced_algorithm_info_set(unit, &tag_info));

    tag_info.tag = DNX_ALGO_OUT_LIF_TAG_IP_TUNNEL_PROFILE;
    tag_info.force_tag = FALSE;
    tag_info.element = ip_orientation_min_val;
    tag_info.nof_elements = (ip_orientation_max_val - ip_orientation_min_val) + 1;
    SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.advanced_algorithm_info_set(unit, &tag_info));

    exchange_tag_info.resource_flags = RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;
    etpp_out_lif_profile_template_data.keep_internal_default_profile =
        DNX_OUT_LIF_PROFILE_KEEP_DEFAULT_IP_TUNNEL_PROFILE;
    exchange_tag_info.resource_alloc_info.tag = DNX_ALGO_OUT_LIF_TAG_IP_TUNNEL_PROFILE;
    /** Allocate default profile with max refs for IP TUNNEL in the etpp outlif smart template */
    exchange_tag_info.nof_refs = max_nof_refs;
    SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.exchange
                    (unit, 0, &etpp_out_lif_profile_template_data, DNX_OUT_LIF_PROFILE_DEFAULT,
                     &exchange_tag_info, &new_etpp_out_lif_profile, &is_first, &is_last));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Create Template Manager for erpp_out_lif_profile, the template is created with the following properties:
 *  entries : each entry represents ERPP OUT-LIF properties, more than one out_lif_profile can point to it.
 *  key : the key represents ERPP_OUT_LIF_PROFILE
 *  the template has a default profile which will always exist in index 0.
 *  max references per profile > number of keys,this is in order to always keep the default profile.
 *  this way no matter how many template exchanges we will do, the default profile won't reach 0 references.
 *  according to the user's flags the profile will be decided and allocated
 *  (in case it doesnt exist already).
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_erpp_out_lif_profile_template_create(
    int unit)
{
    erpp_out_lif_template_data_t erpp_out_lif_template_data;
    dnx_algo_template_create_data_t data;
    int system_headers_mode, iop_mode_outlif_selection;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize the default template data with default values
     */
    sal_memset(&erpp_out_lif_template_data, 0, sizeof(erpp_out_lif_template_data_t));
    erpp_out_lif_template_data.ttl_scope_threshold = DNX_OUT_LIF_PROFILE_DEFAULT_TTL_THRESHOLD;
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    iop_mode_outlif_selection = dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_outlif_selection);
    /*
     * initialize the templates data/properties
     */
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.data_size = sizeof(erpp_out_lif_template_data_t);
    data.default_data = &erpp_out_lif_template_data;
    data.default_profile = 0;
    data.first_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    /** Due to HW issue on IOP mode, the RIFs are added in the namespace of the LIF and have only 1 bit for lif profile*/
    if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ||
        (iop_mode_outlif_selection == FALSE))
    {
        data.nof_profiles = dnx_data_lif.out_lif.nof_erpp_out_lif_profiles_get(unit);
    }
    else
    {
        data.nof_profiles = 2;
    }
    data.max_references = dnx_data_l3.rif.nof_rifs_get(unit);
    sal_strncpy(data.name, DNX_ALGO_ERPP_OUT_LIF_PROFILE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_lif_profile_db.erpp_out_lif_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   algo_out_lif_profile.h file
 */
shr_error_e
dnx_algo_out_lif_profile_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize ERPP and ETPP out_lif_profile templates.
     */
    SHR_IF_ERR_EXIT(dnx_algo_etpp_out_lif_profile_template_create(unit));
    SHR_IF_ERR_EXIT(dnx_algo_erpp_out_lif_profile_template_create(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
