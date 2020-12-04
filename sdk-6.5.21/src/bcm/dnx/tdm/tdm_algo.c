/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file tdm_algo.c
 *
 * TDM Algo utilities.
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
#include <shared/util.h>
#include <shared/utilex/utilex_str.h>
#include <bcm/tdm.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <bcm_int/dnx/algo/algo_tdm.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tdm_access.h>
/**
 * }
 */

void
dnx_tdm_link_mask_print_cb(
    int unit,
    const void *data)
{
    dnx_tdm_link_mask_t *tdm_link_mask = (dnx_tdm_link_mask_t *) data;
    int mask_size = BITS2BYTES(dnx_data_fabric.links.nof_links_get(unit)), i_data;
    char temp_str[RHNAME_MAX_SIZE];

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    for (i_data = 0; i_data < mask_size; i_data++)
    {
        sal_snprintf(temp_str, RHNAME_MAX_SIZE, "Link Mask - bits[%d-%d]", i_data * 8, i_data * 8 + 7);
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, temp_str,
                                       tdm_link_mask->link_mask_data[i_data], NULL, "%d");
    }

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief - 
 * Create Template for TDM Editing Contexts
 *  
 * \param [in] unit - unit id
 *   
 * \return
 *   shr_error_e 
 */
static shr_error_e
dnx_tdm_link_mask_template_create(
    int unit)
{
    dnx_algo_template_create_data_t data;
    dnx_tdm_link_mask_t tdm_link_mask;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set a template for vsi profiles
     */
    sal_memset(&data, 0, sizeof(data));
    sal_memset(&tdm_link_mask, 0, sizeof(dnx_tdm_link_mask_t));

    data.data_size = sizeof(tdm_link_mask);
    data.first_profile = DNX_TDM_LINK_MASK_DEFAULT_ID;
    data.nof_profiles = dnx_data_tdm.params.nof_contexts_get(unit);
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.default_data = &tdm_link_mask;
    data.default_profile = DNX_TDM_LINK_MASK_DEFAULT_ID;
    /*
     * Each VSI can point to one VSI profile.
     */
    data.max_references = dnx_data_tdm.params.nof_stream_ids_get(unit);;
    sal_strncpy(data.name, DNX_ALGO_TDM_LINK_MASK_TEMPLATE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_tdm_db.link_mask.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Initialize ingress context resource
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *     \retval Zero if no error was detected
 *     \retval Negative if error was detected. See \ref shr_error_e

 * \remark
 *
 * \see
 *    dnx_algo_res_create
 */
static shr_error_e
dnx_tdm_ingress_context_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.flags = 0;
    /** The available context IDs defined by BCM APIs are 0 ~ nof_contexts */
    data.first_element = 0;
    data.nof_elements = dnx_data_tdm.params.nof_contexts_get(unit);
    sal_strncpy(data.name, "TDM Ingress Context", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_tdm_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) != TRUE) ||
        (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_NONE))
    {
        /*
         * Leave peacefully
         */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(algo_tdm_db.init(unit));

    /*
     * Initialize the TDM Editing Context Template.
     */
    SHR_IF_ERR_EXIT(dnx_tdm_link_mask_template_create(unit));

    SHR_IF_ERR_EXIT(dnx_tdm_ingress_context_init(unit));
exit:
    SHR_FUNC_EXIT;
}
