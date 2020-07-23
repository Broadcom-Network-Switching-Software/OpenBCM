/** \file appl_ref_oam_appl_init.h
 * General OAM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_OAM_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_OAM_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/error.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm/oam.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <bcm/error.h>
#include <bcm/port.h>
/*
 * }
 */

/** definitions used to avoid using DBAL enums */

typedef enum
{
    EGRESS_ERROR,
    CHAN_TYPE_MISS_ERROR,
    TYPE_MISS_ERROR,
    EM_CORE_ERROR,
    MAID_MISS_ERROR,
    FLEX_CRC_MISS_ERROR,
    MDL_MISS_ERROR,
    CCM_INTRVL_MISS_ERROR,
    MY_DISC_MISS_ERROR,
    SRC_IP_MISS_ERROR,
    YOUR_DISC_MISS_ERROR,
    SRC_PORT_MISS_ERROR,
    RMEP_STATE_CHANGE_ERROR,
    PARITY_ERROR,
    DEBUG_TRAP,
    TIMESTAMP_MISS_ERROR,
    TRAP_CODE_MISS_ERROR,
    RFC_PUNT
} bcm_oamp_error_type_e;

/**
 * \brief - This function initialize OAM:
 *          1. DOWN MEP traps initialization
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_oam_init(
    int unit);
/*
 * }
 */
#endif /* APPL_REF_OAM_INIT_H_INCLUDED */
