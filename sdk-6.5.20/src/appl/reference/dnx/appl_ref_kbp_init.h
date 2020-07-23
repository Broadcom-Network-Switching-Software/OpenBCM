/** \file appl_ref_kbp_init.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_KBP)
/*
 * {
 */
#ifndef APPL_REF_KBP_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_KBP_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <shared/error.h>

/*
 * }
 */

shr_error_e appl_dnx_kbp_init(
    int unit);

shr_error_e appl_dnx_kbp_deinit(
    int unit);

/*
 * }
 */
#endif /* APPL_REF_KBP_INIT_H_INCLUDED */
/*
 * }
 */
#endif /* defined(INCLUDE_KBP) */
