/*! \file field_compression.h
 *
 * BCM FIELD Compression library Shim layer
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _BCM_FIELD_COMPRESSION_H
#define _BCM_FIELD_COMPRESSION_H

#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <shared/error.h>

/*
 * SHR
 */
#define SHR_E_NONE                  _SHR_E_NONE
#define SHR_E_INTERNAL              _SHR_E_INTERNAL
#define SHR_E_MEMORY                _SHR_E_MEMORY
#define SHR_E_UNIT                  _SHR_E_UNIT
#define SHR_E_PARAM                 _SHR_E_PARAM
#define SHR_E_EMPTY                 _SHR_E_EMPTY
#define SHR_E_FULL                  _SHR_E_FULL
#define SHR_E_NOT_FOUND             _SHR_E_NOT_FOUND
#define SHR_E_EXISTS                _SHR_E_EXISTS
#define SHR_E_TIMEOUT               _SHR_E_TIMEOUT
#define SHR_E_BUSY                  _SHR_E_BUSY
#define SHR_E_FAIL                  _SHR_E_FAIL
#define SHR_E_DISABLED              _SHR_E_DISABLED
#define SHR_E_BADID                 _SHR_E_BADID
#define SHR_E_RESOURCE              _SHR_E_RESOURCE
#define SHR_E_CONFIG                _SHR_E_CONFIG
#define SHR_E_UNAVAIL               _SHR_E_UNAVAIL
#define SHR_E_INIT                  _SHR_E_INIT
#define SHR_E_PORT                  _SHR_E_PORT
#define SHR_E_IO                    _SHR_E_IO
#define SHR_E_ACCESS                _SHR_E_ACCESS
#define SHR_E_NO_HANDLER            _SHR_E_NO_HANDLER
#define SHR_E_PARTIAL               _SHR_E_PARTIAL

#define SHR_SUCCESS(rv)             _SHR_E_SUCCESS(rv)
#define SHR_FAILURE(rv)             _SHR_E_FAILURE(rv)

#define _BCM_FIELD_COMP_NULL_CHECK(_ptr)             \
    do {                                        \
        if ((_ptr) == NULL) {                   \
            goto exit;                          \
        }                                       \
    } while (0)

#define _BCM_FIELD_COMP_ALLOC(_ptr, _sz, _str)                   \
            do                                              \
            {                                               \
                if ((_ptr) == NULL) {                       \
                    (_ptr) = sal_alloc(_sz, _str);          \
                } else {                                    \
                    goto exit;                              \
                }                                           \
                _BCM_FIELD_COMP_NULL_CHECK(_ptr);                \
                sal_memset(_ptr, 0, _sz);                   \
            } while(0)

#define _BCM_FIELD_COMP_FREE(_ptr)                   \
    do {                                        \
        if ((_ptr) != NULL) {                   \
            sal_free((void *)(_ptr));           \
            (_ptr) = NULL;                      \
        }                                       \
    } while (0)

#endif /* _BCM_FIELD_COMPRESSION_H */

