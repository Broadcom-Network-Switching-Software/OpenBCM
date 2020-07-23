/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System Abstract Layer customized interface definitions for Switch SDK
 */

#ifndef __SAL_CUSTOM_CONFIG_H__
#define __SAL_CUSTOM_CONFIG_H__

#include <ctype.h>
#include <stdio.h>
#include <sal/core/libc.h>

#ifdef RTOS_STRINGS

#include <stddef.h>
#define SAL_CONFIG_DEFINE_SIZE_T        0

#include <inttypes.h>
#define SAL_CONFIG_DEFINE_UINT8_T       0
#define SAL_CONFIG_DEFINE_UINT16_T      0
#define SAL_CONFIG_DEFINE_UINT32_T      0
#define SAL_CONFIG_DEFINE_UINT64_T      0
#define SAL_CONFIG_DEFINE_UINTPTR_T     0
#define SAL_CONFIG_DEFINE_INT8_T        0
#define SAL_CONFIG_DEFINE_INT16_T       0
#define SAL_CONFIG_DEFINE_INT32_T       0
#define SAL_CONFIG_DEFINE_INT64_T       0
#define SAL_CONFIG_DEFINE_PRIu32        0
#define SAL_CONFIG_DEFINE_PRId32        0
#define SAL_CONFIG_DEFINE_PRIx32        0
#define SAL_CONFIG_DEFINE_PRIu64        0
#define SAL_CONFIG_DEFINE_PRId64        0
#define SAL_CONFIG_DEFINE_PRIx64        0

#include <stdbool.h>
#define SAL_CONFIG_DEFINE_BOOL_T        0

#include <stdlib.h>

#endif /* RTOS_STRINGS */

#define SAL_NO_STDC


#include <assert.h>
#ifdef SAL_CONFIG_DEFINE_ASSERT
#undef SAL_CONFIG_DEFINE_ASSERT
#endif
#define SAL_CONFIG_DEFINE_ASSERT        0

/*!
 * \endcond
 */

#include <sdk_ltsw_config.h>

#endif /* __SAL_CUSTOM_CONFIG_H__ */
