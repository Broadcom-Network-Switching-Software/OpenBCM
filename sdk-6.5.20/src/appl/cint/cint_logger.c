/*
 * $Id: cint_logger.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger.c
 * Purpose:     CINT logger. Generates C-like logs from executing BCM APIs in
 *              order to replicate API call sequences in another environment
 */

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1

#include <shared/bsl.h>
#include <shared/bslnames.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h> 
#include <appl/diag/bslsink.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/parse.h>
#include <appl/diag/cmdlist.h>

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

/*
 * global configuration and state data for the logger
 */
cint_logger_global_cfg_data_t cint_logger_cfg = {
    .enabled        = 0                      , /* disabled by default                                */
    .mode           = cintLoggerModeCache    , /* default mode is to cache                           */
    .logLayer       = bslLayerAppl           , /* default BSL Layer to send logs to - Appl           */
    .logSource      = bslSourceCintapilogger , /* default BSL Source to send logs to - CintApiLogger */
    .cint_filter_fn = NULL                   , /* no default CINT function for filtering             */
    .user_filter_cb = NULL                   , /* no user-defined filtering callback function        */
    .tls_key        = NULL                   , /* initializer for tls_key                            */
    .mutex          = NULL                   , /* initializer for mutex                              */
    .ctxt_head      = NULL                   , /* head of api_params                              */

    .base_indent    = 4                      , /* base indent level for generated CINT output        */

    .log_to_file    = 0                      , /* logging to a file - disabled by default            */
    .logfile        = NULL                   , /* logging to a file - log file name                  */
    .logfile_fp     = NULL                   , /* logging to a file - FILE handle                    */
};
static cint_variable_t *vcint_logger_cfg = NULL;

static bslsink_sink_t _cint_logger_bslsink;

/***********************************************************************************/
/*                                                                                 */
/* Integrate with the CINT interpreter. Expose data structures and config variable */
/* so that user can access these from the interpreter.                             */
/*                                                                                 */
/***********************************************************************************/

static cint_enum_map_t __cint_enum_map__cint_logger_mode_t[] = {
    { "cintLoggerModePreload" , cintLoggerModePreload },
    { "cintLoggerModeLog"     , cintLoggerModeLog     },
    { "cintLoggerModeCache"   , cintLoggerModeCache   },
    { "cintLoggerModeReplay"  , cintLoggerModeReplay  },
    { NULL                                            },
};

static cint_enum_type_t __cint_logger_enums[] = {
    { "cint_logger_mode_t"    , __cint_enum_map__cint_logger_mode_t },
    { NULL                    ,                                     },
};

static cint_parameter_desc_t __cint_struct_members__cint_logger_global_cfg_data_t[] = {
    { "int"                         , "enabled"            , 0 , 0 },
    { "cint_logger_mode_t"          , "mode"               , 0 , 0 },
    { "int"                         , "logLayer"           , 0 , 0 },
    { "int"                         , "logSource"          , 0 , 0 },
    { "char"                        , "cint_filter_fn"     , 1 , 0 },
    { "void"                        , "user_filter_cb"     , 1 , 0 },
    { "void"                        , "tls_key"            , 1 , 0 },
    { "void"                        , "mutex"              , 1 , 0 },
    { "cint_logger_api_params_t"    , "ctxt_head"          , 1 , 0 },
    { "int"                         , "count"              , 0 , 0 },
    { "int"                         , "base_indent"        , 0 , 0 },
    { "int"                         , "log_to_file"        , 0 , 0 },
    { "char"                        , "logfile"            , 1 , 0 },
    { "void"                        , "logfile_fp"         , 1 , 0 },
    { NULL                          , NULL                 , 0 , 0 },
};

static void*
__cint_maddr__cint_logger_global_cfg_data_t (void* p, int mnum, cint_struct_type_t* parent)
{
    void *rv;
    cint_logger_global_cfg_data_t *s = p;

    switch (mnum) {
    case  0: rv = &(s->enabled);        break;
    case  1: rv = &(s->mode);           break;
    case  2: rv = &(s->logLayer);       break;
    case  3: rv = &(s->logSource);      break;
    case  4: rv = &(s->cint_filter_fn); break;
    case  5: rv = &(s->user_filter_cb); break;
    case  6: rv = &(s->tls_key);        break;
    case  7: rv = &(s->mutex);          break;
    case  8: rv = &(s->ctxt_head);      break;
    case  9: rv = &(s->count);          break;
    case 10: rv = &(s->base_indent);    break;
    case 11: rv = &(s->log_to_file);    break;
    case 12: rv = &(s->logfile);        break;
    case 13: rv = &(s->logfile_fp);     break;
    default: rv = NULL;                 break;
    }

    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cint_logger_rtime_t[] = {
    { "unsigned int" , "last" , 0 , 0 },
    { "unsigned int" , "min"  , 0 , 0 },
    { "unsigned int" , "max"  , 0 , 0 },
    { "unsigned int" , "avg"  , 0 , 0 },
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
    { "void"                , "next"            , 1 , 0 },
    { "void"                , "prev"            , 1 , 0 },
    { "char"                , "fn"              , 1 , 0 },
    { "void"                , "params"          , 1 , 0 },
    { "int"                 , "basetype_size"   , 0 , CINT_CONFIG_MAX_FPARAMS+1 },
    { "SHR_BITDCL"          , "is_basetype_arr" , 0 , _SHR_BITDCLSIZE(CINT_CONFIG_MAX_FPARAMS+1) },
    { "SHR_BITDCL"          , "is_charptr"      , 0 , _SHR_BITDCLSIZE(CINT_CONFIG_MAX_FPARAMS+1) },
    { "int"                 , "nargs"           , 0 , 0 },
    { "int"                 , "called_count"    , 0 , 0 },
    { "cint_logger_rtime_t" , "arg_ptrs"        , 0 , 0 },
    { "cint_logger_rtime_t" , "preload"         , 0 , 0 },
    { "cint_logger_rtime_t" , "call1"           , 0 , 0 },
    { "cint_logger_rtime_t" , "dispatch"        , 0 , 0 },
    { "cint_logger_rtime_t" , "call2"           , 0 , 0 },
    { "cint_logger_rtime_t" , "total"           , 0 , 0 },
    { NULL                  , NULL              , 0 , 0 },
};

static void*
__cint_maddr__cint_logger_api_params_t (void* p, int mnum, cint_struct_type_t* parent)
{
    void *rv;
    cint_logger_api_params_t *s = p;

    switch (mnum) {
    case  0: rv = &(s->next);            break;
    case  1: rv = &(s->prev);            break;
    case  2: rv = &(s->fn);              break;
    case  3: rv = &(s->params);          break;
    case  4: rv = &(s->basetype_size);   break;
    case  5: rv = &(s->is_basetype_arr); break;
    case  6: rv = &(s->is_charptr);      break;
    case  7: rv = &(s->nargs);           break;
    case  8: rv = &(s->called_count);    break;
    case  9: rv = &(s->arg_ptrs);        break;
    case 10: rv = &(s->preload);         break;
    case 11: rv = &(s->call1);           break;
    case 12: rv = &(s->dispatch);        break;
    case 13: rv = &(s->call2);           break;
    case 14: rv = &(s->total);           break;
    default: rv = NULL;                  break;
    }

    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cint_logger_call_ctxt_t[] = {
    { "char"         , "api"             , 1 , 0 },
    { "unsigned int" , "flags"           , 0 , 0 },
    { "void"         , "caller_tid"      , 1 , 0 },
    { "char"         , "caller_tname"    , 1 , 0 },
    { "char"         , "caller_file"     , 1 , 0 },
    { "int"          , "caller_line"     , 0 , 0 },
    { "char"         , "caller_function" , 1 , 0 },
    { "int"          , "skip_dispatch"   , 1 , 0 },
    { "int"          , "call_id"         , 0 , 0 },
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

static cint_data_t _cint_logger_cint_data = {
    NULL,
    NULL,
    __cint_logger_structures,
    __cint_logger_enums,
    NULL,
    NULL,
    NULL,
};

static void
_cint_logger_cint_init ()
{
    cint_datatype_t dt;

    cint_interpreter_add_data(&_cint_logger_cint_data, NULL);

    vcint_logger_cfg = NULL;

    if (cint_datatype_find("cint_logger_global_cfg_data_t", &dt) == CINT_E_NONE) {
        cint_variable_create(&vcint_logger_cfg, "cint_logger_cfg", &dt.desc,
                             CINT_VARIABLE_F_SDATA|CINT_VARIABLE_F_NODESTROY,
                             &cint_logger_cfg);
    }
}

static int
_cint_logger_cint_interp_listener (void *cookie, cint_interpreter_event_t event)
{
    switch (event) {
    case cintEventReset:
        _cint_logger_cint_init();
        break;
    default:
        break;
    }
    return CINT_E_NONE;
}

/***********************************************************************************/
/*                                                                                 */
/* BSL sink to log generated output to a separate file. Also the point where post  */
/* processing - e.g. conversion to C - can be chained into the processing flow.    */
/*                                                                                 */
/***********************************************************************************/

static int
_cint_logger_bsl_vfprintf (void *f, const char *format, va_list args)
{
    void *file_fp = cint_logger_cfg.logfile_fp;
    int enabled   = cint_logger_cfg.log_to_file;
    int retv = 0;

    if (!enabled || !file_fp) {
        return 0;
    }

    retv = CINT_VFPRINTF((FILE*) file_fp, format, args);

    CINT_FFLUSH(file_fp);

    return retv;
}

static int
_cint_logger_bsl_check (bsl_meta_t *meta)
{
    cint_logger_thread_data_t *tdata;

    return (((meta->layer  == cint_logger_cfg.logLayer) &&
             (meta->source == cint_logger_cfg.logSource) &&
             (meta->severity == bslSeverityVerbose)) ||
            ((tdata = cint_logger_thread_specific()) &&
             (*tdata).logger_is_active));
}


static int
_cint_logger_bsl_cleanup (bslsink_sink_t *sink)
{
    if (cint_logger_cfg.logfile_fp) {
        CINT_FCLOSE(cint_logger_cfg.logfile_fp);
        cint_logger_cfg.logfile_fp = NULL;
    }
    return 0;
}

static int
_cint_logger_bsl_init ()
{
    bslsink_sink_t *sink;

    sink = &_cint_logger_bslsink;
    bslsink_sink_t_init(sink);
    CINT_STRNCPY(sink->name, "CintApiLoggerSink", sizeof(sink->name));
    sink->vfprintf = _cint_logger_bsl_vfprintf;
    sink->check = _cint_logger_bsl_check;
    sink->cleanup = _cint_logger_bsl_cleanup;

    sink->enable_range.min = bslSeverityOff+1;
    sink->enable_range.max = bslSeverityCount-1;

    sink->prefix_range.min = bslSeverityOff+1;
    sink->prefix_range.max = bslSeverityWarn;

    bslsink_sink_add(sink);

    return 0;
}

/***********************************************************************************/
/*                                                                                 */
/* Main CINT logging functionality follows                                         */
/*                                                                                 */
/***********************************************************************************/

static void
_cint_logger_get_array_count (cint_logger_api_params_t *ctxt, void *arg_ptrs[], int array_cnt_indx, int *array_cnt)
{
    cint_parameter_desc_t *params, *ppdt;
    int nargs, array_cnt_size;
    void *array_cnt_ptr;

    params = ctxt->params;
    nargs = ctxt->nargs;

    if (array_cnt_indx >= nargs) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("FATAL ERROR -- array_cnt_indx is out of bounds (%d >= %d)\n"), array_cnt_indx, nargs));
        return;
    }

    ppdt = &params[array_cnt_indx];
    if (!(ppdt->flags & CINT_PARAM_VL)) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("FATAL ERROR -- pointed parameter is not array length (%s %s)\n"), ppdt->basetype, ppdt->name));
        return;
    }

    array_cnt_size = ctxt->basetype_size[array_cnt_indx];

    array_cnt_ptr  = arg_ptrs[array_cnt_indx];

    /* expect array_cnt to be an integer-like variable */
    switch (array_cnt_size) {
    case 1 : *array_cnt = *(unsigned char*)      array_cnt_ptr; break;
    case 2 : *array_cnt = *(unsigned short*)     array_cnt_ptr; break;
    case 4 : *array_cnt = *(unsigned long*)      array_cnt_ptr; break;
    case 8 : *array_cnt = *(unsigned long long*) array_cnt_ptr; break;
    default: break;
    }
}

static void
_cint_logger_copy_function_arguments (cint_logger_thread_data_t *tdata,
                                      cint_logger_call_ctxt_t *call_ctxt,
                                      cint_logger_api_params_t *ctxt,
                                      void *arg_ptrs[],
                                      void *buffer,
                                      int *bufsz)

{
    cint_logger_call_ctxt_t *scall_ctxt;
    cint_parameter_desc_t *params;
    cint_parameter_desc_t pds[CINT_CONFIG_MAX_FPARAMS+1];
    int array_size_save[CINT_CONFIG_MAX_FPARAMS+1];
    void *arg_ptrs_save[CINT_CONFIG_MAX_FPARAMS+1];
    int i, j, nargs, req, array_cnt_indx, array_cnt, copy_size, done, len;
    char *dst;
    void *ptr;

    params = ctxt->params;
    nargs = ctxt->nargs;

    for (i = 0; params[i].name && (i < nargs); i++) {
        pds[i] = params[i];
        arg_ptrs_save[i] = arg_ptrs[i];
        if (!pds[i].pcount && SHR_BITGET(ctxt->is_basetype_arr, i) && arg_ptrs_save[i]) {
            arg_ptrs_save[i] = *(void**)arg_ptrs_save[i];
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
    req += sizeof(*scall_ctxt) +
           ((call_ctxt->api) ? CINT_STRLEN(call_ctxt->api) + 1 : 0) +
           ((call_ctxt->caller_file) ? CINT_STRLEN(call_ctxt->caller_file) + 1 : 0) +
           ((call_ctxt->caller_function) ? CINT_STRLEN(call_ctxt->caller_function) + 1 : 0);

    for (i = 0; (i < nargs) && pds[i].name; i++) {

        req += pds[i].pcount * sizeof(void*);

        array_cnt = 1;

        if (SHR_BITGET(ctxt->is_charptr, i)) {

            ptr = *(void**) arg_ptrs[i];
            for (j = pds[i].pcount; j > 1; j--) {
                ptr = (ptr) ? *(void**)ptr : NULL;
            }

            array_cnt = (ptr) ? CINT_STRLEN((char*)ptr) + 1 : 0;

        } else if (pds[i].flags & CINT_PARAM_VP) {

            array_cnt_indx = pds[i].flags & CINT_PARAM_IDX;
            _cint_logger_get_array_count(ctxt, arg_ptrs, array_cnt_indx, &array_cnt);

        }

        array_size_save[i] = array_cnt * ctxt->basetype_size[i];
        req += array_size_save[i];
    }

    if (*bufsz < req) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- buffer is not large enough to copy all contents (%d vs %d required)\n"), *bufsz, req));
        return;
    }

    dst = buffer;

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

        for (i = 0; (i < nargs) && pds[i].name; i++) {

            if (pds[i].pcount < 0) {
                continue;
            }

            if (pds[i].pcount) {

                if (arg_ptrs_save[i]) {
                    *(void**)dst = *(void**)arg_ptrs_save[i];
                } else {
                    *(void**)dst = NULL;
                }

                arg_ptrs_save[i] = *(void**)dst;
                dst += sizeof(void*);

                if (SHR_BITGET(ctxt->is_charptr, i) && (pds[i].pcount == 1) &&
                    arg_ptrs_save[i] && (copy_size = array_size_save[i])) {

                    CINT_MEMCPY(dst, arg_ptrs_save[i], copy_size);
                    dst += copy_size;
                    arg_ptrs_save[i] = NULL;
                }

                pds[i].pcount--;
                done = 0;
                continue;
            }

            if (arg_ptrs_save[i] && (copy_size = array_size_save[i])) {

                CINT_MEMCPY(dst, arg_ptrs_save[i], copy_size);
                dst += copy_size;
                arg_ptrs_save[i] = NULL;
            }

            pds[i].pcount--;
            done = 0;
        }

    } while (!done);

    copy_size = dst - (char*)buffer;
    if ((*bufsz < copy_size) || (copy_size != req)) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- buffer may have overflowed (%d vs %d copied), or copied size different from computed buffer size\n"), *bufsz, copy_size));
    }

    *bufsz = copy_size;
}

static void
_cint_logger_log_variable (cint_parameter_desc_t *ppdt, const char *pfx, void *src, int indent)
{
    cint_variable_t *v = NULL;
    char *name;
    int buflen, rc;

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
                  (BSL_META("Failed to allocate %s (%s) -- %d\n"), name, ppdt->basetype, rc));
        CINT_FREE(name);
        return;
    }

    cint_variable_print(v, indent, name);

    cint_variable_free(v);

    CINT_FREE(name);
}

static void
_indent (int cnt)
{
    while (cnt && cnt--) {
        CINT_PRINTF(" ");
    }
}

static void
_cint_rep_chr (char *str, char ch, int bufsz, int cnt)
{
    if (cnt > (bufsz - 1)) {
        cnt = bufsz - 1;
    }
    CINT_MEMSET(str, ch, cnt);
    str[cnt] = '\0';
}

static void
_cint_logger_log_arguments_buffer (cint_logger_call_ctxt_t *call_ctxt,
                                   cint_logger_api_params_t *ctxt,
                                   void *buffer, int bufsz)
{
    cint_logger_call_ctxt_t *scall_ctxt;
    cint_parameter_desc_t *params, *ppdt;
    cint_parameter_desc_t pds[CINT_CONFIG_MAX_FPARAMS+1];
    cint_parameter_desc_t voidptr_pds = { "void", NULL, 1, 0 };
    void *arg_ptrs_save[CINT_CONFIG_MAX_FPARAMS+1];
    int i, nargs, array_cnt_indx, array_cnt, dim, first, done, src_offset;
    unsigned int flags;
    int indent;
    /*
     * variables to hold '_' prefixes for auto variables created due to pointer
     * dereference. we don't expect this to be more than 3. Use 10.
     */
    int max_pfx_len = 20;
    char pfx[max_pfx_len], pfx2[max_pfx_len];
    cint_parameter_desc_t call_ctxt_pds = { "cint_logger_call_ctxt_t", "this", 0, 0 };
    char *src, *src_save, *str;

    indent = cint_logger_cfg.base_indent;
    params = ctxt->params;
    nargs = ctxt->nargs;
    flags = call_ctxt->flags;

    src = buffer;

    scall_ctxt = (void*) src;
    src += sizeof(*scall_ctxt) +
           ((scall_ctxt->api) ? CINT_STRLEN(scall_ctxt->api) + 1 : 0) +
           ((scall_ctxt->caller_file) ? CINT_STRLEN(scall_ctxt->caller_file) + 1 : 0) +
           ((scall_ctxt->caller_function) ? CINT_STRLEN(scall_ctxt->caller_function) + 1 : 0);

    src_save = src;

    for (i = 0; params[i].name && (i < nargs); i++) {
        pds[i] = params[i];
        arg_ptrs_save[i] = src;
        if (params[i].pcount) {
            src += sizeof(void*);
            if (SHR_BITGET(ctxt->is_charptr, i) && (params[i].pcount == 1)) {
                src += CINT_STRLEN(src) + 1;
            }
        } else {
            src += ctxt->basetype_size[i];
        }
    }

    src = src_save;

    if (flags & CINT_PARAM_IN) {
        _indent(indent);
        CINT_PRINTF("{\n");
    } else if (flags & CINT_PARAM_OUT) {
        _indent(indent);
        CINT_PRINTF("if (0) {\n");
    }

    indent += 4;

    first = 1;

    do {

        done = 1;

        for (i = 0; (i < nargs) && pds[i].name; i++) {

            if (pds[i].pcount < 0) {
                continue;
            }

            if ((i == 0) & !(flags & CINT_PARAM_OUT)) {
                src += ctxt->basetype_size[i];
                pds[i].pcount--;
                continue;
            }

            if ((pds[i].pcount == 0) && !CINT_STRCMP(pds[i].basetype, "void")) {
                continue;
            }

            _cint_rep_chr(pfx, '_', max_pfx_len, params[i].pcount - pds[i].pcount);

            array_cnt = 1;

            if (SHR_BITGET(ctxt->is_charptr, i) && (pds[i].pcount <= 1)) {

                if (pds[i].pcount == 1) {

                    voidptr_pds.name = pds[i].name;

                    CINT_SNPRINTF(pfx2, max_pfx_len, "orig_%s", pfx);

                    _cint_logger_log_variable(&voidptr_pds, pfx2, src, indent);

                    voidptr_pds.name = NULL;

                    str = src + sizeof(void*);

                    _cint_logger_log_variable(&pds[i], pfx, &str, indent);

                    src += sizeof(void*) + CINT_STRLEN(str) + 1; /* sizeof void* is accounted below */

                }

                pds[i].pcount--;
                done = 0;
                continue;

            } else if (!first && (pds[i].flags & CINT_PARAM_VP)) {

                array_cnt_indx = pds[i].flags & CINT_PARAM_IDX;

                _cint_logger_get_array_count(ctxt, arg_ptrs_save, array_cnt_indx, &array_cnt);

                for (dim = CINT_CONFIG_ARRAY_DIMENSION_LIMIT - 1; dim > 0; dim--) {
                    pds[i].dimensions[dim] = pds[i].dimensions[dim-1];
                }
                pds[i].dimensions[0] = array_cnt;
                pds[i].num_dimensions++;

            }

            _cint_logger_log_variable(&pds[i], pfx, src, indent);

            if (pds[i].pcount) {
                src += sizeof(void*);
            } else {
                src += array_cnt * ctxt->basetype_size[i];
            }

            pds[i].pcount--;
            done = 0;
            continue;
        }

        first = 0;

    } while (!done);

    _cint_logger_log_variable(&call_ctxt_pds, "", scall_ctxt, indent);

    src_offset = (int)(src - (char*)buffer);
    if (src_offset != bufsz) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("Logger did not align to end of buffer (%s) -- offset %d vs expected %d\n"),
                   call_ctxt->api, src_offset, bufsz));
    }

    for (i = 0; (i < nargs) && params[i].name; i++) {

        if (!params[i].pcount) {
            continue;
        }

        pds[i] = params[i];

        for (pds[i].pcount--, first = 1; pds[i].pcount >= 0; pds[i].pcount--, first = 0) {

            if ((SHR_BITGET(ctxt->is_charptr, i) || !CINT_STRCMP(pds[i].basetype, "void")) && first) {
                continue;
            }

            _cint_rep_chr(pfx, '_', max_pfx_len, pds[i].pcount+1);
            _cint_rep_chr(pfx2, '_', max_pfx_len, pds[i].pcount);

            _indent(indent);
            CINT_PRINTF("%s%s = &%s%s;\n", pfx2, pds[i].name, pfx, pds[i].name);
        }
    }

    CINT_PRINTF("\n");

    _indent(indent);
    CINT_PRINTF("%s(", ctxt->fn);

    ppdt = params;
    for (ppdt++; ppdt->name; ppdt++) {
        CINT_PRINTF("%s%s", (first++ > 0) ? ", " : "", ppdt->name);
    }

    CINT_PRINTF(");\n");

    indent -= 4;
    _indent(indent);
    CINT_PRINTF("}\n");
}

static void
_cint_logger_inject_variables (cint_logger_thread_data_t *tdata,
                               cint_logger_call_ctxt_t *call_ctxt,
                               cint_logger_call_ctxt_t *orig_call_ctxt,
                               int *orig_r,
                               cint_logger_api_params_t *ctxt,
                               void *arg_ptrs[])
{
    int i, rc;
    cint_variable_t *v[CINT_CONFIG_MAX_FPARAMS+1];
    cint_variable_t *vcall_ctxt = NULL, *vorig_call_ctxt = NULL, *vorig_r = NULL;
    cint_parameter_desc_t call_ctxt_pds = { "cint_logger_call_ctxt_t", NULL, 0, 0 };
    cint_parameter_desc_t *ppdt, *params;
    void *src;


    params = ctxt->params;

    /*
     * Create variables for all API arguments by name.
     */

    CINT_MEMSET(v, 0, (CINT_CONFIG_MAX_FPARAMS + 1) * sizeof(void*));

    for (i=0, src = arg_ptrs[i], ppdt = &params[i];
         ppdt->name;
         i++, src = arg_ptrs[i], ppdt = &params[i]) {

        if (!src) {
            continue;
        }

        rc = cint_variable_create(&v[i], ppdt->name, ppdt, CINT_VARIABLE_F_SDATA, src);
        if ((rc != CINT_E_NONE) || !v[i]) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("Failed to allocate %s (%s) -- %d\n"), ppdt->name, ppdt->basetype, rc));
            return;
        }
    }

    /*
     * create new variable for the call_ctxt
     */
    rc = cint_variable_create(&vcall_ctxt,
                              "this",
                              &call_ctxt_pds,
                              CINT_VARIABLE_F_SDATA,
                              call_ctxt);
    if ((rc != CINT_E_NONE) || !vcall_ctxt) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("Failed to allocate %s (%s) -- %d\n"), call_ctxt_pds.name, call_ctxt_pds.basetype, rc));
    }

    if (cint_logger_cfg.mode == cintLoggerModeReplay) {

        if (orig_call_ctxt) {

            rc = cint_variable_create(&vorig_call_ctxt,
                                      "orig",
                                      &call_ctxt_pds,
                                      CINT_VARIABLE_F_SDATA,
                                      orig_call_ctxt);
            if ((rc != CINT_E_NONE) || !vorig_call_ctxt) {
                LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                          (BSL_META("Failed to allocate %s (%s) -- %d\n"), call_ctxt_pds.name, call_ctxt_pds.basetype, rc));
            }
        }

        if (orig_r) {

            rc = cint_variable_create(&vorig_r,
                                      "orig_r",
                                      &params[0],
                                      CINT_VARIABLE_F_SDATA,
                                      orig_r);
            if ((rc != CINT_E_NONE) || !vorig_r) {
                LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                          (BSL_META("Failed to allocate %s (%s) -- %d\n"), call_ctxt_pds.name, call_ctxt_pds.basetype, rc));
            }
        }
    }
}

static int
_cint_logger_run_filter_function (cint_logger_thread_data_t *tdata,
                                  cint_logger_call_ctxt_t *call_ctxt,
                                  cint_logger_api_params_t *ctxt,
                                  void *arg_ptrs[])
{
    cint_ast_t *i_ast, *f_ast;
    cint_variable_t *rv;
    cint_atomic_type_t *ap;
    const char *cint_filter_fn = cint_logger_cfg.cint_filter_fn;
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

static void
_cint_logger_log_function_arguments (cint_logger_thread_data_t *tdata,
                                     cint_logger_call_ctxt_t *call_ctxt,
                                     cint_logger_api_params_t *ctxt,
                                     void *arg_ptrs[])
{
    void *prev_scope = NULL;
    const char *fn;
    int filter;
    int recsz = 10240;
    char rec[recsz];
    cint_variable_t *vcall_ctxt = NULL, *vr = NULL;
    cint_logger_call_ctxt_t *orig_call_ctxt = NULL;
    int *orig_r = NULL;
    cint_parameter_desc_t call_ctxt_pds = { "cint_logger_call_ctxt_t", NULL, 0, 0 };
    cint_datatype_t dt;

    fn = ctxt->fn;

    if (cint_logger_cfg.user_filter_cb ||
        cint_logger_cfg.cint_filter_fn ||
        cint_logger_cfg.mode == cintLoggerModeReplay) {

        CINT_VARIABLES_LOCK;

       /*
        * when replaying configuration we want to use the call_ctxt declared from
        * original logs. 
        */
        if (cint_logger_cfg.mode == cintLoggerModeReplay) {

            vcall_ctxt = cint_variable_find("this", 1);
            if (!vcall_ctxt ||
                (cint_datatype_find(call_ctxt_pds.basetype, &dt) != CINT_E_NONE) || 
                (cint_type_check(&vcall_ctxt->dt, &dt) == 0)) {

                LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                          (BSL_META("Failed to find variable \"this\" of type \"cint_logger_call_ctxt_t\" in replay mode\n")));
            } else {
                orig_call_ctxt = vcall_ctxt->data;
            }

            /*
             * we expect to find a named return value variable if replaying a
             * post-dispatch call. If it is available, load its value into the
             * current rv so that it will be available to the filter function.
             */
            vr = cint_variable_find("r", 1);
            if (!vr ||
                (cint_datatype_find(ctxt->params[0].basetype, &dt) != CINT_E_NONE) || 
                (cint_type_check(&vr->dt, &dt) == 0)) {

                if (orig_call_ctxt && (orig_call_ctxt->flags & CINT_PARAM_OUT)) {
                    LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                              (BSL_META("Failed to find variable \"r\" of type \"%s\" in replay mode\n"), ctxt->params[0].basetype));
                }
            } else {
                orig_r = vr->data;
            }
        }

        /*
         * Switch temporarily to global scope and a new local scope under it.
         * Inject API parameters and call_ctxt as named variables. Then invoke
         * the filter function.
         */
        prev_scope = cint_variable_scope_global_set(fn);
        cint_variable_lscope_push(fn);

        _cint_logger_inject_variables(tdata, call_ctxt, orig_call_ctxt, orig_r, ctxt, arg_ptrs);

        filter = ((cint_logger_cfg.user_filter_cb &&   cint_logger_cfg.user_filter_cb(tdata, call_ctxt, ctxt, arg_ptrs)) ||
                  (cint_logger_cfg.cint_filter_fn && _cint_logger_run_filter_function(tdata, call_ctxt, ctxt, arg_ptrs)));

        cint_variable_lscope_pop(fn);
        cint_variable_scope_set(fn, prev_scope);

        CINT_VARIABLES_UNLOCK;

        if (filter) {
            return; 
        }

    }

    /*
     * When replaying CINT, log the original call_ctxt if it is available
     * instead of the current call_ctxt
     */
    if (orig_call_ctxt && (cint_logger_cfg.mode == cintLoggerModeReplay)) {
        orig_call_ctxt->flags = call_ctxt->flags;
        call_ctxt = orig_call_ctxt;
    }

    _cint_logger_copy_function_arguments(tdata, call_ctxt, ctxt, arg_ptrs, rec, &recsz);

    _cint_logger_log_arguments_buffer(call_ctxt, ctxt, rec, recsz);

}

static void
_cint_logger_cache_function_arguments (cint_logger_thread_data_t *tdata,
                                       cint_logger_call_ctxt_t *call_ctxt,
                                       cint_logger_api_params_t *ctxt,
                                       void *arg_ptrs[])
{
    int recsz = 10240;
    char rec[recsz];

    _cint_logger_copy_function_arguments(tdata, call_ctxt, ctxt, arg_ptrs, rec, &recsz);

    /*_cint_logger_log_arguments_buffer(call_ctxt, rec, recsz);*/
}

void
cint_log_function_arguments (const char *fn, void **api_params, unsigned int flags, int call_id, void *arg_ptrs[], int *skip_dispatch)
{
    cint_parameter_desc_t *params, *ppdt;
    cint_datatype_t dt;
    cint_function_t *func;
    cint_logger_thread_data_t *tdata;
    cint_logger_api_params_t *ctxt;
    int i, nargs, is_basetype_arr, is_charptr, is_basetype_void;
    cint_logger_call_ctxt_t call_ctxt;

    if (!fn || !arg_ptrs || !api_params) {

        return;
    }

    tdata = cint_logger_thread_specific();

    CINT_LOGGER_LOCK;

    (*tdata).logger_is_active = 1;

    if (!(ctxt = *api_params)) {
        ctxt = CINT_MALLOC(sizeof(*ctxt));
        if (!ctxt) {
            LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("FATAL ERROR -- malloc ctxt failed for function %s\n"), fn));
            goto exit;
        }
        CINT_LOGGER_INIT_API_PARAMS_T(ctxt);

        ctxt->fn = CINT_STRDUP(fn);
        if (cint_logger_cfg.ctxt_head) {
            cint_logger_cfg.ctxt_head->prev = ctxt;
        }
        ctxt->next = cint_logger_cfg.ctxt_head;
        cint_logger_cfg.ctxt_head = ctxt;

        *api_params = ctxt;
    }

    if (!cint_logger_cfg.enabled || !tdata || (*tdata).disabled) {

        goto exit;
    }

    cmd_cint_initialize();

    if (!(params = ctxt->params) &&
        (cint_datatype_find(fn, &dt) == CINT_E_NONE) &&
        (dt.flags & CINT_DATATYPE_F_FUNC)) {

        func = dt.basetype.p;
        params = func->params;
        if (!params) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("FATAL ERROR -- no arguments table for function %s\n"), fn));
            goto exit;
        }

        ctxt->params = params;
        nargs = ctxt->nargs = cint_parameter_count(params);
        if (nargs > (CINT_CONFIG_MAX_FPARAMS+1)) {
            LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("FATAL ERROR -- %s has more than max %d arguments (%d)\n"), fn, CINT_CONFIG_MAX_FPARAMS, nargs));
            goto exit;
        }

        for (i = 0, ppdt = params; (i < nargs) && ppdt->name; i++, ppdt++) {

            CINT_MEMSET(&dt, 0, sizeof(dt));
            if (cint_datatype_find(ppdt->basetype, &dt) != CINT_E_NONE) {
                LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                          (BSL_META("FATAL ERROR -- %s param %d has unknown basetype %s\n"), fn, i, ppdt->basetype));
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

    if (!params) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("FATAL ERROR -- no arguments table for function %s\n"), fn));
        goto exit;
    }

    nargs = ctxt->nargs;
    if (nargs > (CINT_CONFIG_MAX_FPARAMS+1)) {
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("FATAL ERROR -- %s has more than max %d arguments (%d)\n"), fn, CINT_CONFIG_MAX_FPARAMS, nargs));
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

    switch (cint_logger_cfg.mode) {
    case cintLoggerModeLog:
    case cintLoggerModeReplay:
        _cint_logger_log_function_arguments(tdata, &call_ctxt, ctxt, arg_ptrs);
        break;
    case cintLoggerModeCache:
        _cint_logger_cache_function_arguments(tdata, &call_ctxt, ctxt, arg_ptrs);
        break;
    default:
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("ERROR -- invalid operating mode %d\n"), cint_logger_cfg.mode));
        break;
    }

exit:

    (*tdata).logger_is_active = 0;

    CINT_LOGGER_UNLOCK;

}

/***********************************************************************************/
/*                                                                                 */
/* Handler for the CintApiLogging shell command                                    */
/*                                                                                 */
/***********************************************************************************/

char cint_logger_usage[] = "\
CintApiLogging          \n\
    ENable=[0/1]        \n\
    Mode=[1/2/3]        \n\
    Source=<number>     \n\
    Layer=<number>      \n\
    FunctioN=<string>   \n\
    File=<string>       \n\
    BaseIndent=<number> \n\
";

cmd_result_t
cint_logger_cmd (int unit, args_t *a)
{
    cint_logger_global_cfg_data_t new_cfg;
    parse_table_t pt;
    cint_parameter_desc_t cfg_pds = { "cint_logger_global_cfg_data_t", "new_cfg", 0, 0 };
    char *func = NULL, *file = NULL;

    cmd_cint_initialize();

    if (!ARG_CNT(a)) {
        cli_out("\nCurrent configuration:\n\n");
        cint_variable_print(vcint_logger_cfg, 4, NULL);
        return CMD_OK;
    }

    new_cfg = cint_logger_cfg;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Enable"     , PQ_DFL | PQ_INT , NULL                    , &new_cfg.enabled        , NULL);
    parse_table_add(&pt, "Mode"       , PQ_DFL | PQ_INT , NULL                    , &new_cfg.mode           , NULL);
    parse_table_add(&pt, "Source"     , PQ_DFL | PQ_INT , NULL                    , &new_cfg.logSource      , NULL);
    parse_table_add(&pt, "Layer"      , PQ_DFL | PQ_INT , NULL                    , &new_cfg.logLayer       , NULL);
    parse_table_add(&pt, "FunctioN"   , PQ_STRING       , new_cfg.cint_filter_fn  , &func                   , NULL);
    parse_table_add(&pt, "File"       , PQ_STRING       , new_cfg.logfile         , &file                   , NULL);
    parse_table_add(&pt, "BaseIndent" , PQ_DFL | PQ_INT , NULL                    , &new_cfg.base_indent    , NULL);

    if (parse_arg_eq(a, &pt) <= 0) {
        cli_out("Error: invalid option \"%s\"\n", ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    parse_eq_format(&pt);

    /*
     * Curate the cint_filter_fn and logfile arguments passed by the user.
     * Option must be set if it is not the empty string, not the case
     * insensitive string "null", or same as the existing value. Else it must
     * be set to NULL.
     */
    if (func && CINT_STRNCMP(func, "", 100) && CINT_STRNCASECMP(func, "NULL", 100)) {

        if (new_cfg.cint_filter_fn && CINT_STRNCMP(func, new_cfg.cint_filter_fn, 100)) {
            CINT_FREE(new_cfg.cint_filter_fn);
        }
        new_cfg.cint_filter_fn = CINT_STRDUP(func);
    } else {

        if (new_cfg.cint_filter_fn) {
            CINT_FREE(new_cfg.cint_filter_fn);
        }
        new_cfg.cint_filter_fn = NULL;
    }

    if (file && CINT_STRNCMP(file, "", 100) && CINT_STRNCASECMP(file, "NULL", 100)) {

        if (new_cfg.logfile && CINT_STRNCMP(file, new_cfg.logfile, 100)) {
            CINT_FREE(new_cfg.logfile);
        }
        new_cfg.logfile = CINT_STRDUP(file);
    } else {

        if (new_cfg.logfile) {
            CINT_FREE(new_cfg.logfile);
        }
        new_cfg.logfile = NULL;
    }

    parse_arg_eq_done(&pt);

    cli_out("\nDesired configuration: (actual may differ)\n\n");
    _cint_logger_log_variable(&cfg_pds, "", &new_cfg, 4);

    /*
     * validate all the options before applying them
     */
    if ((new_cfg.enabled != 0) && (new_cfg.enabled != 1)) {

        cli_out("Error: Enable %d is invalid\n", new_cfg.enabled);
        return CMD_USAGE;
    }

    if ((new_cfg.mode != cintLoggerModeLog) && 
        (new_cfg.mode != cintLoggerModeReplay) &&
        (new_cfg.mode != cintLoggerModeCache)) {

        cli_out("Error: Mode %d is invalid\n", new_cfg.mode);
        return CMD_USAGE;
    }

    if ((new_cfg.logLayer < 0) || (new_cfg.logLayer >= bslLayerCount)) {

        cli_out("Error: logLayer %d is invalid\n", new_cfg.logLayer);
        return CMD_USAGE;
    }

    if ((new_cfg.logSource < 0) || (new_cfg.logSource >= bslSourceCount)) {

        cli_out("Error: logSource %d is invalid\n", new_cfg.logSource);
        return CMD_USAGE;
    }

    CINT_LOGGER_LOCK;

    if (new_cfg.enabled) {

        /*
         * if a file is open, close it if either there's no name assigned for
         * it, or a new file name has been provided by the user
         */
        if (cint_logger_cfg.logfile_fp &&
            (!cint_logger_cfg.logfile ||
             (new_cfg.logfile && CINT_STRNCMP(cint_logger_cfg.logfile, new_cfg.logfile, 1024)))) {

            CINT_FCLOSE(cint_logger_cfg.logfile_fp);
            cint_logger_cfg.logfile_fp = new_cfg.logfile_fp = NULL;
        }

        if (!cint_logger_cfg.logfile_fp && new_cfg.logfile) {

            new_cfg.logfile_fp = cint_logger_cfg.logfile_fp = CINT_FOPEN(new_cfg.logfile, "a");
            if (new_cfg.logfile_fp) {
                new_cfg.log_to_file = 1;
            }
        }

        /*
         * Logging requires verbose severity at logLayer and Source
         */
        bslenable_set(cint_logger_cfg.logLayer, cint_logger_cfg.logSource, bslSeverityVerbose);
        cli_out("Debug %s %s %s - was configured\n",
                bsl_layer2str(cint_logger_cfg.logLayer),
                bsl_source2str(cint_logger_cfg.logSource),
                bsl_severity2str(bslSeverityVerbose));

    } else {

        if (cint_logger_cfg.logfile_fp) {

            CINT_FCLOSE(cint_logger_cfg.logfile_fp);
            cint_logger_cfg.logfile_fp = new_cfg.logfile_fp = NULL;
        }

        new_cfg.log_to_file = 0;
    }

    cint_logger_cfg.enabled        = new_cfg.enabled;
    cint_logger_cfg.mode           = new_cfg.mode;
    cint_logger_cfg.logLayer       = new_cfg.logLayer;
    cint_logger_cfg.logSource      = new_cfg.logSource;
    cint_logger_cfg.cint_filter_fn = new_cfg.cint_filter_fn;
    cint_logger_cfg.logfile        = new_cfg.logfile;
    cint_logger_cfg.logfile_fp     = new_cfg.logfile_fp;
    cint_logger_cfg.log_to_file    = new_cfg.log_to_file;

    CINT_LOGGER_UNLOCK;

    return CMD_OK;
}

static void
_cint_logger_tls_destructor (void *tdata)
{
    CINT_FREE(tdata);
}

static void
_cint_logger_init ()
{

    if (!cint_logger_cfg.mutex) {

        SAL_GLOBAL_LOCK;

        if (!cint_logger_cfg.mutex) {

            cint_logger_cfg.mutex = sal_mutex_create("cint_logger_mutex");
            if (!cint_logger_cfg.mutex) {
                LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                          (BSL_META("FATAL ERROR -- failed to allocate cint_logger_mutex\n")));
            }
        }

        SAL_GLOBAL_UNLOCK;
    }

    /*
     * Since there is no pthread_once() equivalent SAL API use the Global mutex
     */
    if (!cint_logger_cfg.tls_key) {

        SAL_GLOBAL_LOCK;

        if (!cint_logger_cfg.tls_key) {

            cint_logger_cfg.tls_key = sal_tls_key_create(_cint_logger_tls_destructor);
            if (!cint_logger_cfg.tls_key) {
                LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                          (BSL_META("FATAL ERROR -- failed to allocate cint_logger tls_key\n")));
            }
        }

        SAL_GLOBAL_UNLOCK;
    }

}

/***********************************************************************************/
/*                                                                                 */
/* Call all the inits                                                              */
/*                                                                                 */
/***********************************************************************************/

void
cint_logger_cint_init ()
{
    static int cint_logger_cint_inited = 0;

    if (cint_logger_cint_inited) {
        return;
    }

    _cint_logger_bsl_init();

    _cint_logger_cint_init();
    cint_interpreter_event_register(_cint_logger_cint_interp_listener, NULL);

    _cint_logger_init();

    cint_logger_cint_inited = 1;
}

void
cint_logger_set_caller_context (const char *f, int l, const char *F)
{
    cint_logger_thread_data_t *tdata;

    tdata = cint_logger_thread_specific();

    (*tdata).caller_file = f;
    (*tdata).caller_line = l;
    (*tdata).caller_func = F;
}

void*
cint_logger_thread_specific ()
{
    cint_logger_thread_data_t *tdata;

    _cint_logger_init();

    tdata = sal_tls_key_get(cint_logger_cfg.tls_key);
    if (!tdata) {
        tdata = CINT_MALLOC(sizeof(*tdata));
        if (!tdata) {
            LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("FATAL ERROR -- malloc tdata failed\n")));
            return NULL;
        }

        CINT_MEMSET(tdata, 0, sizeof(*tdata));
        sal_tls_key_set(cint_logger_cfg.tls_key, tdata);
    }

    return tdata;
}


#else
typedef int cint_logger_c_not_empty; /* Make ISO compilers happy. */
#endif
