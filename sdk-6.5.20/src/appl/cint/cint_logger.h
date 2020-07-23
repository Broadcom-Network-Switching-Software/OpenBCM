/*
 * $Id: cint_logger.h
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger.h
 * Purpose:     CINT logger, see cint_logger.c
 */

#ifndef __CINT_LOGGER_H__
#define __CINT_LOGGER_H__

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1

#include <shared/bitop.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include "cint_config.h"
#include "cint_types.h"

/*
 * Adds logger structures and variables definitions to the CINT interpreter
 */
extern void cint_logger_cint_init(void);

/*
 * Fetches the per-thread data maintained by the logger
 */
extern void* cint_logger_thread_specific(void);

/*
 * Logger top-level function - invoked from BCM dispatch.c
 */
extern void cint_log_function_arguments (const char *fn, void **api_params, unsigned int flags, int call_id, void *arg_ptrs[], int *skip_dispatch);

/*
 * API behind CINT_LOGGER_SET_CALLER_CONTEXT macro  application can use to
 * indicate the FILE, LINE and FUNCTION from which an API is invoked
 */
extern void cint_logger_set_caller_context (const char *f, int l, const char *F);

/*
 * CINT internal routine to fetch system timestamp in usec resolution
 */
extern unsigned long cint_timer_get(void);

static inline void cint_logger_update_arg_ptrs (void *arg_ptrs[], int nargs, ...)
{
    int i;
    va_list ap;

    va_start(ap, nargs);

    for (i = 0; i < nargs; i++) {
        arg_ptrs[i] = va_arg(ap, void*);
    }

    va_end(ap);
}

/*
 * structure to hold related run-time statistics for any given object together
 */
typedef struct cint_logger_rtime_s {

    /*
     * the last run-time computed
     */
    unsigned int last;

    /*
     * minimum run-time measurement so far
     */
    unsigned int min;

    /*
     * maximum run-time measurement so far
     */
    unsigned int max;

    /*
     * average of all run-times measured so far
     */
    unsigned int avg;

} cint_logger_rtime_t;

/*
 * Initialize a run-time structure. Mainly set min to something large.
 */
#define CINT_LOGGER_INIT_RTIME_T(s) \
{                                   \
    (s).last = 0;                   \
    (s).min  = ~0x0;                \
    (s).max  = 0;                   \
    (s).avg  = 0;                   \
}

/*
 * enum for all the logger modes
 */
typedef enum {

    /*
     * preload or cache the CINT data
     */
    cintLoggerModePreload,

    /*
     * log API calls to BSL layer and source
     */
    cintLoggerModeLog,

    /*
     * only cache API call data into memory
     */
    cintLoggerModeCache,

    /*
     * replaying previously generated logs, expected to be used on SVK
     */
    cintLoggerModeReplay,

} cint_logger_mode_t;

/*
 * structure for per API cached data
 */
typedef struct cint_logger_api_params_s {

    /*
     * prev and next pointers for building a link list of cached data
     */
    void *next, *prev;

    /*
     * API for which this cache was created
     */
    char *fn;

    /*
     * CINT parameters data for the API
     */
    cint_parameter_desc_t *params;

    /*
     * sizes of the basetypes of each parameter
     */
    int basetype_size[CINT_CONFIG_MAX_FPARAMS+1];

    /*
     * whether the basetype is an array (i.e. typedef)
     */
    SHR_BITDCLNAME(is_basetype_arr, CINT_CONFIG_MAX_FPARAMS+1);

    /*
     * whether the parameter is a char pointer
     */
    SHR_BITDCLNAME(is_charptr, CINT_CONFIG_MAX_FPARAMS+1);

    /*
     * number of API arguments +1 (return value)
     */
    int nargs;

    /*
     * number of times the API was invoked
     */
    int called_count;

    /*
     * run-time data for loading the argument pointer array
     */
    cint_logger_rtime_t arg_ptrs;

    /*
     * run-time data for the preload phase of this API
     */
    cint_logger_rtime_t preload;

    /*
     * run-time data for the before-dispatch logging for this API
     */
    cint_logger_rtime_t call1;

    /*
     * run-time data for the dispatch routine for this API
     */
    cint_logger_rtime_t dispatch;

    /*
     * run-time data for the after-dispatch logging for this API
     */
    cint_logger_rtime_t call2;

    /*
     * total run-time data for the API
     */
    cint_logger_rtime_t total;

} cint_logger_api_params_t;

/*
 * Initialize the cached data structure for an API. Mainly the run-time data.
 */
#define CINT_LOGGER_INIT_API_PARAMS_T(p)  \
{                                            \
    CINT_MEMSET((p), 0, sizeof(*(p)));       \
    CINT_LOGGER_INIT_RTIME_T((p)->arg_ptrs); \
    CINT_LOGGER_INIT_RTIME_T((p)->preload);  \
    CINT_LOGGER_INIT_RTIME_T((p)->call1);    \
    CINT_LOGGER_INIT_RTIME_T((p)->dispatch); \
    CINT_LOGGER_INIT_RTIME_T((p)->call2);    \
    CINT_LOGGER_INIT_RTIME_T((p)->total);    \
}

/*
 * structure for per-thread logger data
 */
typedef struct cint_logger_thread_data_s {

    /*
     * whether logging is disabled for this thread
     */
    int disabled;

    /*
     * whether logging is in progress for this thread
     */
    int logger_is_active;

    /*
     * application is reponsible for indicating the following three to the
     * logger accurately. This information will help to reconstruct the
     * structure of the application code.
     * Respectively the FILE, LINE and FUNCTION from where the API was invoked
     */
    const char *caller_file;
    int caller_line;
    const char *caller_func;

} cint_logger_thread_data_t;

/*
 * structure holding the data relevant for a call to the API
 */
typedef struct cint_logger_call_ctxt_s {

    /*
     * API to which this context belongs
     */
    char *api;

    /*
     * flags with which the top-level logger routine was called
     */
    unsigned int flags;

    /*
     * thread_id of the caller.
     */
    sal_thread_t caller_tid;

    /*
     * name of the calling thread
     */
    const char *caller_tname;

    /*
     * application FILE from which the API was called
     */
    const char *caller_file;

    /*
     * LINE in FILE from which the API was called
     */
    int caller_line;

    /*
     * allocation FUNCTION from which the API was called
     */
    const char *caller_function;

    /*
     * pointer to the skip_dispatch boolean
     */
    int *skip_dispatch;

    /*
     * block-ID - generally a counter incremented at each call
     */
    int call_id;

} cint_logger_call_ctxt_t;

typedef int (*cint_logger_user_filter_cb_t)(cint_logger_thread_data_t*, cint_logger_call_ctxt_t*, cint_logger_api_params_t*, void * []);

/*
 * structure to hold the global configuration data for the logger
 */
typedef struct cint_logger_global_cfg_data_s {

    /*
     * whether the logger is enabled?
     */
    int enabled;

    /*
     * logger mode
     */
    cint_logger_mode_t mode;

    /*
     * BSL layer from which logs will be sent
     */
    bsl_layer_t logLayer;

    /*
     * BSL source from which logs will be sent
     */
    bsl_source_t logSource;

    /*
     * name of the CINT function which will serve as a filter for the logger
     */
    char *cint_filter_fn;

    /*
     * callback function provided by the application
     */
    cint_logger_user_filter_cb_t user_filter_cb;

    /*
 */
    sal_tls_key_t *tls_key;

    /*
     * logger mutex
     */
    sal_mutex_t mutex;

    /*
     * head of the list of API parameters
     */
    cint_logger_api_params_t *ctxt_head;

    /*
     * running counter which serves as block-id in the logs
     */
    int count;

    /*
     * the base indent level in terms of prefixed spaces when generating logs
     */
    int base_indent;

    /*
     * parameters for the logging to file
     */
    int log_to_file;
    char *logfile;
    void *logfile_fp;

} cint_logger_global_cfg_data_t;

extern cint_logger_global_cfg_data_t cint_logger_cfg;
#define CINT_LOGGER_LOCK   sal_mutex_take(cint_logger_cfg.mutex, sal_mutex_FOREVER)
#define CINT_LOGGER_UNLOCK sal_mutex_give(cint_logger_cfg.mutex)


#define UPDATE_AVG_MIN_MAX(sp, item)                        \
{                                                           \
    unsigned long long total;                               \
    total = (sp.item.avg * sp.called_count + sp.item.last); \
    sp.item.avg = total / (sp.called_count + 1);            \
    if (sp.item.last > sp.item.max) {                       \
        sp.item.max = sp.item.last;                         \
    }                                                       \
    if (sp.item.last < sp.item.min) {                       \
        sp.item.min = sp.item.last;                         \
    }                                                       \
}

#define UPDATE_ARG_PTRS(...) \
        cint_logger_update_arg_ptrs(&arg_ptrs[1], nargs, ## __VA_ARGS__)

#define CINT_LOGGER_CALL_DISPATCH(ret, f_api, args, n_args, args_by_ref)                                               \
do {                                                                                                                   \
    static cint_logger_api_params_t *api_params = NULL;                                                                \
    void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1] = { &ret, };                                                             \
    int nargs = n_args;                                                                                                \
    int skip_dispatch = 0;                                                                                             \
    unsigned long total_t1    = 0, total_t2    = 0;                                                                    \
    unsigned long arg_ptrs_t1 = 0, arg_ptrs_t2 = 0;                                                                    \
    unsigned long preload_t1  = 0, preload_t2  = 0;                                                                    \
    unsigned long call1_t1    = 0, call1_t2    = 0;                                                                    \
    unsigned long dispatch_t1 = 0, dispatch_t2 = 0;                                                                    \
    unsigned long call2_t1    = 0, call2_t2    = 0;                                                                    \
    int call_id = 0;                                                                                                   \
                                                                                                                       \
                                                                                                                       \
    total_t1 = cint_timer_get();                                                                                       \
                                                                                                                       \
    preload_t1  = cint_timer_get();                                                                                    \
    if (!api_params) {                                                                                                 \
        /*                                                                                                             \
         * Preload: initialize the CINT infrastructure if not yet done, query and cache the CINT parameter description \
         * for use by successive invocations of the API. Separated as a step by itself so that preload can be done     \
         * for any/all APIs before actual API calls begin, maybe as part of the application init sequence              \
         */                                                                                                            \
        cint_log_function_arguments(__FUNCTION__, (void**)&api_params, CINT_PARAM_IDX,                                 \
                                    call_id, arg_ptrs, &skip_dispatch);                                                \
    }                                                                                                                  \
    preload_t2  = cint_timer_get();                                                                                    \
                                                                                                                       \
    CINT_LOGGER_LOCK;                                                                                                  \
                                                                                                                       \
    cint_logger_cfg.count++;                                                                                           \
    call_id = cint_logger_cfg.count;                                                                                   \
                                                                                                                       \
    CINT_LOGGER_UNLOCK;                                                                                                \
                                                                                                                       \
    if (cint_logger_cfg.mode == cintLoggerModePreload) {                                                               \
        goto store_stats;                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    arg_ptrs_t1 = cint_timer_get();                                                                                    \
    UPDATE_ARG_PTRS args_by_ref;                                                                                       \
    arg_ptrs_t2 = cint_timer_get();                                                                                    \
                                                                                                                       \
    /*                                                                                                                 \
     * Before Dispatch: Log arguments before the dispatch routine is called                                            \
     */                                                                                                                \
    skip_dispatch = 0;                                                                                                 \
    call1_t1 = cint_timer_get();                                                                                       \
    cint_log_function_arguments(__FUNCTION__, (void**)&api_params, CINT_PARAM_IN,                                      \
                                call_id, arg_ptrs, &skip_dispatch);                                                    \
    call1_t2 = cint_timer_get();                                                                                       \
                                                                                                                       \
    /*                                                                                                                 \
     * skip_dispatch may be used by the filter routine to render an API call ineffective. The filter routine has       \
     * access to the ret value variable and can write desired value into it when skipping the dispatch routine         \
     */                                                                                                                \
    if (!skip_dispatch) {                                                                                              \
        dispatch_t1 = cint_timer_get();                                                                                \
        ret = f_api args;                                                                                              \
        dispatch_t2 = cint_timer_get();                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    /*                                                                                                                 \
     * After Dispatch: Log arguments After the dispatch routine is called. This will log the return value and          \
     * any values output by the dispatch routing.                                                                      \
     */                                                                                                                \
    call2_t1 = cint_timer_get();                                                                                       \
    cint_log_function_arguments(__FUNCTION__, (void**)&api_params, CINT_PARAM_OUT,                                     \
                                call_id, arg_ptrs, &skip_dispatch);                                                    \
    call2_t2 = cint_timer_get();                                                                                       \
                                                                                                                       \
    total_t2 = cint_timer_get();                                                                                       \
                                                                                                                       \
store_stats:                                                                                                           \
                                                                                                                       \
    if (api_params) {                                                                                                  \
                                                                                                                       \
        CINT_LOGGER_LOCK;                                                                                              \
                                                                                                                       \
        (*api_params).arg_ptrs.last = arg_ptrs_t2  - arg_ptrs_t1 ;                                                     \
        (*api_params).preload.last  = preload_t2   - preload_t1  ;                                                     \
        (*api_params).call1.last    = call1_t2     - call1_t1    ;                                                     \
        (*api_params).dispatch.last = dispatch_t2  - dispatch_t1 ;                                                     \
        (*api_params).call2.last    = call2_t2     - call2_t1    ;                                                     \
        (*api_params).total.last    = total_t2     - total_t1    ;                                                     \
        UPDATE_AVG_MIN_MAX((*api_params) , arg_ptrs);                                                                  \
        UPDATE_AVG_MIN_MAX((*api_params) , preload );                                                                  \
        UPDATE_AVG_MIN_MAX((*api_params) , call1   );                                                                  \
        UPDATE_AVG_MIN_MAX((*api_params) , dispatch);                                                                  \
        UPDATE_AVG_MIN_MAX((*api_params) , call2   );                                                                  \
        UPDATE_AVG_MIN_MAX((*api_params) , total   );                                                                  \
        (*api_params).called_count++;                                                                                  \
                                                                                                                       \
        CINT_LOGGER_UNLOCK;                                                                                            \
                                                                                                                       \
    }                                                                                                                  \
                                                                                                                       \
} while(0)

#define CINT_LOGGER_SET_CALLER_CONTEXT(f,l,F) \
        cint_logger_set_caller_context(f,l,F)

#else

#define CINT_LOGGER_CALL_DISPATCH(ret, f_api, args, n_args, args_by_ref) ret = f_api args

#define CINT_LOGGER_SET_CALLER_CONTEXT(f,l,F)

#endif

#endif
