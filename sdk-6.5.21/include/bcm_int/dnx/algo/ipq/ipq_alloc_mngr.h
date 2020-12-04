/**
 * \file algo/ipq/ipq_alloc_mngr.h 
 *
 * Internal DNX IPQ Alloc Mngr APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __IPQ_ALLOC_MNGR_H_INCLUDED__
#define __IPQ_ALLOC_MNGR_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Includes
 * {
 */
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
/*
 * }
 */

/*
 * Structs:
 * {
 */

/*
 * Functions:
 * {
 */

/**
 * \brief
 *   Callback function used to print the TC profile mapping
 *
 * \param [in] unit - Relevant unit.
 * \param [in] data - data to print. here it is the TC to VOQ offset mapping
 *
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
void dnx_algo_ipq_tc_map_profile_print_cb(
    int unit,
    const void *data);

/*
 * } 
 */

#endif /* __IPQ_ALLOC_MNGR_H_INCLUDED__ */
