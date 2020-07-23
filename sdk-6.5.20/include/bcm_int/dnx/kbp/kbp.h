/** \file kbp.h
 *
 * Functions for handling kbp init sequences.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_KBP)

#ifndef _KBP_INCLUDED__
/*
 * {
 */

#define _KBP_INCLUDED__

/*************
 * INCLUDES  *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * DEFINES   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */
/*
 * }
 */

/*************
 * ENUMS     *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * TYPE DEFS *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * GLOBALS   *
 */
/*
 * {
 */
/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */
shr_error_e dnx_kbp_init(
    int unit);

shr_error_e dnx_kbp_deinit(
    int unit);

shr_error_e dnx_kbp_info_get(
    int unit,
    int kbp_inst,
    bcm_switch_kbp_info_t * kbp_info);

/*
 * }
 */
#endif /* __KBP_INCLUDED__ */
#endif /* defined(INCLUDE_KBP) */
