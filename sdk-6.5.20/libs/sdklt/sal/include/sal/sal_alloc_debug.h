/*! \file sal_alloc_debug.h
 *
 * Debug hooks for sal_alloc API.
 *
 * These hooks allow an application to intercept all calls to the \ref
 * sal_alloc API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_ALLOC_DEBUG_H
#define SAL_ALLOC_DEBUG_H

#include <sal_config.h>

/*! Debug hooks for SAL allc API. */
typedef struct sal_debug_alloc_s {

    /*! Debug hook for \ref sal_alloc. */
    void *(*alloc)(size_t sz, char *s);

    /*! Debug hook for \ref sal_free. */
    void (*free)(void *addr);

} sal_debug_alloc_t;


/*! Macro to declare debug hooks in SAL implementation. */
#define SAL_ALLOC_DEBUG_DECL sal_debug_alloc_t sal_debug_alloc

/*! SAL debug hooks are currently stored in a global variable. */
extern sal_debug_alloc_t sal_debug_alloc;

/*! Special memory signature to identify intercepted API calls. */
#define SAL_ALLOC_SIG   0xd3adb33f

/*! Write debug signature to \c _addr. */
#define SAL_ALLOC_SIG_SET(_addr) do {           \
        *((uint32_t *)_addr) = SAL_ALLOC_SIG;   \
    } while (0)

/*! Clear debug signature from \c _addr. */
#define SAL_ALLOC_SIG_CLR(_addr) do {           \
        *((uint32_t *)_addr) = 0;               \
    } while (0)

/*! Validate debug signature at \c _addr. */
#define SAL_ALLOC_SIG_CHK(_addr) \
    (_addr && *((uint32_t *)_addr) == SAL_ALLOC_SIG)

/*! Macro to intercept \ref sal_alloc in the SAL implementation. */
#define SAL_ALLOC_DEBUG_ALLOC(_sz, _s)  do {                    \
        if (sal_debug_alloc.alloc && _s && _s[0] != '#') {      \
            return sal_debug_alloc.alloc(_sz, _s);              \
        }                                                       \
    } while (0)

/*! Macro to intercept \ref sal_free in the SAL implementation. */
#define SAL_ALLOC_DEBUG_FREE(_addr)  do {                               \
        if (sal_debug_alloc.free && !SAL_ALLOC_SIG_CHK(_addr)) {        \
            sal_debug_alloc.free(_addr);                                \
            return;                                                     \
        }                                                               \
    } while (0)

/*! Macro to let application assign debug hook for \ref sal_alloc. */
#define SAL_ALLOC_DEBUG_ALLOC_SET(_f)  do {     \
        sal_debug_alloc.alloc = _f;             \
    } while (0)

/*! Macro to let application assign debug hook for \ref sal_free. */
#define SAL_ALLOC_DEBUG_FREE_SET(_f)  do {      \
        sal_debug_alloc.free = _f;              \
    } while (0)

#endif /* SAL_ALLOC_DEBUG_H */
