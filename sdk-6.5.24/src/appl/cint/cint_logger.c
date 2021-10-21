/*
 * $Id: cint_logger.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger.c
 * Purpose:     CINT logger. Generates C-like logs from executing BCM APIs in
 *              order to replicate API call sequences in another environment
 */

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

#include <shared/bsl.h>
#include <shared/bslnames.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <appl/diag/bslsink.h>
#include <appl/diag/parse.h>
#include <appl/diag/cmdlist.h>
#include <soc/drv.h>

#include "cint_config.h"
#include "cint_interpreter.h"
#include "cint_porting.h"
#include "cint_variables.h"
#include "cint_error.h"
#include "cint_eval_asts.h"
#include "cint_internal.h"
#include "cint_datatypes.h"
#include "cint_debug.h"
#include "cint_parser.h"
#include "cint_logger.h"
#include "cint_logger_circular_buffer.h"
#include "cint_logger_replay.h"

/*
 * We support logging for at most CINT_LOGGER_MAX_N_APIS APIs. Create array of
 * as many api_params structures. An entry should be requested first time an
 * API is called and a reference should be retained for use during subsequent
 * calls.
 */
static cint_logger_api_params_t cint_logger_api_params[CINT_LOGGER_MAX_N_APIS];

/*
 * Global configuration and state for the logger
 */
cint_logger_global_cfg_data_t Cint_logger_cfg = {
    .enabled        = 1                                , /* disabled by default                                */
    .mode           = cintLoggerModeCache              , /* default mode is to cache                           */

    .cache_size     = CINT_LOGGER_CACHE_SIZE_DEFAULT   , /* default cache size                                 */
    .cache_nfreeze  = CINT_LOGGER_N_FREEZE_DEFAULT     , /* number of initial calls which are stored always    */
    .cbuf           = NULL                             , /* circular buffer for storing logged entries         */

    .cint_filter_fn = NULL                             , /* no default CINT function for filtering             */
    .user_filter_cb = NULL                             , /* no user-defined filtering callback function        */

    .tls_key        = NULL                             , /* initializer for tls_key                            */
    .mutex          = NULL                             , /* initializer for mutex                              */

    .ctxt_head      = cint_logger_api_params           , /* head of list of api_params                         */
    .next_free_ctxt = 0                                , /* next available api_params structure                */

    .count          = 0                                , /* running counter which sets call-id in the logs     */

    .base_indent    = CINT_LOGGER_BASE_INDENT_DEFAULT  , /* base indent level for generated CINT output        */

    .consumer_thread_stksz = CINT_LOGGER_THREAD_STKSZ_DEFAULT, /* stack size for the consumer thread           */
    .consumer_thread_prio  = CINT_LOGGER_THREAD_PRIO_DEFAULT , /* priority of the consumer thread              */

    .log_to_file    = CINT_LOGGER_FILE_OP_NONE         , /* logging to a file - disabled by default            */
    .logfile        = NULL                             , /* logging to a file - log file name                  */
    .logfile_fp     = NULL                             , /* logging to a file - FILE handle                    */
    .cint_logfile_filter_fn = NULL                     , /* logging to a file - filter the entries             */

    .raw_logfile    = NULL                             , /* raw logging to a file - log file name              */
    .raw_logfile_fp = NULL                             , /* raw logging to a file - FILE handle                */

    .cint_replay_filter_fn = NULL                      , /* replay filter function                             */

    .cint_var = NULL                                   , /* holds the CINT variable for global logger cfg      */
    .reinit         = 0                                , /* signal to reinitialize. Used for tr 141 integration*/
    .tr141          = 0                                , /* SDK test infra sets this to indicate tr 141 is on  */
};


static void cint_logger_init(void);

/*
 * At each request one entry is returned until all api_params entries are used
 * up.  Then NULL is returned, which causes ERROR message to be issued. If a
 * user needs to log more APIs then they need to rebuild the SDK with a higher
 * value for CINT_LOGGER_MAX_N_APIS
 *
 * Parameters
 *   None
 *
 * Returns
 *   Returns next available api_params structure, NULL if none available
 */
static inline cint_logger_api_params_t*
cint_logger_get_api_params_buffer (void)
{
    cint_logger_api_params_t *ret_api_params = NULL;

    if (Cint_logger_cfg.next_free_ctxt < CINT_LOGGER_MAX_N_APIS) {

        ret_api_params = &cint_logger_api_params[Cint_logger_cfg.next_free_ctxt];

        Cint_logger_cfg.next_free_ctxt++;
    }

    return (ret_api_params);
}

/*
 * Initialize a run-time structure. Mainly set min to something large.
 *
 * Parameters
 *   [in] rt
 *     Pointer to cint_logger_rtime_t structure
 *
 * Returns
 *   None
 *
 * Remarks
 *   Initializes the cint_logger_rtime_t structure. Mainly, sets all attributes
 *   to zero except "min". Min is set to the largest possible value for its
 *   datatype
 */
static void
cint_logger_rtime_t_init (cint_logger_rtime_t *rt)
{
   (*rt).last =  0;
   (*rt).min  = ~0;
   (*rt).max  =  0;
   (*rt).avg  =  0;
}

/*
 * Initialize the cached data structure for an API. Mainly the run-time data.
 *
 * Parameters
 *   [in] ctxt
 *     Pointer to cint_logger_api_params_t structure to initialize
 *
 * Returns
 *   None
 *
 * Remarks
 *     Sets all fields to NULL/Zero except the "min" attribute of the members
 *     holding run-time statistics. Min is set to the largest possible value
 *     for its datatype
 */
void
cint_logger_api_params_t_init (cint_logger_api_params_t *ctxt)
{
    CINT_MEMSET(ctxt, 0, sizeof(*ctxt));
    cint_logger_rtime_t_init(&ctxt->arg_ptrs);
    cint_logger_rtime_t_init(&ctxt->preload);
    cint_logger_rtime_t_init(&ctxt->call1);
    cint_logger_rtime_t_init(&ctxt->dispatch);
    cint_logger_rtime_t_init(&ctxt->call2);
    cint_logger_rtime_t_init(&ctxt->total);
}

/*
 * Sink for logging to file
 */
static bslsink_sink_t cint_logger_bslsink;

/***********************************************************************************/
/*                                                                                 */
/* Integrate with the CINT interpreter. Expose data structures and config variable */
/* so that user can access these from the interpreter.                             */
/*                                                                                 */
/***********************************************************************************/

static cint_enum_map_t __cint_enum_map__cint_logger_mode_e[] = {
    { "cintLoggerModeLog"     , cintLoggerModeLog     },
    { "cintLoggerModeCache"   , cintLoggerModeCache   },
    { "cintLoggerModeReplay"  , cintLoggerModeReplay  },
    { NULL                                            },
};

static cint_enum_type_t __cint_logger_enums[] = {
    { "cint_logger_mode_e"    , __cint_enum_map__cint_logger_mode_e },
    { NULL                    ,                                     },
};

static cint_parameter_desc_t __cint_struct_members__cint_logger_global_cfg_data_t[] = {
    { "int"                         , "enabled"                , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "cint_logger_mode_e"          , "mode"                   , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "cache_size"             , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "cache_nfreeze"          , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "void"                        , "cbuf"                   , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "char"                        , "cint_filter_fn"         , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "void"                        , "user_filter_cb"         , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "void"                        , "tls_key"                , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "void"                        , "mutex"                  , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "cint_logger_api_params_t"    , "ctxt_head"              , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "next_free_ctxt"         , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "count"                  , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "base_indent"            , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "consumer_thread_stksz"  , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "consumer_thread_prio"   , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "log_to_file"            , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "char"                        , "logfile"                , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "void"                        , "logfile_fp"             , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "char"                        , "cint_logfile_filter_fn" , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "char"                        , "raw_logfile"            , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "void"                        , "raw_logfile_fp"         , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "char"                        , "cint_replay_filter_fn"  , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "void"                        , "cint_var"               , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "reinit"                 , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                         , "tr141"                  , 0 , 0 , CINT_VARIABLE_F_CONST },
    { NULL                          , NULL                     , 0 , 0 },
};

static void*
__cint_maddr__cint_logger_global_cfg_data_t (void* p, int mnum, cint_struct_type_t* parent)
{
    void *rv;
    cint_logger_global_cfg_data_t *s = p;

    switch (mnum) {
    case  0: rv = &(s->enabled);                break;
    case  1: rv = &(s->mode);                   break;
    case  2: rv = &(s->cache_size);             break;
    case  3: rv = &(s->cache_nfreeze);          break;
    case  4: rv = &(s->cbuf);                   break;
    case  5: rv = &(s->cint_filter_fn);         break;
    case  6: rv = &(s->user_filter_cb);         break;
    case  7: rv = &(s->tls_key);                break;
    case  8: rv = &(s->mutex);                  break;
    case  9: rv = &(s->ctxt_head);              break;
    case 10: rv = &(s->next_free_ctxt);         break;
    case 11: rv = &(s->count);                  break;
    case 12: rv = &(s->base_indent);            break;
    case 13: rv = &(s->consumer_thread_stksz);  break;
    case 14: rv = &(s->consumer_thread_prio);   break;
    case 15: rv = &(s->log_to_file);            break;
    case 16: rv = &(s->logfile);                break;
    case 17: rv = &(s->logfile_fp);             break;
    case 18: rv = &(s->cint_logfile_filter_fn); break;
    case 19: rv = &(s->raw_logfile);            break;
    case 20: rv = &(s->raw_logfile_fp);         break;
    case 21: rv = &(s->cint_replay_filter_fn);  break;
    case 22: rv = &(s->cint_var);               break;
    case 23: rv = &(s->reinit);                 break;
    case 24: rv = &(s->tr141);                  break;
    default: rv = NULL;                         break;
    }

    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cint_logger_rtime_t[] = {
    { "unsigned int" , "last" , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "unsigned int" , "min"  , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "unsigned int" , "max"  , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "unsigned int" , "avg"  , 0 , 0 , CINT_VARIABLE_F_CONST },
    { NULL           , NULL   , 0 , 0 },
};

static void*
__cint_maddr__cint_logger_rtime_t (void* p, int mnum, cint_struct_type_t* parent)
{
    void *rv;
    cint_logger_rtime_t *s = p;

    switch (mnum) {
    case  0: rv = &(s->last); break;
    case  1: rv = &(s->min);  break;
    case  2: rv = &(s->max);  break;
    case  3: rv = &(s->avg);  break;
    default: rv = NULL;       break;
    }

    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cint_logger_api_params_t[] = {
    { "char"                     , "fn"              , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "cint_logger_api_params_t" , "api_params"      , 2 , 0 , CINT_VARIABLE_F_CONST },
    { "void"                     , "params"          , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                      , "basetype_size"   , 0 , CINT_CONFIG_MAX_FPARAMS+1 , CINT_VARIABLE_F_CONST },
    { "SHR_BITDCL"               , "is_basetype_arr" , 0 , _SHR_BITDCLSIZE(CINT_CONFIG_MAX_FPARAMS+1) , CINT_VARIABLE_F_CONST },
    { "SHR_BITDCL"               , "is_charptr"      , 0 , _SHR_BITDCLSIZE(CINT_CONFIG_MAX_FPARAMS+1) , CINT_VARIABLE_F_CONST },
    { "int"                      , "nargs"           , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "int"                      , "called_count"    , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "cint_logger_rtime_t"      , "arg_ptrs"        , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "cint_logger_rtime_t"      , "preload"         , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "cint_logger_rtime_t"      , "call1"           , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "cint_logger_rtime_t"      , "dispatch"        , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "cint_logger_rtime_t"      , "call2"           , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "cint_logger_rtime_t"      , "total"           , 0 , 0 , CINT_VARIABLE_F_CONST },
    { NULL                       , NULL              , 0 , 0 },
};

static void*
__cint_maddr__cint_logger_api_params_t (void* p, int mnum, cint_struct_type_t* parent)
{
    void *rv;
    cint_logger_api_params_t *s = p;

    switch (mnum) {
    case  0: rv = &(s->fn);              break;
    case  1: rv = &(s->api_params);      break;
    case  2: rv = &(s->params);          break;
    case  3: rv = &(s->basetype_size);   break;
    case  4: rv = &(s->is_basetype_arr); break;
    case  5: rv = &(s->is_charptr);      break;
    case  6: rv = &(s->nargs);           break;
    case  7: rv = &(s->called_count);    break;
    case  8: rv = &(s->arg_ptrs);        break;
    case  9: rv = &(s->preload);         break;
    case 10: rv = &(s->call1);           break;
    case 11: rv = &(s->dispatch);        break;
    case 12: rv = &(s->call2);           break;
    case 13: rv = &(s->total);           break;
    default: rv = NULL;                  break;
    }

    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cint_logger_call_ctxt_t[] = {
    { "char"         , "api"             , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "unsigned int" , "flags"           , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "void"         , "caller_tid"      , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "char"         , "caller_tname"    , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "char"         , "caller_file"     , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "int"          , "caller_line"     , 0 , 0 , CINT_VARIABLE_F_CONST },
    { "char"         , "caller_function" , 1 , 0 , CINT_VARIABLE_F_CONST },
    { "int"          , "skip_dispatch"   , 1 , 0 }, /* should not be CONST */
    { "int"          , "call_id"         , 0 , 0 , CINT_VARIABLE_F_CONST },
#if CINT_CONFIG_INCLUDE_POSIX_TIMER == 1
    { "cint_time_t"  , "timestamp"       , 0, 0  , CINT_VARIABLE_F_CONST },
#endif
    { NULL           , NULL              , 0 , 0 },
};

static void*
__cint_maddr__cint_logger_call_ctxt_t (void* p, int mnum, cint_struct_type_t* parent)
{
    void *rv;
    cint_logger_call_ctxt_t *s = p;

    switch (mnum) {
    case  0: rv = &(s->api);                 break;
    case  1: rv = &(s->flags);               break;
    case  2: rv = &(s->caller_tid);          break;
    case  3: rv = &(s->caller_tname);        break;
    case  4: rv = &(s->caller_file);         break;
    case  5: rv = &(s->caller_line);         break;
    case  6: rv = &(s->caller_function);     break;
    case  7: rv = &(s->skip_dispatch);       break;
    case  8: rv = &(s->call_id);             break;
#if CINT_CONFIG_INCLUDE_POSIX_TIMER == 1
    case  9: rv = &(s->timestamp);           break;
#endif
    default: rv = NULL;                      break;
    }

    return rv;
}

static cint_struct_type_t __cint_logger_structures[] = {
     CINT_STRUCT_TYPE_DEFINE(cint_logger_rtime_t          ),
     CINT_STRUCT_TYPE_DEFINE(cint_logger_api_params_t     ),
     CINT_STRUCT_TYPE_DEFINE(cint_logger_global_cfg_data_t),
     CINT_STRUCT_TYPE_DEFINE(cint_logger_call_ctxt_t      ),
     { NULL },
};

static cint_data_t cint_logger_cint_data = {
    NULL,
    NULL,
    __cint_logger_structures,
    __cint_logger_enums,
    NULL,
    NULL,
    NULL,
};

/*
 * Injects the logger configuration and state into CINT interpreter as a
 * variable so that it is available to CINT scripts, filter functions. See
 * cint_api_logger_stats.c, cint_api_logger_filter_tests.c scripts in CINT
 * examples folder and the logger test CINT template cint_logger_inline_tests.t
 * for use-cases.
 *
 * Parameters
 *   None
 *
 * Returns
 *   None
 */
static void
cint_logger_cint_integ (void)
{
    cint_datatype_t dt;
    cint_variable_t *rvar = NULL;

    cint_interpreter_add_data(&cint_logger_cint_data, NULL);

    CINT_MEMSET(&dt, 0, sizeof(dt));

    if (cint_datatype_find("cint_logger_global_cfg_data_t", &dt) == CINT_E_NONE) {

        cint_variable_create(&rvar, "cint_logger_cfg", &dt.desc,
                             /*
                              * static data, i.e. the memory for this variable
                              * is not to be allocated by cint_variable_create
                              * It uses the pointer provided below.
                              */
                             CINT_VARIABLE_F_SDATA     |
                             /*
                              * CINT infrastructure will not try to free this
                              * variable. See cint_variable_free
                              */
                             CINT_VARIABLE_F_NODESTROY |
                             /*
                              * Mark this variable CONST in the CINT
                              * interpreter. That way users will receive an
                              * error if they attempt to update the logger
                              * configuration through CINT interpreter. Users
                              * should either use configuration commands or SOC
                              * properties to configure the logger.
                              */
                             CINT_VARIABLE_F_CONST,
                             &Cint_logger_cfg);
    }

    Cint_logger_cfg.cint_var = rvar;
}

/*
 * CINT interpreter frees up all variables when cint_reset() is called. This
 * listener is registered with the CINT interpreter to handle the Reset event
 * and re-inject the logger configuration and state variable.
 *
 * Parameters
 *   Same as mandated by the cint_interpreter_event_register API
 *   [in] cookie
 *     Opaque value supplied when registering for CINT interpreter events. We
 *     have no use for this.
 *   [in] event
 *     The interpreter event for which we were called
 *
 * Returns
 *   CINT_E_NONE - always succeed
 */
static int
cint_logger_cint_interp_listener (void *cookie, cint_interpreter_event_t event)
{
    switch (event) {
    case cintEventReset:
        cint_logger_cint_integ();
        break;
    default:
        break;
    }
    return CINT_E_NONE;
}

/*
 * This function toggles the CONST flag on cint_logger_call_ctxt_t fields. When
 * mode is not Replay, the inline filter function is prevented from being able
 * to write to the fields of call_ctxt structure by marking them CONST. However
 * when the mode is Replay, these fields are marked not CONST so that they may
 * be loaded into the original call_ctxt structure while Replaying logs
 * generated by the API logger.
 *
 * Parameters
 *  [in] set
 *    Boolean value. When set, CONST flag is set on the member fields; when
 *    cleared, the CONST flag is cleared from member fields.
 *
 * Returns
 *   None
 */
void
cint_logger_cint_const_fields (int set)
{
    /*
     * This list of field names should be kept in sync with the fields
     * specified in __cint_struct_members__cint_logger_call_ctxt_t above.
     */
    static const char *const_fields[] = {
       "api"             ,
       "flags"           ,
       "caller_tid"      ,
       "caller_tname"    ,
       "caller_file"     ,
       "caller_line"     ,
       "caller_function" ,
       /*"skip_dispatch" , -- should not be const */
       "call_id"         ,
       "timestamp"       ,
       NULL
    };
    const char **pconst_field = const_fields;
    cint_parameter_desc_t *ppdt = __cint_struct_members__cint_logger_call_ctxt_t;

    if (set) {

        /*
         * When mode is not Replay, set the CONST indication on all fields listed
         * in const_fields.
         */

        for (; ppdt->name; ppdt++) {

            for (pconst_field = const_fields;
                 *pconst_field;
                 pconst_field++) {

                if (!CINT_STRCMP(ppdt->name, *pconst_field)) {

                    ppdt->flags |= CINT_VARIABLE_F_CONST;
                }
            }
        }
    } else {

        /*
         * When mode is Replay, clear CONST indication on all fields. This way
         * the original call_ctxt will be populated from the logs when
         * Replaying previously generated output of the API logger.
         */

        for (; ppdt->name; ppdt++) {

            ppdt->flags &= ~CINT_VARIABLE_F_CONST;
        }
    }

}


/*
 * This is top-level function for integration of the Logger with the CINT
 * interpreter. It is called only once ever because the CINT infrastructure
 * does not respond to SDK deinit/init/tr 141, therefore the logger also
 * injects its information to the CINT infrastructure only once.
 *
 * Parameters
 *   None
 *
 * Returns
 *   None
 *
 * It calls the logger utility API to inject logger definitions and global
 * configuration variable into the CINT interpreter, and registers a listener
 * for cint_reset() so that those can be restored with user executes
 * cint_reset().
 */
void
cint_logger_cint_init (void)
{
    static int cint_logger_cint_inited = 0;
    int it, pbmp_port_max, prod, print_buf_sz;

    if (cint_logger_cint_inited) {
        return;
    }

    /*
     * We assume here that bcm_pbmp_t is the datatype that requires the largest
     * print buffer, when all ports from port #0 to BCM_PBMP_PORT_MAX are added
     * to a variable of this type. Here we compute the print buffer size
     * required to print such a variable without any truncation.
     */
    print_buf_sz = 0;

    prod = 1;

    pbmp_port_max = BCM_PBMP_PORT_MAX;

    /*
     * Following logic is explained with the following example:
     *
     * When BCM_PBMP_PORT_MAX is 1280, before the loop we add up space for
     * opening brace, a space, number 0, a space (so 4 total bytes). At each
     * iteration, we add up space needed by the numbers and a trailing
     * space for each. Number of i-digit numbers is (10^i - 10^(i-1))
     *
     * at first iteration add all single digit numbers 1-9 => (9 * (1+1))
     * at second iteration add all two digit numbers 10-99 => (90 * (2+1))
     * at third iteration add all three digit numbers 100-999 => (900 * (3+1))
     * at fourth iteration the loop breaks since 1280 is not greater than 10^4.
     * After the loop we add up all 4 digit numbers upto BCM_PBMP_PORT_MAX
     * i.e. 1000-1280 => (281 * (4+1))
 */

    /*
     * '{ 0 ' - opening brace, space, port#0, space (4 bytes)
     */
    print_buf_sz += 4;

    /*
     * assume an int can hold 10^6 and max number of ports is less than 10^7
     */
    for (it = 1; it <= 6; it++) {
        /*
         * running product which is equal to 10^i at each iteration
         */
        prod *= 10;

        /*
         * if BCM_PBMP_PORT_MAX > 10^i, then all i-digit numbers can be part of
         * a PBMP. Number of i-digit numbers is (10^i - 10^(i-1))
         */
        if (pbmp_port_max > prod) {
            /*
             * Number of i-digit numbers is (10^i - 10^(i-1))
             * Buffer required for each i-digit number is (i+1), where i is for
             * the number itself and an extra byte for holding the separator
             * whitespace
             */
            print_buf_sz += (prod - (prod/10)) * (it+1);

        } else {
            /*
             * Done... no need to check higher powers of 10
             */
            break;
        }
    }

    /*
     * Calculate the space required by remaining numbers which are smaller
     * than the next higher power of 10. First get the previous power of 10
     * upto which space has been accounted
     */
    prod /= 10;

    print_buf_sz += (pbmp_port_max - prod + 1) * (it+1);

    /*
     * '}\0' - closing brace and one byte for terminating NULL character (2 bytes)
     */
    print_buf_sz += 2;

    if (print_buf_sz > CINT_CONFIG_MAX_STACK_PRINT_BUFFER) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- with BCM_PBMP_PORT_MAX=%d bcm_pbmp_t may need up to %d print buffer vs. configured max size %d\n"),
                   pbmp_port_max, print_buf_sz, CINT_CONFIG_MAX_STACK_PRINT_BUFFER));
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- SDK should be rebuilt with CINT_CONFIG_MAX_STACK_PRINT_BUFFER >= %d\n"),
                   print_buf_sz));
    }

    /*
     * Injects logger configuration data-structure and associated datatypes
     * into the CINT interpreter, so CINT scripts can read those.
     */
    cint_logger_cint_integ();
    cint_interpreter_event_register(cint_logger_cint_interp_listener, NULL);

    cint_logger_cint_inited = 1;
}

/***********************************************************************************/
/*                                                                                 */
/* Integration with the CINT interpreter Done.                                     */
/*                                                                                 */
/***********************************************************************************/

/***********************************************************************************/
/*                                                                                 */
/* BSL sink to log generated output to a separate file. Also the point where post  */
/* processing - e.g. conversion to C - can be chained into the processing flow.    */
/*                                                                                 */
/***********************************************************************************/

/*
 * Logger handler for bslsink_sink_t.vfprintf vector. Invoked from BSL.
 *
 * Parameters
 *   Same as vfprintf
 *
 * Returns
 *   Same as vfprintf
 *
 * Writes to FILEs if opened, flushes the FILE handles and returns string length
 * of written output.
 */
static int
cint_logger_bsl_vfprintf (void *f, const char *format, va_list args)
{
    void *file_fp;
    cint_logger_thread_data_t *tdata;
    int retv = 0;

    if ((tdata = cint_logger_thread_specific()) && (file_fp = (*tdata).logfile_fp)) {

        retv = CINT_VFPRINTF((FILE*) file_fp, format, args);

        CINT_FFLUSH(file_fp);
    }

    if (!(file_fp = Cint_logger_cfg.logfile_fp) ||
        !(Cint_logger_cfg.log_to_file & CINT_LOGGER_FILE_OP_CINT)) {

        return retv;
    }

    retv = CINT_VFPRINTF((FILE*) file_fp, format, args);

    CINT_FFLUSH(file_fp);

    return retv;
}

/*
 * Logger handler for bslsink_sink_t.check vector. Invoked from BSL.
 *
 * Parameters
 *   [in] meta
 *     bsl_meta_t provides meta-data about the log
 *
 * Returns
 *   Boolean - whether log is admitted (TRUE) or rejected (FALSE)
 */
static int
cint_logger_bsl_check (bsl_meta_t *meta)
{
    cint_logger_thread_data_t *tdata;

    return (((meta->layer  == bslLayerAppl) &&
             (meta->source == bslSourceCintapilogger) &&
             (meta->severity == bslSeverityInfo)) ||
            ((tdata = cint_logger_thread_specific()) &&
             (*tdata).logger_is_active));
}


/*
 * Logger handler for bslsink_sink_t.cleanup vector. Invoked from BSL.
 *
 * Parameters
 *   [in] sink
 *     bslsink_sink_t to clean-up.
 *
 * Returns
 *   Zero. We just close FILE if one is open.
 */
static int
cint_logger_bsl_cleanup (bslsink_sink_t *sink)
{
    if (Cint_logger_cfg.logfile_fp) {
        CINT_FCLOSE(Cint_logger_cfg.logfile_fp);
        Cint_logger_cfg.logfile_fp = NULL;
    }
    return 0;
}

/*
 * Creates a BSL sink for writing generated logs to a FILE.
 *
 * Parameters
 *   None
 *
 * Returns
 *   Zero.
 *
 * Registers the newly created sink with BSL.
 */
static int
cint_logger_bsl_init (void)
{
    bslsink_sink_t *sink;
    static int cint_logger_bsl_inited = 0;

    /*
     * BSL does not react to deinit/tr 141. Also there is no way to deregister
     * a sink from BSL. Therefore, this init is called only once ever.
     */
    if (cint_logger_bsl_inited) {
        return 0;
    }

    sink = &cint_logger_bslsink;
    bslsink_sink_t_init(sink);
    CINT_STRNCPY(sink->name, "CintApiLoggerSink", sizeof(sink->name));
    sink->vfprintf = cint_logger_bsl_vfprintf;
    sink->check = cint_logger_bsl_check;
    sink->cleanup = cint_logger_bsl_cleanup;

    sink->enable_range.min = bslSeverityOff+1;
    sink->enable_range.max = bslSeverityCount-1;

    sink->prefix_range.min = bslSeverityOff+1;
    sink->prefix_range.max = bslSeverityWarn;

    bslsink_sink_add(sink);

    cint_logger_bsl_inited = 1;

    return 0;
}

/***********************************************************************************/
/*                                                                                 */
/* BSL sink code for logging generated output to a separate file.                  */
/*                                                                                 */
/***********************************************************************************/

/***********************************************************************************/
/*                                                                                 */
/* Main API logging functions follow                                               */
/*                                                                                 */
/***********************************************************************************/

/*
 * Set initial configuration from SOC properties. Notice here that SOC
 * properties are read only once ever, unless reinit has been requested in
 * response to tr 141 in which case they will be read once again. If SOC
 * properties are not set in either case, preset default values are taken here.
 *
 * Parameters
 *   None
 *
 * Returns
 *   None
 *
 * Remarks
 *   Sets the initial configuration for the logging based on SOC properties.
 */
static void
cint_logger_cfg_init (void)
{
    int enabled, mode;
    int cache_size, cache_nfreeze;
    int base_indent;
    int thread_stksz, thread_prio;
    int log_to_file;
    char *logfile, *raw_logfile;
    void *logfile_fp, *raw_logfile_fp;
    static int cint_logger_cfg_inited = 0;

    /*
     * SOC properties are to be read only once ever, unless reinit has been
     * requested in response to tr 141. Reset the static boolean if reinit in
     * progress
     */

    if (Cint_logger_cfg.reinit) {
        cint_logger_cfg_inited = 0;
    }

    if (cint_logger_cfg_inited) {
        return;
    }

    enabled = CINT_CONFIG_GET(spn_API_LOGGER_ENABLE, 0);
    enabled = (enabled) ? 1 : 0;

    mode = CINT_CONFIG_GET(spn_API_LOGGER_MODE, cintLoggerModeCache);
    if ((mode != cintLoggerModeLog) && (mode != cintLoggerModeCache)) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- mode %d invalid, setting it to %d\n"), mode, cintLoggerModeCache));
        mode = cintLoggerModeCache;
    }

    base_indent = CINT_CONFIG_GET(spn_API_LOGGER_BASE_INDENT, 4);
    if (base_indent < 0) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- base_indent %d invalid, setting it to %d\n"), mode, 4));
        base_indent = 4;
    }

    cache_size = CINT_CONFIG_GET(spn_API_LOGGER_CACHE_SIZE, CINT_LOGGER_CACHE_SIZE_DEFAULT);
    if (cache_size < CINT_LOGGER_MIN_CACHE_SIZE) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- cache_size %d invalid, must be at least %d\n"), cache_size, CINT_LOGGER_MIN_CACHE_SIZE));
        cache_size = CINT_LOGGER_MIN_CACHE_SIZE;
    }

    cache_nfreeze = CINT_CONFIG_GET(spn_API_LOGGER_CACHE_NFREEZE, CINT_LOGGER_N_FREEZE_DEFAULT);
    if (cache_nfreeze < CINT_LOGGER_MIN_N_FREEZE) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- cache_nfreeze %d invalid, must be at least %d\n"), cache_size, CINT_LOGGER_MIN_N_FREEZE));
        cache_nfreeze = CINT_LOGGER_MIN_N_FREEZE;
    }

    thread_stksz = CINT_CONFIG_GET(spn_API_LOGGER_THREAD_STKSZ, CINT_LOGGER_THREAD_STKSZ_DEFAULT);
    if (thread_stksz < 0) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- thread_stksz %d invalid, setting it to %d\n"), thread_stksz, CINT_LOGGER_THREAD_STKSZ_DEFAULT));
        thread_stksz = CINT_LOGGER_THREAD_STKSZ_DEFAULT;
    }

    thread_prio = CINT_CONFIG_GET(spn_API_LOGGER_THREAD_PRIO, CINT_LOGGER_THREAD_PRIO_DEFAULT);
    if (thread_prio < 0) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- thread_prio %d invalid, setting it to %d\n"), thread_prio, CINT_LOGGER_THREAD_PRIO_DEFAULT));
        thread_prio = CINT_LOGGER_THREAD_PRIO_DEFAULT;
    }

    log_to_file = CINT_LOGGER_FILE_OP_NONE;
    logfile = CINT_CONFIG_GET_STR(spn_API_LOGGER_LOGFILE);
    logfile_fp = NULL;
    if (logfile && CINT_STRCMP(logfile, "")) {

        if ((logfile_fp = CINT_FOPEN(logfile, "a")) != NULL) {

            log_to_file = CINT_CONFIG_GET(spn_API_LOGGER_LOGFILE_OPTYPE, CINT_LOGGER_FILE_OP_DEFAULT);

            if ((log_to_file != CINT_LOGGER_FILE_OP_RAW) && (log_to_file != CINT_LOGGER_FILE_OP_CINT)) {

                log_to_file = CINT_LOGGER_FILE_OP_DEFAULT;
            }

            logfile = CINT_STRDUP(logfile);

        } else {

            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("ERROR -- failed to open file \"%s\" for writing\n"), logfile));

            log_to_file = CINT_LOGGER_FILE_OP_NONE;
            logfile = NULL;
            logfile_fp = NULL;
        }
    }

    raw_logfile = CINT_CONFIG_GET_STR(spn_API_LOGGER_RAW_LOGFILE);
    raw_logfile_fp = NULL;
    if (raw_logfile && CINT_STRCMP(raw_logfile, "")) {

        if ((raw_logfile_fp = CINT_FOPEN(raw_logfile, "a")) != NULL) {

            raw_logfile = CINT_STRDUP(raw_logfile);

        } else {

            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("ERROR -- failed to open file \"%s\" for writing\n"), raw_logfile));

            raw_logfile = NULL;
            raw_logfile_fp = NULL;
        }
    }


    Cint_logger_cfg.enabled               = enabled;
    Cint_logger_cfg.mode                  = mode;
    Cint_logger_cfg.cache_size            = cache_size;
    Cint_logger_cfg.cache_nfreeze         = cache_nfreeze;
    Cint_logger_cfg.base_indent           = base_indent;
    Cint_logger_cfg.consumer_thread_stksz = thread_stksz;
    Cint_logger_cfg.consumer_thread_prio  = thread_prio;
    Cint_logger_cfg.log_to_file           = log_to_file;
    Cint_logger_cfg.logfile               = logfile;
    Cint_logger_cfg.logfile_fp            = logfile_fp;
    Cint_logger_cfg.raw_logfile           = raw_logfile;
    Cint_logger_cfg.raw_logfile_fp        = raw_logfile_fp;

    cint_logger_cfg_inited = 1;
}

/*
 * Length of variable length array argument is indicated in another int-like
 * argument of the API. Fetch the value given its index in the params list.
 *
 * Parameters
 *   [in] ctxt
 *     The api_params structure
 *   [in] arg_ptrs
 *     Array of pointers to the arguments
 *   [in] array_cnt_indx
 *     Index of the array count argument
 *   [out] array_cnt
 *     Pointer to int where array count is returned
 *
 * Returns
 *   None
 */
static void
cint_logger_get_array_count (cint_logger_api_params_t *ctxt, void *arg_ptrs[], int array_cnt_indx, int *array_cnt)
{
    cint_parameter_desc_t *params, *ppdt;
    int nargs, array_cnt_size;
    void *array_cnt_ptr;

    params = ctxt->params;
    nargs = ctxt->nargs;

    if (array_cnt_indx >= nargs) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("FATAL ERROR -- %s array_cnt_indx is out of bounds (%d >= %d)\n"), ctxt->fn, array_cnt_indx, nargs));
        return;
    }

    ppdt = &params[array_cnt_indx];
    if (!(ppdt->flags & CINT_PARAM_VL)) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("FATAL ERROR -- %s pointed parameter is not array length (%s %s)\n"), ctxt->fn, ppdt->basetype, ppdt->name));
        return;
    }

    array_cnt_size = ctxt->basetype_size[array_cnt_indx];

    array_cnt_ptr  = arg_ptrs[array_cnt_indx];

    /* expect array_cnt to be an integer-like variable */
    switch (array_cnt_size) {
    case sizeof(uint8)  : *array_cnt = *(uint8*)  array_cnt_ptr; break;
    case sizeof(uint16) : *array_cnt = *(uint16*) array_cnt_ptr; break;
    case sizeof(uint32) : *array_cnt = *(uint32*) array_cnt_ptr; break;
    case sizeof(uint64) : *array_cnt = COMPILER_64_LO(*(uint64*) array_cnt_ptr); break;
    default: break;
    }
}

/*
 * Calculates the buffer space required to store one log entry which includes
 * contextual information and all API arguments, with pointers derefrenced to
 * store the pointed objects, including variable length arrays
 *
 * Parameters
 *   [in] call_ctxt
 *     context specific to this particular call
 *   [in] ctxt
 *     The api_params structure
 *   [in] arg_ptrs
 *     Array of pointers to the arguments
 *   [out] array_size_save
 *     Saves the size in bytes of each argument, including variable length arrays
 *   [out] bufsz
 *     Integer where to save the computed buffer space requirement
 *
 * Returns
 *   None
 */
static void
cint_logger_calculate_buffer_req (cint_logger_call_ctxt_t *call_ctxt,
                                  cint_logger_api_params_t *ctxt,
                                  void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1],
                                  int array_size_save[CINT_CONFIG_MAX_FPARAMS+1],
                                  int *bufsz)
{
    cint_logger_call_ctxt_t *scall_ctxt;
    cint_parameter_desc_t *params;
    cint_parameter_desc_t pds[CINT_CONFIG_MAX_FPARAMS+1];
    void *arg_ptrs_save[CINT_CONFIG_MAX_FPARAMS+1];
    char _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    int ii, nargs, req, array_cnt_indx, array_cnt, done;

    params = ctxt->params;
    nargs = ctxt->nargs;

    for (ii = 0; params[ii].name && (ii < nargs); ii++) {
        pds[ii] = params[ii];
        arg_ptrs_save[ii] = arg_ptrs[ii];
        array_size_save[ii] = 0;
        if (!pds[ii].pcount && SHR_BITGET(ctxt->is_basetype_arr, ii) && arg_ptrs_save[ii]) {
            arg_ptrs_save[ii] = *(void**)arg_ptrs_save[ii];
        }
    }

    /* calculate required storage */
    req = 0;

    /*
     * the call context structure will be appended as a whole. It has string
     * type members - api, caller_file, caller_function. These if not NULL will
     * also be appended right after the call context structure, in order and
     * with NULL termination.
     */
    req += sizeof(ctxt) + sizeof(*scall_ctxt) +
           ((call_ctxt->api) ? CINT_STRLEN(call_ctxt->api) + 1 : 0) +
           ((call_ctxt->caller_file) ? CINT_STRLEN(call_ctxt->caller_file) + 1 : 0) +
           ((call_ctxt->caller_function) ? CINT_STRLEN(call_ctxt->caller_function) + 1 : 0);

    do {

        done = 1;

        for (ii = 0; (ii < nargs) && pds[ii].name; ii++) {

            /*
             * Do not access output pointers in the first call, and input
             * pointers in the second call.
             *
             * call_ctxt->flags provides whether processing call1 (logger
             * called at input of API) or call2 (logger called at output of
             * API). Therefore, call_ctxt->flags and flags for a parameter
             * must be the same in order to process the parameter further.
             *
             * Restricting this check to parameters other than the return
             * value which has flags=0, and also only to pointers.
             *
             * 1. ii=0 is excluded because is is reserved for the return value,
             * which is not filtered
             *
             * 2. For 'call_context->flags', the value CINT_PARAM_IN indicates
             * 'input stage' (call1) and the value CINT_PARAM_OUT indicates
             * 'output stage' (call2).
             *
             * 3. For params[ii].flags, the value CINT_PARAM_IN indicates an
             * input parameter while CINT_PARAM_OUT  indicates an output
             * parameter
             *
             * 4. params[ii].pcount when > 0 indicates that the parameter is a
             * pointer. pcount provides the number of pointer indirections.
             *
             * 5. pds[ii].pcount is equal to params[ii].pcount at first
             * iteration. At this point arg_ptrs[ii] points to the pointer
             * argument which may be stored (no dereference occurs here).
 */
            if ((ii != 0) && (params[ii].pcount > 0) &&
                (params[ii].pcount > pds[ii].pcount) &&
                !(call_ctxt->flags & params[ii].flags & CINT_PARAM_INOUT)) {

                continue;
            }

            if (pds[ii].pcount < 0) {
                continue;
            }

            array_cnt = 1;

            if ((pds[ii].pcount == 0) && (params[ii].pcount > 0)) {

                if ((pds[ii].flags & CINT_PARAM_OUT) &&
                    !(pds[ii].flags & CINT_PARAM_IN) &&
                    (params[ii].pcount > 1) &&
                    (call_ctxt->flags & CINT_PARAM_IN) &&
                    arg_ptrs_save[ii]) {

                    LOG_WARN(BSL_LS_APPL_CINTAPILOGGER,
                             (BSL_META("WARNING -- %s OUT parameter is not NULL (%s %s)\n"), call_ctxt->api, cint_datatype_format_pd(&params[ii], _rstr, 0), params[ii].name));
                }

                if (!arg_ptrs_save[ii]) {

                    array_cnt = 0;

                } else if (SHR_BITGET(ctxt->is_charptr, ii)) {

                    array_cnt = CINT_STRLEN(arg_ptrs_save[ii]) + 1;

                } else if (pds[ii].flags & CINT_PARAM_VP) {

                    array_cnt_indx = pds[ii].flags & CINT_PARAM_IDX;

                    /*
                     * If the pointer is not NULL, AND
                     * the parameter is a variable length array, AND
                     * the parameter holding its length is later in the list, AND
                     * that parameter is a pointer with pcount equal to that of
                     * the variable length array (equal to or greater, but when
                     * can this situation arise??)
                     *
                     * Then during decode we will not be able to find its size
                     * because the array_cnt will be at an unknown offset.
                     *
                     * To help the decode process, actual copy size will be saved
                     * just after the pointer as a uint32.
                     *
                     * Here the first two conditions are already met. Check for
                     * the remaining two, and if met account for an additional
                     * uint32.
                     */
                    if ((array_cnt_indx > ii) &&
                        (params[array_cnt_indx].pcount == params[ii].pcount)) {

                        req += sizeof(uint32);
                    }

                    cint_logger_get_array_count(ctxt, arg_ptrs_save, array_cnt_indx, &array_cnt);
                }

                if (array_cnt == 0) {

                    pds[ii].pcount--;
                    done = 0;
                    continue;
                }
            }

            if (pds[ii].pcount) {

                req += sizeof(void*);

                arg_ptrs_save[ii] = (arg_ptrs_save[ii]) ? *(void**)arg_ptrs_save[ii] : NULL;

                pds[ii].pcount--;
                done = 0;
                continue;
            }

            array_size_save[ii] = array_cnt * ctxt->basetype_size[ii];

            req += array_size_save[ii];

            pds[ii].pcount--;
            done = 0;
            continue;
        }

    } while (!done);

    LOG_VERBOSE(BSL_LS_APPL_CINTAPILOGGER,
                (BSL_META("VERBOSE -- %s calculated buffer req %d bytes\n"), call_ctxt->api, req));

    *bufsz = req;
}


/*
 * Build the log buffer entry. Copies the contextual information and all API
 * arguments, with pointers derefrenced to store the pointed objects including
 * variable length arrays
 *
 * Parameters
 *   [in] tdata
 *     Thread specific data for the calling thread
 *   [in] call_ctxt
 *     context specific to this particular call
 *   [in] ctxt
 *     The api_params structure
 *   [in] arg_ptrs
 *     Array of pointers to the arguments
 *   [in] array_size_save
 *     Size in bytes of each argument, including variable length arrays
 *   [in] buffer
 *     The destination buffer where to copy all the information
 *   [in] bufsz
 *     Size of the destination buffer; this was earlier computed in the calculate_buffer_req
 *
 * Returns
 *    None. Copied size is compared to bufsz; any discrepancy is reported in an
 *    error message. An error would suggest an incorrect/inconsistent algorithm
 *    being used for computing buffer requirement and building the buffer. This
 *    error should never happen.
 */
static void
cint_logger_copy_function_arguments (cint_logger_thread_data_t *tdata,
                                     cint_logger_call_ctxt_t *call_ctxt,
                                     cint_logger_api_params_t *ctxt,
                                     void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1],
                                     int array_size_save[CINT_CONFIG_MAX_FPARAMS+1],
                                     void *buffer,
                                     int bufsz)
{
    cint_logger_call_ctxt_t *scall_ctxt;
    cint_logger_api_params_t **sctxt;
    cint_parameter_desc_t *params;
    cint_parameter_desc_t pds[CINT_CONFIG_MAX_FPARAMS+1];
    void *arg_ptrs_save[CINT_CONFIG_MAX_FPARAMS+1];
    char _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    int ii, nargs, array_cnt_indx, copy_size, done, len;
    char *dst;

    params = ctxt->params;
    nargs = ctxt->nargs;

    for (ii = 0; params[ii].name && (ii < nargs); ii++) {
        pds[ii] = params[ii];
        arg_ptrs_save[ii] = arg_ptrs[ii];
        if (!pds[ii].pcount && SHR_BITGET(ctxt->is_basetype_arr, ii) && arg_ptrs_save[ii]) {
            arg_ptrs_save[ii] = *(void**)arg_ptrs_save[ii];
        }
    }

    dst = buffer;

    sctxt = (void*) dst;
    *sctxt = ctxt;
    dst += sizeof(ctxt);

    scall_ctxt = (void*) dst;
    *scall_ctxt = *call_ctxt;
    dst += sizeof(*call_ctxt);


    if (call_ctxt->api) {
        len = CINT_STRLEN(call_ctxt->api);
        CINT_STRNCPY(dst, call_ctxt->api, len+1);
        scall_ctxt->api = dst;
        dst += (len + 1);
    }

    if (call_ctxt->caller_file) {
        len = CINT_STRLEN(call_ctxt->caller_file);
        CINT_STRNCPY(dst, call_ctxt->caller_file, len+1);
        scall_ctxt->caller_file = dst;
        dst += (len + 1);
    }

    if (call_ctxt->caller_function) {
        len = CINT_STRLEN(call_ctxt->caller_function);
        CINT_STRNCPY(dst, call_ctxt->caller_function, len+1);
        scall_ctxt->caller_function = dst;
        dst += (len + 1);
    }

    do {

        done = 1;

        for (ii = 0; (ii < nargs) && pds[ii].name; ii++) {

            /*
             * Do not access output pointers in the first call, and input
             * pointers in the second call.
             *
             * call_ctxt->flags provides whether processing call1 (logger
             * called at input of API) or call2 (logger called at output of
             * API). Therefore, call_ctxt->flags and flags for a parameter
             * must be the same in order to process the parameter further.
             *
             * Restricting this check to parameters other than the return
             * value which has flags=0, and also only to pointers.
             *
             * 1. ii=0 is excluded because is is reserved for the return value,
             * which is not filtered
             *
             * 2. For 'call_context->flags', the value CINT_PARAM_IN indicates
             * 'input stage' (call1) and the value CINT_PARAM_OUT indicates
             * 'output stage' (call2).
             *
             * 3. For params[ii].flags, the value CINT_PARAM_IN indicates an
             * input parameter while CINT_PARAM_OUT  indicates an output
             * parameter
             *
             * 4. params[ii].pcount when > 0 indicates that the parameter is a
             * pointer. pcount provides the number of pointer indirections.
             *
             * 5. pds[ii].pcount is equal to params[ii].pcount at first
             * iteration. At this point arg_ptrs[ii] points to the pointer
             * argument, which may be stored (no dereference occurs here).
 */
            if ((ii != 0) && (params[ii].pcount > 0) &&
                (params[ii].pcount > pds[ii].pcount) &&
                !(call_ctxt->flags & params[ii].flags & CINT_PARAM_INOUT)) {

                continue;
            }

            if (pds[ii].pcount < 0) {
                continue;
            }

            if (pds[ii].pcount) {

                /*
                 * If arg_ptrs_save[ii] is not NULL, it is pointing to a
                 * pointer. Store that pointer, or NULL to the arguments
                 * buffer. Update arg_ptrs_save[ii] to the stored pointer, to
                 * be processed similarly in the next iteration.  Then, advance
                 * our pointer into arguments buffer by size of data we just
                 * copied, i.e. sizeof a pointer.
                 */
                if (arg_ptrs_save[ii]) {
                    *(void**)dst = *(void**)arg_ptrs_save[ii];
                } else {
                    *(void**)dst = NULL;
                }

                arg_ptrs_save[ii] = *(void**)dst;
                dst += sizeof(void*);

                pds[ii].pcount--;
                done = 0;
                continue;
            }

            if ((pds[ii].flags & CINT_PARAM_OUT) &&
                !(pds[ii].flags & CINT_PARAM_IN) &&
                (pds[ii].pcount == 0) &&
                (params[ii].pcount > 1) &&
                (call_ctxt->flags & CINT_PARAM_IN) &&
                arg_ptrs_save[ii]) {

                LOG_WARN(BSL_LS_APPL_CINTAPILOGGER,
                         (BSL_META("WARNING -- %s OUT parameter is not NULL (%s %s); possible uninitialized pointer dereference follows\n"),
                                   call_ctxt->api, cint_datatype_format_pd(&params[ii], _rstr, 0), params[ii].name));
            }

            /*
             * If the pointer is not NULL, AND
             * the parameter is a variable length array, AND
             * the parameter holding its length is later in the list, AND
             * that parameter is a pointer with pcount equal to that of the
             * variable length array (equal to or greater, but when can this
             * situation arise??)
             *
             * Then during decode we will not be able to find its size
             * because the array_cnt will be indicated at an unknown
             * offset.
             *
             * To help the decode process, save the actual copy size here
             * as a uint32. We implicitly assume that no variable length
             * array argument will have size more than 4GB
             */
            if (arg_ptrs_save[ii] &&
                (pds[ii].flags & CINT_PARAM_VP) &&
                ((array_cnt_indx = pds[ii].flags & CINT_PARAM_IDX) > ii) &&
                (params[array_cnt_indx].pcount == params[ii].pcount)) {

                *(uint32*)dst = array_size_save[ii];
                dst += sizeof(uint32);
            }

            if (arg_ptrs_save[ii] && (copy_size = array_size_save[ii])) {

                CINT_MEMCPY(dst, arg_ptrs_save[ii], copy_size);
                dst += copy_size;
                arg_ptrs_save[ii] = NULL;
            }

            pds[ii].pcount--;
            done = 0;
        }

    } while (!done);

    copy_size = dst - (char*)buffer;
    if (copy_size != bufsz) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- %s buffer may have overflowed, or copied size different from computed buffer size (%d vs %d copied)\n"), call_ctxt->api, bufsz, copy_size));
    }
}

/*
 * Create a variable and do the equivalent of "cint> print variable;" on it.
 *
 * Parameters
 *   [in] ppdt
 *     cint parameter description for the variable to be created. Includes its name
 *   [in] pfx
 *     Any prefix to be applied to the variable name. Typically one or more '_'
 *   [in] src
 *     Pointer to the buffer that holds the variable's value.
 *   [in] indent
 *     Indentation level to be passed to "print variable;"
 *
 * Returns
 *   None
 */
void
cint_logger_log_variable (cint_parameter_desc_t *ppdt, const char *pfx, void *src, int indent)
{
    cint_variable_t *v = NULL;
    char *name;
    int buflen, rc;
    char _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};

    buflen = CINT_STRLEN(pfx) + CINT_STRLEN(ppdt->name) + 2;
    name = CINT_MALLOC(buflen);
    if (!name) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("FATAL ERROR - malloc failed\n")));
        return;
    }
    CINT_SNPRINTF(name, buflen, "%s%s", pfx, ppdt->name);

    /*
     * Add a variable by name so that we can print it. Do not push it to local
     * scope so that non thread-safe list operations in CINT infrastructure which
     * are not required here are avoided.
     */
    rc = cint_variable_create_no_add(&v, NULL, ppdt, CINT_VARIABLE_F_SDATA | CINT_VARIABLE_F_AUTO, src);
    if ((rc != CINT_E_NONE) || !v) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("Failed to allocate %s (%s) -- %d\n"), name, cint_datatype_format_pd(ppdt, _rstr, 0), rc));
        CINT_FREE(name);
        return;
    }

    cint_variable_print(v, indent, name);

    cint_variable_free(v);

    CINT_FREE(name);
}

/*
 * Print count number of spaces
 *
 * Parameters
 *   [in] count
 *     Number of spaces to print
 *
 * Returns
 *   None
 */
static void
cint_logger_indent (int count)
{
    while (count && count--) {
        CINT_PRINTF(" ");
    }
}

/*
 * Logger internal utility routine which fills cnt number of character ch in
 * the string buffer str, subject to a maximum of (bufsz-1) characters. The
 * string is always NUL terminated.
 *
 * Parameters
 *   [out] str
 *     Destination of where to store the repeated pattern of character
 *   [in] ch
 *     Character which to repeat
 *   [in] bufsz
 *     Size of the destination buffer
 *   [in] cnt
 *     Number of times to write the character ch into buffer
 *
 * Returns
 *   None. The string is NUL terminated.
 */
void
cint_rep_chr (char *str, char ch, int bufsz, int cnt)
{
    if (cnt > (bufsz - 1)) {
        cnt = bufsz - 1;
    }
    CINT_MEMSET(str, ch, cnt);
    str[cnt] = '\0';
}

/*
 * Logger internal utility to copy arguments buffers/entries and update a few
 * pointers. Currently only the string members of the call_ctxt structure need
 * to be fixed to point inside the new buffer...other than that all logger
 * entries are self-contained
 *
 * Parameters
 *   [inout] new_buf
 *     Buffer which needs to be fixed up. Caller must allocate it
 *   [in] orig_buf
 *     Original logger entry buffer which can be used as reference
 *   [in] bufsz
 *     Size of the buffer to be fixed up.
 *
 * Fixes the string pointers in call_ctxt structure in buf
 */
void
cint_logger_copy_arguments_buffer (void *new_buf, void *orig_buf, int bufsz)
{
    char *src = orig_buf;
    char *dst = new_buf;
    cint_logger_call_ctxt_t *scall_ctxt1, *scall_ctxt2;

    CINT_MEMCPY(dst, src, bufsz);

    /* skip over the ctxt, which is a pointer */
    src += sizeof(void*);
    dst += sizeof(void*);

    /* call_ctxt structure lies here... */
    scall_ctxt1 = (cint_logger_call_ctxt_t*) src;
    scall_ctxt2 = (cint_logger_call_ctxt_t*) dst;

    scall_ctxt2->api = scall_ctxt2->caller_file = scall_ctxt2->caller_function = NULL;

    if (scall_ctxt1->api) {

        scall_ctxt2->api = ((char*)new_buf + (scall_ctxt1->api - (char*)orig_buf));

    }

    if (scall_ctxt1->caller_file) {

        scall_ctxt2->caller_file = ((char*)new_buf + (scall_ctxt1->caller_file - (char*)orig_buf));

    }

    if (scall_ctxt1->caller_function) {

        scall_ctxt2->caller_function = ((char*)new_buf + (scall_ctxt1->caller_function - (char*)orig_buf));

    }

    /* All three pointers were fixed...Done */
}

/*
 * Logger internal utility routine to decode a previously recorded buffer. This
 * is used to get the pointers to original arguments, with valid pointers in
 * pointer arguments using dummy_ptrs array below. Then later CINT variables
 * are injected using the arg_ptrs so that a user-defined CINT filter/formatter
 * function could take actions based on the values. I.e. a typical sequence
 * will be the following, after which the buffer may be logged to a file, say.
 *
 * decode_arguments_buffer
 *    .
 *    .
 * inject_variables
 *    .
 *    .
 * run_filter_function
 *
 * Parameters
 *   [in] buffer
 *     Pointer to the source buffer to decode
 *   [in] bufsz
 *     Size of the source buffer to be decoded.
 *   [out] pcall_ctxt
 *     Pointer to the context information specific to the call which resulted
 *     in the buffer being decoded. Pointer into the buffer is returned.
 *   [out] pctxt
 *     Pointer to api_params structure of the API which generated the buffer
 *     being decoded.
 *   [out] arg_ptrs
 *     Array of pointers to API arguments found in the buffer. These are also
 *     pointers into the buffer. However, pointers found in the buffer will no
 *     longer be valid hence pointer arguments point into the dummy_ptrs array.
 *   [out] dummy_ptrs
 *     Array of dummy_ptrs to hold pointer arguments decoded from the buffer.
 *     These pointers eventually point back into the buffer where the base
 *     object was copied during buffer generation.
 *   [in] ptrs_are_valid
 *     Boolean indicating whether ctxt and call_ctxt ptrs decoded from the
 *     buffer should be considered valid. False when a buffer which was
 *     previously generated and written to file.
 *
 * Returns
 *   None
 *
 * Total bytes decoded is compared against the input bufsz; any discrepancy is
 * reported in an error message. An error would suggest an incorrect/
 * inconsistent algorithm being used for building the buffer and decoding the
 * buffer. This error should never happen.
 */
static void
cint_logger_decode_arguments_buffer (cint_logger_call_ctxt_t **pcall_ctxt,
                                     cint_logger_api_params_t **pctxt,
                                     void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1],
                                     void *dummy_ptrs[CINT_CONFIG_MAX_FPARAMS+1][5],
                                     void *buffer,
                                     int bufsz,
                                     int ptrs_are_valid)
{
    cint_logger_call_ctxt_t tcall_ctxt, *scall_ctxt, *call_ctxt;
    cint_logger_api_params_t **sctxt, *ctxt;
    char *src, *src_save;
    void *orig_ptr;
    cint_parameter_desc_t *params;
    cint_parameter_desc_t pds[CINT_CONFIG_MAX_FPARAMS+1];
    void *arg_ptrs_save[CINT_CONFIG_MAX_FPARAMS+1];
    int cur_dummy_ptr[CINT_CONFIG_MAX_FPARAMS+1];
    int ii, nargs, done, skip_size, array_cnt, array_cnt_indx, src_offset;

    src = buffer;

    sctxt = (void*)src;
    src += sizeof(*sctxt);

    scall_ctxt = (void*) src;
    tcall_ctxt = *scall_ctxt;
    src += sizeof(*scall_ctxt);

    if (scall_ctxt->api) {
        tcall_ctxt.api = src;
        src += CINT_STRLEN(src) + 1;
    }

    if (scall_ctxt->caller_file) {
        tcall_ctxt.caller_file = src;
        src += CINT_STRLEN(src) + 1;
    }

    if (scall_ctxt->caller_function) {
        tcall_ctxt.caller_function = src;
        src += CINT_STRLEN(src) + 1;
    }

    /*
     * If pointers in the arguments buffer are valid, we are going to return
     * those pointers in *pctxt and *pcall_ctxt. Else we need to rebuild both
     * the api_params and the call_ctxt structures...in this case *pctxt and
     * *pcall_ctxt must both provide storage for those results.
     */
    if (ptrs_are_valid) {
        ctxt = *sctxt;
        call_ctxt = scall_ctxt;
    } else {
        **pcall_ctxt = tcall_ctxt;
        call_ctxt = *pcall_ctxt;
        ctxt = *pctxt;
        cint_logger_api_params_t_init(ctxt);
        ctxt->fn = call_ctxt->api;
        cint_logger_init_api_params(ctxt);
        if (!ctxt->params) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("ERROR: failed to find API %s...cannot proceed\n"), ctxt->fn));
            return;
        }
    }

    *pctxt = ctxt;
    *pcall_ctxt = call_ctxt;

    params = ctxt->params;
    nargs  = ctxt->nargs;

    CINT_MEMSET(cur_dummy_ptr, 0, sizeof(cur_dummy_ptr));

    src_save = src;

    for (ii = 0; params[ii].name && (ii < nargs); ii++) {

        pds[ii] = params[ii];

        cur_dummy_ptr[ii] = 0;

        if (pds[ii].pcount) {

            arg_ptrs[ii] = arg_ptrs_save[ii] = &dummy_ptrs[ii][cur_dummy_ptr[ii]];

            src += sizeof(void*);

        } else {

            arg_ptrs[ii] = arg_ptrs_save[ii] = src;

            src += ctxt->basetype_size[ii];

        }
    }

    src = src_save;

    do {

        done = 1;

        for (ii = 0; (ii < nargs) && pds[ii].name; ii++) {

            /*
             * Do not access output pointers in the first call, and input
             * pointers in the second call.
             *
             * call_ctxt->flags provides whether processing call1 (logger
             * called at input of API) or call2 (logger called at output of
             * API). Therefore, call_ctxt->flags and flags for a parameter
             * must be the same in order to process the parameter further.
             *
             * Restricting this check to parameters other than the return
             * value which has flags=0, and also only to pointers.
             *
             * 1. ii=0 is excluded because is is reserved for the return value,
             * which is not filtered
             *
             * 2. For 'call_context->flags', the value CINT_PARAM_IN indicates
             * 'input stage' (call1) and the value CINT_PARAM_OUT indicates
             * 'output stage' (call2).
             *
             * 3. For params[ii].flags, the value CINT_PARAM_IN indicates an
             * input parameter while CINT_PARAM_OUT  indicates an output
             * parameter
             *
             * 4. params[ii].pcount when > 0 indicates that the parameter is a
             * pointer. pcount provides the number of pointer indirections.
             *
             * 5. pds[ii].pcount is equal to params[ii].pcount at first
             * iteration. At this point arg_ptrs[ii] points to the pointer
             * argument, which is stored (no dereference occured).
 */
            if ((ii != 0) && (params[ii].pcount > 0) &&
                (params[ii].pcount > pds[ii].pcount) &&
                !(call_ctxt->flags & params[ii].flags & CINT_PARAM_INOUT)) {

                continue;
            }

            if (pds[ii].pcount < 0) {
                continue;
            }

            array_cnt = 1;

            if (pds[ii].pcount) {

                orig_ptr = *(void**)src;

                src += sizeof(void*);

                if (orig_ptr) {

                    dummy_ptrs[ii][cur_dummy_ptr[ii]] = &dummy_ptrs[ii][cur_dummy_ptr[ii]+1];

                } else {

                    dummy_ptrs[ii][cur_dummy_ptr[ii]] = NULL;
                }

                cur_dummy_ptr[ii]++;
                pds[ii].pcount--;
                done = 0;
                continue;
            }

            if ((pds[ii].pcount == 0) && (params[ii].pcount > 0)) {

                if (!dummy_ptrs[ii][cur_dummy_ptr[ii]-1]) {

                    array_cnt = 0;

                } else if (pds[ii].flags & CINT_PARAM_VP) {

                    array_cnt_indx = pds[ii].flags & CINT_PARAM_IDX;


                    /*
                     * If the pointer is not NULL, AND
                     * the parameter is a variable length array, AND
                     * the parameter holding its length is later in the list, AND
                     * that parameter is a pointer with pcount equal to that of
                     * the variable length array (equal to or greater, but when
                     * can this situation arise??)
                     *
                     * Then during decode we will not be able to find its size
                     * because the array_cnt is at this time at an unknown
                     * offset,
                     *
                     * To help the decode process, actual copy size was saved
                     * here as a uint32.
                     *
                     * Here the first two conditions are already met. Check for
                     * the remaining two, and if met use the uint32 to compute
                     * array_cnt, and skip past it.
                     */
                    if ((array_cnt_indx > ii) &&
                        (params[array_cnt_indx].pcount == params[ii].pcount)) {

                        array_cnt = *(uint32*)src / ctxt->basetype_size[ii];
                        src += sizeof(uint32);

                    } else {

                        cint_logger_get_array_count(ctxt, arg_ptrs_save, array_cnt_indx, &array_cnt);
                    }

                } else if (SHR_BITGET(ctxt->is_charptr, ii)) {

                    array_cnt = CINT_STRLEN(src) + 1;
                }

                if (array_cnt == 0) {

                    pds[ii].pcount--;
                    done = 0;
                    continue;
                }

                arg_ptrs_save[ii] = dummy_ptrs[ii][cur_dummy_ptr[ii]-1]  = src;
            }

            skip_size = array_cnt * ctxt->basetype_size[ii];

            src += skip_size;

            pds[ii].pcount--;
            done = 0;
            continue;
        }

    } while (!done);

    src_offset = (int)(src - (char*)buffer);
    if (src_offset != bufsz) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("Logger Buffer Decoder did not align to end of buffer (%s) -- offset %d vs expected %d\n"),
                   call_ctxt->api, src_offset, bufsz));
    }

}

/*
 * Log the provided buffer in a CINT-like textual output. The output is
 * structured in a way such that it may be programmatically converted into a
 * valid CINT - i.e. something that the CINT interpreter will interpret without
 * errors.
 *
 * Parameters
 *   [in] call_ctxt
 *     context specific to this particular call. It is valid only when doing
 *     inline logging, i.e. when it is called during the execution of the API.
 *     It is set to NULL when processing a previously recorded arguments
 *     buffer.
 *   [in] ctxt
 *     The api_params structure. This is also valid only when doing inline
 *     logging.
 *   [out] buffer
 *     Pointer to the source buffer to log
 *   [out] bufsz
 *     Size of the source buffer to be logged
 *
 * Returns
 *   None
 *
 * This routine does the equivalent of decode routine above, but since the
 * intention is only to convert to human readable form dummy_ptrs are not
 * computed, and values printed for pointer arguments are the original pointer
 * values - i.e. from the time the arguments buffer was built.
 *
 * Size of the decoded data is compared against the input bufsz. Any
 * discrepancy is reported as an error message. An error would suggest an
 * incorrect/ inconsistent algorithm being used for building the buffer and
 * logging/serializing the buffer. This error should never happen.
 */
void
cint_logger_log_arguments_buffer (cint_logger_call_ctxt_t *call_ctxt,
                                  cint_logger_api_params_t *ctxt,
                                  void *buffer, int bufsz)
{
    cint_logger_call_ctxt_t tcall_ctxt, *scall_ctxt;
    cint_logger_api_params_t tctxt, **sctxt;
    cint_parameter_desc_t *params, *ppdt;
    cint_parameter_desc_t pds[CINT_CONFIG_MAX_FPARAMS+1];
    cint_parameter_desc_t voidptr_pds = { "void", NULL, 1, 0 };
    void *ptr_args[CINT_CONFIG_MAX_FPARAMS+1][5], *null_ptr = NULL;
    char v1str[256], v2str[256];
    void *arg_ptrs_save[CINT_CONFIG_MAX_FPARAMS+1];
    int ii, nargs, array_cnt_indx, array_cnt, dim, first, done, src_offset, pcount;
    unsigned int flags;
    int indent;
    /*
     * variables to hold '_' prefixes for auto variables created due to pointer
     * dereference. we don't expect this to be more than 3. Use 20.
     */
    char pfx[20], pfx2[20], sfx[20];
    cint_parameter_desc_t call_ctxt_pds = { "cint_logger_call_ctxt_t", REPLAY_CURR_CTXT, 0, 0 };
    char *src, *src_save, *str;

    src = buffer;

    sctxt = (void*)src;
    src += sizeof(*sctxt);

    scall_ctxt = (void*) src;
    tcall_ctxt = *scall_ctxt;
    src += sizeof(*scall_ctxt);

    if (scall_ctxt->api) {
        tcall_ctxt.api = src;
        src += CINT_STRLEN(src) + 1;
    }

    if (scall_ctxt->caller_file) {
        tcall_ctxt.caller_file = src;
        src += CINT_STRLEN(src) + 1;
    }

    if (scall_ctxt->caller_function) {
        tcall_ctxt.caller_function = src;
        src += CINT_STRLEN(src) + 1;
    }

    if (!call_ctxt) {
        call_ctxt = &tcall_ctxt;
    }

    if (!ctxt) {
        ctxt = &tctxt;
        cint_logger_api_params_t_init(ctxt);
        ctxt->fn = call_ctxt->api;
        cint_logger_init_api_params(ctxt);
        if (!ctxt->params) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("ERROR: failed to find API %s...cannot proceed\n"), ctxt->fn));
            return;
        }
    }

    indent = Cint_logger_cfg.base_indent;
    params = ctxt->params;
    nargs = ctxt->nargs;
    flags = call_ctxt->flags;

    CINT_MEMSET(ptr_args, 0, sizeof(ptr_args));

    src_save = src;

    for (ii = 0; params[ii].name && (ii < nargs); ii++) {
        pds[ii] = params[ii];
        arg_ptrs_save[ii] = src;
        if (params[ii].pcount) {
            src += sizeof(void*);
        } else {
            src += ctxt->basetype_size[ii];
        }
    }

    src = src_save;

    if (flags & CINT_PARAM_IN) {
        cint_logger_indent(indent);
        CINT_PRINTF("{\n");
    } else if (flags & CINT_PARAM_OUT) {
        cint_logger_indent(indent);
        CINT_PRINTF("if (0) {\n");
    }

    indent += Cint_logger_cfg.base_indent;

    first = 1;

    do {

        done = 1;

        for (ii = 0; (ii < nargs) && pds[ii].name; ii++) {

            /*
             * Do not access output pointers in the first call, and input
             * pointers in the second call.
             *
             * call_ctxt->flags provides whether processing call1 (logger
             * called at input of API) or call2 (logger called at output of
             * API). Therefore, call_ctxt->flags and flags for a parameter
             * must be the same in order to process the parameter further.
             *
             * Restricting this check to parameters other than the return
             * value which has flags=0, and also only to pointers.
             *
             * 1. ii=0 is excluded because is is reserved for the return value,
             * which is not filtered
             *
             * 2. For 'call_context->flags', the value CINT_PARAM_IN indicates
             * 'input stage' (call1) and the value CINT_PARAM_OUT indicates
             * 'output stage' (call2).
             *
             * 3. For params[ii].flags, the value CINT_PARAM_IN indicates an
             * input parameter while CINT_PARAM_OUT  indicates an output
             * parameter
             *
             * 4. params[ii].pcount when > 0 indicates that the parameter is a
             * pointer. pcount provides the number of pointer indirections.
 */
            if ((ii != 0) && (params[ii].pcount > 0) &&
                (params[ii].pcount > pds[ii].pcount) &&
                !(flags & params[ii].flags & CINT_PARAM_INOUT)) {

                continue;
            }

            if (pds[ii].pcount < 0) {
                continue;
            }

            if ((ii == 0) & !(flags & CINT_PARAM_OUT)) {
                src += ctxt->basetype_size[ii];
                pds[ii].pcount--;
                continue;
            }

            if ((pds[ii].pcount == 0) && !CINT_STRCMP(pds[ii].basetype, "void")) {

                pds[ii].pcount--;
                done = 0;
                continue;
            }

            array_cnt = 1;

            arg_ptrs_save[ii] = src;

            if (pds[ii].pcount) {

                ptr_args[ii][params[ii].pcount - pds[ii].pcount] = *(void**)arg_ptrs_save[ii];
            }

            cint_rep_chr(pfx, '_', sizeof(pfx), params[ii].pcount - pds[ii].pcount);

            if (SHR_BITGET(ctxt->is_charptr, ii) && (pds[ii].pcount == 1)) {

                voidptr_pds.name = pds[ii].name;

                CINT_SNPRINTF(pfx2, sizeof(pfx2), "orig_%s", pfx);

                cint_logger_log_variable(&voidptr_pds, pfx2, src, indent);

                voidptr_pds.name = NULL;

                src += sizeof(void*);

                cint_logger_log_variable(&pds[ii], pfx, &null_ptr, indent);

                pds[ii].pcount--;
                done = 0;
                continue;
            }

            if ((pds[ii].pcount == 0) && (params[ii].pcount > 0)) {

                if (!ptr_args[ii][params[ii].pcount -1]) {

                    array_cnt = 0;

                } else if (pds[ii].flags & CINT_PARAM_VP) {

                    array_cnt_indx = pds[ii].flags & CINT_PARAM_IDX;

                    /*
                     * If the pointer is not NULL, AND
                     * the parameter is a variable length array, AND
                     * the parameter holding its length is later in the list, AND
                     * that parameter is a pointer with pcount equal to that of
                     * the variable length aray (equal to or greater, but when
                     * can this situation arise??)
                     *
                     * Then during decode we will not be able to find its size
                     * because the array_cnt is at this time at an unknown
                     * offset.
                     *
                     * To help the decode process, actual copy size was saved
                     * here as a uint32.
                     *
                     * Here the first two conditions are already met. Check for
                     * the remaining two, and if met use the uint32 to compute
                     * array_cnt, and skip past it.
                     */
                    if ((array_cnt_indx > ii) &&
                        (params[array_cnt_indx].pcount == params[ii].pcount)) {

                        array_cnt = *(uint32*)src / ctxt->basetype_size[ii];
                        src += sizeof(uint32);

                    } else {

                        cint_logger_get_array_count(ctxt, arg_ptrs_save, array_cnt_indx, &array_cnt);
                    }

                } else if (SHR_BITGET(ctxt->is_charptr, ii)) {

                    str = src;

                    pds[ii].pcount = 1;
                    cint_logger_log_variable(&pds[ii], pfx, &str, indent);
                    pds[ii].pcount = 0;

                    src += CINT_STRLEN(str) + 1;

                    array_cnt = 0;
                }

                if (array_cnt == 0) {

                    pds[ii].pcount--;
                    done = 0;
                    continue;
                }

                for (dim = CINT_CONFIG_ARRAY_DIMENSION_LIMIT - 1; dim > 0; dim--) {
                    pds[ii].dimensions[dim] = pds[ii].dimensions[dim-1];
                }
                pds[ii].dimensions[0] = array_cnt;
                pds[ii].num_dimensions++;

            }

            cint_logger_log_variable(&pds[ii], pfx, src, indent);

            if (pds[ii].pcount) {
                src += sizeof(void*);
            } else {
                src += array_cnt * ctxt->basetype_size[ii];
            }

            pds[ii].pcount--;
            done = 0;
            continue;
        }

        first = 0;

    } while (!done);

    cint_logger_log_variable(&call_ctxt_pds, "", call_ctxt, indent);

    src_offset = (int)(src - (char*)buffer);
    if (src_offset != bufsz) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("Logger did not align to end of buffer (%s) -- offset %d vs expected %d\n"),
                   call_ctxt->api, src_offset, bufsz));
    }

    CINT_PRINTF("\n");

    for (ii = 0; (ii < nargs) && params[ii].name; ii++) {

        if ((ii != 0) && (params[ii].pcount  > 0) &&
            !(flags & params[ii].flags & CINT_PARAM_INOUT)) {

            continue;
        }

        if (!params[ii].pcount) {
            continue;
        }

        if (!ptr_args[ii][0]) {
            continue;
        }

        for (pcount = 0; pcount < params[ii].pcount; pcount++) {

            if (!ptr_args[ii][pcount]) {
                break;
            }

            array_cnt = 1;

            sfx[0] = '\0';

            if (pcount == (params[ii].pcount -1)) {

                if (!CINT_STRCMP(params[ii].basetype, "void")) {
                    break;
                }

                if (params[ii].flags & CINT_PARAM_VP) {

                    array_cnt_indx = pds[ii].flags & CINT_PARAM_IDX;

                    cint_logger_get_array_count(ctxt, arg_ptrs_save, array_cnt_indx, &array_cnt);
                }

                if (array_cnt == 0) {
                    break;
                }

                CINT_SNPRINTF(sfx, sizeof(sfx), "[0]");

            }

            cint_rep_chr(pfx, '_', sizeof(pfx),  pcount);
            cint_rep_chr(pfx2, '_', sizeof(pfx2), pcount+1);

            CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s",   pfx, params[ii].name);
            CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s%s", pfx2, params[ii].name, sfx);

            cint_logger_indent(indent);
            CINT_PRINTF("%s = &%s;\n", v1str, v2str);
        }

        CINT_PRINTF("\n");

    }

    cint_logger_indent(indent);
    CINT_PRINTF("%s(", ctxt->fn);

    ppdt = params;
    for (ppdt++; ppdt->name; ppdt++) {
        CINT_PRINTF("%s%s", (first++ > 0) ? ", " : "", ppdt->name);
    }

    CINT_PRINTF(");\n");

    indent -= Cint_logger_cfg.base_indent;
    cint_logger_indent(indent);
    CINT_PRINTF("}\n");
}

/*
 * In the current CINT variable scope, create CINT variables with same names as
 * the API arguments. The caller would typically push a new variable scope so
 * that the newly created variable names do not collide with existing variables
 * declared in the CINT interpreter.
 *
 * These variables are instantiated over the same memory as the original
 * arguments using the arg_ptrs, so they have the exact same values when read.
 * Any writes to these variables therefore also alter the underlying API
 * arguments.
 *
 * Variables for all API arguments are created, with name prefixed by the
 * supplied prefix string param_name_pfx. This prefix is set to the empty
 * string when injecting CINT variables for actual API arguments; during replay
 * this prefix is used to indicate whether it represents the arguments from
 * call1 logs or from call2 logs.
 *
 * Additionally a variable by name supplied in call_ctxt_name is created
 * mapping to the API call_ctxt.
 *
 * After this call it is expected to invoke a user supplied filter / formatter
 * CINT function which can refer to these variables directly without a need to
 * declare them. This also allows that CINT function to be written in an API
 * independent manner.
 *
 * Parameters
 *   [in] tdata
 *     Logger thread-specific data for the calling thread
 *   [in] call_ctxt
 *     Call ctxt of the API being logged
 *   [in] ctxt
 *     API params structure for the API being logged
 *   [in] arg_ptrs
 *     pointers to all arguments of the API being logged
 *   [in] call_ctxt_name
 *     Name of the CINT variable created over the call_ctxt structure
 *   [in] param_name_pfx
 *     Prefix to add to the name of CINT variable of each of the API parameters
 *
 * All the injected variables are automatically freed up by the CINT
 * infrastructure once the caller destroys the scope pushed before calling this
 * function.
 */
static void
cint_logger_inject_variables (cint_logger_thread_data_t *tdata,
                              cint_logger_call_ctxt_t *call_ctxt,
                              cint_logger_api_params_t *ctxt,
                              void *arg_ptrs[],
                              const char *call_ctxt_name,
                              const char *param_name_pfx)
{
    int i, rc;
    cint_variable_t *vcall_ctxt = NULL;
    cint_variable_t *v[CINT_CONFIG_MAX_FPARAMS+1];
    cint_parameter_desc_t call_ctxt_pds = { "cint_logger_call_ctxt_t", NULL, 0, 0 };
    cint_parameter_desc_t *params, *ppdt;
    char _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    char var_name[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    void *src;

    params = ctxt->params;

    /*
     * Create variables for all API arguments by name prefixed with
     * param_name_pfx
     */
    CINT_MEMSET(v, 0, sizeof(v));

    for (i=0, src = arg_ptrs[i], ppdt = &params[i];
         ppdt->name;
         i++, src = arg_ptrs[i], ppdt = &params[i]) {

        if (!src) {
            continue;
        }

        CINT_SNPRINTF(var_name, sizeof(var_name), "%s%s", param_name_pfx, ppdt->name);

        rc = cint_variable_create(&v[i], var_name, ppdt, CINT_VARIABLE_F_SDATA, src);
        if ((rc != CINT_E_NONE) || !v[i]) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("Failed to allocate %s (%s) -- %d\n"), ppdt->name, cint_datatype_format_pd(ppdt, _rstr, 0), rc));
            return;
        }
    }

    /*
     * create new variable for the call_ctxt
     */
    rc = cint_variable_create(&vcall_ctxt,
                              call_ctxt_name,
                              &call_ctxt_pds,
                              CINT_VARIABLE_F_SDATA,
                              call_ctxt);
    if ((rc != CINT_E_NONE) || !vcall_ctxt) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("Failed to allocate %s (%s) -- %d\n"), call_ctxt_pds.name, cint_datatype_format_pd(&call_ctxt_pds, _rstr, 0), rc));
    }
}

/*
 * Search the provided filter function in the CINT interpreter. If found,
 * execute it and return its return value. It is possible to call BCM APIs from
 * the CINT function which may or may not be logged. Currently we do not log
 * those by disabling logging for the caller thread before invoking the
 * function, and restoring after.
 *
 * Parameters
 *   [in] tdata
 *     Thread specific data for the calling thread. This holds the boolean
 *     per-thread disable for the logger.
 *   [in] call_ctxt
 *     context specific to the API call in the context of which the filter
 *     is to be called
 *   [in] ctxt
 *     The api_params structure
 *   [in] arg_ptrs
 *     Array of pointers to the API arguments
 *   [in] cint_filter_fn
 *     Name of the filter/formatter CINT function to invoke
 *
 * Returns
 *   Returns value of the CINT function is returned
 */
static int
cint_logger_run_filter_function (cint_logger_thread_data_t *tdata,
                                 cint_logger_call_ctxt_t *call_ctxt,
                                 cint_logger_api_params_t *ctxt,
                                 void *arg_ptrs[],
                                 const char *cint_filter_fn)
{
    cint_ast_t *i_ast, *f_ast;
    cint_variable_t *rv;
    cint_atomic_type_t *ap;
    int save_disabled;
    int ret = 0;

    i_ast = cint_ast_identifier(cint_filter_fn);
    f_ast = cint_ast_function(i_ast, NULL);

    /*
     * cint_filter_fn may itself have API calls. We do not want to log those.
     * So disable logging for this thread. Take care to not enable if logging
     * was already disabled for this thread.
     *
     * Should we log those calls? -- TBD
     */

    save_disabled = (*tdata).disabled;
    (*tdata).disabled = 1;

    rv = cint_eval_ast(f_ast);

    (*tdata).disabled = save_disabled;

    cint_ast_free_single(f_ast);
    cint_ast_free_single(i_ast);

    if (rv && (rv->dt.flags & CINT_DATATYPE_F_ATOMIC) &&
        (ap = rv->dt.basetype.ap) &&
        (ap->flags & CINT_ATOMIC_TYPE_F_INT)) {

        ret = cint_long_value(rv);

    } else {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- CINT function \"%s\" taking no arguments and returning int must exist (return 0 = dump; !0 = filter out)\n"), cint_filter_fn));
        /*
         * let's not let user-defined CINT function errors prevent further
         * processing by cint infrastructure which we are using here.
         */
        cint_errno = CINT_E_NONE;
    }

    return ret;
}

/*
 * Allocates and builds buffer for storing an API call entry consisting of
 * contextual information and all API arguments. The storage is allocated from
 * a circular buffer queue, and once the entry is built acknowleges this back
 * to the circular buffer infrastructure so that any consumers can start to
 * process it. This is the top-level handler when mode is Cache. There is no
 * option to execute user-supplied CINT filter functions in this case since the
 * user can always filter out any API calls they didn't need to log in the
 * circular buffer consumer callback.
 *
 * Parameters
 *   [in] tdata
 *     Thread specific data for the calling thread. This holds the boolean
 *     per-thread disable for the logger.
 *   [in] call_ctxt
 *     context specific to the API call in the context of which the filter
 *     is to be called
 *   [in] ctxt
 *     The api_params structure
 *   [in] arg_ptrs
 *     Array of pointers to the API arguments
 *   [out] buffer
 *     Pointer to the allocated buffer is returned in this variable
 *   [out] bufsz
 *     Size of the allocated buffer is returned in this variable
 *
 * Returns
 *   None
 *
 * There are no other outputs.
 */
static void
cint_logger_cache_function_arguments (cint_logger_thread_data_t *tdata,
                                      cint_logger_call_ctxt_t *call_ctxt,
                                      cint_logger_api_params_t *ctxt,
                                      void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1],
                                      char **buffer_p,
                                      int *bufsz_p)
{
    int array_size_save[CINT_CONFIG_MAX_FPARAMS+1];
    void *buf;
    int bufsz = 0;

    if (!buffer_p || !bufsz_p) {
        return;
    }

    cint_logger_calculate_buffer_req(call_ctxt, ctxt, arg_ptrs, array_size_save, &bufsz);

    buf = cbuf_get_buffer(Cint_logger_cfg.cbuf, bufsz, 0);
    if (!buf) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- could not allocate buffer of size %d\n"), bufsz));
        return;
    }

    cint_logger_copy_function_arguments(tdata, call_ctxt, ctxt, arg_ptrs, array_size_save, buf, bufsz);

    cbuf_get_buffer_done(Cint_logger_cfg.cbuf, buf);

    *buffer_p = buf;
    *bufsz_p  = bufsz;

    return;
}


/*
 * This is a lightweight routine to serialize one entry to file, intended to be
 * called inline with API calls when its log entry is created. The buffer is
 * written to file as-is in binary format, therefore it can be decoded
 * correctly only by the same SW that generated it -- i.e. processor
 * architecture and SDK version must be the same.
 *
 * Parameters
 *   [in] ctxt
 *     API params structure for this buffer entry
 *   [in] raw_logfile
 *     Name of the file to which raw entry is to be written
 *   [in] raw_logfile_fp
 *     FILE* to which the raw entry is to be written
 *   [in] buf
 *     Buffer containing the data to write
 *   [in] bufsz
 *     Size of the data in buffer
 *
 * Returns
 *   0 on success, -1 on detection of failure during FILE IO
 *
 * Entry is written to the logfile and logfile is flushed.
 */
static int
cint_logger_write_buffer_to_file (cint_logger_api_params_t *ctxt,
                                  char *raw_logfile, void *raw_logfile_fp,
                                  void *buf, int bufsz)
{
    int osz = 0;

    /*
     * If a file is available to write to and the writing mode is RAW, write
     * the generated record buffer now.
     */
    if (raw_logfile_fp && raw_logfile) {

        if ((osz = CINT_FWRITE(&bufsz, 1, sizeof(int), raw_logfile_fp)) != sizeof(int)) {

            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("ERROR -- (%s) failed to write buffer into file %s (wrote %d vs %d expected). Closing the file...\n"),
                       ctxt->fn,
                       raw_logfile,
                       osz, (int)sizeof(int)));

            CINT_FCLOSE(raw_logfile_fp);

            return -1;
        }

        if ((osz = CINT_FWRITE(buf, 1, bufsz, raw_logfile_fp)) != bufsz) {

            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("ERROR -- (%s) failed to write buffer into file %s (wrote %d vs %d expected). Closing the file...\n"),
                       ctxt->fn,
                       raw_logfile,
                       osz, bufsz));

            CINT_FCLOSE(raw_logfile_fp);

            return -1;
        }

        CINT_FFLUSH(raw_logfile_fp);

    }

    return 0;
}

/*
 * This is the top-level handler when mode is Log . If there exists a
 * user-defined CINT filter function, it is executed after first injecting CINT
 * variables by the same names as the API arguments which also map onto those
 * arguments. The filter function receives the same values as were passed in
 * the API arguments, and may update those arguments.
 *
 * Additionally, the user-defined CINT filter function can also return two
 * booleans - one which is provided as a pointer in the call_ctxt datastructure
 * and gates whether the per-device dispatch function is invoked; and another
 * one which indicates to the logger whether to create and log an entry for
 * this call or not.
 *
 *
 * Parameters
 *   [in] tdata
 *     Thread specific data for the calling thread.
 *   [in] call_ctxt
 *     context specific to the API call
 *   [in] ctxt
 *     The api_params structure
 *   [in] arg_ptrs
 *     Array of pointers to the API arguments
 *
 * Returns
 *   None
 *
 * If the call is not "filtered" then an entry is created in the circular
 * buffer, and that entry is also logged to CINT-like textual output. There
 * are no other outputs.
 */
static void
cint_logger_log_function_arguments (cint_logger_thread_data_t *tdata,
                                    cint_logger_call_ctxt_t *call_ctxt,
                                    cint_logger_api_params_t *ctxt,
                                    void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1])
{
    void *prev_scope = NULL;
    const char *fn;
    int filter = 0;
    int recsz = 0;
    char *rec = NULL;
    int ii, nargs;
    cint_parameter_desc_t *params;
    void *arg_ptrs_save[CINT_CONFIG_MAX_FPARAMS+1];

    params = ctxt->params;
    nargs = ctxt->nargs;
    fn = ctxt->fn;

    if (Cint_logger_cfg.user_filter_cb ||
        Cint_logger_cfg.cint_filter_fn) {

        CINT_VARIABLES_LOCK;

        /*
         * Switch temporarily to global scope and a new local scope under it.
         * Inject API parameters and call_ctxt as named variables. Then invoke
         * the filter function.
         */
        prev_scope = cint_variable_scope_global_set(fn);
        cint_variable_lscope_push(fn);

        for (ii = 0; params[ii].name && (ii < nargs); ii++) {
            arg_ptrs_save[ii] = arg_ptrs[ii];
            if (!params[ii].pcount && SHR_BITGET(ctxt->is_basetype_arr, ii) && arg_ptrs_save[ii]) {
                arg_ptrs_save[ii] = *(void**)arg_ptrs_save[ii];
            }
        }

        cint_logger_inject_variables(tdata, call_ctxt, ctxt, arg_ptrs_save, REPLAY_CURR_CTXT, "");

        filter = ((Cint_logger_cfg.cint_filter_fn &&
                        cint_logger_run_filter_function(tdata, call_ctxt, ctxt, arg_ptrs, Cint_logger_cfg.cint_filter_fn)) ||
                  (Cint_logger_cfg.user_filter_cb &&
                        Cint_logger_cfg.user_filter_cb(tdata, call_ctxt, ctxt, arg_ptrs)));

        cint_variable_lscope_pop(fn);
        cint_variable_scope_set(fn, prev_scope);

        CINT_VARIABLES_UNLOCK;

    }

    if (filter) {
        return;
    }

    cint_logger_cache_function_arguments(tdata, call_ctxt, ctxt, arg_ptrs, &rec, &recsz);
    if (!rec || !recsz) {
        return;
    }

    if (cint_logger_write_buffer_to_file(ctxt,
                                         Cint_logger_cfg.raw_logfile,
                                         Cint_logger_cfg.raw_logfile_fp,
                                         rec, recsz) < 0) {

        if (Cint_logger_cfg.raw_logfile) {

            CINT_FREE(Cint_logger_cfg.raw_logfile);
        }

        Cint_logger_cfg.raw_logfile    = NULL;
        Cint_logger_cfg.raw_logfile_fp = NULL;
    }
}

/*
 * Very similar to cint_logger_cache_function_arguments, except that instead of
 * creating a logging entry on the circular buffer it allocates (malloc) a
 * buffer into which the logging entry is written.
 *
 * Parameters
 *   [in] tdata
 *     Thread specific data for the calling thread. This holds the boolean
 *     per-thread disable for the logger.
 *   [in] call_ctxt
 *     context specific to the API call in the context of which the filter
 *     is to be called
 *   [in] ctxt
 *     The api_params structure
 *   [in] arg_ptrs
 *     Array of pointers to the API arguments
 *   [out] buffer_p
 *     Pointer to the allocated buffer is returned in this variable
 *   [out] bufsz_p
 *     Size of the allocated buffer is returned in this variable
 *
 * Returns
 *   None
 */
static void
cint_logger_cache_params_for_replay (cint_logger_thread_data_t *tdata,
                                     cint_logger_call_ctxt_t *call_ctxt,
                                     cint_logger_api_params_t *ctxt,
                                     void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1],
                                     char **buffer_p,
                                     int *bufsz_p)
{
    int array_size_save[CINT_CONFIG_MAX_FPARAMS+1];
    void *buf;
    int bufsz;

    cint_logger_calculate_buffer_req(call_ctxt, ctxt, arg_ptrs, array_size_save, &bufsz);

    buf = CINT_MALLOC(bufsz);
    CINT_MEMSET(buf, 0, bufsz);

    cint_logger_copy_function_arguments(tdata, call_ctxt, ctxt, arg_ptrs, array_size_save, buf, bufsz);

    *buffer_p = buf;
    *bufsz_p = bufsz;
}

/*
 * Top level handler when the logging mode is Replay. Replay mode is to be used
 * when CINT was automatically created from previously generated logs and the
 * logger is invoked from the API calls from such CINT.
 *
 * For successful operation it is assumed that (1) the original logs from which
 * the CINT was generated, contained both the call1 and call2 logging entries
 * for every API call which is to be applied; (2) that such logs were passed
 * through $SDK/tools/cint2c.pl which takes care of translating those logs into
 * valid C code; and that (3) the initial state of the system where Replay is being
 * attempted is exactly equal to the state of the system from which the logs
 * were generated, just prior to them being generated.
 *
 * If (1) and (2) are true, then every API called in the original system, will
 * receive two invocations during Replay -- once when the call1 entry is
 * executed and once when the call2 entry is executed.
 *
 * Further, just like in any API invocation, the logger will be called twice
 * for each of those two API invocations.
 *
 * Therefore during Replay the logger receives total 4 calls for each original
 * API call. These 4 calls may be named call_11, call_12, call_21 and call_22
 * -- respectively for call1 during replay of call1 logs, call2 during replay
 * of call1 logs, call1 during replay of call2 logs and call2 during replay of
 * call2 logs.
 *
 *
 *
 * During the logging process, output pointers are not dereferenced during the
 * input stage and input pointers are not dereferenced during the output stage.
 * This results in the original inputs and outputs to be present in two
 * separate logging entries - i.e. input pointers are all available only in the
 * call1 logging entry and output pointers are all available only in the call2
 * logging entry.
 *
 *
 *
 * The routine here performs the following tasks in order to facilitate calling
 * of the original API during Replay.
 *
 * First, Call1 and Call2 logging entries of the same original API call are
 * matched using the caller thread ID and call_id in a custom database, See
 * cint_logger_replay.c
 *
 * 1. During call_11, input parameters are saved in a temporary logging entry.
 * This logging entry is created exactly like an entry on the circular buffer
 * is created, except that it is written to a malloc'd buffer.
 *
 * Additionally, the API implementation is bypassed using skip_dispatch and
 * logging is marked to be filtered. This is the "default" behavior which the
 * user may override in the CINT filter function.
 *
 * 2. During call_12, nothing special needs to be done. So this becomes a noop
 *
 * 3. During call_21, original output parameters are stored in a temporary
 * logging entry just as inputs were stored in #1 above.
 *
 * However, unlike in #1, this API implementation is to be executed therefore
 * skip_dispatch is not set. Buffers of appropriate size are allocated for both
 * input and output pointers; inputs are populated from the logging entry
 * created at #1 and outputs are initialized to NULL values (buffers are filled
 * with zeros).
 *
 * A user supplied filter function is invoked which may bypass the API
 * implementation if so desired (i.e. this API is effectively never replayed)
 *
 * 4. During call_22, output buffers populated by the API implementation are
 * available. A user-supplied filter function may compare the outputs generated
 * during API replay against the outputs generated by the original API and take
 * appropriate action. Perhaps warn the user...
 *
 * After the user-supplied filter function is invoked, all that remains is to
 * free up the temporary logging entries created at step #1 and step #3.
 *
 * TBD: the logger can obviously compare the newly generated output buffer
 * against the original output buffer in a field-by-field comparison, and issue
 * warnings, or halt further processing of the replay CINT. However, this is
 * currently not implemented.
 *
 *
 *
 * Note that the original inputs and outputs are available to the CINT filter
 * function, whenever possible -- e.g. during call_11  and call12 inputs are
 * available but outputs are not. During call_21 and call_22 both original
 * inputs and original outputs are available.
 *
 *
 *
 * All API parameters from call1 logging entry are prefixed with "log1_", all
 * API parameters from call2 logging entry are prefixed with "log2_" -- e.g.
 * in the context of say
 *
 *     'int bcm_port_enable_get(int unit, bcm_port_t port, int* enable)'
 *
 * the following variables are available to the CINT filter function at the
 * respective stages.
 *
 * call_11
 *
 *      unit     , port     , enable
 *      log1_unit, log1_port, log1_enable
 *
 *   Note that since during call1 of the logging process only input pointers
 *   are dereferenced and enable is an API output, both enable and log1_enable
 *   will be dangling pointers.
 *
 * call_12
 *
 *   Same as call_11, plus return value variable r
 *
 * call_21
 *
 *      unit     , port     , enable
 *      log1_unit, log1_port, log1_enable
 *      log2_unit, log2_port, log2_enable
 *      log2_r
 *
 *   As described at #3 above the output pointer enable will be allocated by the
 *   replay helper below and pointed buffer will contain all zeros. log1_enable
 *   will still be a dangling pointer. log2_enable will point to a variable
 *   holding the originally returned output value.
 *
 *   Additionally, the original return value will also be available in log2_r
 *
 * call_22
 *
 *   Same as call_21, plus the API return value variable r. If the API dispatch
 *   was not bypassed by the CINT filter function, enable now holds the output
 *   generated locally i.e. by the API implementation, and the CINT filter
 *   function may now check *enable against *log2_enable to know whether the
 *   result of Replay was exactly the same or not.
 *
 *   The replay helper here may also compare these two outputs and issue a
 *   warning. This is currently TBD.
 *
 * Parameters
 *   [in] tdata
 *     Thread specific data for the calling thread.
 *   [in] call_ctxt
 *     context specific to the API call
 *   [in] ctxt
 *     The api_params structure
 *   [in] arg_ptrs
 *     Array of pointers to the API arguments
 *
 * Returns
 *   None
 *
 *
 * If a CINT filter function does not alter the default behavior here, two new
 * Logging entries will be created holding parameter values from the dispatched
 * API at step #3 above.
 */
static void
cint_logger_replay_function_arguments (cint_logger_thread_data_t *tdata,
                                       cint_logger_call_ctxt_t *call_ctxt,
                                       cint_logger_api_params_t *ctxt,
                                       void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1])
{
    void *prev_scope = NULL;
    const char *fn;
    cint_parameter_desc_t *params;
    int ii, nargs;
    cint_logger_replay_info_t *replay_info;
    cint_logger_call_ctxt_t *orig_call_ctxt = NULL;
    cint_logger_call_ctxt_t *call_ctxt_call1 = NULL;
    cint_logger_call_ctxt_t *call_ctxt_call2 = NULL;
    cint_logger_api_params_t *ctxt_call1 = NULL;
    cint_logger_api_params_t *ctxt_call2 = NULL;
    int r_ignored;
    void *arg_ptrs_save[CINT_CONFIG_MAX_FPARAMS+1] ={ NULL, };
    void *arg_ptrs_cint[CINT_CONFIG_MAX_FPARAMS+1] = { &r_ignored, };
    void *dummy_ptrs_cint[CINT_CONFIG_MAX_FPARAMS+1] = { NULL, };
    void *arg_ptrs_call1[CINT_CONFIG_MAX_FPARAMS+1] = { NULL, };
    void *arg_ptrs_call2[CINT_CONFIG_MAX_FPARAMS+1] = { NULL, };
    void *dummy_ptrs_call1[CINT_CONFIG_MAX_FPARAMS+1][5] = {{ NULL, },};
    void *dummy_ptrs_call2[CINT_CONFIG_MAX_FPARAMS+1][5] = {{ NULL, },};
    void **alloc_bufs;
    char *str;
    int alloc_sz, array_cnt, array_cnt_indx, array_cnt_size;
    void *array_cnt_ptr;
    char *rec;
    int recsz;
    int filter = 0;

    /*
     * Determine whether indeed an API replay is in progress -- if a variable
     * "this" is found holding the call_ctxt structure and variables by the
     * same name as the API parameters are found in the immediate local scope
     * of the CINT interpreter, then it is determined that a replay is in
     * progress. The pointer to the original call_ctxt structure is returned
     * and arg_ptrs_cint[] is populated such that each member points to the
     * corresponding argument's value in CINT interpreter.
     *
     * If a replay is not in progress, do nothing and exit... this is not an
     * error, it is possible that in the system where replay is being attempted
     * there are other threads invoking BCM APIs, and the code-flow will land
     * up here since mode is Replay.
     */
    if (!cint_logger_is_replay(tdata, call_ctxt, ctxt, &orig_call_ctxt, arg_ptrs_cint, dummy_ptrs_cint)) {

        LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("DEBUG -- %s was invoked with mode Replay, but Replay info is not available\n"),
                   call_ctxt->api));
        return;
    }

    /*
     * Two separate logging entries provide complete data of the original API
     * call. Find a slot to cache both pieces of information -- a unique slot
     * is found based on the original caller thread ID, and the original call
     * ID.
     */
    cint_logger_call_ctxt_to_replay_info(orig_call_ctxt, &replay_info);
    if (!replay_info) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- replay_info slot was not found (%s,%p,%d)\n"),
                   orig_call_ctxt->api, (void*)orig_call_ctxt->caller_tid, orig_call_ctxt->call_id));
        return;
    }

    params = ctxt->params;
    nargs = ctxt->nargs;
    fn = ctxt->fn;

    if (orig_call_ctxt->flags & CINT_PARAM_IN) {

        /*
         * Replay of call1 entry. Need not create new logging entries in this
         * pass because the API implementation is going to be bypassed below.
         */
        filter = 1;

        if (call_ctxt->flags & CINT_PARAM_IN) {

            /*
             * call_11
             */


            /*
             * Create a new entry holding original API parameters from call1
             * logs, and store its reference in the replay_info
             */
            cint_logger_cache_params_for_replay(tdata, orig_call_ctxt, ctxt, arg_ptrs_cint, &rec, &recsz);

            replay_info->call1_buffer = rec;

            replay_info->call1_bufsz  = recsz;

            /*
             * bypass API implementation for this call by default
             */
            *(call_ctxt->skip_dispatch) = 1;

            /*
             * Set return value to UNAVAIL so that other inline observers of
             * API calls are not misled.
             */
            *(int*) arg_ptrs[0] = SOC_E_UNAVAIL;

        } else if (call_ctxt->flags & CINT_PARAM_OUT) {

            /*
             * call_12 , nothing special to do here.
             */
        }

    } else if (orig_call_ctxt->flags & CINT_PARAM_OUT) {

        /*
         * Replay of call2 entry. This is where API implementation is going to
         * be called after allocating output buffers. Therefore, this call data
         * should not be filtered out.
         *
         * Resulting logging output may be diff'd against the original logs to
         * understand differences.
         */
        filter = 0;

        if (call_ctxt->flags & CINT_PARAM_IN) {

            /*
             * call_21, this is where most of the work gets done.
             */

            /*
             * Here the API implementation should be called.
             */
            *(call_ctxt->skip_dispatch) = 0;

            /*
             * Create a new entry holding original API parameters from call2
             * logs, and store its reference in the replay_info
             */
            cint_logger_cache_params_for_replay(tdata, orig_call_ctxt, ctxt, arg_ptrs_cint, &rec, &recsz);

            replay_info->call2_buffer = rec;

            replay_info->call2_bufsz  = recsz;

            /*
             * Decode the entry holding original inputs. This is needed to fill
             * new input pointers with the original input values.
             */
            cint_logger_decode_arguments_buffer(&call_ctxt_call1,
                                                &ctxt_call1,
                                                arg_ptrs_call1,
                                                dummy_ptrs_call1,
                                                replay_info->call1_buffer,
                                                replay_info->call1_bufsz,
                                                1);

            alloc_bufs = replay_info->alloc_bufs;

            /*
             * Now allocate new buffers for all the API inputs and outputs that
             * are pointers. Need to allocate input pointers also, because
             * sometimes the same pointer may hold both input and output
             * values, i.e. CINT_PARAM_INOUT parameters.
             */
            for (ii = 0; params[ii].name && (ii < nargs); ii++) {

                /*
                 * Nothing to do if the parameter is not a pointer, OR,
                 * parameter is an input but the original input pointer was
                 * NULL, OR, parameter is an output but the user supplied a
                 * NULL value to that pointer.
                 */
                if (!params[ii].pcount ||
                    ((params[ii].flags & CINT_PARAM_IN)  && !*(void**) arg_ptrs_call1[ii]) ||
                    ((params[ii].flags & CINT_PARAM_OUT) && !*(void**) arg_ptrs_cint[ii])) {

                    continue;
                }

                /*
                 * If pcount is 1, need to allocate a buffer of size of the
                 * pointed datatype, including accomodating the case of
                 * variable length arrays. Else, just need to allocate space
                 * for a pointer
                 */
                if (params[ii].pcount == 1) {

                    array_cnt = 1;

                    /*
                     * If the parameter is a variable length array, allocate
                     * required number of objects of the basetype
                     */
                    if (params[ii].flags & CINT_PARAM_VP) {

                        array_cnt_indx = params[ii].flags & CINT_PARAM_IDX;

                        array_cnt_size = ctxt->basetype_size[array_cnt_indx];

                        array_cnt_ptr = (params[array_cnt_indx].pcount) ? *(void**) arg_ptrs_cint[array_cnt_indx] : arg_ptrs_cint[array_cnt_indx];

                        switch (array_cnt_size) {
                        case sizeof(uint8)  : array_cnt = *(uint8*)  array_cnt_ptr; break;
                        case sizeof(uint16) : array_cnt = *(uint16*) array_cnt_ptr; break;
                        case sizeof(uint32) : array_cnt = *(uint32*) array_cnt_ptr; break;
                        case sizeof(uint64) : array_cnt = COMPILER_64_LO(*(uint64*) array_cnt_ptr); break;
                        default: break;
                        }

                    } else if (SHR_BITGET(ctxt->is_charptr, ii)) {

                        /*
                         * Character pointers are a special case of a variable
                         * length array in that the pointed data indicates its
                         * length (i.e. via NULL termination)
                         */
                        str = *(char**) arg_ptrs_call1[ii];

                        array_cnt = CINT_STRLEN(str) + 1;
                    }

                    alloc_sz = array_cnt * ctxt->basetype_size[ii];

                } else {

                    alloc_sz = sizeof(void*);
                }

                alloc_bufs[ii] = CINT_MALLOC(alloc_sz);

                if (params[ii].flags & CINT_PARAM_IN) {

                    /*
                     * Parameter is an input; copy its value from the call1 entry
                     */
                    CINT_MEMCPY(alloc_bufs[ii], *(void**) arg_ptrs_call1[ii], alloc_sz);

                } else {

                    /*
                     * Parameter is an output; set the output buffer to zeros
                     */
                    CINT_MEMSET(alloc_bufs[ii], 0, alloc_sz);
                }

                /*
                 * Point to the newly allocate buffer
                 */
                *(void**)arg_ptrs[ii] = alloc_bufs[ii];
            }

        } else if (call_ctxt->flags & CINT_PARAM_OUT) {

            /*
             * call_22, noop for now.
             *
             * May compare output values against the original output values
             * available from the call2 entry and issue informational or
             * warning messages. This is TBD.
             */

        }
    }

    if (Cint_logger_cfg.cint_replay_filter_fn) {

        CINT_VARIABLES_LOCK;

        /*
         * Switch temporarily to global scope and a new local scope under it.
         * Inject API parameters and call_ctxt as named variables. Then invoke
         * the filter function.
         */
        prev_scope = cint_variable_scope_global_set(fn);
        cint_variable_lscope_push(fn);

        for (ii = 0; params[ii].name && (ii < nargs); ii++) {
            arg_ptrs_save[ii] = arg_ptrs[ii];
            if (!params[ii].pcount && SHR_BITGET(ctxt->is_basetype_arr, ii) && arg_ptrs_save[ii]) {
                arg_ptrs_save[ii] = *(void**)arg_ptrs_save[ii];
            }
        }

        /*
         * Inject the actual API parameters by name. These were copied by the
         * CINT interpreter while sourcing the CINT and pointer type members
         * were altered in call_21 above. Also inject "this" for the current
         * call_ctxt.
         */
        cint_logger_inject_variables(tdata, call_ctxt, ctxt, arg_ptrs_save, REPLAY_CURR_CTXT, "");

        /*
         * If the call1 buffer is available, inject the original API parameters
         * by name prefixed with "log1_". Also inject the original call_ctxt in
         * the call1 logs by name "log1".
         */
        if (replay_info->call1_buffer && replay_info->call1_bufsz) {

            cint_logger_decode_arguments_buffer(&call_ctxt_call1,
                                                &ctxt_call1,
                                                arg_ptrs_call1,
                                                dummy_ptrs_call1,
                                                replay_info->call1_buffer,
                                                replay_info->call1_bufsz,
                                                1);

            cint_logger_inject_variables(tdata, call_ctxt_call1, ctxt_call1, arg_ptrs_call1,
                                         REPLAY_LOG1_CTXT, REPLAY_LOG1_VPFX);
        }

        /*
         * If the call2 buffer is available, inject the original API parameters
         * by name prefixed with "log2_". Also inject the original call_ctxt in
         * the call2 logs by name "log2".
         */
        if (replay_info->call2_buffer && replay_info->call2_bufsz) {

            cint_logger_decode_arguments_buffer(&call_ctxt_call2,
                                                &ctxt_call2,
                                                arg_ptrs_call2,
                                                dummy_ptrs_call2,
                                                replay_info->call2_buffer,
                                                replay_info->call2_bufsz,
                                                1);

            cint_logger_inject_variables(tdata, call_ctxt_call2, ctxt_call2, arg_ptrs_call2,
                                         REPLAY_LOG2_CTXT, REPLAY_LOG2_VPFX);
        }

        /*
         * Invoke the CINT filter function.
         */
        filter = (Cint_logger_cfg.cint_replay_filter_fn &&
                        cint_logger_run_filter_function(tdata, call_ctxt, ctxt, arg_ptrs, Cint_logger_cfg.cint_replay_filter_fn));

        cint_variable_lscope_pop(fn);
        cint_variable_scope_set(fn, prev_scope);

        CINT_VARIABLES_UNLOCK;

    }

    if (!filter) {

        cint_logger_log_function_arguments(tdata, call_ctxt, ctxt, arg_ptrs);
    }

    if (orig_call_ctxt->flags & CINT_PARAM_IN) {

        if (call_ctxt->flags & CINT_PARAM_IN) {

            /*
             * call_11 ... nothing to do here.
             */

        } else if (call_ctxt->flags & CINT_PARAM_OUT) {

            /*
             * call_12 ... nothing to do here.
             */

        }

    } else if (orig_call_ctxt->flags & CINT_PARAM_OUT) {

        if (call_ctxt->flags & CINT_PARAM_IN) {

            /*
             * call_21 ... nothing to do here.
             */

        } else if (call_ctxt->flags & CINT_PARAM_OUT) {

            /*
             * call_22 ... free the temporary logging entries created during
             * call_11 and call_21, also free up the replay_info.
             *
             * This is also another opportunity to verify whether actual
             * outputs are different from the original outputs, after the CINT
             * filter function was executed. TBD.
             */

            cint_logger_call_ctxt_to_replay_info_done(orig_call_ctxt, replay_info);
        }

    }

}

/*
 * Initialize the api_params structure for the API named in ctxt->fn
 * Goes over each API parameter description and captures in bitmaps whether
 * those arguments are char pointer, array, or void; and also the size of each
 * parameter.
 *
 * Parameters
 *   [in] ctxt
 *     The api_params structure to initialize. Only ctxt->fn needs to be set to
 *     the API name
 *
 * Returns
 *   None
 */
void
cint_logger_init_api_params (cint_logger_api_params_t *ctxt)
{
    cint_parameter_desc_t *params, *ppdt;
    cint_datatype_t dt;
    cint_function_t *func;
    int i, nargs;
    int is_basetype_arr, is_charptr, is_basetype_void;

    if (ctxt->params) {
        return;
    }

    CINT_MEMSET(&dt, 0, sizeof(dt));

    CINT_VARIABLES_LOCK;

    if ((cint_datatype_find(ctxt->fn, &dt) == CINT_E_NONE) &&
        (dt.flags & CINT_DATATYPE_F_FUNC)) {

        func = dt.basetype.p;
        params = func->params;
        if (!params) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("FATAL ERROR -- no arguments table for function %s\n"), ctxt->fn));
            goto exit;
        }

        ctxt->params = params;
        nargs = ctxt->nargs = cint_parameter_count(params);
        if (nargs > (CINT_CONFIG_MAX_FPARAMS+1)) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("FATAL ERROR -- %s has more than max %d arguments (%d)\n"), ctxt->fn, CINT_CONFIG_MAX_FPARAMS, nargs));
            goto exit;
        }

        for (i = 0, ppdt = params; (i < nargs) && ppdt->name; i++, ppdt++) {

            CINT_MEMSET(&dt, 0, sizeof(dt));
            if (cint_datatype_find(ppdt->basetype, &dt) != CINT_E_NONE) {
                LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                          (BSL_META("FATAL ERROR -- %s param %d has unknown basetype %s\n"), ctxt->fn, i, ppdt->basetype));
                goto exit;
            }

            is_basetype_arr  = (dt.desc.num_dimensions > 0);
            is_charptr       = (ppdt->pcount && !CINT_STRCMP(ppdt->basetype, "char"));
            is_basetype_void = !CINT_STRCMP(ppdt->basetype, "void");

            SHR_BITWRITE(ctxt->is_basetype_arr, i, is_basetype_arr);
            SHR_BITWRITE(ctxt->is_charptr,      i, is_charptr);

            ctxt->basetype_size[i] = (is_basetype_void) ? 0 : cint_datatype_size(&dt);

        }
    }

exit:
    CINT_VARIABLES_UNLOCK;

}

/*
 * Top-level Logger Function.
 * This is the entry point for logging any API call, called twice from
 * dispatch.c through the BCM_CALL_DISPATCH MACRO. If it was desired to enable
 * logging for any arbitrary API, say a non-dispatchable API, then a call
 * should be placed for this function from the point of entry and point of exit
 * of such API, suppyling the following inputs.
 *
 * Parameters
 *   [in] fn
 *     Name of the API which is to be logged. This API name must be declared in
 *     the CINT interpreter in order for the API to be logged.  Logger
 *     maintains a reference to this pointer so the caller must pass a static
 *     string in this parameter.
 *   [in] api_params
 *     Pointer to a pointer which should be initially NULL. Logger searches the
 *     parameter descriptor for this API in the CINT interpreter using its name
 *     fn and stores it in *api_params. This search is expensive in terms of
 *     execution time, hence the caller must send back the descriptor in
 *     *api_params at all subsequent calls to the API.
 *   [in] flags
 *     currently only indicates whether the call was placed before or after
 *     dispatch. May be expanded in the future to signal additional
 *     information.
 *   [in] call_id
 *     Unique number describing the caller's sequence; currently set to the
 *     API's sequence number. This is captured in the call context structure
 *     passed downstream to the actual logging routines.
 *   [in] arg_ptrs
 *     pointers to the arguments
 *   [out] skip_dispatch
 *     pointer to a Boolean which controls whether API implementation will be
 *     executed or not. This boolean may be written from the CINT filter
 *     function.
 *
 * Returns
 *   None
 *
 * Remarks
 *   Creates an entry in the circular buffer and possibly also logs it to
 *   console, depending on the logging mode.
 */
void
cint_log_function_arguments (const char *fn, void **api_params, unsigned int flags, int call_id, void *arg_ptrs[], int *skip_dispatch)
{
    cint_logger_thread_data_t *tdata;
    cint_logger_api_params_t *ctxt;
    int nargs;
    cint_logger_call_ctxt_t call_ctxt;
    int recsz = 0;
    char *rec = NULL;
    int cint_logger_unroll = 0;

    tdata = cint_logger_thread_specific();

    if (!fn || !arg_ptrs || !api_params || !tdata) {

        goto exit;
    }

    CINT_LOGGER_LOCK;

    (*tdata).logger_is_active = CINT_LOGGER_CINT_OP_BSL;

    cint_logger_unroll++;

    if (!(ctxt = *api_params)) {

        ctxt = cint_logger_get_api_params_buffer();

        if (!ctxt) {

            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("ERROR -- ran out of free api_params structures for %s (max %d allowed). "
                                "Need to increase CINT_LOGGER_MAX_N_APIS and rebuild the SDK to log larger number of APIs\n"),
                       fn, CINT_LOGGER_MAX_N_APIS));
            goto exit;
        }

        cint_logger_api_params_t_init(ctxt);

        ctxt->fn = fn;

        ctxt->api_params = (cint_logger_api_params_t**) api_params;

        *api_params = ctxt;
    }

    cint_logger_cfg_init();

    if (!Cint_logger_cfg.reinit && (!Cint_logger_cfg.enabled || (*tdata).disabled)) {

        goto exit;
    }

    cmd_cint_initialize();

    cint_logger_init();

    cint_logger_init_api_params(ctxt);

    if (!ctxt->params) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- no arguments table for function %s; exitting\n"), ctxt->fn));
        goto exit;
    }

    nargs = ctxt->nargs;
    if (nargs > (CINT_CONFIG_MAX_FPARAMS+1)) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- %s has more than max %d arguments (%d)\n"), ctxt->fn, CINT_CONFIG_MAX_FPARAMS, nargs));
        goto exit;
    }

    if ((flags & CINT_PARAM_IDX) == CINT_PARAM_IDX) {
        goto exit;
    }

    call_ctxt.api = ctxt->fn;
    call_ctxt.flags = flags;
    call_ctxt.caller_tid = sal_thread_self();
    call_ctxt.caller_tname = NULL;
    call_ctxt.caller_file = (*tdata).caller_file;
    call_ctxt.caller_line = (*tdata).caller_line;
    call_ctxt.caller_function = (*tdata).caller_func;
    call_ctxt.skip_dispatch = skip_dispatch;
    call_ctxt.call_id = call_id;
#if CINT_CONFIG_INCLUDE_POSIX_TIMER == 1
    cint_gettime(&call_ctxt.timestamp);
#endif

    switch (Cint_logger_cfg.mode) {
    case cintLoggerModeLog:
        cint_logger_log_function_arguments(tdata, &call_ctxt, ctxt, arg_ptrs);
        break;
    case cintLoggerModeReplay:
        cint_logger_replay_function_arguments(tdata, &call_ctxt, ctxt, arg_ptrs);
        break;
    case cintLoggerModeCache:
        cint_logger_cache_function_arguments(tdata, &call_ctxt, ctxt, arg_ptrs, &rec, &recsz);
        break;
    default:
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- invalid operating mode %d\n"), Cint_logger_cfg.mode));
        break;
    }

exit:

    if (cint_logger_unroll >= 1) {

        (*tdata).logger_is_active = CINT_LOGGER_CINT_OP_NONE;

        CINT_LOGGER_UNLOCK;
    }

}

/*
 * Logger's printf routine, called from cint_printk.
 *
 * Parameters
 *   Same as LIBC vprintf
 *
 * Returns
 *   Same as vprintf; string length of the generated output
 */
int
cint_logger_vprintf (const char* fmt, va_list args)
{
    void *file_fp;
    cint_logger_thread_data_t *tdata;
    int rv = 0;
    va_list args2;
    char obuf[1024], *obuf2;
    int ototal, obuf_size = sizeof(obuf);
    unsigned int bsl_lss;

    tdata = cint_logger_thread_specific();

    if (tdata && (file_fp = (*tdata).logfile_fp) &&
        ((*tdata).logger_is_active & CINT_LOGGER_CINT_OP_THREAD_FILE)) {

        rv = CINT_VFPRINTF((FILE*) file_fp, fmt, args);

        CINT_FFLUSH(file_fp);

    } else if ((file_fp = Cint_logger_cfg.logfile_fp) &&
               (Cint_logger_cfg.log_to_file & CINT_LOGGER_FILE_OP_CINT) && tdata &&
               ((*tdata).logger_is_active & CINT_LOGGER_CINT_OP_GLOBAL_FILE)) {

        rv = CINT_VFPRINTF((FILE*) file_fp, fmt, args);

        CINT_FFLUSH(file_fp);

    } else {

        bsl_lss = BSL_LS_APPL_CINTAPILOGGER | BSL_INFO;

        va_copy(args2, args);

        ototal = vsnprintf(obuf, sizeof(obuf), fmt, args2);

        if (ototal >= obuf_size) {

            obuf_size = ototal + 1;

            obuf2 = sal_alloc(obuf_size, "cint_logger_vprintf output buffer - 2nd attempt");

            ototal = vsnprintf(obuf2, obuf_size, fmt, args);

            if (ototal >= obuf_size) {
               /* assert(); -- This shouldn't happen */
            }

            rv = bsl_printf("<c=%u>%s", bsl_lss, obuf2);

            sal_free(obuf2);

        } else {

            rv = bsl_printf("<c=%u>%s", bsl_lss, obuf);

        }

        va_end(args2);
    }

    return rv;

}

/*
 * Destructor for per-thread data-structure; this is called by the thread
 * library when a thread terminates
 *
 * Parameters
 *   [in] tdata
 *     opaque value for the thread library
 *
 * Returns
 *   None
 */
static void
cint_logger_tls_destructor (void *tdata)
{
    CINT_FREE(tdata);
}

/*
 * Init for multi-threading related objects; logger mutex and the key for
 * thread-specific objects
 *
 * Parameters
 *   None
 *
 * Returns
 *   None
 */
static void
cint_logger_thread_init (void)
{

    if (!Cint_logger_cfg.mutex) {

        SAL_GLOBAL_LOCK;

        if (!Cint_logger_cfg.mutex) {

            Cint_logger_cfg.mutex = sal_mutex_create("cint_logger_mutex");

            if (!Cint_logger_cfg.mutex) {

                LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                          (BSL_META("FATAL ERROR -- failed to allocate cint_logger_mutex\n")));

                SAL_GLOBAL_UNLOCK;

                return;
            }
        }

        SAL_GLOBAL_UNLOCK;
    }

    /*
     * Since there is no pthread_once() -equivalent SAL API use the Logger mutex
     *
     * Do not use SAL_GLOBAL_LOCK here because 1) the logger mutex is available
     * here so we don't need to; and 2) sal_tls_key_create uses sal_splhi and
     * in some places in the SDK SAL_GLOBAL_LOCK is taken after sal_splhi;
     * reverse order of locking can lead to deadlock.
 */
    if (!Cint_logger_cfg.tls_key) {

        CINT_LOGGER_LOCK;

        if (!Cint_logger_cfg.tls_key) {

            Cint_logger_cfg.tls_key = sal_tls_key_create(cint_logger_tls_destructor);

            if (!Cint_logger_cfg.tls_key) {

                LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                          (BSL_META("FATAL ERROR -- failed to allocate cint_logger tls_key\n")));
            }
        }

        CINT_LOGGER_UNLOCK;
    }

}

/*
 * Logger internal utility routine which decodes the provided log buffer,
 * pushes a new variable scope, injects variables by same name as the API
 * arguments, and invokes the provided CINT function.
 *
 * Parameters
 *   [in] fn
 *     Name of the CINT filter/formatter function to be invoked
 *   [in] buffer
 *     Pointer to the source buffer to decode
 *   [in] bufsz
 *     Size of the source buffer
 *   [in] ptrs_are_valid
 *     Boolean indicating whether ctxt and call_ctxt ptrs should be considered
 *     valid. False only when a buffer which was previously generated and
 *     written to file.
 *   [out] pcall_ctxt
 *   [out] ctxt
 *     Returned for the decoded buffer. These may point to static variables,
 *     e.g. when ptrs_are_valid is FALSE, and therefore this API is not thread
 *     safe.
 *
 * Returns
 *   Returns the return value of CINT filter function
 */
int
cint_logger_run_filter_offline (const char *fn, void *buffer, int bufsz,
                                int ptrs_are_valid,
                                cint_logger_call_ctxt_t **pcall_ctxt,
                                cint_logger_api_params_t **pctxt)
{
    cint_logger_thread_data_t *tdata = NULL;
    void *prev_scope = NULL;
    int filter;
    void *arg_ptrs[CINT_CONFIG_MAX_FPARAMS+1];
    void *dummy_ptrs[CINT_CONFIG_MAX_FPARAMS+1][5];

    tdata = cint_logger_thread_specific();

    cint_logger_decode_arguments_buffer(pcall_ctxt, pctxt,
                                        arg_ptrs, dummy_ptrs,
                                        buffer, bufsz,
                                        ptrs_are_valid);

    CINT_VARIABLES_LOCK;

    /*
     * Switch temporarily to global scope and a new local scope under it.
     * Inject API parameters and call_ctxt as named variables. Then invoke
     * the filter function.
     */
    prev_scope = cint_variable_scope_global_set(fn);
    cint_variable_lscope_push(fn);

    cint_logger_inject_variables(tdata, *pcall_ctxt, *pctxt, arg_ptrs, REPLAY_CURR_CTXT, "");

    filter = cint_logger_run_filter_function(tdata, *pcall_ctxt, *pctxt, arg_ptrs, fn);

    cint_variable_lscope_pop(fn);
    cint_variable_scope_set(fn, prev_scope);

    CINT_VARIABLES_UNLOCK;

    return filter;
}

/*
 * Consumer callback for the CINT logger circular buffer. This does the task of
 * serializing each log entry to a file, if logging to file is enabled. Invokes
 * the user-supplied CINT filter /formatter function if present, and logs only
 * if the filter function returns FALSE, i.e. not filter.
 *
 * Callback routine blocks the consumer thread which in turn may block all
 * producers...i.e. the logger requesting buffers from the circular buffer
 * infrastructure.
 *
 * Parameters
 *   Same as any circular buffer callback function
 *   [in] opaque
 *     An opaque value supplied during creation of circular_buffer_t
 *   [in] cbuf
 *     The circular_buffer_t itself from which buffer was allocated
 *   [in] buf
 *     The buffer entry to log
 *   [in] bufsz
 *     Size of the buffer entry to log
 *
 * Returns
 *   None; calls the log_arguments_buffer routine which generates CINT-like
 *   output, IF logging to a file is configured.
 */
static void
cint_logger_cbuf_log_to_file_cb (void *opaque, circular_buffer_t *cbuf, void *buf, int bufsz)
{
    cint_logger_thread_data_t *tdata;
    cint_logger_call_ctxt_t tcall_ctxt, *call_ctxt = NULL;
    cint_logger_api_params_t tctxt, *ctxt = NULL;
    int filter = 0;

    if (!buf || !bufsz ||
        !Cint_logger_cfg.logfile_fp ||
        !Cint_logger_cfg.log_to_file) {

        return;
    }

    if (Cint_logger_cfg.log_to_file & CINT_LOGGER_FILE_OP_RAW) {

        ctxt = *(cint_logger_api_params_t**) buf;

        if (cint_logger_write_buffer_to_file(ctxt,
                                             Cint_logger_cfg.logfile,
                                             Cint_logger_cfg.logfile_fp,
                                             buf, bufsz) < 0) {

            if (Cint_logger_cfg.logfile) {

                CINT_FREE(Cint_logger_cfg.logfile);
            }

            Cint_logger_cfg.logfile    = NULL;
            Cint_logger_cfg.logfile_fp = NULL;
        }

    } else if ((Cint_logger_cfg.log_to_file & CINT_LOGGER_FILE_OP_CINT) &&
               (tdata = cint_logger_thread_specific())) {


        (*tdata).logger_is_active = CINT_LOGGER_CINT_OP_GLOBAL_FILE;

        if (Cint_logger_cfg.cint_logfile_filter_fn) {

            call_ctxt = &tcall_ctxt;
            ctxt = &tctxt;
            filter = cint_logger_run_filter_offline(Cint_logger_cfg.cint_logfile_filter_fn,
                                                    buf, bufsz, 1,
                                                    &call_ctxt, &ctxt);
        }

        if (!filter) {

            CINT_VARIABLES_LOCK;

            cint_logger_log_arguments_buffer(call_ctxt, ctxt, buf, bufsz);

            CINT_VARIABLES_UNLOCK;
        }

        (*tdata).logger_is_active = CINT_LOGGER_CINT_OP_NONE;
    }
}

/***********************************************************************************/
/*                                                                                 */
/* Call all the Inits                                                              */
/*                                                                                 */
/***********************************************************************************/


/*
 * The top level Init routine for the logger. Here all the key subsystems are
 * initialized and data structures are allocated. Inits are to be called only
 * once ever, unless a reinit has been requested in response to tr 141. This
 * routine assumes that cint_logger_cfg_init, which processes all the SOC
 * properties and sets up the global configuration data-structure accordingly,
 * has already been called.
 *
 * Parameters
 *   [in]
 *     None
 *   [out]
 *     None
 *
 * Sets up all the data structures for API logging to work. It clears the
 * reinit boolean once done. Therefore, the renit boolean is set from tr 141
 * and cleared here.
 */
static void
cint_logger_init (void)
{
    static int cint_logger_inited = 0;

    /*
     * Allocate a circular buffer only if logging is enabled and one has not
     * already been allocated.
     */
    if (!Cint_logger_cfg.cbuf && Cint_logger_cfg.enabled) {

        /*
         * Create a circular buffer with attributes which may be controlled by
         * the user via SOC properties. Register the log-to-file consumer
         * callback with this circular buffer. Consumer thread is the "offload"
         * thread created for every circular buffer for processing each entry;
         * the log-to-file callback will be invoked from the consumer thread.
         */
        Cint_logger_cfg.cbuf = cbuf_circular_buffer_t_create("cint logger circular buffer",
                                                             Cint_logger_cfg.cache_size,
                                                             Cint_logger_cfg.cache_nfreeze,
                                                             Cint_logger_cfg.consumer_thread_stksz,
                                                             Cint_logger_cfg.consumer_thread_prio,
                                                             cint_logger_cbuf_log_to_file_cb, NULL);
        /*
         * This buffer must be allocated successfully otherwise the logger cannot
         * function. Disable logging on failure.
         */
        if (!Cint_logger_cfg.cbuf) {

            LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("FATAL ERROR -- failed to allocate a circular buffer; disabling logging\n")));

            Cint_logger_cfg.enabled = 0;
            /*assert(Cint_logger_cfg.cbuf != NULL)*/
        }

    }

    if (Cint_logger_cfg.reinit) {
        cint_logger_inited = 0;
    }

    if (cint_logger_inited) {
        return;
    }

    /*
     * This is redundant call because this init was called from the top-level
     * logger function also. Here for completenes' sake.
     */
    cint_logger_cfg_init();

    /*
     * Register a new sink with BSL for logging to file
     */
    cint_logger_bsl_init();

    /*
     * Integration with the CINT interpreter
     */
    cint_logger_cint_init();

    /*
     * This is redundant call because this init is called from API to fetch
     * thread-specific datastructure as a first step in the top-level logger
     * function also. Here for completenes' sake.
     */
    cint_logger_thread_init();

    cint_logger_inited = 1;

    Cint_logger_cfg.reinit = 0;
}

/*
 * Utility function which applications may use to indicate FILE, LINE and
 * FUNCTION from which a BCM API is called. This information is stored in the
 * thread-specific datastructure and stored inside the call context structure.
 *
 * Parameters
 *   [in] f - FILE, l - LINE, F - FUNCTION of the caller
 *
 * Returns
 *   None
 */
void
cint_logger_set_caller_context (const char *f, int l, const char *F)
{
    cint_logger_thread_data_t *tdata;

    tdata = cint_logger_thread_specific();

    (*tdata).caller_file = f;
    (*tdata).caller_line = l;
    (*tdata).caller_func = F;
}

/*
 * Returns the thread-specific datastructure of the current thread. If it was
 * not set before, allocates a new thread-specific datastructure, sets it as
 * such and returns it. This datastructure is free'd in the destructor which is
 * called when the thread terminates
 *
 * Parameters
 *   None
 *
 * Returns
 *   Thread-specific datastructure for the current thread.
 */
void*
cint_logger_thread_specific (void)
{
    cint_logger_thread_data_t *tdata;

    cint_logger_thread_init();

    tdata = sal_tls_key_get(Cint_logger_cfg.tls_key);

    if (!tdata) {

        tdata = CINT_MALLOC(sizeof(*tdata));

        if (!tdata) {

            LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("FATAL ERROR -- malloc tdata failed\n")));
            return NULL;
        }

        CINT_MEMSET(tdata, 0, sizeof(*tdata));

        sal_tls_key_set(Cint_logger_cfg.tls_key, tdata);
    }

    return tdata;
}


/*
 * Resets the logger to its initial clean state, ready for reinitialization
 * based on the current values of SOC properties. This function is being used
 * for integration with tr 141.
 *
 * All resources are freed and states are reset with the notable exception of
 * multi-threading related objects, i.e. the logger mutex and the tls_key for
 * thread-specific data; and the CINT integration. The mutex and tls_key are
 * freed only if reset was called from tr 141, because then the caller thread
 * should be the only thread executing and possibly holding those objects.
 *
 * Parameters
 *   [in] none
 *   [out] none
 *
 * Returns
 *   None.
 *
 * It sets the "reinit" boolean in the global configuration data structure.
 * That boolean is checked for re-initializing the logger.
 */
void
cint_logger_reset (void)
{
    int it;

    /*
     * First, mark the logger disabled
     */
    Cint_logger_cfg.enabled = 0;

    /*
     * then wait for any ongoing logging to complete, then lock out any new
     * attempts to do so. There should be no new attempts though, because
     * logger was disabled above.
     */
    CINT_LOGGER_LOCK;

    /*
     * First, we reset the global configuration data structure
     */

    /*
     * Destroy the circular buffer... this should stop the consumer thread and
     * free up the memory allocated for the circular buffer
     */
    cbuf_circular_buffer_t_destroy(Cint_logger_cfg.cbuf);
    Cint_logger_cfg.cbuf = NULL;

    /*
     * With all new logging stopped and the consumer thread gone, it is safe to
     * handle the file pointers now. Close the FILE handles if they exist.
     * Close also causes associated streams to be flushed, so any buffered data
     * is written off to file.
     */
    if (Cint_logger_cfg.logfile_fp) {
        CINT_FCLOSE(Cint_logger_cfg.logfile_fp);
        Cint_logger_cfg.logfile_fp = NULL;
    }

    if (Cint_logger_cfg.raw_logfile_fp) {
        CINT_FCLOSE(Cint_logger_cfg.raw_logfile_fp);
        Cint_logger_cfg.raw_logfile_fp = NULL;
    }

    /*
     * Handle all parameters of type string (char*). These were STRDUP from
     * user input and must be free'd else memory leaks will occur
     */
    if (Cint_logger_cfg.cint_filter_fn) {
        CINT_FREE(Cint_logger_cfg.cint_filter_fn);
        Cint_logger_cfg.cint_filter_fn = NULL;
    }

    if (Cint_logger_cfg.logfile) {
        CINT_FREE(Cint_logger_cfg.logfile);
        Cint_logger_cfg.logfile = NULL;
    }

    if (Cint_logger_cfg.cint_logfile_filter_fn) {
        CINT_FREE(Cint_logger_cfg.cint_logfile_filter_fn);
        Cint_logger_cfg.cint_logfile_filter_fn = NULL;
    }

    if (Cint_logger_cfg.raw_logfile) {
        CINT_FREE(Cint_logger_cfg.raw_logfile);
        Cint_logger_cfg.raw_logfile = NULL;
    }

    /*
     * Clean up the api-params structures...each used structure has a pointer
     * back to the APIs static pointer to itself. Clear that pointer to delink
     * itself from that API and then the entire table of structures may be
     * cleared.
     */
    for (it = 0; it < Cint_logger_cfg.next_free_ctxt; it++) {
        if (cint_logger_api_params[it].api_params) {
            *cint_logger_api_params[it].api_params = NULL;
            cint_logger_api_params[it].api_params = NULL;
        }
    }
    CINT_MEMSET(cint_logger_api_params, 0, sizeof(cint_logger_api_params));
    Cint_logger_cfg.next_free_ctxt = 0;

    /*
     * Mode will be reset to cache. Set the CONST attribute on call_ctxt fields
     */
    cint_logger_cint_const_fields(TRUE);

    /*
     * Now reset all remaining members to their default values. Multi-threading
     * related objects - the mutex and the key for thread-specific data - are
     * not release. Since the CINT infra does not integrate with tr 141, the
     * CINT variable for global configuration will also not be released.
     *
     * Important:
     * enabled must be set to TRUE in order for the logger to not get locked
     * out of reinitialization. If the corresponding SOC property is not set or
     * it is set to 0/Disabled then it will get reset during the first API
     * call.
     */
    Cint_logger_cfg.enabled        = 1;
    Cint_logger_cfg.mode           = cintLoggerModeCache;

    Cint_logger_cfg.cache_size     = CINT_LOGGER_CACHE_SIZE_DEFAULT;
    Cint_logger_cfg.cache_nfreeze  = CINT_LOGGER_N_FREEZE_DEFAULT;

    Cint_logger_cfg.user_filter_cb = NULL;

    /*
     * Should count also be reset?
     *
    Cint_logger_cfg.count          = 0;
     */

    Cint_logger_cfg.base_indent    = CINT_LOGGER_BASE_INDENT_DEFAULT;

    Cint_logger_cfg.consumer_thread_stksz = CINT_LOGGER_THREAD_STKSZ_DEFAULT;
    Cint_logger_cfg.consumer_thread_prio  = CINT_LOGGER_THREAD_PRIO_DEFAULT;

    Cint_logger_cfg.log_to_file    = CINT_LOGGER_FILE_OP_NONE;

    /*
     * Set ourselves up for reinitialization
     */
    Cint_logger_cfg.reinit         = 1;

    CINT_LOGGER_UNLOCK;

}

/*
 * This utility function is called from the SDK test infrastructure to indicate
 * whether tr 141 is in progress.
 *
 * Parameters
 *   [in] value
 *     Boolean indicating whether tr 141 is in progress
 *
 * Returns
 *  None
 *
 * This boolean is tested later to check whether logger should be reset, if the
 * relevant SOC property is set.
 */
void
cint_logger_tr141_set (int value)
{
    Cint_logger_cfg.tr141 = value;
}

/*
 * Test the boolean which was set from cint_logger_tr141_set. Indicates whether
 * tr141 is in progress, and thus whether logger should be reset if the
 * relevant SOC property is also set.
 *
 * Parameters
 *   [in] none
 *
 * Returns
 *   TRUE/FALSE whether tr141 is in progress or not.
 */
int
cint_logger_tr141_get (void)
{
    return Cint_logger_cfg.tr141;
}

/* } */
#else
/* { */
/* Make ISO compilers happy. */
typedef int cint_logger_c_not_empty;
/* } */
#endif
