/** \file appl_ref_board.h
 *
 * functions which are board specific
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FILE_APPL_REF_DNXC_INIT_BOARD_H_INCLUDED
/* { */
#define FILE_APPL_REF_DNXC_INIT_BOARD_H_INCLUDED

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

#include <sal/types.h>

/*
* MACROs:
* {
*/

/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */

/*
 * }
 */



/*
* Function Declarations:
* {
*/

/*
 * This function will perform a system reset on the given unit or report if a system reset is supported.
 * If SOC_SYSTEM_RESET_FLAG_IS_SUPPORTED is specified in flags, it will return SOC_E_NONE if supported.
 * Otherwise it will perform a system reset if supported, and return SOC_E_NONE on success.
 */
int dnxc_perform_system_reset(
    int unit,
    uint32 flags);

/*
 * }
 */

/* } */
#endif
