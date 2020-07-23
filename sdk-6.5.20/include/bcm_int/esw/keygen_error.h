/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_INT_FIELD_KEYGEN_ERROR_H
#define _BCM_INT_FIELD_KEYGEN_ERROR_H

#include <shared/bsl.h>
#include <bcm/error.h>

#define BCMI_KEYGEN_FUNC_ENTER(_unit) bcm_error_t _func_rv = BCM_E_NONE

#define BCMI_KEYGEN_FUNC_EXIT() return _func_rv

#define BCMI_KEYGEN_EXIT() goto exit

#define BCMI_KEYGEN_FUNC_VAL_IS(_rv)  (_func_rv == (_rv))

#define BCMI_KEYGEN_FUNC_ERR()  BCMI_KEYGEN_FAILURE(_func_rv)

#define BCMI_KEYGEN_SUCCESS(_expr) ((_expr) >= 0)

#define BCMI_KEYGEN_FAILURE(_expr) ((_expr) < 0)

#define BCMI_KEYGEN_IF_ERR_EXIT(_expr)     \
    do {                                   \
        bcm_error_t _rv = _expr;           \
        if (BCMI_KEYGEN_FAILURE(_rv)) {    \
            _func_rv = (_rv);              \
            BCMI_KEYGEN_EXIT();            \
        }                                  \
    } while(0)

#define BCMI_KEYGEN_IF_ERR_VERBOSE_EXIT(_expr)   \
    do {                                         \
        bcm_error_t _rv = _expr;                 \
        if (BCMI_KEYGEN_FAILURE(_rv)) {          \
            _func_rv = _rv;                      \
            BCMI_KEYGEN_EXIT();                  \
        }                                        \
    } while(0)


#define BCMI_KEYGEN_RETURN_VAL_EXIT(_expr) \
    do {                                   \
        _func_rv = _expr;                  \
        BCMI_KEYGEN_EXIT();                \
    } while (0)

#define BCMI_KEYGEN_NULL_CHECK(_ptr, _rv)  \
    do {                                   \
        if ((_ptr) == NULL) {              \
            _func_rv = _rv;                \
            BCMI_KEYGEN_EXIT();            \
        }                                  \
    } while (0)

#define BCMI_KEYGEN_ALLOC(_ptr, _sz, _str)          \
    do                                              \
    {                                               \
        if ((_ptr) == NULL) {                       \
            (_ptr) = sal_alloc(_sz, _str);          \
        } else {                                    \
            BCMI_KEYGEN_EXIT();                     \
        }                                           \
        BCMI_KEYGEN_NULL_CHECK(_ptr, BCM_E_MEMORY); \
        sal_memset(_ptr, 0, _sz);                   \
    } while(0)

#define BCMI_KEYGEN_FREE(_ptr)                  \
    do {                                        \
        if ((_ptr) != NULL) {                   \
            sal_free((void *)(_ptr));           \
            (_ptr) = NULL;                      \
        }                                       \
    } while (0)

#define BCMI_KEYGEN_IF_ERR_EXIT_EXCEPT_IF(_expr, _rv_except)  \
    do {                                                      \
        int _rv = _expr;                                      \
        if (BCMI_KEYGEN_FAILURE(_rv) && _rv != _rv_except) {  \
            _func_rv = _rv;                                   \
            BCMI_KEYGEN_EXIT();                               \
        }                                                     \
    } while(0)

#endif

