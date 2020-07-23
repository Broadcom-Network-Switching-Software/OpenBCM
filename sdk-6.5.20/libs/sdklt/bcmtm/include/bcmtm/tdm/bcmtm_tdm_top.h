/*! \file bcmtm_tdm_top.h
 *
 * Top header for TDM algorithm.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_TDM_TOP_H
#define BCMTM_TDM_TOP_H

#ifdef _TDM_STANDALONE
    /*! General headers. */
    #include <alloca.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    /*! TDM prints. */
    #define TDM_LOG_NORMAL(...)           \
                printf(__VA_ARGS__)
    #define TDM_LOG_WARN(...)             \
                printf("TDM WARN:  ");    \
                printf(__VA_ARGS__)
    #define TDM_LOG_ERROR(...)            \
                printf("TDM ERROR:  ");   \
                printf(__VA_ARGS__)

    #define TDM_PRINT8(a,b,c,d,e,f,g,h,i) TDM_LOG_NORMAL(a,b,c,d,e,f,g,h,i)
    #define TDM_PRINT7(a,b,c,d,e,f,g,h) TDM_LOG_NORMAL(a,b,c,d,e,f,g,h)
    #define TDM_PRINT6(a,b,c,d,e,f,g) TDM_LOG_NORMAL(a,b,c,d,e,f,g)
    #define TDM_PRINT5(a,b,c,d,e,f) TDM_LOG_NORMAL(a,b,c,d,e,f)
    #define TDM_PRINT4(a,b,c,d,e) TDM_LOG_NORMAL(a,b,c,d,e)
    #define TDM_PRINT3(a,b,c,d) TDM_LOG_NORMAL(a,b,c,d)
    #define TDM_PRINT2(a,b,c) TDM_LOG_NORMAL(a,b,c)
    #define TDM_PRINT1(a,b) TDM_LOG_NORMAL(a,b)
    #define TDM_PRINT0(a) TDM_LOG_NORMAL(a)

    #define TDM_WARN8(a,b,c,d,e,f,g,h,i) TDM_LOG_WARN(a,b,c,d,e,f,g,h,i)
    #define TDM_WARN7(a,b,c,d,e,f,g,h) TDM_LOG_WARN(a,b,c,d,e,f,g,h)
    #define TDM_WARN6(a,b,c,d,e,f,g) TDM_LOG_WARN(a,b,c,d,e,f,g)
    #define TDM_WARN5(a,b,c,d,e,f) TDM_LOG_WARN(a,b,c,d,e,f)
    #define TDM_WARN4(a,b,c,d,e) TDM_LOG_WARN(a,b,c,d,e)
    #define TDM_WARN3(a,b,c,d) TDM_LOG_WARN(a,b,c,d)
    #define TDM_WARN2(a,b,c) TDM_LOG_WARN(a,b,c)
    #define TDM_WARN1(a,b) TDM_LOG_WARN(a,b)
    #define TDM_WARN0(a) TDM_LOG_WARN(a)

    #define TDM_ERROR8(a,b,c,d,e,f,g,h,i) TDM_LOG_ERROR(a,b,c,d,e,f,g,h,i)
    #define TDM_ERROR7(a,b,c,d,e,f,g,h) TDM_LOG_ERROR(a,b,c,d,e,f,g,h)
    #define TDM_ERROR6(a,b,c,d,e,f,g) TDM_LOG_ERROR(a,b,c,d,e,f,g)
    #define TDM_ERROR5(a,b,c,d,e,f) TDM_LOG_ERROR(a,b,c,d,e,f)
    #define TDM_ERROR4(a,b,c,d,e) TDM_LOG_ERROR(a,b,c,d,e)
    #define TDM_ERROR3(a,b,c,d) TDM_LOG_ERROR(a,b,c,d)
    #define TDM_ERROR2(a,b,c) TDM_LOG_ERROR(a,b,c)
    #define TDM_ERROR1(a,b) TDM_LOG_ERROR(a,b)
    #define TDM_ERROR0(a) TDM_LOG_ERROR(a)

    #define TDM_BIG_BAR TDM_LOG_NORMAL("TDM: %s\n", \
        "##################################################################");
    #define TDM_SML_BAR TDM_LOG_NORMAL("TDM: %s\n", \
        "------------------------------------------------------------------");
    /*! Compiler memory ops. */
    #define TDM_EXTERN extern
    #define TDM_ALLOC(_sz,_id) malloc(_sz)
    #define TDM_FREE(_sz) free(_sz)
    #define TDM_MEMSET(_addr,_val,_len) memset(_addr, _val, _len)
    #define TDM_MEMCPY(_dst,_src,_len) memcpy(_dst,_src,_len)
    #define TDM_ARRSET(_addr, _val, _len)                   \
                    {                                       \
                        int idx;                            \
                        for (idx = 0; idx < _len; idx++) {  \
                            _addr[idx] = _val;              \
                        }                                   \
                    }
    /*! TDM headers. */
    #include <bcmtm_tdm_defines.h>
    #include <bcmtm_tdm_misc.h>
    #include <bcmtm_tdm_sched.h>
    #include <bcmtm_tdm_cfg.h>
    #include <bcmtm_tdm_drv.h>
#else
    /*! General headers. */
    #include <bsl/bsl.h>
    #include <sal/sal_alloc.h>
    #include <sal/sal_libc.h>

    #ifndef BSL_LS_SOC_TDM
    #define BSL_LS_SOC_TDM   BSL_LS_BCMTM_TDM
    #endif
    #ifndef BSL_LS_SOC_PORT
    #define BSL_LS_SOC_PORT  BSL_LS_BCMTM_FLEXPORT
    #endif

    /*! TDM prints. */
    #define TDM_PRINT8(a,b,c,d,e,f,g,h,i) \
            LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h,i))
    #define TDM_PRINT7(a,b,c,d,e,f,g,h)   \
            LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h))
    #define TDM_PRINT6(a,b,c,d,e,f,g)     \
            LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
    #define TDM_PRINT5(a,b,c,d,e,f)       \
            LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
    #define TDM_PRINT4(a,b,c,d,e)         \
            LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
    #define TDM_PRINT3(a,b,c,d)           \
            LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
    #define TDM_PRINT2(a,b,c)             \
            LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
    #define TDM_PRINT1(a,b)               \
            LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b))
    #define TDM_PRINT0(a)                 \
            LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META("%s"),a))

    #define TDM_WARN6(a,b,c,d,e,f,g)      \
                LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
    #define TDM_WARN5(a,b,c,d,e,f)        \
                LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
    #define TDM_WARN4(a,b,c,d,e)          \
                LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
    #define TDM_WARN3(a,b,c,d)            \
                LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
    #define TDM_WARN2(a,b,c)              \
                LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
    #define TDM_WARN1(a,b)                \
                LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b))
    #define TDM_WARN0(a)                  \
                LOG_WARN(BSL_LS_SOC_TDM, (BSL_META("%s"),a))

    #define TDM_ERROR8(a,b,c,d,e,f,g,h,i) \
                LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h,i))
    #define TDM_ERROR7(a,b,c,d,e,f,g,h)   \
                LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h))
    #define TDM_ERROR6(a,b,c,d,e,f,g)     \
                LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
    #define TDM_ERROR5(a,b,c,d,e,f)       \
                LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
    #define TDM_ERROR4(a,b,c,d,e)         \
                LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
    #define TDM_ERROR3(a,b,c,d)           \
                LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
    #define TDM_ERROR2(a,b,c)             \
                LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
    #define TDM_ERROR1(a,b)               \
                LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b))
    #define TDM_ERROR0(a)                 \
                LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a)))

    #define TDM_BIG_BAR TDM_PRINT1("TDM: %s\n", \
        "##################################################################");
    #define TDM_SML_BAR TDM_PRINT1("TDM: %s\n", \
        "------------------------------------------------------------------");
    /*! Compiler memory ops. */
    #define TDM_EXTERN
    #define TDM_ALLOC(_sz,_id) sal_alloc(_sz,_id)
    #define TDM_FREE(_sz) sal_free(_sz)
    #define TDM_MEMSET(_addr,_val,_len) sal_memset(_addr, _val, _len)
    #define TDM_MEMCPY(_dst,_src,_len) sal_memcpy(_dst,_src,_len)
    #define TDM_ARRSET(_addr, _val, _len)                   \
                    {                                       \
                        int idx;                            \
                        for (idx = 0; idx < _len; idx++) {  \
                            _addr[idx] = _val;              \
                        }                                   \
                    }
    /*! TDM headers. */
    #include <bcmtm/tdm/bcmtm_tdm_defines.h>
    #include <bcmtm/tdm/bcmtm_tdm_misc.h>
    #include <bcmtm/tdm/bcmtm_tdm_sched.h>
    #include <bcmtm/tdm/bcmtm_tdm_cfg.h>
    #include <bcmtm/tdm/bcmtm_tdm_drv.h>
#endif

#endif /* BCMTM_TDM_TOP_H */
