/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_porting.h
 * Purpose:     CINT porting interfaces
 */

#ifndef CINT_PORTING_H
#define CINT_PORTING_H

#include "cint_config.h"

#if CINT_CONFIG_INCLUDE_SDK_SAL == 1
/*
 * Use the SDK SAL interface
 */
#include "cint_porting_sal.h"
#endif /* CINT_CONFIG_INCLUDE_SDK_SAL == 1 */


#if CINT_CONFIG_INCLUDE_SDKLT == 1
/*
 * Use the SDKLT SAL interface
 */
#include "cint_porting_sdklt.h"
#endif /* CINT_CONFIG_INCLUDE_SDKLT == 1 */


/*
 * Test interfaces for CINT portability
 */
#if CINT_CONFIG_INCLUDE_STUBS == 1
#include "cint_stubs.h"
#endif /* CINT_CONFIG_INCLUDE_STUBS == 1 */


/*
 * Standard C Library compatibility
 */
#include "cint_porting_stdlib.h"

#if CINT_CONFIG_INCLUDE_MAIN == 1
#ifndef COMPILER_ATTRIBUTE
#define COMPILER_ATTRIBUTE(_a) __attribute__ (_a)
#endif
#endif /* CINT_CONFIG_INCLUDE_MAIN == 1 */

#endif /* CINT_PORTING_H */

