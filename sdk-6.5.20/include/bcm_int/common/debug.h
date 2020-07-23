/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __BCM_DEBUG_EX_H__
#define __BCM_DEBUG_EX_H__

#include <bcm/debug.h>
#if (defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
#include <soc/dcmn/dcmn_gen_err.h>
#endif

#if defined(BROADCOM_DEBUG)

/*
 * Unified message format for all modules 
 * In order to use this format _ERR_MSG_MODULE_NAME must be defined 
 * For example in Fabric module the following will be in top of the c file: 
 *  #ifdef _ERR_MSG_MODULE_NAME
 *    #error "_ERR_MSG_MODULE_NAME redefined"
 *  #endif
 *  #define _ERR_MSG_MODULE_NAME FABRIC
 * And in the bottom:
 *  #undef _ERR_MSG_MODULE_NAME
*/
#define _BCM_MSG(string) "%s[%d]%s unit %d: " string "\n", __FILE__, __LINE__, FUNCTION_NAME(), unit
#define _BCM_MSG_NO_UNIT(string) "%s[%d]%s: " string "\n", __FILE__, __LINE__, FUNCTION_NAME()
#define _BSL_BCM_MSG(string) _ERR_MSG_MODULE_NAME, unit, string "\n"
#define _BSL_BCM_MSG_NO_UNIT(string) _ERR_MSG_MODULE_NAME, BSL_UNIT_UNKNOWN, string "\n"

#else
#define _BCM_MSG(string) string
#define _BCM_MSG_NO_UNIT(string) string

#define _BSL_BCM_MSG(string) _ERR_MSG_MODULE_NAME, unit, string
#define _BSL_BCM_MSG_NO_UNIT(string) _ERR_MSG_MODULE_NAME, BSL_UNIT_UNKNOWN, string
#endif
#define _BSL_BCM_MSG_STR(string) _ERR_MSG_MODULE_NAME, unit, string

/*Single point of exit*/
#define BCM_EXIT goto exit

/* Must appear at each function right after parameters definition */
#define BCM_INIT_FUNC_DEFS \
    int _rv = BCM_E_NONE, _lock_taken = 0; \
    (void)_lock_taken; \
    LOG_DEBUG(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "enter\n"))); 

#define BCM_INIT_FUNC_DEFS_NO_UNIT \
    int _rv = BCM_E_NONE, _lock_taken = 0; \
    (void)_lock_taken; \
    LOG_DEBUG(_ERR_MSG_MODULE_NAME, (BSL_META("enter\n"))); 

#define BCM_FUNC_ERROR \
    BCM_FAILURE(_rv)

#if (defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
#define BCM_FUNC_IF_ERR(_rv) \
    GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, _rv); \
    if (BCM_FAILURE(_rv))
#else
#define BCM_FUNC_IF_ERR(_rv) \
    if (BCM_FAILURE(_rv))
#endif

#define BCM_FUNC_RETURN \
    LOG_DEBUG(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "exit\n"))); \
    return _rv;

#define BCM_RETURN_VAL_EXIT(rv_override) \
    _rv = rv_override; \
    BCM_EXIT;

#define BCM_FUNC_RETURN_VOID \
    LOG_DEBUG(_ERR_MSG_MODULE_NAME, (BSL_META_U(unit, "exit\n"))); \
    COMPILER_REFERENCE(_rv);

#define BCM_FUNC_RETURN_NO_UNIT \
    LOG_DEBUG(_ERR_MSG_MODULE_NAME, (BSL_META("exit\n"))); \
    return _rv;

#define BCM_FUNC_RETURN_VOID_NO_UNIT \
    LOG_DEBUG(_ERR_MSG_MODULE_NAME, (BSL_META("exit\n"))); \
    COMPILER_REFERENCE(_rv);

#if (defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
#define BCM_IF_ERR_CONT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if(__err__rc != BCM_E_NONE) { \
            LOG_WARN(_ERR_MSG_MODULE_NAME,       \
                     (BSL_META_U(unit, "%s\n"),  \
                      bcm_errmsg(__err__rc)));   \
            _rv = __err__rc; \
        } \
      } while(0)

#define BCM_IF_ERR_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if(__err__rc != BCM_E_NONE) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME,      \
                      (BSL_META_U(unit, "%s\n"), \
                       bcm_errmsg(__err__rc)));  \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCM_IF_ERR_NOT_UNAVAIL_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if(__err__rc != BCM_E_NONE && __err__rc != BCM_E_UNAVAIL) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME,      \
                      (BSL_META_U(unit, "%s\n"), \
                       bcm_errmsg(__err__rc)));  \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCM_IF_ERR_NOT_E_NOT_FOUND_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
		GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if (__err__rc != BCM_E_NONE) { \
            if(__err__rc != BCM_E_NOT_FOUND) { \
                LOG_ERROR(_ERR_MSG_MODULE_NAME,         \
                          (BSL_META_U(unit, "%s\n"),    \
                           bcm_errmsg(__err__rc)));     \
            } \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)
#else
#define BCM_IF_ERR_CONT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            LOG_WARN(_ERR_MSG_MODULE_NAME,       \
                     (BSL_META_U(unit, "%s\n"),  \
                      bcm_errmsg(__err__rc)));   \
            _rv = __err__rc; \
        } \
      } while(0)

#define BCM_IF_ERR_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME,      \
                      (BSL_META_U(unit, "%s\n"), \
                       bcm_errmsg(__err__rc)));  \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCM_IF_ERR_NOT_UNAVAIL_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE && __err__rc != BCM_E_UNAVAIL) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME,      \
                      (BSL_META_U(unit, "%s\n"), \
                       bcm_errmsg(__err__rc)));  \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCM_IF_ERR_NOT_E_NOT_FOUND_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if (__err__rc != BCM_E_NONE) { \
            if(__err__rc != BCM_E_NOT_FOUND) { \
                LOG_ERROR(_ERR_MSG_MODULE_NAME,  \
                          (BSL_META_U(unit, "%s\n"),    \
                           bcm_errmsg(__err__rc)));     \
            } \
            else \
            { \
                LOG_VERBOSE(_ERR_MSG_MODULE_NAME,  \
                          (BSL_META_U(unit, "%s\n"),    \
                           bcm_errmsg(__err__rc)));     \
            } \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)
#endif

#define BCM_IF_ERR_EXIT_NO_UNIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME,      \
                      (BSL_META("%s\n"),         \
                       bcm_errmsg(__err__rc)));  \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCM_IF_ERR_EXIT_MSG(_rc, stuff) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME, stuff); \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCM_ERR_EXIT_MSG(_rc, stuff) \
      do { \
            LOG_ERROR(_ERR_MSG_MODULE_NAME, stuff); \
            _rv = _rc; \
            BCM_EXIT; \
      } while(0)
      
#define BCM_ERR_EXIT_NO_MSG(_rc) \
      do { \
            _rv = _rc; \
            BCM_EXIT; \
      } while(0)
#define BCM_NULL_CHECK(arg) \
    do {   \
        if ((arg) == NULL) \
            { BCM_ERR_EXIT_MSG(BCM_E_PARAM,(BSL_META_U(unit, "null parameter\n"))); } \
    } while (0)

#define BCM_NULL_CHECK_NO_UNIT(arg) \
    do {   \
        if ((arg) == NULL) \
            { BCM_ERR_EXIT_MSG(BCM_E_PARAM,(BSL_META("null parameter\n"))); } \
    } while (0)

#define BCM_ALLOC(pointer, mem_size, alloc_comment)   \
  do {   \
      if((pointer) == NULL) { \
          (pointer) = sal_alloc((mem_size), (alloc_comment)); \
      } else {\
          BCM_ERR_EXIT_MSG(BCM_E_PARAM,(BSL_META("attempted to allocate to a non NULL pointer: %s\n"), (alloc_comment))); \
      } \
  } while (0)

#define BCM_FREE(pointer)   \
  do {   \
      if((pointer) != NULL) { \
          sal_free((void*)(pointer)); \
          (pointer) = NULL; \
      } \
  } while (0)

#ifdef BROADCOM_DEBUG
/* If expr is false, then an error will be thrown and the error message will be the expression failed. */
/* This is a version of assert without exit. */
# define BCM_VERIFY(expr) do { if (!(expr)) { BCM_ERR_EXIT_MSG(BCM_E_FAIL, ("ASSERTION FAILED (%s:%d): %s\n", __FILE__, __LINE__, #expr)); } } while(0)
#else
# define BCM_VERIFY(expr) ((void)0)
#endif /* BROADCOM_DEBUG */

#define BCMDNX_FUNC_ERROR \
    BCM_FAILURE(_rv)

/* Must appear at each function right after parameters definition */
#define BCMDNX_INIT_FUNC_DEFS \
    int _rv = BCM_E_NONE, _lock_taken = 0; \
    (void)_lock_taken; \
    _bsl_vverbose(_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("enter")); 

#define BCMDNX_INIT_FUNC_DEFS_NO_UNIT \
    int unit; \
	int _rv = BCM_E_NONE, _lock_taken = 0; \
    (void)_lock_taken; \
    unit = BSL_UNIT_UNKNOWN; \
    _bsl_vverbose(_ERR_MSG_MODULE_NAME, unit, _BCM_MSG_NO_UNIT("enter")); 

#define BCMDNX_FUNC_RETURN \
    _bsl_vverbose(_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("exit")); \
    return _rv;

#define BCMDNX_FUNC_RETURN_VOID \
    _bsl_vverbose(_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("exit"));\
    COMPILER_REFERENCE(_rv);

#define BCMDNX_FUNC_RETURN_NO_UNIT \
    _bsl_vverbose(_ERR_MSG_MODULE_NAME, BSL_UNIT_UNKNOWN, _BCM_MSG_NO_UNIT("exit")); \
    return _rv;

#define BCMDNX_FUNC_RETURN_VOID_NO_UNIT \
    _bsl_vverbose(_ERR_MSG_MODULE_NAME, BSL_UNIT_UNKNOWN, _BCM_MSG_NO_UNIT("exit"));\
    COMPILER_REFERENCE(_rv);

#if (defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
#define BCMDNX_IF_ERR_CONT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_warn(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            _rv = __err__rc; \
        } \
      } while(0)

#define BCMDNX_IF_ERR_VERB_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_verbose(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCMDNX_IF_ERR_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

/* meant for use after exit: , In case of error log it and if it is the first error, update _rv */
#define BCMDNX_IF_ERR_REPORT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            if (_rv != BCM_E_NONE) _rv = __err__rc; \
        } \
      } while(0)

#define BCMDNX_IF_ERR_NOT_UNAVAIL_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if(__err__rc != BCM_E_NONE && __err__rc != BCM_E_UNAVAIL) { \
            _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        GEN_ERR_CHK(unit, GEN_ERR_TYPE_BCM, __err__rc); \
        if (__err__rc != BCM_E_NONE) { \
            if(__err__rc != BCM_E_NOT_FOUND) { \
                _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            } \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)
#define BCMDNX_IF_ERR_NOT_E_FAIL_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if (__err__rc != BCM_E_NONE) { \
            if(__err__rc != BCM_E_FAIL) { \
                _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            } else { \
                _bsl_verbose(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            }\
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)
#else
#define BCMDNX_IF_ERR_CONT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_warn(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            _rv = __err__rc; \
        } \
      } while(0)

#define BCMDNX_IF_ERR_VERB_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_verbose(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)
#define BCMDNX_IF_ERR_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)
/* meant for use after exit: , In case of error log it and if it is the first error, update _rv */
#define BCMDNX_IF_ERR_REPORT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            if (_rv != BCM_E_NONE) _rv = __err__rc; \
        } \
      } while(0)

#define BCMDNX_IF_ERR_NOT_UNAVAIL_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE && __err__rc != BCM_E_UNAVAIL) { \
            _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if (__err__rc != BCM_E_NONE) { \
            if(__err__rc != BCM_E_NOT_FOUND) { \
                _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            } \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCMDNX_IF_ERR_NOT_E_FAIL_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if (__err__rc != BCM_E_NONE) { \
            if(__err__rc != BCM_E_FAIL) { \
                _bsl_error(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            } else { \
                _bsl_verbose(_BSL_BCM_MSG("%s"),bcm_errmsg(__err__rc)); \
            }\
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)
#endif

#define BCMDNX_IF_ERR_EXIT_NO_UNIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_error(_BSL_BCM_MSG_NO_UNIT("%s"),bcm_errmsg(__err__rc)); \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCMDNX_IF_ERR_EXIT_MSG(_rc, stuff) \
      do { \
        int __err__rc = _rc; \
        if(__err__rc != BCM_E_NONE) { \
            _bsl_error stuff ; \
            _rv = __err__rc; \
            BCM_EXIT; \
        } \
      } while(0)

#define BCMDNX_ERR_EXIT_MSG(_rc, stuff) \
      do { \
            _bsl_error stuff ; \
            _rv = _rc; \
            BCM_EXIT; \
      } while(0)      

#define BCMDNX_ERR_EXIT_VERB_MSG(_rc, stuff) \
      do { \
            _bsl_verbose stuff ; \
            _rv = _rc; \
            BCM_EXIT; \
      } while(0)

#define BCMDNX_NULL_CHECK(arg) \
    do {   \
        if ((arg) == NULL) \
        { BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("null parameter" ))); } \
    } while (0)

#define BCMDNX_NULL_CHECK_NO_UNIT(arg) \
    do {   \
        if ((arg) == NULL) \
        { BCMDNX_ERR_EXIT_MSG_NO_UNIT(BCM_E_PARAM,(_BSL_BCM_MSG_NO_UNIT("null parameter" ))); } \
    } while (0)

#define BCMDNX_ALLOC(pointer, mem_size, alloc_comment)   \
  do {   \
      if((pointer) == NULL) { \
          (pointer) = sal_alloc((mem_size), (alloc_comment)); \
      } else {\
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG_NO_UNIT("attempted to allocate to a non NULL pointer: %s"), (alloc_comment))); \
      } \
  } while (0)

#define BCMDNX_ALLOC_AND_CLEAR(pointer, mem_size, alloc_comment)   \
        do {   \
            if((pointer) == NULL) { \
                (pointer) = sal_alloc((mem_size), (alloc_comment)); \
                if ((pointer) == NULL) { \
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory"))); \
                } \
                sal_memset((pointer), 0x00, mem_size); \
            } else {\
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG_NO_UNIT("attempted to allocate to a non NULL pointer: %s"), (alloc_comment))); \
            } \
        } while (0)

#ifdef BROADCOM_DEBUG
/* If expr is false, then an error will be thrown and the error message will be the expression failed. */
/* This is a version of assert without exit. */
# define BCMDNX_VERIFY(expr) do { if (!(expr)) { BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_ERR_MSG_MODULE_NAME, BSL_UNIT_UNKNOWN, "ASSERTION FAILED (%s:%d): %s\n", __FILE__, __LINE__, #expr)); } } while(0)
#else
# define BCMDNX_VERIFY(expr) ((void)0)
#endif /* BROADCOM_DEBUG */

#endif /* __BCM_DEBUG_EX_H__ */
