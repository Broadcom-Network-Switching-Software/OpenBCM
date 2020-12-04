/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_INTERNAL_H
#define SAL_INTERNAL_H

#include <sal_config.h>

/*
 * SAL_INT_VAR_VALUE can be used to dump values of configuration
 * variables during compile time. The corresponding source code will
 * look something like this:
 *
 * #define MY_LOCAL_VAR    1
 *
 * #include <sal/sal_internal.h>
 * #if SAL_INT_DUMP_CONFIG
 * #pragma message(SAL_INT_VAR_VALUE(MY_LOCAL_VAR))
 * #endif
 */
#ifndef SAL_INT_DUMP_CONFIG
#define SAL_INT_DUMP_CONFIG     0
#endif

#define SAL_INT_STRINGIFY(x) #x
#define SAL_INT_VAR_VALUE(var) #var "="  SAL_INT_STRINGIFY(var)

/*
 * Create 32-bit signature from ASCII characters.
 * Signatures are used for internal sanity checks of data structures.
 */
#define SAL_SIG_DEF(_s0, _s1, _s2, _s3) \
    (((_s0) << 24) | ((_s1) << 16) | ((_s2) << 8) | (_s3))

#endif /* SAL_INTERNAL_H */
