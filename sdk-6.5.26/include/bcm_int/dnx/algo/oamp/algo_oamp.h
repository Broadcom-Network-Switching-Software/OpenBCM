/**
 * \file algo_oamp.h Internal DNX OAMP Template Management
ent This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
ent 
ent Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_OAMP_H_INCLUDED
/*
 * { 
 */
#define ALGO_OAMP_H_INCLUDED

#include <bcm/oam.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

 /**
 * Resource name defines for algo oamp
 * \see
 * dnx_oam_init_templates
 * {
 */

#define DNX_ALGO_OAMP_2B_MAID_FULL_MEP_ID    "OAMP_2B_MAID_FULL_MEP_ID"
#define DNX_ALGO_OAMP_2B_MAID_SHORT_MEP_ID   "OAMP_2B_MAID_SHORT_MEP_ID"
#define DNX_ALGO_OAMP_ICC_MAID_FULL_MEP_ID   "OAMP_ICC_MAID_FULL_MEP_ID"
#define DNX_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID  "OAMP_ICC_MAID_SHORT_MEP_ID"
#define DNX_ALGO_OAMP_EXTRA_MEP_ID           "OAMP_EXTRA_MEP_ID"
#define DNX_ALGO_OAMP_FULL_TUPLES_MEP_ID     "OAMP_FULL_TUPLES_MEP_ID"
#define DNX_ALGO_OAMP_TEMPLATE_DELAY_HISTOGRAM    "TWAMP DELAY HISTOGRAM PROFILE"
#define DNX_ALGO_OAM_CH_2_SMALL_PROFILE      "DNX_ALGO_OAM_CH_2_SMALL_PROFILE"

/*
 * Callback defines for the sw state resource manager.
 */
#define SW_STATE_ALGO_OAMP_2B_MAID_FULL_MEP_ID    algo_oamp_db.oamp_2b_maid_full_mep_id
#define SW_STATE_ALGO_OAMP_2B_MAID_SHORT_MEP_ID   algo_oamp_db.oamp_2b_maid_short_mep_id
#define SW_STATE_ALGO_OAMP_ICC_MAID_FULL_MEP_ID   algo_oamp_db.oamp_icc_maid_full_mep_id
#define SW_STATE_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID  algo_oamp_db.oamp_icc_maid_short_mep_id
#define SW_STATE_ALGO_OAMP_EXTRA_MEP_ID           algo_oamp_db.oamp_extra_mep_id
#define SW_STATE_ALGO_OAMP_FULL_TUPLES_MEP_ID     algo_oamp_db.oamp_full_tuples_mep_id

#define DNX_ALGO_OAMP_PP_PORT_2_SYS_PORT_TEMPLATE   "OAMP_PP_PORT_2_SYS_PORT"
#define DNX_ALGO_OAMP_PUNT_GOOD_PROFILE_TEMPLATE    "OAMP_PUNT_GOOD_PROFILE"

/** Pool name where mep_id is located according group name type and entry type */
#define DNX_OAMP_MEP_ID_RESOURCE_NAME(is_2byte_maid, is_full, is_extra_pool) ( \
                              (is_extra_pool)              ? DNX_ALGO_OAMP_EXTRA_MEP_ID         : \
                             ((is_2byte_maid && is_full)   ? DNX_ALGO_OAMP_2B_MAID_FULL_MEP_ID  : \
                             (((is_2byte_maid && !is_full) ? DNX_ALGO_OAMP_2B_MAID_SHORT_MEP_ID : \
                             (((!is_2byte_maid && is_full) ? DNX_ALGO_OAMP_ICC_MAID_FULL_MEP_ID : \
                                                             DNX_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID))))))

/** Resource callback where mep_id is located according group name type and entry type */
#define DNX_OAMP_MEP_ID_RESOURCE(is_2byte_maid, is_full, is_extra_pool, exec) ( \
                              (is_extra_pool)              ? SW_STATE_ALGO_OAMP_EXTRA_MEP_ID.exec         : \
                             ((is_2byte_maid && is_full)   ? SW_STATE_ALGO_OAMP_2B_MAID_FULL_MEP_ID.exec  : \
                             (((is_2byte_maid && !is_full) ? SW_STATE_ALGO_OAMP_2B_MAID_SHORT_MEP_ID.exec : \
                             (((!is_2byte_maid && is_full) ? SW_STATE_ALGO_OAMP_ICC_MAID_FULL_MEP_ID.exec : \
                                                             SW_STATE_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID.exec))))))

/** Number of sub entries that couldn't be used for endpoint in ICC MAID group in one MEP_DB entry */
#define DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE 3

/** Steps of entry for jumbo TLV */
#define JUMBO_TLV_ENTRIES_STEPS                16

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
shr_error_e dnx_algo_oamp_init(
    int unit);

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
shr_error_e dnx_oam_remote_endpoint_id_free(
    int unit,
    bcm_oam_endpoint_t rmep_id);

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
shr_error_e dnx_oam_remote_endpoint_id_alloc(
    int unit,
    int alloc_flags,
    bcm_oam_endpoint_t * rmep_id);

/**
 * \brief - Printing callback for the BFD ITMH/PPH priority
 *        templates.  BFD ITMH/PPH profiles are used to construct
 *        the ITMH header and PPH header for packets transmitted from OAM/BFD
 *        endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The 
 *        BFD ITMH/PPH priority profile structure algo_oamp_v2_itmh_pph_priority_t is
 *        defined and described above.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oamp_itmh_pph_priority_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Callback function for printing OAM punt profile template data
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Punt profile template data to be printed.
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_oam_punt_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Callback function for printing OAM punt good profile
 *          template data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Punt profile template data to be printed.
 *
 * \return
 *
 * \remark
 *   * A print callback function is required to create a
 *     template. It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oamp_punt_good_profile_print_cb(
    int unit,
    const void *data);

/*
 * } 
 */
#endif /* ALGO_OAMP_H_INCLUDED */
