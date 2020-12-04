/*
 * $Id: cint_logger_test_tokens.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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


#define ITER_V            "ii"
#define MEMCPY_SRC_V      "src"
#define MEMCPY_DST_V      "dst"
#define VOID_PTR_V1       "v1"
#define VOID_PTR_V2       "v2"
#define ARGS_CHECK_RES_V  "args_check_res"
#define BEFORE_VPFX       "b_"
#define AFTER_VPFX        "a_"
#define INITIAL_VPFX      "i_"

#define CINT_IS_COMPARABLE(ppdt)                                     \
    ((ppdt)->pcount ||                                               \
     ((cint_datatype_find((ppdt)->basetype, &dt) == CINT_E_NONE) &&  \
       (dt.flags & CINT_DATATYPE_F_ATOMIC) &&                        \
       (dt.basetype.ap->flags & CINT_ATOMIC_TYPE_FLAGS_CSIL)))


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
#define DECL_EXTRA_ARGS , const char* vpfx
CINT_LOGGER_TESTS_TOKEN_HANDLER(__DECL)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    cint_parameter_desc_t *params, pds[CINT_CONFIG_MAX_FPARAMS+1];
    char _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    char _astr[256] = {0};
    char pfx1[20], pfx2[20];
    char *type, *apos;
    int i, nargs, indent;
    int first, done, dim;
    DECL_RUNNERS;

    INIT_RUNNERS;

    params = ctxt->params;
    nargs = ctxt->nargs;

    indent = PREFIX_LEN;

    for (i = 0; (i < nargs) && params[i].name; i++) {

        pds[i] = params[i];
    }

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    first = 1;

    do {

        done = 1;

        for (i = 0; (i < nargs) && pds[i].name; i++) {

            if (pds[i].pcount < 0) {
                continue;
            }

            if ((pds[i].pcount == 0) && !CINT_STRCMP(pds[i].basetype, "void")) {
                continue;
            }

            if (!first && (pds[i].pcount == 0)) {

                if (!i_vl_array_cnt[i]) {

                    pds[i].pcount--;

                    done = 0;

                    continue;
                }

                for (dim = CINT_CONFIG_ARRAY_DIMENSION_LIMIT - 1; dim > 0; dim--) {

                    pds[i].dimensions[dim] = pds[i].dimensions[dim-1];
                }

                pds[i].dimensions[0] = i_vl_array_cnt[i];

                pds[i].num_dimensions++;
            }

            type = cint_datatype_format_pd(&pds[i], _rstr, 0);

            if (pds[i].num_dimensions) {

                apos = CINT_STRCHR(type, '[');

                if (apos) {

                    CINT_STRNCPY(_astr, apos, sizeof(_astr));
                    *apos = '\0';

                } else {

                    LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                              (BSL_META("ERROR: %s: failed to find '[' in array type %s (num_dimensions %d)\n"),
                               "__DECL", type, pds[i].num_dimensions));
                    _astr[0] = '\0';
                }

            } else {

                _astr[0] = '\0';
            }

            cint_rep_chr(pfx2, '_', sizeof(pfx2), params[i].pcount - pds[i].pcount);

            /* indent datatype _pfx+name [arr]; */
            CINT_SNPRINTF_E(RUNNERS, "%s%-15s %s%s%s%s;\n", pfx1, type, pfx2, vpfx, pds[i].name, _astr);

            pds[i].pcount--;

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
    TOKEN_HANDLER(__DECL)(FIXED_ARGS, INITIAL_VPFX);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(DECL_ARGS_EXPECTED_BEFORE)
{
    TOKEN_HANDLER(__DECL)(FIXED_ARGS, BEFORE_VPFX);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(DECL_ARGS_EXPECTED_AFTER)
{
    TOKEN_HANDLER(__DECL)(FIXED_ARGS, AFTER_VPFX);
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
#define DECL_EXTRA_ARGS , const char* vpfx
CINT_LOGGER_TESTS_TOKEN_HANDLER(__INIT_ARGS)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    uint8 **values = NULL;
    int *vl_array_cnt = NULL, array_cnt_indx;
    cint_parameter_desc_t *params;
    int i, indent, nargs, nbytes, pcount;
    char pfx1[20], pfx2[20], pfx3[20];
    char v1str[256], v2str[256], _astr[256];
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

    for (i = 0; (i < nargs) && params[i].name; i++) {

        for (pcount = 0; pcount < params[i].pcount; pcount++) {

            cint_rep_chr(pfx2, '_', sizeof(pfx2), pcount);

            cint_rep_chr(pfx3, '_', sizeof(pfx3), pcount+1);

            CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s%s", pfx2, vpfx, params[i].name);

            CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s%s%s", pfx3, vpfx, params[i].name, ((pcount == (params[i].pcount-1)) ? "[0]" : ""));

            if ((pcount == (params[i].pcount - 1)) && (vl_array_cnt[i] == 0)) {

                CINT_SNPRINTF_E(RUNNERS, "%s%s = NULL;\n", pfx1, v1str);
            } else {

                CINT_SNPRINTF_E(RUNNERS, "%s%s = &%s;\n", pfx1, v1str, v2str);
            }

            CINT_SNPRINTF_E(RUNNERS, "\n");

        }
    }

    for (i = 0; (i < nargs) && params[i].name; i++) {

        nbytes = vl_array_cnt[i] * ctxt->basetype_size[i];

        if (!nbytes) {

            continue;
        }

        cint_rep_chr(pfx2, '_', sizeof(pfx2), params[i].pcount);

        CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s%s_bytes", pfx2, vpfx, params[i].name);

        CINT_SNPRINTF_E(RUNNERS, "%suint8 %s[%d] = {", pfx1, v1str, nbytes);

        cint_logger_tests_tokens_hexdump(RUNNERS, pfx1, 4, values[i], nbytes);

        CINT_SNPRINTF_E(RUNNERS, "\n");

        CINT_SNPRINTF_E(RUNNERS, "%s};\n", pfx1);

        if (CINT_PARAM_IS_VP(params[i].flags)) {

            array_cnt_indx = params[i].flags & CINT_PARAM_IDX;

            cint_rep_chr(pfx2, '*', sizeof(pfx2), params[array_cnt_indx].pcount);

            CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s%s", pfx2, vpfx, params[array_cnt_indx].name);

            CINT_SNPRINTF_E(RUNNERS, "%s/* sometimes the VL arguments comes later */\n", pfx1);

            CINT_SNPRINTF_E(RUNNERS, "%s%s = %d;\n", pfx1, v2str, vl_array_cnt[i]);

            CINT_SNPRINTF_E(RUNNERS, "%sfor (%s = 0; %s < %s; %s++) {\n", pfx1, ITER_V, ITER_V, v2str, ITER_V);

            cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent+4);
        }

        _astr[0] = '\0';

        for (pcount = params[i].pcount; pcount > 1; pcount--) {

            CINT_STRNCAT(_astr, "[0]", sizeof(_astr) - CINT_STRLEN(_astr));
        }

        if (pcount) {

            CINT_SNPRINTF(_astr, sizeof(_astr), "%s[%s]", _astr, (CINT_PARAM_IS_VP(params[i].flags) ? ITER_V : "0"));
        }

        CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s%s", vpfx, params[i].name, _astr);

        CINT_SNPRINTF_E(RUNNERS, "%s%s = &%s[0];\n", pfx1,
                        MEMCPY_SRC_V, v1str);

        CINT_SNPRINTF_E(RUNNERS, "%s%s = &%s;\n", pfx1,
                        MEMCPY_DST_V, v2str);

        CINT_SNPRINTF_E(RUNNERS, "%ssal_memcpy(%s, %s, sizeof(%s));\n", pfx1,
                        MEMCPY_DST_V, MEMCPY_SRC_V, v2str);

        if (CINT_PARAM_IS_VP(params[i].flags)) {

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
    TOKEN_HANDLER(__INIT_ARGS)(FIXED_ARGS, INITIAL_VPFX);
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(INIT_ARGS_EXPECTED_BEFORE)
{
    TOKEN_HANDLER(__INIT_ARGS)(FIXED_ARGS, BEFORE_VPFX);
    /*NULL_TOKEN(INIT_ARGS_EXPECTED_BEFORE);*/
}

CINT_LOGGER_TESTS_TOKEN_HANDLER(INIT_ARGS_EXPECTED_AFTER)
{
    TOKEN_HANDLER(__INIT_ARGS)(FIXED_ARGS, AFTER_VPFX);
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

    CINT_SNPRINTF_E(RUNNERS, "%sint %s;\n", pfx1, ITER_V);
    CINT_SNPRINTF_E(RUNNERS, "%svoid *%s, *%s;\n", pfx1, MEMCPY_SRC_V, MEMCPY_DST_V);
    CINT_SNPRINTF_E(RUNNERS, "%svoid *%s, *%s;\n", pfx1, VOID_PTR_V1, VOID_PTR_V2);
    CINT_SNPRINTF_E(RUNNERS, "%sint %s;\n", pfx1, ARGS_CHECK_RES_V);

    /*COPY_SUFFIX;*/

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(DECL_LOCAL_VARIABLES);
}

#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS , const char* vpfx
CINT_LOGGER_TESTS_TOKEN_HANDLER(__ARGS_CHECK)
#undef DECL_EXTRA_ARGS
#define DECL_EXTRA_ARGS
{
    int i, nargs, array_cnt_indx, pcount, indent;
    char pfx1[20], pfx2[20];
    char v1str[256], v2str[256], _astr[256];
    char *type, _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    cint_parameter_desc_t *params, pds[CINT_CONFIG_MAX_FPARAMS+1];
    cint_datatype_t dt;
    DECL_RUNNERS;

    INIT_RUNNERS;

    /*COPY_PREFIX;*/

    params = ctxt->params;
    nargs = ctxt->nargs;

    indent = PREFIX_LEN;

    cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent);

    for (i = 1; (i < nargs) && params[i].name; i++) {

        pds[i] = params[i];

        type = cint_datatype_format_pd(&pds[i], _rstr, 0);

        CINT_SNPRINTF_E(RUNNERS, "%sprintf(\"    checking %s %s\\n\");\n\n", pfx1, type, params[i].name);

        if (CINT_PARAM_IS_VP(params[i].flags)) {

            array_cnt_indx = params[i].flags & CINT_PARAM_IDX;

            cint_rep_chr(pfx2, '*', sizeof(pfx2), params[array_cnt_indx].pcount);

            CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s", pfx2, params[array_cnt_indx].name);

            CINT_SNPRINTF_E(RUNNERS, "%sfor (%s = 0; %s < %s; %s++) {\n", pfx1, ITER_V, ITER_V, v1str, ITER_V);

            cint_rep_chr(pfx1, ' ', sizeof(pfx1), indent+4);
        }

        _astr[0] = '\0';

        for (pcount = params[i].pcount; pcount > 1; pcount--) {

            CINT_STRNCAT(_astr, "[0]", sizeof(_astr) - CINT_STRLEN(_astr));
        }

        if ((pcount == 1) && CINT_STRCMP(params[i].basetype, "void")) {

            CINT_SNPRINTF(_astr, sizeof(_astr), "%s[%s]", _astr, (CINT_PARAM_IS_VP(params[i].flags) ? ITER_V : "0"));
        }

        CINT_SNPRINTF(v1str, sizeof(v1str), "%s%s", params[i].name, _astr);

        CINT_SNPRINTF(v2str, sizeof(v2str), "%s%s%s", vpfx, params[i].name, _astr);

        pds[i] = params[i];

        pds[i].pcount = 0;

        if (CINT_IS_COMPARABLE(&pds[i])) {

            CINT_SNPRINTF_E(RUNNERS, "%sif (%s != %s) {\n", pfx1, v1str, v2str);

        } else {

            CINT_SNPRINTF_E(RUNNERS, "%sif (sal_memcmp(&%s, &%s, sizeof(%s)) != 0) {\n", pfx1, v1str, v2str, v1str);

        }

        CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"ERROR: %s did not match %s\\n\");\n", pfx1, v1str, v2str);

        CINT_SNPRINTF_E(RUNNERS, "%s    %s = %d;\n", pfx1, ARGS_CHECK_RES_V, i);

        CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx1);

        if (CINT_PARAM_IS_VP(params[i].flags)) {

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
    TOKEN_HANDLER(__ARGS_CHECK)(FIXED_ARGS, BEFORE_VPFX);
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

CINT_LOGGER_TESTS_TOKEN_HANDLER(ARGS_CHECK_AFTER)
{
    TOKEN_HANDLER(__ARGS_CHECK)(FIXED_ARGS, AFTER_VPFX);
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

        CINT_SNPRINTF_E(RUNNERS, "%s%s = &%s;\n", pfx1, VOID_PTR_V2, params[i].name);

        CINT_SNPRINTF_E(RUNNERS, "%sif (%s != %s) {\n", pfx1, VOID_PTR_V1, VOID_PTR_V2);

        CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"auto-injected variable #%d,%s does not map onto original argument\\n\");\n", pfx1, i, params[i].name);

        CINT_SNPRINTF_E(RUNNERS, "%s    printf(\"    $%s expected vs $%s found - expected when called from CINT\\n\");\n", pfx1, VOID_PTR_V1, VOID_PTR_V2);

        CINT_SNPRINTF_E(RUNNERS, "%s}\n", pfx1);

        CINT_SNPRINTF_E(RUNNERS, "\n");

        if (!params[i].pcount) {

            continue;
        }

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

CINT_LOGGER_TESTS_TOKEN_HANDLER(RETVAL)
{
    cint_parameter_desc_t *params;
    int rv;
    DECL_RUNNERS;

    INIT_RUNNERS;

    COPY_PREFIX;

    params = ctxt->params;

    rv = *(int*)output_values[0];

    CINT_SNPRINTF_E(RUNNERS, "%s = %d;", params[0].name, rv);

    COPY_SUFFIX;

    RETURN_TOTSZ;

    NULL_TERMINATION;

    OSZ_CHECK(RETVAL);
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
