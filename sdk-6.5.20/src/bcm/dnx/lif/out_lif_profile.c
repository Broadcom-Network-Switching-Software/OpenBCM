/** \file out_lif_profile.c
 *
 *  out_lif profile management for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF
/*
 * Include files
 * {
 */
#include <bcm_int/dnx/algo/lif_mngr/algo_out_lif_profile.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/l3/l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_lif_profile_access.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>

/*
 * }
 */
/*
 * Defines and Macros
 * {
 */
/*
 * }
 */
/**
 * \brief - Configure ETPP_OUT_LIF_PROFILE_TABLE table 
 */
static shr_error_e
dnx_etpp_out_lif_profile_configure_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ETPP_OUT_LIF_PROFILE_TABLE init
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_ORIENTATION, INST_SINGLE,
                                 DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE,
                                 DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PRIORITY_MAP_PROFILE, INST_SINGLE,
                                 DNX_OUT_LIF_PROFILE_OAM_PROFILE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure ERPP_OUT_LIF_PROFILE_TABLE table 
 */
static shr_error_e
dnx_erpp_out_lif_profile_filters_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ERPP_OUT_LIF_PROFILE_TABLE init
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_MULTICAST_SAME_INTERFACE_FILTERS, INST_SINGLE,
                                 TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_UC_SAME_INTERFACE_FILTERS, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_TTL_SCOPE_INDEX, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_INDICATION_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RQP_DISCARD_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INVALID_OTM_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DSS_STACKING_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_MULTICAST_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXCLUDE_SRC_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_EQUALS_DESTINATION_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNACCEPTABLE_FRAME_TYPE_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNKNOWN_DA_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLEM_PP_TRAP_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLEM_NON_PP_TRAP_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_SCOPING_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ZERO_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_ONE_ENABLE, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MTU_CHECK_ENABLE, INST_SINGLE, TRUE);
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_erpp_filters_non_separate_enablers))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_FILTERS_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_FILTERS_ENABLE, INST_SINGLE, TRUE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_VERSION_FILTER_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_HEADER_CHECKSUM_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_HEADER_LENGTH_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_TOTAL_LENGTH_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_OPTIONS_FILTER_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP_EQUAL_DIP_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_DIP_ZERO_FILTER_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4_SIP_MULTICAST_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_VERSION_FILTER_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_SIP_IS_MC_FILTER_ENABLE, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IS_ALL_ZEROES_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_SIP_IS_ALL_ZEROES_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_LOOPBACK_SRC_OR_DST_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_NEXT_HEADER_IS_ZEROES_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_LINK_LOCAL_DST_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_SITE_LOCAL_DST_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_LINK_LOCAL_SRC_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_SITE_LOCAL_SRC_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_IPV4_CMP_DST_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_IPV4_MAPPED_DST_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_MC_DST_FILTER_FILTER_ENABLE, INST_SINGLE,
                                     TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_SEQ_NUM_ZERO_FLAGS_ZERO_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_SEQ_NUM_ZERO_FLAGS_SET_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_SYN_AND_FIN_ARE_SET_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_SRC_PORT_EQUALS_DST_PORT_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_FRAGMENT_INCOMPLETE_HDR_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCP_FRAGMENT_OFFSET_LESS_THAN_8_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDP_SRC_PORT_EQUALS_DST_PORT_FILTER_ENABLE,
                                     INST_SINGLE, TRUE);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_module_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_configure_init(unit));
    SHR_IF_ERR_EXIT(dnx_erpp_out_lif_profile_filters_init(unit));

exit:

    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_etpp_out_lif_profile_exchange(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 flags,
    etpp_out_lif_profile_info_t * etpp_out_lif_profile_info,
    int old_etpp_out_lif_profile,
    int *new_etpp_out_lif_profile,
    uint8 *is_last)
{
    etpp_out_lif_template_data_t etpp_out_lif_profile_template_data;
    uint32 entry_handle_id;
    uint8 is_first;
    smart_template_alloc_info_t tag_info;
    int ref_count = 0;
    etpp_out_lif_template_data_t etpp_out_lif_profile_default_template_data;
    uint8 use_default_tag = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * local variables init
     */
    sal_memset(&etpp_out_lif_profile_template_data, 0, sizeof(etpp_out_lif_template_data_t));
    sal_memset(&etpp_out_lif_profile_default_template_data, 0, sizeof(etpp_out_lif_template_data_t));
    sal_memset(&tag_info, 0, sizeof(smart_template_alloc_info_t));
    is_first = 0;

    /** Get the default template data of the default profile */
    SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.profile_data_get
                    (unit, 0, &ref_count, &etpp_out_lif_profile_default_template_data));

    /*
     * In case that the provided data is the same as the default data,
     * set use_default_tag in order to use later the 'exchange' correctly
     */
    if ((etpp_out_lif_profile_info->out_lif_orientation ==
         etpp_out_lif_profile_default_template_data.out_lif_orientation)
        && (etpp_out_lif_profile_info->etpp_split_horizon_enable ==
            etpp_out_lif_profile_default_template_data.etpp_split_horizon_enable)
        && (etpp_out_lif_profile_info->oam_lif_profile == etpp_out_lif_profile_default_template_data.oam_lif_profile))
    {
        tag_info.resource_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        use_default_tag = TRUE;
    }
    /** If the table is IPV4/6_TUNNEL, calculate according to the ARR prefix and support only split horizon */
    if ((dbal_table_id == DBAL_TABLE_EEDB_IPV4_TUNNEL) ||
        ((dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB)
         && (old_etpp_out_lif_profile >= DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE))
        || (dbal_table_id == DBAL_TABLE_EEDB_IPV6_TUNNEL))
    {
        if (etpp_out_lif_profile_info->oam_lif_profile != DNX_OUT_LIF_PROFILE_OAM_PROFILE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid configuration for table. Configuring oam_profile is not supported ");
        }
        /** use tag DNX_ALGO_OUT_LIF_TAG_IP_TUNNEL_PROFILE (profiles in range of 56 to 63) */
        tag_info.resource_alloc_info.tag = DNX_ALGO_OUT_LIF_TAG_IP_TUNNEL_PROFILE;
        if (use_default_tag == FALSE)
        {
            etpp_out_lif_profile_template_data.out_lif_orientation = etpp_out_lif_profile_info->out_lif_orientation;
        }
        else
        {
            *new_etpp_out_lif_profile = DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE;
            etpp_out_lif_profile_template_data.keep_internal_default_profile =
                DNX_OUT_LIF_PROFILE_KEEP_DEFAULT_IP_TUNNEL_PROFILE;
            etpp_out_lif_profile_default_template_data.keep_internal_default_profile =
                DNX_OUT_LIF_PROFILE_KEEP_DEFAULT_IP_TUNNEL_PROFILE;
        }
    }
    /** If the table is MPLS_TUNNEL, calculate according to the ARR prefix and support only oam profile */
    else if (dbal_table_id == DBAL_TABLE_EEDB_MPLS_TUNNEL)
    {
        if (etpp_out_lif_profile_info->out_lif_orientation != DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid configuration for table. Configuring orientation is not supported ");
        }
        /** use tag DNX_ALGO_OUT_LIF_TAG_MPLS_PROFILE (profiles in range of 52 to 55) */
        tag_info.resource_alloc_info.tag = DNX_ALGO_OUT_LIF_TAG_MPLS_PROFILE;
        if (use_default_tag == FALSE)
        {
            etpp_out_lif_profile_template_data.oam_lif_profile = etpp_out_lif_profile_info->oam_lif_profile;
        }
        else
        {
            *new_etpp_out_lif_profile = DNX_OUT_LIF_PROFILE_DEFAULT;
        }
    }
    else
    {
        /** use tag DNX_ALGO_OUT_LIF_TAG_GENERAL_PROFILE (profiles in range of 0 to 51) */
        tag_info.resource_alloc_info.tag = DNX_ALGO_OUT_LIF_TAG_GENERAL_PROFILE;
        if (use_default_tag == FALSE)
        {
            /** Allow all field to be managed for 6b range of outlif profile*/
            etpp_out_lif_profile_template_data.out_lif_orientation = etpp_out_lif_profile_info->out_lif_orientation;
            etpp_out_lif_profile_template_data.etpp_split_horizon_enable = DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE;
            etpp_out_lif_profile_template_data.oam_lif_profile = etpp_out_lif_profile_info->oam_lif_profile;
        }
        else
        {
            *new_etpp_out_lif_profile = DNX_OUT_LIF_PROFILE_DEFAULT;
        }
    }
    /** resource_flags should be set, otherwise it will ignore the used tag */
    tag_info.resource_flags = RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;
    if (use_default_tag == TRUE)
    {
        /** In case of default data, 'exchange' should be called with flag DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID */
        SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.exchange
                        (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &etpp_out_lif_profile_default_template_data,
                         old_etpp_out_lif_profile, &tag_info, new_etpp_out_lif_profile, &is_first, is_last));
    }
    else
    {
        /** In case of not default data, 'exchange' should be called with no flags */
        SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.exchange
                        (unit, 0, &etpp_out_lif_profile_template_data, old_etpp_out_lif_profile,
                         &tag_info, new_etpp_out_lif_profile, &is_first, is_last));
    }

    /*
     * write to HW
     * if we have a new out_lif_profile then write out_lif_profile properties to HW
     */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, *new_etpp_out_lif_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_ORIENTATION, INST_SINGLE,
                                     etpp_out_lif_profile_info->out_lif_orientation);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE,
                                     DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PRIORITY_MAP_PROFILE, INST_SINGLE,
                                     etpp_out_lif_profile_info->oam_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /** If this is the last reference to the profile - clear the entry */
    if (*is_last)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, old_etpp_out_lif_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_ORIENTATION, INST_SINGLE,
                                     DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPLIT_HORIZON_ENABLE, INST_SINGLE,
                                     DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_ENABLE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PRIORITY_MAP_PROFILE, INST_SINGLE,
                                     DNX_OUT_LIF_PROFILE_OAM_PROFILE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_erpp_out_lif_profile_exchange(
    int unit,
    erpp_out_lif_profile_info_t * erpp_out_lif_profile_info,
    int old_erpp_out_lif_profile,
    int *new_erpp_out_lif_profile)
{
    erpp_out_lif_template_data_t erpp_out_lif_profile_template_data;
    uint32 entry_handle_id;
    uint8 is_last, is_first;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * local variables init
     */
    sal_memset(&erpp_out_lif_profile_template_data, 0, sizeof(erpp_out_lif_template_data_t));
    is_last = 0;
    is_first = 0;

    /** Exchange the old profile with a new one, based on the input data*/
    erpp_out_lif_profile_template_data.ttl_scope_threshold = erpp_out_lif_profile_info->ttl_scope_threshold;
    SHR_IF_ERR_EXIT(algo_lif_profile_db.erpp_out_lif_profile.exchange
                    (unit, 0, &erpp_out_lif_profile_template_data, old_erpp_out_lif_profile,
                     NULL, new_erpp_out_lif_profile, &is_first, &is_last));
    /*
     * write to HW
     * if we have a new erpp_out_lif_profile then write erpp_out_lif_profile properties to HW
     */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_TTL_SCOPE_INDEX, INST_SINGLE,
                                     *new_erpp_out_lif_profile);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, *new_erpp_out_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ERPP_TTL_SCOPE_TABLE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_TTL_SCOPE_INDEX, *new_erpp_out_lif_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_SCOPE, INST_SINGLE,
                                     erpp_out_lif_profile_template_data.ttl_scope_threshold);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /** If this is the last reference to the profile - clear the entry */
    if (is_last)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ERPP_OUT_LIF_PROFILE_TABLE, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_TTL_SCOPE_INDEX, INST_SINGLE, 0);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, old_erpp_out_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ERPP_TTL_SCOPE_TABLE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTLIF_TTL_SCOPE_INDEX, old_erpp_out_lif_profile);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
void
etpp_out_lif_profile_info_t_init(
    int unit,
    etpp_out_lif_profile_info_t * etpp_out_lif_profile_info)
{
    etpp_out_lif_profile_info->out_lif_orientation = DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION;
    etpp_out_lif_profile_info->etpp_split_horizon_enable = DNX_OUT_LIF_PROFILE_SPLIT_HORIZON_DISABLE;
    etpp_out_lif_profile_info->oam_lif_profile = DNX_OUT_LIF_PROFILE_OAM_PROFILE;
    etpp_out_lif_profile_info->keep_internal_default_profile = 0;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_etpp_out_lif_profile_get_data(
    int unit,
    int etpp_out_lif_profile,
    etpp_out_lif_profile_info_t * etpp_out_lif_profile_info)
{
    int ref_count;
    etpp_out_lif_template_data_t etpp_out_lif_profile_template_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&etpp_out_lif_profile_template_data, 0, sizeof(etpp_out_lif_template_data_t));

    SHR_NULL_CHECK(etpp_out_lif_profile_info, _SHR_E_PARAM, "NULL input - etpp_out_lif_profile_info");
    /*
     * Make sure the whole output structure is, initially, set to zero.
     */
    sal_memset(etpp_out_lif_profile_info, 0, sizeof(*etpp_out_lif_profile_info));

    /*
     * check if out_lif_profile is in range
     */
    if ((etpp_out_lif_profile < 0) | (etpp_out_lif_profile >= dnx_data_lif.out_lif.nof_out_lif_profiles_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "etpp_out_lif_profile out of range : %d", etpp_out_lif_profile);
    }
    SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.profile_data_get
                    (unit, etpp_out_lif_profile, &ref_count, &etpp_out_lif_profile_template_data));
    /*
     * assign out_lif_orientation, split horizon and oam profile
     */
    etpp_out_lif_profile_info->out_lif_orientation = etpp_out_lif_profile_template_data.out_lif_orientation;
    etpp_out_lif_profile_info->etpp_split_horizon_enable = etpp_out_lif_profile_template_data.etpp_split_horizon_enable;
    etpp_out_lif_profile_info->oam_lif_profile = etpp_out_lif_profile_template_data.oam_lif_profile;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
void
erpp_out_lif_profile_info_t_init(
    int unit,
    erpp_out_lif_profile_info_t * erpp_out_lif_profile_info)
{
    erpp_out_lif_profile_info->ttl_scope_threshold = DNX_OUT_LIF_PROFILE_DEFAULT_TTL_THRESHOLD;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_erpp_out_lif_profile_get_data(
    int unit,
    int erpp_out_lif_profile,
    erpp_out_lif_profile_info_t * erpp_out_lif_profile_info)
{
    int ref_count, system_headers_mode, iop_mode_outlif_selection;
    erpp_out_lif_template_data_t erpp_out_lif_profile_template_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    iop_mode_outlif_selection = dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_outlif_selection);

    sal_memset(&erpp_out_lif_profile_template_data, 0, sizeof(erpp_out_lif_template_data_t));

    SHR_NULL_CHECK(erpp_out_lif_profile_info, _SHR_E_PARAM, "NULL input - erpp_out_lif_profile_info");
    /*
     * Make sure the whole output structure is, initially, set to zero.
     */
    sal_memset(erpp_out_lif_profile_info, 0, sizeof(erpp_out_lif_profile_info_t));
    /** Due to HW issue on IOP mode, the RIFs are added in the namespace of the LIF and have only 1 bit for lif profile*/
    if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ||
        (iop_mode_outlif_selection == FALSE))
    {
        erpp_out_lif_profile = erpp_out_lif_profile & (dnx_data_lif.out_lif.nof_erpp_out_lif_profiles_get(unit) - 1);
    }
    else
    {
        erpp_out_lif_profile = erpp_out_lif_profile & 0x1;
    }

    SHR_IF_ERR_EXIT(algo_lif_profile_db.erpp_out_lif_profile.profile_data_get
                    (unit, erpp_out_lif_profile, &ref_count, &erpp_out_lif_profile_template_data));
    /*
     * assign TTL scope threshold per given profile
     */
    erpp_out_lif_profile_info->ttl_scope_threshold = erpp_out_lif_profile_template_data.ttl_scope_threshold;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_etpp_out_lif_profile_get_ref_count(
    int unit,
    int etpp_out_lif_profile,
    int *ref_count)
{
    etpp_out_lif_template_data_t etpp_out_lif_profile_template_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&etpp_out_lif_profile_template_data, 0, sizeof(etpp_out_lif_profile_template_data));

    SHR_IF_ERR_EXIT(algo_lif_profile_db.etpp_out_lif_profile.profile_data_get
                    (unit, etpp_out_lif_profile, ref_count, &etpp_out_lif_profile_template_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_set(
    int unit,
    bcm_gport_t port,
    int new_out_lif_profile)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int global_out_lif;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_TUNNEL(port))
    {
        /*
         * OUTRIF or TUNNEL ID
         */
        global_out_lif = BCM_GPORT_TUNNEL_ID_GET(port);
    }
    else
    {
        /*
         * OUTLIF 
         */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        global_out_lif = gport_hw_resources.global_out_lif;
    }

    SHR_IF_ERR_EXIT(dnx_lif_lib_replace_glem(unit, BCM_CORE_ALL, global_out_lif, LIF_LIB_GLEM_KEEP_OLD_VALUE,
                                             new_out_lif_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   out_lif_profile.h file
 */
shr_error_e
dnx_out_lif_profile_get(
    int unit,
    bcm_gport_t port,
    int *out_lif_profile_p)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 local_lif_dummy;
    uint32 out_lif_profile;
    int global_out_lif;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(out_lif_profile_p, _SHR_E_PARAM, "out_lif_profile_p");

    if (BCM_GPORT_IS_TUNNEL(port))
    {
        /*
         * OUTRIF or TUNNEL ID
         */
        global_out_lif = BCM_GPORT_TUNNEL_ID_GET(port);
    }
    else
    {
        /*
         * OUTLIF 
         */
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
        global_out_lif = gport_hw_resources.global_out_lif;
    }

    if (dnx_data_lif.out_lif.outlif_profile_width_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_lif_lib_read_from_glem(unit, 0, global_out_lif, &local_lif_dummy, &out_lif_profile));
        *out_lif_profile_p = out_lif_profile;
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}
