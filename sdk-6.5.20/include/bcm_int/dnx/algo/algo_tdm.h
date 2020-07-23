/**
 * \file algo_tdm.h
 * Internal DNX TDM Managment APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef ALGO_TDM_H_INCLUDED
/*
 * { 
 */
#define ALGO_TDM_H_INCLUDED

#include <sal/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bitop.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fabric.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * Resource name defines for algo tdm.
 * {
 */
#define DNX_ALGO_TDM_LINK_MASK_TEMPLATE     "TDM Link Mask Template"

#define DNX_TDM_LINK_MASK_DEFAULT_ID            0

/**
 * }
 */

/**
 * \brief - data for ingress editing context.
 */
typedef struct
{
    /** bits for link mask data */
    uint32 link_mask_data[_SHR_BITDCLSIZE(DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS)];
} dnx_tdm_link_mask_t;

/**
 * \brief - Print an entry of the vsi profile template. See
 *       \ref dnx_algo_template_print_data_cb for more details.
 * 
 * \param [in] unit - unit id
 * \param [in] data - Pointer to the struct to be printed.
 *      \b As \b input - \n
 *       The pointer is expected to hold a struct of type uint32[2]. \n
 *       It's the user's responsibility to verify it beforehand.
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_tdm_link_mask_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Initialize tdm algorithms.
 * 
 * \param [in] unit - unit id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_tdm_init(
    int unit);

/**
 * \brief - De-Initialize tdm algorithms.
 * 
 * \param [in] unit - unit ID
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_tdm_deinit(
    int unit);

/*
 * } 
 */
#endif /* ALGO_TDM_H_INCLUDED */
