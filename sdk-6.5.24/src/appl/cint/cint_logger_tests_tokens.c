/*
 * $Id: cint_logger_test_tokens.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger_test_tokens.c
 * Purpose:     CINT logger Self Test Routines
 */

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

#include <assert.h>
#include <shared/bsl.h>
#include <shared/bslnames.h>
#include <sal/types.h>
#include <sal/core/libc.h>

#include "cint_porting.h"
#include "cint_datatypes.h"
#include "cint_logger.h"
#include "cint_logger_tests_tokens.h"
#include "cint_logger_replay.h"

#define TOKEN_HANDLER(tok) \
_cint_logger_tests_handle_tok_ ## tok

#define CINT_LOGGER_TESTS_TOKEN(tok)                     \
static void                                              \
TOKEN_HANDLER(tok)(                                      \
        cint_logger_api_params_t *ctxt,                  \
        uint8 *input_values[CINT_CONFIG_MAX_FPARAMS+1],  \
        int i_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],   \
        uint8 *output_values[CINT_CONFIG_MAX_FPARAMS+1], \
        int o_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],   \
        const char templ[],                              \
        cint_logger_tests_token_t *token,                \
        char *obuf,                                      \
        int  *obufsz);

#include "cint_logger_tests_tokens.i"
#undef CINT_LOGGER_TESTS_TOKEN

#define CINT_LOGGER_TESTS_TOKEN(tok) TOKEN_HANDLER(tok),

static
cint_logger_tests_token_handler_t token_handlers[] = {
#include "cint_logger_tests_tokens.i"
};
#undef CINT_LOGGER_TESTS_TOKEN

static
int token_sizes[CINT_LOGGER_TESTS_TOKEN_MAX+1];

static int test_item = 0;
static cint_logger_mode_e test_mode = cintLoggerModeCache;

void
cint_logger_tests_tokens_init (void)
{
#define CINT_LOGGER_TESTS_TOKEN(tok) \
    token_sizes[TOKEN_TYPE(tok)] = CINT_STRLEN(#tok) + 2;

#include "cint_logger_tests_tokens.i"
#undef CINT_LOGGER_TESTS_TOKEN
}


#define CINT_LOGGER_TESTS_TOKEN_HANDLER(tok)             \
static void                                              \
TOKEN_HANDLER(tok)(                                      \
        cint_logger_api_params_t *ctxt,                  \
        uint8 *input_values[CINT_CONFIG_MAX_FPARAMS+1],  \
        int i_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],   \
        uint8 *output_values[CINT_CONFIG_MAX_FPARAMS+1], \
        int o_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],   \
        const char templ[],                              \
        cint_logger_tests_token_t *token,                \
        char *obuf,                                      \
        int *obufsz                                      \
        /*                                               \
         * Provide a way for a token handler to have     \
         * extra arguments. Where additional arguments   \
         * are required, DECL_EXTRA_ARGS may be #define  \
         * to a , -prefixed and separated list of extra  \
         * arguments with their types. To be defined to  \
         * nothing when extra arguments are not needed.  \
         */                                              \
        DECL_EXTRA_ARGS                                  \
        )

#define DECL_EXTRA_ARGS

#define FIXED_ARGS                     \
        ctxt,                          \
        input_values, i_vl_array_cnt,  \
        output_values, o_vl_array_cnt, \
        templ,                         \
        token,                         \
        obuf,                          \
        obufsz

#define FIXED_ARGS_NESTED              \
        ctxt,                          \
        input_values, i_vl_array_cnt,  \
        output_values, o_vl_array_cnt, \
        templ,                         \
        token,                         \
        tmp_obuf,                      \
        &tmp_obufsz


#define DECL_RUNNERS    \
    char *rdst;         \
    int rbufsz, rtotsz;

#define INIT_RUNNERS                                                          \
    /*                                                                        \
     * obufsz must always be provided. When obuf is NULL, handlers compute    \
     * size of the output and return it in *obufsz. When obuf is not NULL,    \
     * *obufsz provides its size and at end of each handler size of output    \
     * generated is compared against it (should be same, see OSZ_CHECK)       \
     */                                                                       \
    if (!obufsz) {                                                            \
        LOG_FATAL(BSL_LS_APPL_CINTAPILOGGER,                                  \
                  (BSL_META("FATAL: %s: obufsz must always be provided.\n"),  \
                   __func__));                                                \
        assert(obufsz != NULL);                                               \
        return;                                                               \
    }                                                                         \
    rdst = obuf;                                                              \
    rbufsz = *obufsz;                                                         \
    rtotsz = 0;

#define RUNNERS \
    &rdst, &rbufsz, &rtotsz

#define OSZ_CHECK(tok)                                                                               \
                                                                                                     \
    if (rtotsz != *obufsz) {                                                                         \
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,                                                         \
                  (BSL_META("ERROR: %s: size of output different from computed size (%d vs %d)\n"),  \
                   #tok, rtotsz, *obufsz));                                                          \
    }                                                                                                \
                                                                                                     \
    if (rbufsz != 0) {                                                                               \
        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,                                                         \
                  (BSL_META("ERROR: %s: running buffer size did not reduce to ZERO (%d orig %d)\n"), \
                   #tok, rbufsz, *obufsz));                                                          \
    }                                                                                                \
                                                                                                     \

#define OSZ_NESTED \
    *obufsz = rtotsz;


#define TOKEN_OFS  (token->offset)
#define TOKEN_LEN  (token_sizes[token->type])

#define PREFIX_OFS (token->line_offset)
#define PREFIX_LEN (TOKEN_OFS - PREFIX_OFS)

#define SUFFIX_OFS (TOKEN_OFS + TOKEN_LEN)
#define SUFFIX_LEN (token->line_offset + token->line_length - SUFFIX_OFS)

#define COPY_PREFIX                                         \
                                                            \
    if (rdst) {                                             \
                                                            \
        CINT_MEMCPY(rdst, &templ[PREFIX_OFS], PREFIX_LEN);  \
        rdst += PREFIX_LEN;                                 \
    }                                                       \
                                                            \
    rbufsz -= ((rbufsz) ? PREFIX_LEN : 0);                  \
    rtotsz += PREFIX_LEN;

#define COPY_TOKEN                                          \
                                                            \
    if (rdst) {                                             \
                                                            \
        CINT_MEMCPY(rdst, &templ[TOKEN_OFS], TOKEN_LEN);    \
        rdst += TOKEN_LEN;                                  \
    }                                                       \
                                                            \
    rbufsz -= ((rbufsz) ? TOKEN_LEN : 0);                   \
    rtotsz += TOKEN_LEN;

#define COPY_SUFFIX                                         \
                                                            \
    if (rdst) {                                             \
                                                            \
        CINT_MEMCPY(rdst, &templ[SUFFIX_OFS], SUFFIX_LEN);  \
        rdst += SUFFIX_LEN;                                 \
    }                                                       \
                                                            \
    rbufsz -= ((rbufsz) ? SUFFIX_LEN : 0);                  \
    rtotsz += SUFFIX_LEN;

#define COPY_TOKEN                                          \
                                                            \
    if (rdst) {                                             \
                                                            \
        CINT_MEMCPY(rdst, &templ[TOKEN_OFS], TOKEN_LEN);    \
        rdst += TOKEN_LEN;                                  \
    }                                                       \
                                                            \
    rbufsz -= ((rbufsz) ? TOKEN_LEN : 0);                   \
    rtotsz += TOKEN_LEN;


#define NULL_TOKEN(tok) \
                        \
    DECL_RUNNERS;       \
                        \
    INIT_RUNNERS;       \
                        \
    COPY_PREFIX;        \
                        \
    COPY_SUFFIX;        \
                        \
    RETURN_TOTSZ;       \
                        \
    NULL_TERMINATION;   \
                        \
    OSZ_CHECK(tok);

#define NOP_TOKEN(tok)  \
                        \
    DECL_RUNNERS;       \
                        \
    INIT_RUNNERS;       \
                        \
    COPY_PREFIX;        \
                        \
    COPY_TOKEN;         \
                        \
    COPY_SUFFIX;        \
                        \
    RETURN_TOTSZ;       \
                        \
    NULL_TERMINATION;   \
                        \
    OSZ_CHECK(tok);

#define DELETE_TOKEN_AND_LINE( tok) \
                                    \
    DECL_RUNNERS;                   \
                                    \
    INIT_RUNNERS;                   \
                                    \
    RETURN_TOTSZ;                   \
                                    \
    NULL_TERMINATION;               \
                                    \
    OSZ_CHECK(tok);

#define REPLACE_TOKEN(tok,s)                           \
                                                       \
    DECL_RUNNERS;                                      \
                                                       \
    INIT_RUNNERS;                                      \
                                                       \
    COPY_PREFIX;                                       \
                                                       \
    CINT_SNPRINTF_E(RUNNERS, "%s", s);                 \
                                                       \
    COPY_SUFFIX;                                       \
                                                       \
    RETURN_TOTSZ;                                      \
                                                       \
    NULL_TERMINATION;                                  \
                                                       \
    OSZ_CHECK(tok);

#define RETURN_TOTSZ      \
                          \
    if (!obuf) {          \
                          \
        *obufsz = rtotsz; \
        return;           \
    }

#define NULL_TERMINATION  \
    *rdst = '\0';         \
    rbufsz--;             \
    rtotsz++;             \
    rdst++;

#define NESTED_TOKEN_HANDLER(tok,...)  \
{                                      \
    int tmp_obufsz = 0;                \
    char *tmp_obuf = rdst;             \
    if (tmp_obuf) {                    \
        tmp_obufsz = *obufsz - rtotsz; \
    }                                  \
    TOKEN_HANDLER(tok)(FIXED_ARGS_NESTED, ## __VA_ARGS__);           \
    rtotsz += tmp_obufsz;              \
    if (tmp_obuf) {                    \
        rdst += tmp_obufsz;            \
        rbufsz -= tmp_obufsz;          \
    }                                  \
}


/*
 * List of iterator variables to be used in the generated code. Typically used
 * as the looping variable in for loops.
 */
#define ITER_V            "ii"
#define ITER_V2           "jj"
#define ITER_V3           "kk"
#define ITER_V4           "ll"
#define ITER_V5           "mm"
#define ITER_V6           "nn"
#define ITER_V7           "oo"
#define ITER_V8           "pp"

/*
 * Names of source and destination variables to be used wherever sal_memcpy or
 * sal_memset is required.
 */
#define MEMCPY_SRC_V      "src"
#define MEMCPY_DST_V      "dst"
#define UINT8_PTR_V       "u8"

/*
 * Variables for pointer comparisons.
 */
#define VOID_PTR_V1       "v1"
#define VOID_PTR_V2       "v2"

/*
 * Name of the variable holding index of the parameter which does not have
 * expected value.
 */
#define ARGS_CHECK_RES_V  "args_check_res"

/*
 * Prefixes assigned to variables holding values expected or initial values of
 * API parameters.
 */
#define BEFORE_VPFX       "b_"
#define AFTER_VPFX        "a_"
#define INITIAL_VPFX      "i_"


#define CINT_SNPRINTF_E cint_logger_snprintf_e

static int
cint_logger_snprintf_e(char **rdst, int *rsz, int *rtotsz,
                       const char*fmt, ...)
                       COMPILER_ATTRIBUTE((format (printf, 4, 5)));

static int
cint_logger_snprintf_e (char **rdst, int *rsz, int *rtotsz,
                        const char*fmt, ...)
{
    int olen, adjlen;
    va_list va_args;

    /*
     * if any of the runner pointers are NULL return error. if the running
     * destination is NULL but the running size is non-zero return error.
     */
    if (!rdst || !rsz || !rtotsz || (!*rdst && *rsz)) {

        return 0;
    }

    va_start(va_args, fmt);

    if ((olen = CINT_VSNPRINTF(*rdst, *rsz, fmt, va_args)) >= 0) {

        adjlen = ((*rsz > olen) ? olen : *rsz);

        *rdst += ((*rdst) ? adjlen : 0);

        *rsz  -= (( *rsz) ? adjlen : 0);

        *rtotsz += olen;
    }

    va_end(va_args);

    return olen;

}

CINT_LOGGER_TESTS_TOKEN_HANDLER(NONE)
{
    NULL_TOKEN(NONE);
}

#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS , const char* unit_str
CINT_LOGGER_TESTS_TOKEN_HANDLER(__UNIT)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    REPLACE_TOKEN(__UNIT, unit_str);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(UNIT)
{
    int unit;
    char unit_str[10];

    unit = *(int*)input_values[1];
    CINT_SNPRINTF(unit_str, sizeof(unit_str), "%d", unit);

    TOKEN_HANDLER(__UNIT)(FIXED_ARGS, unit_str);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(API)
{
    REPLACE_TOKEN(API, ctxt->fn);
}

#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS , int hi
CINT_LOGGER_TESTS_TOKEN_HANDLER(__TEST_TID)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    sal_thread_t self_tid;
    uint32 test_tid;
    DECL_RUNNERS;

    INIT_RUNNERS;

    COPY_PREFIX;

    self_tid = sal_thread_self();

    if (hi) {
#ifdef PTRS_ARE_64BITS
        test_tid = PTR_HI_TO_INT(self_tid);
#else
        test_tid = 0;
#endif
    } else {
        test_tid = PTR_TO_INT(self_tid);
    }

    CINT_SNPRINTF_E(RUNNERS, "0x%08x", test_tid);

    COPY_SUFFIX;

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(__TEST_TID);

}

CINT_LOGGER_TESTS_TOKEN_HANDLER(TEST_TID_LO)
{
    TOKEN_HANDLER(__TEST_TID)(FIXED_ARGS, FALSE);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(TEST_TID_HI)
{
    TOKEN_HANDLER(__TEST_TID)(FIXED_ARGS, TRUE);
}

#undef DECL_EXTRA_ARGS
/*
 * Two additional inputs for this handler:
 *   vpfx  - a prefix for all variable names
 *   flags - CINT_PARAM_IN or/and CINT_PARAM_OUT indicating whether variables
 *           for parameters of type input, output or both should be declared.
 */
#define DECL_EXTRA_ARGS , const char* vpfx, int flags
CINT_LOGGER_TESTS_TOKEN_HANDLER(__DECL)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    cint_parameter_desc_t *params, pds[CINT_CONFIG_MAX_FPARAMS+1];
    char _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    char _astr[256] = {0};
    char pfx1[20], pfx2[20];
    char *type, *apos;
    int ii, nargs, indent;
    int first, done, dim;
    DECL_RUNNERS;

    INIT_RUNNERS;

    params = ctxt->params;
    nargs = ctxt->nargs;

    indent = PREFIX_LEN;

    for (ii = 0; (ii < nargs) && params[ii].name; ii++) {

        pds[ii] = params[ii];
    }

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    first = 1;

    do {

        done = 1;

        for (ii = 0; (ii < nargs) && pds[ii].name; ii++) {

            /*
             * Do not declare output pointers for the first call, and input
             * pointers for the second call.
             *
             * flags indicates whether input, output or both type of parameters
             * are to be processed.
             *
             * Restricting this check to parameters other than the return value
             * has flags=0, and also only to pointers.
             *
             * 1. ii=0 is excluded because is is reserved for the return value,
             * which is not filtered
             *
             * 2. For 'flags', the value CINT_PARAM_IN indicates 'input stage'
             * (call1) and the value CINT_PARAM_OUT indicates 'output stage'
             * (call2).
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
                !(flags & params[ii].flags & CINT_PARAM_INOUT)) {

                continue;
            }


            if (pds[ii].pcount < 0) {
                continue;
            }

            if ((pds[ii].pcount == 0) && !CINT_STRCMP(pds[ii].basetype, "void")) {
                continue;
            }

            if (!first && (pds[ii].pcount == 0)) {

                if (!i_vl_array_cnt[ii]) {

                    pds[ii].pcount--;

                    done = 0;

                    continue;
                }

                for (dim = CINT_CONFIG_ARRAY_DIMENSION_LIMIT - 1; dim > 0; dim--) {

                    pds[ii].dimensions[dim] = pds[ii].dimensions[dim-1];
                }

                pds[ii].dimensions[0] = i_vl_array_cnt[ii];

                pds[ii].num_dimensions++;
            }

            type = cint_datatype_format_pd(&pds[ii], _rstr, 0);

            if (pds[ii].num_dimensions) {

                apos = CINT_STRCHR(type, '[');

                if (apos) {

                    CINT_STRNCPY(_astr, apos, sizeof(_astr));
                    *apos = '\0';

                } else {

                    LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                              (BSL_META("ERROR: %s: failed to find '[' in array type %s (num_dimensions %d)\n"),
                               "__DECL", type, pds[ii].num_dimensions));
                    _astr[0] = '\0';
                }

            } else {

                _astr[0] = '\0';
            }

            cint_rep_chr(pfx2, '_', sizeof(pfx2), params[ii].pcount - pds[ii].pcount);

            /* indent datatype _pfx+name [arr]; */
            CINT_SNPRINTF_E(RUNNERS, "%s%-15s %s%s%s%s;\n", pfx1, type, pfx2, vpfx, pds[ii].name, _astr);

            pds[ii].pcount--;

            done = 0;
        }

        first = 0;

    } while (!done);

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(__DECL);

}

CINT_LOGGER_TESTS_TOKEN_HANDLER(DECL_ARGS)
{
    /*
     * Declare variables holding both input and output parameters of the API;
     * these variable are passed to the API call.
     */
    TOKEN_HANDLER(__DECL)(FIXED_ARGS, INITIAL_VPFX, CINT_PARAM_INOUT);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(DECL_ARGS_EXPECTED_BEFORE)
{
    /*
     * Declare variables for holding expected values of input parameters of
     * the API during call1
     */
    TOKEN_HANDLER(__DECL)(FIXED_ARGS, BEFORE_VPFX, CINT_PARAM_IN);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(DECL_ARGS_EXPECTED_AFTER)
{
    /*
     * Declare variables for holding expected values of output parameters of
     * the API during call2
     */
    TOKEN_HANDLER(__DECL)(FIXED_ARGS, AFTER_VPFX, CINT_PARAM_OUT);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(DECL_ARGS_CHANGES)
{
    /*TOKEN_HANDLER(__DECL)(FIXED_ARGS, "c");*/
    NULL_TOKEN(DECL_ARGS_CHANGES);
}

static void
cint_logger_tests_tokens_hexdump (char **rdst, int *rsz, int *rtotsz, char *pfx, int indent, uint8 *values, int nbytes)
{
    int j, newline, olen, llen;
    int maxlen = 65;
    char pfx2[20];

    cint_rep_chr(pfx2, ' ', sizeof(pfx2), indent);

    newline = 1; llen = 0;

    for (j = 0; j < nbytes;) {

        if (newline) {

            llen = 0;

            olen = CINT_SNPRINTF_E(rdst, rsz, rtotsz, "\n%s%s", pfx, pfx2);

            llen += olen;

            /* avoid an infinite loop */
            if ((llen+5) > maxlen) {
                break;
            }

            newline = 0;
        }

        if ((llen+5) > maxlen) {

            newline = 1;
            continue;
        }

        olen = CINT_SNPRINTF_E(rdst, rsz, rtotsz, "0x%02x,", values[j]);

        llen += olen;

        j++;
    }

}

#undef DECL_EXTRA_ARGS
/*
 * Two additional inputs for this handler:
 *   vpfx  - a prefix for all variable names
 *   flags - CINT_PARAM_IN or/and CINT_PARAM_OUT indicating whether variables
 *           for parameters of type input, output or both should be
 *           initialized.
 */
#define DECL_EXTRA_ARGS , const char* vpfx, int flags
CINT_LOGGER_TESTS_TOKEN_HANDLER(__INIT_ARGS)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    uint8 **values = NULL;
    int *vl_array_cnt = NULL, array_cnt_indx;
    cint_parameter_desc_t *params;
    int ii, indent, nargs, nbytes, pcount;
    char pfx1[20], pfx2[20], pfx3[20];
    char v1str[256], v2str[256], v3str[256], _astr[256];
    DECL_RUNNERS;

    INIT_RUNNERS;

    switch (token->type) {
    case TOKEN_TYPE(INIT_ARGS):
    case TOKEN_TYPE(INIT_ARGS_EXPECTED_BEFORE):
        values = input_values;
        vl_array_cnt = i_vl_array_cnt;
        break;
    case TOKEN_TYPE(INIT_ARGS_EXPECTED_AFTER):
        values = ((output_values) ? output_values : input_values);
        vl_array_cnt = ((o_vl_array_cnt) ? o_vl_array_cnt : i_vl_array_cnt);
        break;
    default:
        goto exit;
    }

    /*
     * Beyond this point both values and vl_array_cnt are assumed to be
     * not-NULL
     */

    /*
     * uint8 i_unit[<>] = {
     *     0x01, 0x02, 0x03, 0x04,
     * };
     * i_unit = 0x01020304;
     * sal_memcpy(&i_unit, &i_unit[0], sizeof(i_unit));
     *
     * uint8 i_cfg_value[] = { };
     * cfg = &_cfg[0];
     * sal_memcpy(cfg, &i_cfg_value[0], 180);
 */
    params = ctxt->params;
    nargs  = ctxt->nargs;

    indent = PREFIX_LEN;

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    CINT_SNPRINTF_E(RUNNERS, "%s/*\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s * fix the pointers first...\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s */\n", pfx1);

    for (ii = 0; (ii < nargs) && params[ii].name; ii++) {

        /*
         * Output pointers for the first call, and input pointers for the
         * second call were not declared. Do not fix them up.
         *
         * flags indicates whether input, output or both type of parameters
         * are to be processed.
         *
         * Restricting this check to parameters other than the return value
         * has flags=0, and also only to pointers.
         *
         * 1. ii=0 is excluded because is is reserved for the return value,
         * which is not filtered
         *
         * 2. For 'flags', the value CINT_PARAM_IN indicates 'input stage'
         * (call1) and the value CINT_PARAM_OUT indicates 'output stage'
         * (call2).
         *
         * 3. For params[ii].flags, the value CINT_PARAM_IN indicates an
         * input parameter while CINT_PARAM_OUT  indicates an output
         * parameter
         *
         * 4. params[ii].pcount when > 0 indicates that the parameter is a
         * pointer. pcount provides the number of pointer indirections.
 */
        if ((ii != 0) && (params[ii].pcount > 0) &&
            !(flags & params[ii].flags & CINT_PARAM_INOUT)) {

            continue;
        }

        for (pcount = 0; pcount < params[ii].pcount; pcount++) {

            cint_rep_chr(pfx2, '_', sizeof(pfx2), pcount);

            cint_rep_chr(pfx3, '_', sizeof(pfx3), pcount+1);

            CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s%s", pfx2, vpfx, params[ii].name);

            CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s%s%s", pfx3, vpfx, params[ii].name, ((pcount == (params[ii].pcount-1)) ? "[0]" : ""));

            if ((pcount == (params[ii].pcount - 1)) && (vl_array_cnt[ii] == 0)) {

                CINT_SNPRINTF_E(RUNNERS, "%s%s = NULL;\n", pfx1, v1str);
            } else {

                CINT_SNPRINTF_E(RUNNERS, "%s%s = &%s;\n", pfx1, v1str, v2str);
            }

            CINT_SNPRINTF_E(RUNNERS, "\n");

        }
    }

    for (ii = 0; (ii < nargs) && params[ii].name; ii++) {

        /*
         * Output pointers for the first call, and input pointers for the
         * second call were not declared. Therefore, need not output random
         * byte arrays for the objects they point to.
         *
         * 1. ii=0 is excluded because is is reserved for the return value,
         * which is not filtered
         *
         * 2. For 'flags', the value CINT_PARAM_IN indicates 'input stage'
         * (call1) and the value CINT_PARAM_OUT indicates 'output stage'
         * (call2).
         *
         * 3. For params[ii].flags, the value CINT_PARAM_IN indicates an
         * input parameter while CINT_PARAM_OUT  indicates an output
         * parameter
         *
         * 4. params[ii].pcount when > 0 indicates that the parameter is a
         * pointer. pcount provides the number of pointer indirections.
 */
        if ((ii != 0) && (params[ii].pcount > 0) &&
            !(flags & params[ii].flags & CINT_PARAM_INOUT)) {

            continue;
        }

        nbytes = vl_array_cnt[ii] * ctxt->basetype_size[ii];

        if (!nbytes) {

            continue;
        }

        cint_rep_chr(pfx2, '_', sizeof(pfx2), params[ii].pcount);

        CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s%s_bytes", pfx2, vpfx, params[ii].name);

        CINT_SNPRINTF_E(RUNNERS, "%suint8 %s[%d] = {", pfx1, v1str, nbytes);

        cint_logger_tests_tokens_hexdump(RUNNERS, pfx1, 4, values[ii], nbytes);

        CINT_SNPRINTF_E(RUNNERS, "\n");

        CINT_SNPRINTF_E(RUNNERS, "%s};\n", pfx1);

        if (CINT_PARAM_IS_VP(params[ii].flags)) {

            array_cnt_indx = params[ii].flags & CINT_PARAM_IDX;

            cint_rep_chr(pfx2, '*', sizeof(pfx2), params[array_cnt_indx].pcount);

            CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s%s", pfx2, vpfx, params[array_cnt_indx].name);

            CINT_SNPRINTF_E(RUNNERS, "%s/* sometimes the VL arguments comes later */\n", pfx1);

            CINT_SNPRINTF_E(RUNNERS, "%s%s = %d;\n", pfx1, v2str, vl_array_cnt[ii]);

            CINT_SNPRINTF_E(RUNNERS, "%sfor (%s = 0; %s < %s; %s++) {\n", pfx1, ITER_V, ITER_V, v2str, ITER_V);

            cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent+4);
        }

        _astr[0] = '\0';

        for (pcount = params[ii].pcount; pcount > 1; pcount--) {

            CINT_STRNCAT(_astr, "[0]", sizeof(_astr) - CINT_STRLEN(_astr) - 1);
        }

        if (pcount) {

            CINT_SNPRINTF(_astr, sizeof(_astr), "%s[%s]", _astr, (CINT_PARAM_IS_VP(params[ii].flags) ? ITER_V : "0"));
        }

        CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s%s", vpfx, params[ii].name, _astr);

        CINT_SNPRINTF(v3str, sizeof(v3str), "%s*sizeof(%s)", ITER_V, v2str);

        CINT_SNPRINTF_E(RUNNERS, "%s%s = &%s[%s];\n", pfx1,
                        MEMCPY_SRC_V, v1str,
                        (CINT_PARAM_IS_VP(params[ii].flags) ? v3str : "0"));

        CINT_SNPRINTF_E(RUNNERS, "%s%s = &%s;\n", pfx1,
                        MEMCPY_DST_V, v2str);

        CINT_SNPRINTF_E(RUNNERS, "%ssal_memcpy(%s, %s, sizeof(%s));\n", pfx1,
                        MEMCPY_DST_V, MEMCPY_SRC_V, v2str);

        if (CINT_PARAM_IS_VP(params[ii].flags)) {

            cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

            CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx1);
        }

        CINT_SNPRINTF_E(RUNNERS, "\n");

    }

exit:

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(__INIT_ARGS);

}

CINT_LOGGER_TESTS_TOKEN_HANDLER(INIT_ARGS)
{
    /*
     * Generate code to initialize variables meant for holding initial values
     * of both input and output parameters; those variables should have been
     * declared earlier. These variables are later passed to the API call. Also
     * see DECL_ARGS
     */
    TOKEN_HANDLER(__INIT_ARGS)(FIXED_ARGS, INITIAL_VPFX, CINT_PARAM_INOUT);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(INIT_ARGS_EXPECTED_BEFORE)
{
    /*
     * Generate code to initialize variables meant for holding expected values
     * of input parameters during call1. Also see DECL_ARGS_EXPECTED_BEFORE
     */
    TOKEN_HANDLER(__INIT_ARGS)(FIXED_ARGS, BEFORE_VPFX, CINT_PARAM_IN);
    /*NULL_TOKEN(INIT_ARGS_EXPECTED_BEFORE);*/
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(INIT_ARGS_EXPECTED_AFTER)
{
    /*
     * Generate code to initialize variables meant for holding expected values
     * of output parameters during call2. Also see DECL_ARGS_EXPECTED_AFTER
     */
    TOKEN_HANDLER(__INIT_ARGS)(FIXED_ARGS, AFTER_VPFX, CINT_PARAM_OUT);
    /*NULL_TOKEN(INIT_ARGS_EXPECTED_AFTER);*/
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(INIT_ARGS_CHANGES)
{
    NOP_TOKEN(INIT_ARGS_CHANGES);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(DECL_LOCAL_VARIABLES)
{
    int indent;
    char pfx1[20];
    DECL_RUNNERS;

    INIT_RUNNERS;

    /*COPY_PREFIX;*/

    indent = PREFIX_LEN;

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    CINT_SNPRINTF_E(RUNNERS, "%sint %s, %s, %s, %s;\n", pfx1, ITER_V , ITER_V2, ITER_V3, ITER_V4);
    CINT_SNPRINTF_E(RUNNERS, "%sint %s, %s, %s, %s;\n", pfx1, ITER_V5, ITER_V6, ITER_V7, ITER_V8);
    CINT_SNPRINTF_E(RUNNERS, "%svoid *%s, *%s;\n", pfx1, MEMCPY_SRC_V, MEMCPY_DST_V);
    CINT_SNPRINTF_E(RUNNERS, "%suint8 *%s;\n", pfx1, UINT8_PTR_V);
    CINT_SNPRINTF_E(RUNNERS, "%svoid *%s, *%s;\n", pfx1, VOID_PTR_V1, VOID_PTR_V2);
    CINT_SNPRINTF_E(RUNNERS, "%sint %s;\n", pfx1, ARGS_CHECK_RES_V);

    /*COPY_SUFFIX;*/

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(DECL_LOCAL_VARIABLES);
}

#undef DECL_EXTRA_ARGS
/*
 */
#define DECL_EXTRA_ARGS ,  \
    const char* indent,    \
    const char* v1strbase, \
    const char* v2strbase, \
    int argnum,            \
    cint_parameter_desc_t *ppdt, \
    /*
     * This handler (__ARGS_CHECK_ONE) is called recursively and  \
     * at each         iteration, depending on the datatype being \
     * processed, it may need to      output 'for' loops which    \
     * require their own loop variables.                          \
     *                                                            \
     * Those variable names are stored in the array 'iters'       \
     * below.                                                     \
     *                                                            \
     * If at a recursive call the iterator variable is selected   \
     * from this array  starting from the first one then in the   \
     * output we have nested 'for'       loops using the same     \
     * loop variable -- which obviously leads to issues.    If    \
     * the outer loop limit is smaller than inner one, the outer  \
     * loop         terminates early; if outer loop limit is      \
     * bigger we get an infinite loop.                            \
     *                                                            \
     * Therefore, we need a way to remember loop variables that   \
     * have already     been used and start allocating from the   \
     * one following. This argument is   passing index of the     \
     * first available loop variable.                             \
     */                                                           \
    int first_iter
CINT_LOGGER_TESTS_TOKEN_HANDLER(__ARGS_CHECK_ONE)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    cint_datatype_t dt;
    cint_parameter_desc_t *sm_ppdt;
    int ii = 0, jj;
    int is_comparable, is_struct, is_func_ptr, is_pbmp, is_rx_reasons, is_outer_array;
    int is_replay_mode;
    char* iters[] = { ITER_V2, ITER_V3, ITER_V4, ITER_V5, ITER_V6, ITER_V7 };
    char pfx[20];
    char v1str[256], v2str[256], _astr[256];
    DECL_RUNNERS;

    INIT_RUNNERS;

    /*COPY_PREFIX;*/

    CINT_MEMSET(&dt, 0, sizeof(dt));
    is_comparable = 0;
    is_struct = 0;
    is_func_ptr = 0;
    is_pbmp = 0;
    is_rx_reasons = 0;
    is_replay_mode = (test_mode == cintLoggerModeReplay);

    is_outer_array = (ppdt->num_dimensions > 0);

    if ((ppdt->pcount == 0) &&
        cint_datatype_find(ppdt->basetype, &dt) == CINT_E_NONE) {

        is_struct = (dt.flags & CINT_DATATYPE_F_STRUCT);

        is_comparable = ((dt.flags & CINT_DATATYPE_F_ATOMIC) &&
                         (dt.basetype.ap->flags & CINT_ATOMIC_TYPE_FLAGS_CSIL) &&
                         (dt.desc.num_dimensions == 0));

        is_func_ptr = (dt.flags & CINT_DATATYPE_F_FUNC_POINTER);

        is_pbmp = !CINT_STRCMP(ppdt->basetype, "bcm_pbmp_t");

        is_rx_reasons = !CINT_STRCMP(ppdt->basetype, "bcm_rx_reasons_t");
    }

    is_comparable = is_comparable || (ppdt->pcount > 0);

    CINT_SNPRINTF(pfx, sizeof(pfx), "%s", indent);

    _astr[0] = '\0';

    if (is_outer_array) {

        for (ii = 0; ii < ppdt->num_dimensions; ii++) {

            CINT_SNPRINTF_E(RUNNERS, "%sfor(%s = 0; %s < %d; %s++) {\n", indent,
                            iters[ii + first_iter],
                            iters[ii + first_iter], ppdt->dimensions[ii],
                            iters[ii + first_iter]);
            CINT_SNPRINTF(_astr, sizeof(_astr), "%s[%s]", _astr, iters[ii + first_iter]);
            CINT_STRNCAT(pfx, "    ", sizeof(pfx)-CINT_STRLEN(pfx) - 1);
        }
    }

    CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s", v1strbase, _astr);
    CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s", v2strbase, _astr);

    if (is_struct) {

        CINT_SNPRINTF_E(RUNNERS, "%sif (sal_memcmp(&%s, &%s, sizeof(%s)) != 0) {\n", pfx, v1str, v2str, v1str);

        if (is_replay_mode) {

            CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"INFO: %s did not match %s\\n\");\n", pfx, v1str, v2str);

            CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"INFO: expected during Replay, compare every field\\n\");\n", pfx);

        } else {

            CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"ERROR: %s did not match %s\\n\");\n", pfx, v1str, v2str);

            CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"ERROR: printing %s\\n\");\n", pfx, v1str);

            CINT_SNPRINTF_E(RUNNERS, "%s    print %s;\n", pfx, v1str);

            CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"ERROR: printing %s\\n\");\n", pfx, v2str);

            CINT_SNPRINTF_E(RUNNERS, "%s    print %s;\n", pfx, v2str);

            CINT_SNPRINTF_E(RUNNERS, "%s    %s = %d;\n", pfx, ARGS_CHECK_RES_V, argnum);
        }

        CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx);

        if (is_replay_mode) {

            for (sm_ppdt = dt.basetype.sp->struct_members;
                 sm_ppdt->name;
                 sm_ppdt++) {

                CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s.%s", v1strbase, _astr, sm_ppdt->name);
                CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s.%s", v2strbase, _astr, sm_ppdt->name);

                NESTED_TOKEN_HANDLER(__ARGS_CHECK_ONE, pfx, v1str, v2str, argnum, sm_ppdt, first_iter+ii);
            }
        }

    } else {

        if (is_comparable) {

            CINT_SNPRINTF_E(RUNNERS, "%sif (%s != %s) {\n", pfx, v1str, v2str);

        } else {

            /*
             * bcm_pbmp_t and bcm_rx_reasons_t are both bitfields, and the
             * number of bits in these bitfields can be bigger than
             * BCM_PBMP_PORT_MAX and bcmReasonCount respectively.
             *
             * Variables of these datatypes are filled up with random values
             * over all available bits and are copied and compared using
             * sizeof(.) these variables. Hence there is no issue during inline
             * and offline verification.
             *
             * However, cint_variable_print does not print those extra bits so
             * those bits end up being zeros in variables loaded up from the
             * converted CINT during Replay.
             *
             * Therefore, while generating verification code for Replay, we
             * need to generate code to clear all bits beyond BCM_PBMP_PORT_MAX
             * and bcmRxReasonCount from the variables holding their expected
             * values, before proceeding to memcmp over sizeof(.) bytes.
             */

            if (is_replay_mode && is_pbmp) {

                CINT_SNPRINTF_E(RUNNERS, "%s%s = &%s;\n", pfx, MEMCPY_DST_V, v2str);

                CINT_SNPRINTF_E(RUNNERS, "%sfor (%s = BCM_PBMP_PORT_MAX; %s < (8*sizeof(%s)); %s++) {\n",
                                pfx, ITER_V8, ITER_V8, v2str, ITER_V8);

                CINT_SNPRINTF_E(RUNNERS, "%s    SHR_BITCLR(%s, %s);\n", pfx, MEMCPY_DST_V, ITER_V8);

                CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx);
            }

            if (is_replay_mode && is_rx_reasons) {

                CINT_SNPRINTF_E(RUNNERS, "%s%s = &%s;\n", pfx, MEMCPY_DST_V, v2str);

                CINT_SNPRINTF_E(RUNNERS, "%sfor (%s = bcmRxReasonCount; %s < (8*sizeof(%s)); %s++) {\n",
                                pfx, ITER_V8, ITER_V8, v2str, ITER_V8);

                CINT_SNPRINTF_E(RUNNERS, "%s    SHR_BITCLR(%s, %s);\n", pfx, MEMCPY_DST_V, ITER_V8);

                CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx);
            }

            CINT_SNPRINTF_E(RUNNERS, "%sif (sal_memcmp(&%s, &%s, sizeof(%s)) != 0) {\n", pfx, v1str, v2str, v1str);

        }

        CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"ERROR: %s did not match %s\\n\");\n", pfx, v1str, v2str);

        CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"ERROR: printing %s\\n\");\n", pfx, v1str);

        CINT_SNPRINTF_E(RUNNERS, "%s    print %s;\n", pfx, v1str);

        CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"ERROR: printing %s\\n\");\n", pfx, v2str);

        CINT_SNPRINTF_E(RUNNERS, "%s    print %s;\n", pfx, v2str);

        /*
         * Excuse the mismatch if variable is a pointer
         */
        if (ppdt->pcount || is_func_ptr) {

            CINT_SNPRINTF_E(RUNNERS, "%s    /*\n", pfx);
            CINT_SNPRINTF_E(RUNNERS, "%s     * Pointers are expected to differ\n", pfx);
            CINT_SNPRINTF_E(RUNNERS, "%s     *    %s = %d;\n", pfx, ARGS_CHECK_RES_V, argnum);
            CINT_SNPRINTF_E(RUNNERS, "%s     */\n", pfx);
        } else {

            CINT_SNPRINTF_E(RUNNERS, "%s    %s = %d;\n", pfx, ARGS_CHECK_RES_V, argnum);
        }

        CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx);

    }

    if (is_outer_array) {

        for (ii = 0; ii < ppdt->num_dimensions; ii++) {

            jj = CINT_STRLEN(pfx) - 4;
            if (jj >= 0) {
                pfx[jj] = '\0';
            } else {
                pfx[0] = '\0';
            }
            CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx);
        }
    }

    /*COPY_SUFFIX;*/

    /*RETURN_TOTSZ;*/

    /*NULL_TERMINATION;*/

    /*OSZ_CHECK(__ARGS_CHECK_ONE);*/

    OSZ_NESTED;
}

#undef DECL_EXTRA_ARGS
/*
 * Two additional inputs for this handler:
 *   vpfx  - a prefix for names of variables holding expected values of API
 *           parameters.
 *   flags - CINT_PARAM_IN or/and CINT_PARAM_OUT indicating whether code should
 *           be generated to check parameters of type input, output or both.
 */
#define DECL_EXTRA_ARGS , const char* vpfx, int flags
CINT_LOGGER_TESTS_TOKEN_HANDLER(__ARGS_CHECK)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    int ii, nargs, array_cnt_indx, pcount, indent;
    char pfx1[20], pfx2[20];
    char v1str[256], v2str[256], _astr[256];
    char *type, _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    cint_parameter_desc_t *params, pds[CINT_CONFIG_MAX_FPARAMS+1];
    DECL_RUNNERS;

    INIT_RUNNERS;

    /*COPY_PREFIX;*/

    params = ctxt->params;
    nargs = ctxt->nargs;

    indent = PREFIX_LEN;

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    for (ii = 1; (ii < nargs) && params[ii].name; ii++) {

        /*
         * Output pointers during the first call, and input pointers during the
         * second call are not available. Therefore, do not generate code to
         * compare the pointed objects.
         *
         * 1. ii=0 is excluded because is is reserved for the return value,
         * which is not filtered
         *
         * 2. For 'flags', the value CINT_PARAM_IN indicates 'input stage'
         * (call1) and the value CINT_PARAM_OUT indicates 'output stage'
         * (call2).
         *
         * 3. For params[ii].flags, the value CINT_PARAM_IN indicates an
         * input parameter while CINT_PARAM_OUT  indicates an output
         * parameter
         *
         * 4. params[ii].pcount when > 0 indicates that the parameter is a
         * pointer. pcount provides the number of pointer indirections.
 */
        if ((ii != 0) && (params[ii].pcount > 0) &&
            !(flags & params[ii].flags & CINT_PARAM_INOUT)) {

            continue;
        }

        pds[ii] = params[ii];

        type = cint_datatype_format_pd(&pds[ii], _rstr, 0);

        CINT_SNPRINTF_E(RUNNERS, "%sprintf(\"    checking %s %s\\n\");\n\n", pfx1, type, params[ii].name);

        if (CINT_PARAM_IS_VP(params[ii].flags)) {

            array_cnt_indx = params[ii].flags & CINT_PARAM_IDX;

            cint_rep_chr(pfx2, '*', sizeof(pfx2), params[array_cnt_indx].pcount);

            CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s", pfx2, params[array_cnt_indx].name);

            CINT_SNPRINTF_E(RUNNERS, "%sfor (%s = 0; %s < %s; %s++) {\n", pfx1, ITER_V, ITER_V, v1str, ITER_V);

            cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent+4);
        }

        _astr[0] = '\0';

        for (pcount = params[ii].pcount; pcount > 1; pcount--) {

            CINT_STRNCAT(_astr, "[0]", sizeof(_astr) - CINT_STRLEN(_astr) - 1);
        }

        if ((pcount == 1) && CINT_STRCMP(params[ii].basetype, "void")) {

            CINT_SNPRINTF(_astr, sizeof(_astr), "%s[%s]", _astr, (CINT_PARAM_IS_VP(params[ii].flags) ? ITER_V : "0"));
        }

        CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s", params[ii].name, _astr);

        CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s%s", vpfx, params[ii].name, _astr);

        pds[ii] = params[ii];

        pds[ii].pcount = 0;

        NESTED_TOKEN_HANDLER(__ARGS_CHECK_ONE, pfx1, v1str, v2str, ii, &pds[ii], 0);

        if (CINT_PARAM_IS_VP(params[ii].flags)) {

            cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

            CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx1);
        }

        CINT_SNPRINTF_E(RUNNERS, "\n");
    }

    /*COPY_SUFFIX;*/

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(__ARGS_CHECK);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(ARGS_CHECK_BEFORE)
{
    /*
     * Generated code to compare input parameters against variables meant for
     * holding their expected values during call1. Also see
     * INIT_ARGS_EXPECTED_BEFORE
     */
    TOKEN_HANDLER(__ARGS_CHECK)(FIXED_ARGS, BEFORE_VPFX, CINT_PARAM_IN);
    /*NULL_TOKEN(ARGS_CHECK_BEFORE);*/
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(ARGS_CHECK_BEFORE_RES)
{
    REPLACE_TOKEN(ARGS_CHECK_BEFORE_RES, ARGS_CHECK_RES_V);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(ARGS_CHANGES)
{
    NULL_TOKEN(ARGS_CHANGES);
}

/*
 * This token is to be used in the Replay verifier. The logger Replay
 * infrastructure allocates buffers to hold API outputs, which the API
 * implementation is expected to fill up during an actual Replay. Such buffers
 * are all filled with zeros.
 *
 * Since an actual API replay is not being tested here, this token is expanded
 * to code that fulfils two test objectives -- it writes to those buffers, thus
 * testing their validity and size; while doing so it also copies the expected
 * values of those variables so that the verification code that comes later
 * does not error out.
 */
CINT_LOGGER_TESTS_TOKEN_HANDLER(ACCESS_OUTPUTS)
{
    cint_parameter_desc_t *params;
    int ii, nargs, indent;
    char pfx[20];
    int is_replay_mode = (test_mode == cintLoggerModeReplay);
    int array_cnt_indx;
    DECL_RUNNERS;

    INIT_RUNNERS;

    /*COPY_PREFIX;*/

    params = ctxt->params;
    nargs = ctxt->nargs;

    if (!is_replay_mode) {

        goto exit;
    }

    indent = PREFIX_LEN;

    cint_rep_chr(pfx, ' ', sizeof(pfx), indent);

    for (ii = 0; (ii < nargs) && params[ii].name; ii++) {

        if ((ii == 0) || (params[ii].pcount == 0) ||
            !(params[ii].flags & CINT_PARAM_OUT)) {

            continue;
        }

        if (params[ii].pcount == 1) {

            if (params[ii].flags & CINT_PARAM_VP) {

                array_cnt_indx = params[ii].flags & CINT_PARAM_IDX;

                CINT_SNPRINTF_E(RUNNERS, "%sfor (%s = 0; %s < %s%s%s; %s++) {\n", pfx,
                                ITER_V, ITER_V, REPLAY_LOG2_VPFX, params[array_cnt_indx].name,
                                params[array_cnt_indx].pcount ?  "[0]" : "",
                                ITER_V);

                CINT_SNPRINTF_E(RUNNERS, "%s    %s[%s] = %s%s[%s];\n", pfx, params[ii].name, ITER_V, REPLAY_LOG2_VPFX, params[ii].name, ITER_V);

                CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx);
            } else {

                CINT_SNPRINTF_E(RUNNERS, "%s%s[0] = %s%s[0];\n", pfx, params[ii].name, REPLAY_LOG2_VPFX, params[ii].name);
            }
        } else {

            CINT_SNPRINTF_E(RUNNERS, "%s%s[0] = %s%s[0];\n", pfx, params[ii].name, REPLAY_LOG2_VPFX, params[ii].name);
        }

        CINT_SNPRINTF_E(RUNNERS, "\n");
    }

exit:

    /*COPY_SUFFIX;*/

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(ACCESS_OUTPUTS);

}

CINT_LOGGER_TESTS_TOKEN_HANDLER(ARGS_CHECK_AFTER)
{
    /*
     * Generated code to compare output parameters against variables meant for
     * holding their expected values during call2. Also see
     * INIT_ARGS_EXPECTED_AFTER
     */
    TOKEN_HANDLER(__ARGS_CHECK)(FIXED_ARGS, AFTER_VPFX, CINT_PARAM_OUT);
    /*NULL_TOKEN(ARGS_CHECK_AFTER);*/
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(ARGS_CHECK_AFTER_RES)
{
    REPLACE_TOKEN(ARGS_CHECK_AFTER_RES, ARGS_CHECK_RES_V);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(ARGS_CHECK_AFTER_INCL_CHANGES)
{
    NOP_TOKEN(ARGS_CHECK_AFTER_INCL_CHANGES);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(ARGS_CHECK_AFTER_INCL_CHANGES_RES)
{
    REPLACE_TOKEN(ARGS_CHECK_AFTER_INCL_CHANGES_RES, ARGS_CHECK_RES_V);
}

#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS , const char* vpfx
CINT_LOGGER_TESTS_TOKEN_HANDLER(__API_CALL)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    int i, nargs;
    cint_parameter_desc_t *params;
    DECL_RUNNERS;

    INIT_RUNNERS;

    COPY_PREFIX;

    params = ctxt->params;
    nargs = ctxt->nargs;

    CINT_SNPRINTF_E(RUNNERS, "%s%s = %s(", vpfx, params[0].name, ctxt->fn);

    for (i = 1; i < nargs; i++) {

        CINT_SNPRINTF_E(RUNNERS, "%s%s%s", (i > 1) ? ", " : "", vpfx, params[i].name);
    }

    CINT_SNPRINTF_E(RUNNERS, ");");

    COPY_SUFFIX;

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(API_CALL);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(API_CALL)
{
    TOKEN_HANDLER(__API_CALL)(FIXED_ARGS, INITIAL_VPFX);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(DECL_ARG_PTRS)
{
    int indent;
    char pfx1[20];
    DECL_RUNNERS;

    INIT_RUNNERS;

    /*COPY_PREFIX;*/

    indent = PREFIX_LEN;

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    CINT_SNPRINTF_E(RUNNERS, "%svoid *arg_ptrs[%d];\n", pfx1, CINT_CONFIG_MAX_FPARAMS+1);

    CINT_SNPRINTF_E(RUNNERS, "%svoid *ptr_args[%d];\n", pfx1, CINT_CONFIG_MAX_FPARAMS+1);

    /*COPY_SUFFIX;*/

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(DECL_ARG_PTRS);
}

#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS , const char* vpfx
CINT_LOGGER_TESTS_TOKEN_HANDLER(__SET_ARG_PTRS)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    int i, nargs, indent;
    cint_parameter_desc_t *params;
    char pfx1[20];
    char v1str[256];
    DECL_RUNNERS;

    INIT_RUNNERS;

    /*COPY_PREFIX;*/

    params = ctxt->params;
    nargs = ctxt->nargs;

    indent = PREFIX_LEN;

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    for (i = 1; (i < nargs) && params[i].name; i++) {

        CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s", vpfx, params[i].name);

        CINT_SNPRINTF_E(RUNNERS, "%s%s_test.arg_ptrs[%d] = &%s;\n", pfx1, ctxt->fn, i-1, v1str);

        CINT_SNPRINTF_E(RUNNERS, "%s%s_test.ptr_args[%d] = %s;\n",
                                 pfx1, ctxt->fn, i-1, ((params[i].pcount) ? v1str : "NULL"));
    }

    /*COPY_SUFFIX;*/

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(__SET_ARG_PTRS);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(SET_ARG_PTRS)
{
    TOKEN_HANDLER(__SET_ARG_PTRS)(FIXED_ARGS, INITIAL_VPFX);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(CHECK_ARG_PTRS)
{
    int i, nargs, indent;
    cint_parameter_desc_t *params;
    char pfx1[20];
    DECL_RUNNERS;

    INIT_RUNNERS;

    params = ctxt->params;
    nargs = ctxt->nargs;

    /*COPY_PREFIX;*/

    indent = PREFIX_LEN;

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    CINT_SNPRINTF_E(RUNNERS, "%s/*\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s * auto-injected variables map on top of actual API inputs. This means\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s * filter/verifier function may modify the inputs before dispatch, if\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s * required. Check whether variable addresses match. Since CINT clones\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s * input variables, addresses are expected to not match when API is called\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s * from CINT. However, the values received by the dispatch function can\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s * still be modified.\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s *\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s * Pointer-type argument should be same despite variable cloning in CINT\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s * interpreter.\n", pfx1);
    CINT_SNPRINTF_E(RUNNERS, "%s */\n", pfx1);


    for (i = 1; (i < nargs) && params[i].name; i++) {

        CINT_SNPRINTF_E(RUNNERS, "%s%s = %s_test.arg_ptrs[%d];\n", pfx1, VOID_PTR_V1, ctxt->fn, i-1);

        CINT_SNPRINTF_E(RUNNERS, "%sif (this.flags & 0x%x) {\n", pfx1, params[i].flags & CINT_PARAM_INOUT);

        CINT_SNPRINTF_E(RUNNERS, "%s    %s = &%s;\n", pfx1, VOID_PTR_V2, params[i].name);

        CINT_SNPRINTF_E(RUNNERS, "%s    if (%s != %s) {\n", pfx1, VOID_PTR_V1, VOID_PTR_V2);

        CINT_SNPRINTF_E(RUNNERS, "%s        printf(\"auto-injected variable #%d,%s does not map onto original argument\\n\");\n", pfx1, i, params[i].name);

        CINT_SNPRINTF_E(RUNNERS, "%s        printf(\"    $%s expected vs $%s found - expected when called from CINT\\n\");\n", pfx1, VOID_PTR_V1, VOID_PTR_V2);

        CINT_SNPRINTF_E(RUNNERS, "%s    }\n", pfx1);

        CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx1);

        CINT_SNPRINTF_E(RUNNERS, "\n");

        if (!params[i].pcount) {

            continue;
        }

        /*
         * All pointer-type arguments are logged as-is, therefore must be found
         * in the verifier exactly equal to what they were when passed to the
         * API. Generate code to check for this... e.g. if an API named "bcmf"
         * has a pointer named "config" as its second argument, generate the
         * following code; it assumes v1 and v2 were previously declared void*.
         *
         * v1 = bcmf_test.ptr_args[2];
         * v2 = config;
         * if (v1 != v2) {
         *     printf("pointer variable #2,config is not the same s original argument\n");
         *     printf("    $v1 expected vs $v2 found - NOT expected even when called from CINT\n");
         * }
 */
        CINT_SNPRINTF_E(RUNNERS, "%s%s = %s_test.ptr_args[%d];\n", pfx1, VOID_PTR_V1, ctxt->fn, i-1);

        CINT_SNPRINTF_E(RUNNERS, "%s%s = %s;\n", pfx1, VOID_PTR_V2, params[i].name);

        CINT_SNPRINTF_E(RUNNERS, "%sif (%s != %s) {\n", pfx1, VOID_PTR_V1, VOID_PTR_V2);

        CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"pointer variable #%d,%s is not the same as original argument\\n\");\n", pfx1, i, params[i].name);

        CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"    $%s expected vs $%s found - NOT expected even when called from CINT\\n\");\n", pfx1, VOID_PTR_V1, VOID_PTR_V2);

        CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx1);

        CINT_SNPRINTF_E(RUNNERS, "\n");

    }

    /*COPY_SUFFIX;*/

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(CHECK_ARG_PTRS);
}

#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS , const char* rv_str
CINT_LOGGER_TESTS_TOKEN_HANDLER(__RETVAL)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    REPLACE_TOKEN(__REVAL, rv_str);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(RETVAL)
{
    int rv;
    char rv_str[10];

    rv = *(int*)output_values[0];

    CINT_SNPRINTF(rv_str, sizeof(rv_str), "%d", rv);

    TOKEN_HANDLER(__RETVAL)(FIXED_ARGS, rv_str);
}

#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS , const char* vpfx
CINT_LOGGER_TESTS_TOKEN_HANDLER(__RETVAL_CHECK)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    cint_parameter_desc_t *params;
    int rv, indent;
    char pfx1[20];
    DECL_RUNNERS;

    INIT_RUNNERS;

    /*COPY_PREFIX*/

    params = ctxt->params;

    indent = PREFIX_LEN;

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    rv = *(int*)output_values[0];

    CINT_SNPRINTF_E(RUNNERS, "%sif (%s%s != %d) {\n", pfx1, vpfx, params[0].name, rv);

    CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"unexpected return value ($%s%s vs %d expected)\\n\");\n", pfx1, vpfx, params[0].name, rv);

    CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx1);

    /*COPY_SUFFIX*/

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(__RETVAL_CHECK);

}

CINT_LOGGER_TESTS_TOKEN_HANDLER(RETVAL_CHECK)
{
    TOKEN_HANDLER(__RETVAL_CHECK)(FIXED_ARGS, INITIAL_VPFX);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(TEST_ITEM)
{
   DELETE_TOKEN_AND_LINE(TEST_ITEM);
   test_item++;
}

#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS , const char* test_id
CINT_LOGGER_TESTS_TOKEN_HANDLER(__TEST_ID)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    REPLACE_TOKEN(__TEST_ID, test_id);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(TEST_ID)
{
    char test_id[20];

    CINT_SNPRINTF(test_id, 20, "%d", test_item);

    TOKEN_HANDLER(__TEST_ID)(FIXED_ARGS, test_id);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(REPLAY_MODE)
{
   DELETE_TOKEN_AND_LINE(REPLAY_MODE);
   test_mode = cintLoggerModeReplay;
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(RESET_MODE)
{
   DELETE_TOKEN_AND_LINE(RESET_MODE);
   test_mode = cintLoggerModeCache;
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(UNKNOWN)
{
    NULL_TOKEN(UNKNOWN);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(MAX)
{
    NULL_TOKEN(MAX);
}

#undef CINT_LOGGER_TESTS_TOKEN_HANDLER

char*
cint_logger_tests_gen_cint (cint_logger_api_params_t *ctxt,
                            uint8 *input_values[CINT_CONFIG_MAX_FPARAMS+1],
                            int i_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],
                            uint8 *output_values[CINT_CONFIG_MAX_FPARAMS+1],
                            int o_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],
                            const char templ[],
                            cint_logger_tests_token_t tok_offsets[])
{
    char *cint;
    struct {
        char *buffer;
        int cintletsz;
    } *cintlets;
    int i, ntoks, totalsz, toksz, hdrstart, hdrsz, cintletsz;
    char _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};

    for (ntoks = 0; tok_offsets[ntoks].offset >= 0; ntoks++);

    totalsz = (ntoks+1) * sizeof(*cintlets);
    cintlets = CINT_MALLOC(totalsz);
    CINT_MEMSET(cintlets, 0, totalsz);

    totalsz = 0;

    test_item = 0;

    for (i = 1; i < ntoks; i++) {

        toksz = tok_offsets[i].line_length;
        CINT_MEMCPY(_rstr, &templ[tok_offsets[i].line_offset], toksz);
        _rstr[toksz] = '\0';
        LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("%d: Processing token\n+%s-\n"), i, _rstr));

        toksz = 0;
        cintletsz = 0;

        hdrstart = tok_offsets[i-1].line_offset + tok_offsets[i-1].line_length;
        hdrsz = tok_offsets[i].line_offset - hdrstart;

        cintletsz += hdrsz;

        (token_handlers[tok_offsets[i].type])(ctxt,
                                              input_values,
                                              i_vl_array_cnt,
                                              output_values,
                                              o_vl_array_cnt,
                                              templ,
                                              &tok_offsets[i],
                                              NULL,
                                              &toksz);

        LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("%d: token size %d\n"), i, toksz));

        cintletsz += toksz;

        cintlets[i].cintletsz = cintletsz;
        cintlets[i].buffer = CINT_MALLOC(cintletsz+1);

        CINT_MEMCPY(cintlets[i].buffer, &templ[hdrstart], hdrsz);

        toksz++;
        (token_handlers[tok_offsets[i].type])(ctxt,
                                              input_values,
                                              i_vl_array_cnt,
                                              output_values,
                                              o_vl_array_cnt,
                                              templ,
                                              &tok_offsets[i],
                                              &cintlets[i].buffer[hdrsz],
                                              &toksz);

        cintlets[i].buffer[cintletsz] = '\0';

        LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("++++++++++++++++++++++++++++++ %d\n"), i));
        LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("+%s-\n"), cintlets[i].buffer));
        LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("------------------------------ %d, %d bytes, strlen %d\n"),
                   i, cintletsz, (int)CINT_STRLEN(cintlets[i].buffer)));

        totalsz += cintletsz;

    }

    hdrstart = tok_offsets[i-1].line_offset + tok_offsets[i-1].line_length;
    cintletsz = CINT_STRLEN(&templ[hdrstart]);

    cintlets[i].cintletsz = cintletsz;
    cintlets[i].buffer = CINT_MALLOC(cintletsz+1);

    CINT_MEMCPY(cintlets[i].buffer, &templ[hdrstart], cintletsz);

    cintlets[i].buffer[cintletsz] = '\0';

    LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
              (BSL_META("++++++++++++++++++++++++++++++ %d\n"), i));
    LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
              (BSL_META("+%s-\n"), cintlets[i].buffer));
    LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
              (BSL_META("------------------------------ %d, %d bytes, strlen %d\n"),
               i, cintletsz, (int)CINT_STRLEN(cintlets[i].buffer)));


    totalsz += cintletsz;


    cint = CINT_MALLOC(totalsz+1);
    hdrstart = 0;

    for (i = 1; i < ntoks+1; i++) {

        if (!cintlets[i].buffer || !cintlets[i].cintletsz) {

            continue;
        }

        CINT_MEMCPY(&cint[hdrstart],
                    cintlets[i].buffer,
                    cintlets[i].cintletsz);

        hdrstart += cintlets[i].cintletsz;
    }

    cint[hdrstart] = '\0';
    cint[totalsz]  = '\0';

    LOG_INFO(BSL_LS_APPL_CINTAPILOGGER,
             (BSL_META("GENERATED TEST SCRIPT\n")));
    LOG_INFO(BSL_LS_APPL_CINTAPILOGGER,
             (BSL_META("++++++++++++++++++++++++++++++\n")));
    LOG_INFO(BSL_LS_APPL_CINTAPILOGGER,
             (BSL_META("++%s-- %d bytes (%d bytes, offset %d bytes)\n"), cint, (int)CINT_STRLEN(cint), totalsz, hdrstart));
    LOG_INFO(BSL_LS_APPL_CINTAPILOGGER,
             (BSL_META("------------------------------\n")));

    for (i = 0; i < ntoks; i++) {

        if (cintlets[i].buffer) {

            CINT_FREE(cintlets[i].buffer);
        }
    }

    CINT_FREE(cintlets);

    return cint;

}

/* } */
#else
/* { */
/* Make ISO compilers happy. */
typedef int cint_logger_tests_tokens_c_not_empty;
/* } */
#endif
