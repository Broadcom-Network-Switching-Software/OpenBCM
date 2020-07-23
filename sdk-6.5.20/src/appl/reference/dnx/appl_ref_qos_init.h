/** \file appl_ref_l2_init.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_QOS_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_QOS_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/**
 * \brief
 * This function initialize QOS application.
 * The function is add phb&remark&ecn mapping for profile 0.
 * 
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_qos_init(
    int unit);
/*
 * }
 */
#endif /* APPL_REF_QOS_INIT_H_INCLUDED */
