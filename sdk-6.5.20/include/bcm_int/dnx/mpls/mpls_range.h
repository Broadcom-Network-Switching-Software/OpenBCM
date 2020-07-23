/*
 * \file bcm_int/dnx/mpls/mpls_range.h
 * Internal DNX MPLS APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MPLS_RANGE_H_INCLUDED__
/*
 * {
 */
#define __MPLS_RANGE_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/mpls.h>

/**
 * \brief
 *   Check whether any of the specified labels is within one of
 *   the ranges already loaded into HW. If so, indicate that and specify the
 *   found range.
 *   Note that it is enough for just one of the labels to be 'within range' to
 *   make '*is_iml_p' be loaded by a non-zero value.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] num_labels -
 *   Number of labels in the array 'labels' below.
 * \param [in] in_labels_p -
 *   Pointer to an array of MPLS labels. The number of elements on this array
 *   is as per 'num_labels' above. 
 *   If ANY of specified labels is within a range which is already in HW then
 *     this procedure loads '*is_iml' by TRUE (non-zero value) and, also loads
 *     'found_labels' by the labels on the range found in HW.
 *   Otherwise, this procedure loads '*is_iml' by FALSE (zero value)
 * \param [out] is_iml_p -
 *   Pointer to int. 
 *   See 'in_labels' above.
 * \param [out] found_labels_low_p -
 *   Pointer to an MPLS label. To be loaded by the 'low' value, if found.
 *   See 'in_labels' above.
 * \param [out] found_labels_high_p -
 *   Pointer to an MPLS label. To be loaded by the 'high' value, if found.
 *   See 'in_labels' above.
 * \return
 *   Negative in case of an error.
 *   Zero in case of NO ERROR
 * \see
 *   * dnx_mpls_range_action_add_verify
 * \remark
 *   * None
 */
shr_error_e
dnx_mpls_label_iml_range_check(
    int unit,
    int num_labels,
    bcm_mpls_label_t *in_labels_p,
    int *is_iml_p,
    bcm_mpls_label_t *found_labels_low_p,
    bcm_mpls_label_t *found_labels_high_p) ;

/*
 * }
 */
#endif/*__MPLS_RANGE_H_INCLUDED__*/
