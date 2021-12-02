/** \file algo_oamp.c
 * 
 *
 * Resource and templates needed for the OAMP OAM and BFD features.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/oam.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp_v1.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>

/*
 * }
 */
/*
 * DEFINEs
 * {
 */

/** Number of entries in relevant hardware tables */
#define NOF_PUNT_PROFILES             16
#define NOF_ITMH_PPH_PRIORITY_PROFILES(unit) (dnx_data_oam.oamp.feature_get(unit, dnx_data_oam_oamp_oamp_v2) == 1 ? 32 : 8)

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
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oam_punt_profile_print_cb(
    int unit,
    const void *data)
{
#ifdef DNX_SW_STATE_DIAGNOSTIC
    dnx_oam_oamp_punt_event_profile_t *punt_profile = (dnx_oam_oamp_punt_event_profile_t *) data;
#endif
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "punt_enable", punt_profile->punt_enable,
                                   "Indicates whether punt is enabled", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "punt_rate", punt_profile->punt_rate,
                                   "Rate of sampling packets (with events) to CPU", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "rx_state_update_en",
                                   punt_profile->rx_state_update_en,
                                   "Refer dbal_enum_value_field_profile_rx_state_update_en_e", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "profile_scan_state_update_en",
                                   punt_profile->profile_scan_state_update_en,
                                   "Refer dbal_enum_value_field_profile_scan_state_update_en_e", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "mep_rdi_update_loc_en",
                                   punt_profile->mep_rdi_update_loc_en,
                                   "Enable/disable MEP DB RDI indication in case LOC detected", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "mep_rdi_update_loc_clear_en",
                                   punt_profile->mep_rdi_update_loc_clear_en,
                                   "Enable/disable MEP DB RDI indication in case LOC Clear detected", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "mep_rdi_update_rx_en",
                                   punt_profile->mep_rdi_update_rx_en,
                                   "Enable/disable MEP DB RDI indication by copying RDI from rx pkt", NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oamp_itmh_pph_priority_profile_print_cb(
    int unit,
    const void *data)
{
#ifdef DNX_SW_STATE_DIAGNOSTIC
    dnx_oam_itmh_priority_t *itmh_priority = (dnx_oam_itmh_priority_t *) data;
#endif
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "tc", itmh_priority->tc, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dp", itmh_priority->dp, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "qos", itmh_priority->qos, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/**
 * \brief - This function creates the template for the ITMH
 *        TC/DP profiles.  This resource is used by OAM and BFD
 *        accelerated endpoints in the ITMH header of
 *        transmitted packets.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oamp_itmh_pph_priority_profile_id_init(
    int unit)
{
    dnx_algo_template_create_data_t itmh_pph_priority_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&itmh_pph_priority_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    itmh_pph_priority_profile_data.flags = 0;
    itmh_pph_priority_profile_data.first_profile = 0;
    itmh_pph_priority_profile_data.nof_profiles = NOF_ITMH_PPH_PRIORITY_PROFILES(unit);
    itmh_pph_priority_profile_data.max_references = dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    itmh_pph_priority_profile_data.default_profile = 0;
    itmh_pph_priority_profile_data.data_size = sizeof(dnx_oam_itmh_priority_t);
    itmh_pph_priority_profile_data.default_data = NULL;
    sal_strncpy(itmh_pph_priority_profile_data.name, DNX_OAM_TEMPLATE_ITMH_PRIORITY,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_itmh_pph_priority_profile.create(unit, &itmh_pph_priority_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template needed for the
 *        Punt profile management
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *      Punt profiles: mirror the entries in
 *      the OAMP_PUNT_EVENT_HENDLING available for Punt information\n
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_punt_profile_template_init(
    int unit)
{
    dnx_algo_template_create_data_t oam_punt_handling_profile_data;
    uint32 nof_references = 0;
    uint32 nof_scan_rmep_entries = 0;
    uint32 rmep_db_treshold = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_scan_rmep_entries = dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) + 1;
    rmep_db_treshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    if (rmep_db_treshold < nof_scan_rmep_entries)
    {
        nof_references = rmep_db_treshold + (nof_scan_rmep_entries - rmep_db_treshold) / 2;
    }
    else
    {
        nof_references = nof_scan_rmep_entries;
    }

    /** Create template for BFD OAMA Profiles */
    sal_memset(&oam_punt_handling_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    oam_punt_handling_profile_data.flags = 0;
    oam_punt_handling_profile_data.first_profile = DNX_ALGO_PUNT_TABLE_START_ID;
    oam_punt_handling_profile_data.nof_profiles = DNX_ALGO_PUNT_TABLE_COUNT;
    oam_punt_handling_profile_data.max_references = nof_references;
    oam_punt_handling_profile_data.default_profile = 0;
    oam_punt_handling_profile_data.data_size = sizeof(dnx_oam_oamp_punt_event_profile_t);
    oam_punt_handling_profile_data.default_data = NULL;
    sal_strncpy(oam_punt_handling_profile_data.name, DNX_ALGO_OAMP_PUNT_EVENT_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.create(unit, &oam_punt_handling_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oamp_punt_good_profile_print_cb(
    int unit,
    const void *data)
{
#ifdef DNX_SW_STATE_DIAGNOSTIC
    uint16 punt_good_packet_rate = *(uint16 *) data;
#endif
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "Punt good packet rate", punt_good_packet_rate,
                                   NULL, "0x%08X");
    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/**
 * \brief - This function creates the template needed for
            punt good profile management
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oamp_punt_good_profile_template_init(
    int unit)
{
    dnx_algo_template_create_data_t oamp_punt_good_profile_data;
    uint32 nof_references = 0;
    uint32 nof_scan_rmep_entries = 0;
    uint32 rmep_db_treshold = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data */
    nof_scan_rmep_entries = dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) + 1;
    rmep_db_treshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    if (rmep_db_treshold < nof_scan_rmep_entries)
    {
        nof_references = rmep_db_treshold + (nof_scan_rmep_entries - rmep_db_treshold) / 2;
    }
    else
    {
        nof_references = nof_scan_rmep_entries;
    }

    /** Create template for OAMP punt good profile table */
    sal_memset(&oamp_punt_good_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    oamp_punt_good_profile_data.flags = 0;
    oamp_punt_good_profile_data.first_profile = 1;
    oamp_punt_good_profile_data.nof_profiles = 7;
    oamp_punt_good_profile_data.max_references = nof_references;
    oamp_punt_good_profile_data.default_profile = 0;
    oamp_punt_good_profile_data.data_size = sizeof(uint16);
    oamp_punt_good_profile_data.default_data = NULL;
    sal_strncpy(oamp_punt_good_profile_data.name, DNX_ALGO_OAMP_PUNT_GOOD_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oamp_db.oamp_punt_good_profile.create(unit, &oamp_punt_good_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP creation
 *          This function deletes the new RMEP ID
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_id - ID of the remote endpoint.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_remote_endpoint_id_free(
    int unit,
    bcm_oam_endpoint_t rmep_id)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        uint32 self_contained_threshold;
        int alloc_id;
        self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

        if (rmep_id < self_contained_threshold)
        {
            SHR_IF_ERR_EXIT(dnx_oamp_v1_remote_endpoint_id_below_threshold_to_alloc_mngr_id(unit, rmep_id, &alloc_id));
            /*
             * Remove from below threshold pool
             */

            SHR_IF_ERR_EXIT(algo_oam_db.oamp_v1_rmep_id_below_threshold.free_single(unit, alloc_id, NULL));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oamp_v1_remote_endpoint_id_above_threshold_to_alloc_mngr_id(unit, rmep_id, &alloc_id));
            /*
             * Remove from above threshold pool
             */

            SHR_IF_ERR_EXIT(algo_oam_db.oamp_v1_rmep_id_above_threshold.free_single(unit, alloc_id, NULL));
        }
#endif /* BCM_DNX2_SUPPORT */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP creation
 *          This function allocates the new RMEP ID
 *          and handles WITH_ID case as well.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] alloc_flags - flags variable to be used
 *             for resource allocation
 * \param [in,out] rmep_id - Resulting ID.  If ID is
 *        specified, this is an output argument.
 *        Otherwise, it's an input argument.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_remote_endpoint_id_alloc(
    int unit,
    int alloc_flags,
    bcm_oam_endpoint_t * rmep_id)
{

    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        uint32 self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
        uint32 oamp_rmep_scale_limitataion =
            dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_rmep_scale_limitataion);

        int alloc_id;
        uint32 nof_entries_per_rmep = 0;
        shr_error_e rv;
        /** allocate the remote endpoint ID */
        if (_SHR_IS_FLAG_SET(alloc_flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
        {
            /*
             * in case BCM_OAM_ENDPOINT_WITH_ID flag exists, allocate RMEP id, according to endpoint_info->id
             */
            if (*rmep_id < self_contained_threshold)
            {
                SHR_IF_ERR_EXIT(dnx_oamp_v1_remote_endpoint_id_below_threshold_to_alloc_mngr_id
                                (unit, *rmep_id, &alloc_id));
                /*
                 * Allocate from below threshold pool
                 */
                SHR_IF_ERR_EXIT(algo_oam_db.
                                oamp_v1_rmep_id_below_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_oamp_v1_remote_endpoint_id_above_threshold_to_alloc_mngr_id
                                (unit, *rmep_id, &alloc_id));
                /*
                 * Allocate from above threshold pool
                 */
                SHR_IF_ERR_EXIT(algo_oam_db.
                                oamp_v1_rmep_id_above_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id));
            }
        }
        else
        {
            /*
             * First try to allocate from above threshold. If resource is not there, then allocate
             * from below threshold.
             */
            rv = algo_oam_db.oamp_v1_rmep_id_above_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id);
            if (rv == _SHR_E_RESOURCE)
            {
                SHR_IF_ERR_EXIT(algo_oam_db.
                                oamp_v1_rmep_id_below_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id));
                nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 2 : 1;
                *rmep_id = alloc_id * nof_entries_per_rmep;
            }
            else if (rv != _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocate above threshold failed with %d\r\n", rv);
            }
            else
            {
                /*
                 * Allocated from above threshold. 
                 *   if oamp_rmep_scale_limitataion is 0, the endpoint id is 2 times the allocated number + threshold
                 *   if oamp_rmep_scale_limitataion is 1, the endpoint id is 4 times the allocated number + threshold
                 */
                nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 4 : 2;
                *rmep_id = (alloc_id * nof_entries_per_rmep) + self_contained_threshold;
            }
        }
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize templates for all oamp profile types.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_oamp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * init the algo_oapm sw state module.
     */
    SHR_IF_ERR_EXIT(algo_oamp_db.init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oamp_itmh_pph_priority_profile_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_punt_profile_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oamp_punt_good_profile_template_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
