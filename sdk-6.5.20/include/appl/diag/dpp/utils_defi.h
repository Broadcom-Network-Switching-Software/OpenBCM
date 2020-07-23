/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UTILS_DEFI_INCLUDED
/* { */
#define UTILS_DEFI_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * General include file for user interface.
 */
#include <appl/diag/dpp/utils_pure_defi.h>
#include <appl/diag/dpp/dune_chips.h>
/*
 * Rom variables.
 * {
 */
#ifdef  EXTERN
#undef  EXTERN
#endif
#define EXTERN extern
#ifdef  INIT
#undef  INIT
#endif
#ifdef  CONST
#undef  CONST
#endif
#define CONST const
/*
 * }
 */
#undef  CONST

#if !DUNE_BCM
/*
 * Ram variables.
 * {
 */
#include "Utilities/include/utils_ram_defi.h"
/*
 * }
 */
/* } */

/*
 * Syslog prototypes.
 * {
 */
#include "Utilities/include/utils_syslog.h"
/*
 * }
 */
/* } */
#endif /* !DUNE_BCM */

#endif
