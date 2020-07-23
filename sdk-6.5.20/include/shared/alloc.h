/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SHR_ALLOC_H
#define _SHR_ALLOC_H

#include <sal/core/alloc.h>
#include <sal/core/libc.h>

/*
 * sal_free() is redefined here to the NULL-pointer safe version. 
 * 
 * This allows all code within the SDK to use the NULL-safe version without
 * affecting existing SAL implementations or external codebases.
 */

#define sal_free sal_free_safe
 
#endif /* _SHR_ALLOC_H */
