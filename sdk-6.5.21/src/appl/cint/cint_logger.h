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
/* { */

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

#include <shared/bitop.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include "cint_config.h"
#include "cint_types.h"

/*
 * Default values for API logger SOC properties. See api_logging* in
 * propdata.pl
 */
#define CINT_LOGGER_CACHE_SIZE_DEFAULT      (2000000)
#define CINT_LOGGER_MIN_CACHE_SIZE          (100000)
#define CINT_LOGGER_N_FREEZE_DEFAULT        (3800)
#define CINT_LOGGER_MIN_N_FREEZE            (0)
/*
 * Each iteration in cint_variable_print allocates 3 buffers of size
 * CINT_CONFIG_MAX_STACK_PRINT_BUFFER; assume 10 recursions. The user can also
 * set the stack size from SOC property also.
 */
#define CINT_LOGGER_THREAD_STKSZ_DEFAULT    (SAL_THREAD_STKSZ + (30 * CINT_CONFIG_MAX_STACK_PRINT_BUFFER))
#define CINT_LOGGER_THREAD_PRIO_DEFAULT     (50)
#define CINT_LOGGER_BASE_INDENT_DEFAULT     (4)

/*
 * Maximum number of APIs for which logging will be supported simultaneously.
 * Limits the maximum number of per-API context structures allocated by the SDK.
 */
#define CINT_LOGGER_MAX_N_APIS      (500)

/*
 * Flags for file output, set to Cint_logger_cfg.logs_to_file
 *   RAW  - binary output
 *   CINT - CINT-like textual output
 * Default output type is CINT
 */
#define CINT_LOGGER_FILE_OP_NONE    0x0
#define CINT_LOGGER_FILE_OP_RAW     0x1
#define CINT_LOGGER_FILE_OP_CINT    0x2
#define CINT_LOGGER_FILE_OP_DEFAULT CINT_LOGGER_FILE_OP_CINT

/*
 * Flags for cint_printk output, set to (*tdata).logger_is_active
 *   BSL - bsl_printf to BSL_LS_APPL_CINTAPILOGGER
 *   Global File - to Cint_logger_cfg.logfile/_fp
 *   Thread File - to (*tdata).logfile_fp
 */
#define CINT_LOGGER_CINT_OP_NONE        0x0
#define CINT_LOGGER_CINT_OP_BSL         0x1
#define CINT_LOGGER_CINT_OP_GLOBAL_FILE 0x2
#define CINT_LOGGER_CINT_OP_THREAD_FILE 0x4

/*
 * Adds logger structures and variables definitions to the CINT interpreter
 */
extern void cint_logger_cint_init(void);

/*
 * Resets the logger to a clean initial state, ready for reinitialization
 */
extern void cint_logger_reset(void);

/*
 * Set the tr 141 boolean. This is called from the SDK test infrastructure
 * to indicate to the logger whether tr141 is in progress or not.
 */
extern void cint_logger_tr141_set(int value);

/*
 * Check whether tr 141 is executing. Returns the boolean which may have been
 * set previously via the _set API above.
 */
extern int cint_logger_tr141_get(void);

/*
 * Fetches the per-thread data maintained by the logger
 */
extern void* cint_logger_thread_specific(void);

/*
 * Logger top-level function - invoked from BCM dispatch.c
 */
extern void cint_log_function_arguments(const char *fn, void **api_params, unsigned int flags, int call_id, void *arg_ptrs[], int *skip_dispatch);

/*
 * Logger output routine called from cint_printk to generate output when
 * logger_is_active
 */
extern int cint_logger_vprintf(const char* fmt, va_list args);

/*
 * API behind CINT_LOGGER_SET_CALLER_CONTEXT macro  application can use to
 * indicate the FILE, LINE and FUNCTION from which an API is invoked
 */
extern void cint_logger_set_caller_context(const char *f, int l, const char *F);

/*
 * loads the arg_ptrs array with nargs number of pointers; each pointer is a
 * pointer to API arguments. E.g. when called in the context of
 * bcm_port_enable_set the pointers are &unit, &port, and &enable
 */
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
 * enum for all the logger modes
 */
typedef enum {

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

    /*
     * count of logger modes
     */
    cintLoggerModeCount,

} cint_logger_mode_e;

/*
 * structure for per API cached data
 */
typedef struct cint_logger_api_params_s {

    /*
     * API for which this cache was created
     */
    const char *fn;

    /*
     * Point back to the api_params variable of each API. When executing tr 141
     * all per API cached data is cleared. We need this pointer to clear the
     * api_params variable of each API, so that it is freshly allocated at next
     * invocation. See cint_logger_reset() where this is being used.
     */
    struct cint_logger_api_params_s **api_params;

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

    /*
     * per-thread log-file, if required
     */
    void *logfile_fp;

} cint_logger_thread_data_t;

/*
 * structure holding the data relevant for a call to the API
 */
typedef struct cint_logger_call_ctxt_s {

    /*
     * API to which this context belongs
     */
    const char *api;

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
     * FUNCTION from which the API was called
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

    /*
     * System timestamp, if POSIX_TIMER is available
     */
#if CINT_CONFIG_INCLUDE_POSIX_TIMER == 1
    cint_time_t timestamp;
#endif

} cint_logger_call_ctxt_t;

typedef int (*cint_logger_user_filter_cb_t)(cint_logger_thread_data_t*, cint_logger_call_ctxt_t*, cint_logger_api_params_t*, void * []);

/*
 * Keep definition of circular_buffer_t internal to the logger
 */
typedef struct cint_circular_buffer_t circular_buffer_t;


/*
 * structure that holds the global configuration for the logger
 */
typedef struct cint_logger_global_cfg_data_s {

    /*
     * whether the logger is enabled?
     */
    int enabled;

    /*
     * logger mode
     */
    cint_logger_mode_e mode;

    /*
     * total size of the logger in-memory cache
     */
    int cache_size;

    /*
     * number of initial calls which always remain in cache
     */
    int cache_nfreeze;

    /*
     * circular buffer for storing logged entries. This is the primary
     * destination for all logging is allocated dynamically during Init.
     */
    circular_buffer_t *cbuf;

    /*
     * name of the CINT function which will serve as a filter for the logger
     */
    char *cint_filter_fn;

    /*
     * callback function provided by the application
     */
    cint_logger_user_filter_cb_t user_filter_cb;

    /*
     * Thread library key for setting/getting thread-specific object
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
     * Next available api_params structures. This counter also represents the
     * total number of unique APIs called so far.
     */
    int next_free_ctxt;

    /*
     * running counter which serves as block-id in the logs
     */
    int count;

    /*
     * the base indent level in terms of number of spaces prefixed when generating logs
     */
    int base_indent;

    /*
     * stack size for the consumer thread, passed to sal_thread_create
     */
    int consumer_thread_stksz;

    /*
     * priority for the consumer thread. sal_thread_create ignores the priority
     * indication, however retain this for customers who have their own SAL and
     * OS that can use the thread priority
     */
    int consumer_thread_prio;

    /*
     * parameters for the logging to file
     */
    int log_to_file;
    char *logfile;
    void *logfile_fp;
    char *cint_logfile_filter_fn;

    /*
     * parameters for the raw logging to file
     */
    char *raw_logfile;
    void *raw_logfile_fp;

    /*
     * CINT internal variable representing the logger configuration structure
     */
    void *cint_var;

    /*
     * Signal that the logger should reinitialize. This is used for integration
     * with tr 141
     */
    int reinit;

    /*
     * boolean, set when tr 141 is running
     */
    int tr141;

} cint_logger_global_cfg_data_t;

/*
 * structure that holds the global configuration for the logger
 */
extern cint_logger_global_cfg_data_t Cint_logger_cfg;
/*
 * CINT internal routine to fetch system timestamp in usec resolution
 */
extern unsigned long cint_timer_get(void);
extern int cint_gettime(cint_time_t *tv);
extern void cint_timeradd(cint_time_t *a, cint_time_t *b, cint_time_t *res);
extern void cint_timersub(cint_time_t *a, cint_time_t *b, cint_time_t *res);
extern int  cint_timercmp(cint_time_t *a, cint_time_t *b);

/*
 * Logger internal utility routine to log a CINT variable
 */
extern void cint_logger_log_variable(cint_parameter_desc_t *ppdt, const char *pfx, void *src, int indent);

/*
 * Logger internal utility to log a log buffer
 */
extern void cint_logger_log_arguments_buffer(cint_logger_call_ctxt_t *call_ctxt,
                                             cint_logger_api_params_t *ctxt,
                                             void *buffer, int bufsz);

/*
 * Logger internal utility routine to inject variables by same names as the API
 * arguments. This routine is used just before invoking a user defined CINT
 * filter / formatter function which can. Caller should push a new variable
 * scope so that the injected variable names do not collide with variables
 * present in the current scope.
 */
extern void cint_logger_inject_variables(cint_logger_thread_data_t *tdata,
                                         cint_logger_call_ctxt_t *call_ctxt,
                                         cint_logger_call_ctxt_t *orig_call_ctxt,
                                         int *orig_r,
                                         cint_logger_api_params_t *ctxt,
                                         void *arg_ptrs[]);

/*
 * Logger internal utility routine which decodes the provided log buffer,
 * pushes a new variable scope, injects variables by same name as the API
 * arguments, and invokes the provided CINT function.
 */
extern int cint_logger_run_filter_offline(const char *fn, void *buffer, int bufsz,
                                          int ptrs_are_valid,
                                          cint_logger_call_ctxt_t **pcall_ctxt,
                                          cint_logger_api_params_t **pctxt);

/*
 * Logger internal utility routine which fills cnt number of character ch in
 * the buffer pointed by str, subject to a maximum of bufsz characters
 */
extern void cint_rep_chr(char *str, char ch, int bufsz, int cnt);

/*
 * Logger internal utility to initialize a freshly allocated api_params
 * structure. Mainly sets the "min" member of all members holding run-time
 * statistics to the largest possible value for its datatype.
 */
extern void cint_logger_api_params_t_init(cint_logger_api_params_t *ctxt);
/*
 * Logger internal utility routine for initializing the provided api_params
 * structure based on the API name in ctxt->fn.
 */
extern void cint_logger_init_api_params(cint_logger_api_params_t *ctxt);

/*
 * Logger internal utility routine to copy an entry and update few pointers
 */
extern void cint_logger_copy_arguments_buffer(void *new_buf, void *orig_buf, int bufsz);

/*
 * Take/give the logger mutex. One logging entry for one API call must be
 * completed before moving to the next API irrespective of the thread from
 * which the APIs are called. This is ensured using this lock. Most of the
 * logger code executes under this lock.
 */
#define CINT_LOGGER_LOCK   sal_mutex_take(Cint_logger_cfg.mutex, sal_mutex_FOREVER)
#define CINT_LOGGER_UNLOCK sal_mutex_give(Cint_logger_cfg.mutex)


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
    void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1];                                                                         \
    int nargs;                                                                                                         \
    int skip_dispatch;                                                                                                 \
    unsigned long total_t1, total_t2;                                                                                  \
    unsigned long arg_ptrs_t1, arg_ptrs_t2;                                                                            \
    unsigned long preload_t1, preload_t2;                                                                              \
    unsigned long call1_t1, call1_t2;                                                                                  \
    unsigned long dispatch_t1, dispatch_t2;                                                                            \
    unsigned long call2_t1, call2_t2;                                                                                  \
    int call_id;                                                                                                       \
                                                                                                                       \
                                                                                                                       \
    if (!Cint_logger_cfg.enabled) {                                                                                    \
        /*                                                                                                             \
         * Logger is present but not enabled yet. Minimize overhead on the API. Call the dispatch and exit.            \
         */                                                                                                            \
        ret = f_api args;                                                                                              \
                                                                                                                       \
        goto cint_logger_call_dispatch_exit;                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    arg_ptrs[0] = &ret;                                                                                                \
    nargs = n_args;                                                                                                    \
    skip_dispatch = 0;                                                                                                 \
    total_t1    = 0; total_t2    = 0;                                                                                  \
    arg_ptrs_t1 = 0; arg_ptrs_t2 = 0;                                                                                  \
    preload_t1  = 0; preload_t2  = 0;                                                                                  \
    call1_t1    = 0; call1_t2    = 0;                                                                                  \
    dispatch_t1 = 0; dispatch_t2 = 0;                                                                                  \
    call2_t1    = 0; call2_t2    = 0;                                                                                  \
    call_id = 0;                                                                                                       \
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
        cint_log_function_arguments(__func__, (void**)&api_params, CINT_PARAM_IDX,                                     \
                                    call_id, arg_ptrs, &skip_dispatch);                                                \
    }                                                                                                                  \
    preload_t2  = cint_timer_get();                                                                                    \
                                                                                                                       \
    CINT_LOGGER_LOCK;                                                                                                  \
                                                                                                                       \
    Cint_logger_cfg.count++;                                                                                           \
    call_id = Cint_logger_cfg.count;                                                                                   \
                                                                                                                       \
    CINT_LOGGER_UNLOCK;                                                                                                \
                                                                                                                       \
    arg_ptrs_t1 = cint_timer_get();                                                                                    \
    UPDATE_ARG_PTRS args_by_ref;                                                                                       \
    arg_ptrs_t2 = cint_timer_get();                                                                                    \
                                                                                                                       \
    /*                                                                                                                 \
     * Before Dispatch: Log arguments before the dispatch routine is called. This allows the user to run a CINT        \
     * filter function which can elect to bypass the API implementation, or even modify API arguments if required      \
     */                                                                                                                \
    skip_dispatch = 0;                                                                                                 \
    call1_t1 = cint_timer_get();                                                                                       \
    cint_log_function_arguments(__func__, (void**)&api_params, CINT_PARAM_IN,                                          \
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
    cint_log_function_arguments(__func__, (void**)&api_params, CINT_PARAM_OUT,                                         \
                                call_id, arg_ptrs, &skip_dispatch);                                                    \
    call2_t2 = cint_timer_get();                                                                                       \
                                                                                                                       \
    total_t2 = cint_timer_get();                                                                                       \
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
cint_logger_call_dispatch_exit:                                                                                        \
    ;                                                                                                                  \
} while(0)

#define CINT_LOGGER_SET_CALLER_CONTEXT(f,l,F) \
        cint_logger_set_caller_context(f,l,F)

/* } */
#else
/* { */

#define CINT_LOGGER_CALL_DISPATCH(ret, f_api, args, n_args, args_by_ref) ret = f_api args

#define CINT_LOGGER_SET_CALLER_CONTEXT(f,l,F)

/* } */
#endif

/* } */
#endif
