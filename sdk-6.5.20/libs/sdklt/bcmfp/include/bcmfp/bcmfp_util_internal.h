/*! \file bcmfp_util_internal.h
 *
 * Utility Defines/APIs used acorss all FPs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_UTIL_INTERNAL_H
#define BCMFP_UTIL_INTERNAL_H

#define BCMFP_ALLOC(_ptr, _sz, _str)                   \
            do                                         \
            {                                          \
                SHR_ALLOC(_ptr, _sz, _str);            \
                SHR_NULL_CHECK(_ptr, SHR_E_MEMORY);    \
                sal_memset(_ptr, 0, _sz);              \
            } while(0)

#endif /* BCMFP_UTIL_INTERNAL_H */
