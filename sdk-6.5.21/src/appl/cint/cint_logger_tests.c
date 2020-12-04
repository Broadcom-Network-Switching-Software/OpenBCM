/*
 * $Id: cint_logger_test.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger_test.c
 * Purpose:     CINT logger Self Test Routines
 */

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <appl/diag/cmdlist.h>

#include "cint_porting.h"
#include "cint_interpreter.h"

#include "cint_logger.h"
#include "cint_logger_tests_tokens.h"

/*
 * CINT Logger Tests
 */

/* include the CINT script template */
#include "cint_logger_inline_tests.i"


#define CINT_LOGGER_TESTS_MAX_VL 10
#define CINT_LOGGER_TESTS_MAX_STRLEN 200

#define ISALNUM(c) \
    ((c >= '0' && c <= '9') || \
     (c >= 'a' && c <= 'z') || \
     (c >= 'A' && c <= 'Z'))

extern void cint_reset(void);

static void
cint_logger_fill_random (void *buffer, int len, int isstring)
{
   uint8 c, *dst;

   LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
             (BSL_META("0x%p - filling %d random bytes\n"), buffer, len));

   for (dst = buffer; dst && (len > 0); ) {

        c = CINT_RAND() & 0xFF;

        if (isstring && !ISALNUM(c)) {

            continue;
        }

        *dst = c;
        dst++;
        len--;
   }
}



static void
cint_logger_tests_arg_values (cint_logger_api_params_t *ctxt,
                               uint8 *random_values[CINT_CONFIG_MAX_FPARAMS+1],
                               int vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1])
{
    cint_parameter_desc_t *params;
    uint8 *dst;
    int i, len, nargs, isstring;

    params = ctxt->params;
    nargs = ctxt->nargs;

    /*
     * Compute random values for non-pointer and pointed arguments. Note that
     * pointers cannot be random - the logger will crash dereferencing invalid
     * pointers. Pointers can be NULL though, and that should be handled
     * gracefully by the logger.
     */
    for (i = 0, dst = random_values[i];
         (i < nargs) && params[i].name;
         i++, dst = random_values[i]) {

        if (!dst) {

            continue;
        }

        isstring = SHR_BITGET(ctxt->is_charptr,i);

        len = vl_array_cnt[i] * ctxt->basetype_size[i];

        LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("%s: 0x%p filling %d random bytes\n"), params[i].name, dst, len));

        cint_logger_fill_random(dst, len, isstring);

        if (isstring) {

            *(dst+len) = '\0';
        }
    }
}






static cmd_result_t
cint_logger_inline_tests_no_arg_changes (cint_logger_api_params_t *ctxt, int unit, int rv)
{
    cint_parameter_desc_t *params;
    uint8 *input_values[CINT_CONFIG_MAX_FPARAMS+1];
    int i_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1];
    uint8 *output_values[CINT_CONFIG_MAX_FPARAMS+1];
    int o_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1];
    int i, nargs, is_charptr, alloc_sz, trv;
    int array_cnt_indx, array_cnt_size;
    void *array_cnt_ptr;
    char *cint;
    cmd_result_t result = CMD_OK;


    params = ctxt->params;
    nargs = ctxt->nargs;

    for (i = 0; (i < nargs) && params[i].name; i++) {

        if (params[i].flags & CINT_PARAM_VP) {

            i_vl_array_cnt[i] = (CINT_RAND() * CINT_LOGGER_TESTS_MAX_VL) / (CINT_RAND_MAX+1);
        } else {

            i_vl_array_cnt[i] = (!CINT_STRCMP(params[i].basetype, "void") ? 0 : 1);
        }

        is_charptr = SHR_BITGET(ctxt->is_charptr,i);
        if (is_charptr) {

            i_vl_array_cnt[i] = (CINT_RAND() * CINT_LOGGER_TESTS_MAX_STRLEN) / (CINT_RAND_MAX+1);
        }

        alloc_sz = i_vl_array_cnt[i] * ctxt->basetype_size[i];
        alloc_sz += (is_charptr) ? 1 : 0;

        input_values[i]  = CINT_MALLOC(alloc_sz);

        LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("%s: array_cnt %d alloc_sz %d ptr 0x%p\n"), params[i].name, i_vl_array_cnt[i], alloc_sz, input_values[i]));

    }

    cint_logger_tests_arg_values(ctxt, input_values,  i_vl_array_cnt);

    /*
     * Now that we have random values, sanitize some inputs which cannot really
     * be random. "unit" cannot be random because APIs check for its validity
     * even before dispatch. Inputs holding lengths of variable length arrays
     * also cannot be random and must equal the actual length.
     */

    *(int*) input_values[1] = unit;

    for (i = 0; (i < nargs) && params[i].name; i++) {

        if (params[i].flags & CINT_PARAM_VP) {

            array_cnt_indx = params[i].flags & CINT_PARAM_IDX;

            array_cnt_size = ctxt->basetype_size[array_cnt_indx];
            

            array_cnt_ptr = input_values[array_cnt_indx];

            /* expect array_cnt to be an integer-like variable */
            switch (array_cnt_size) {
            case sizeof(uint8)  : *(uint8*)  array_cnt_ptr = (uint8)  i_vl_array_cnt[i]; break;
            case sizeof(uint16) : *(uint16*) array_cnt_ptr = (uint16) i_vl_array_cnt[i]; break;
            case sizeof(uint32) : *(uint32*) array_cnt_ptr = (uint32) i_vl_array_cnt[i]; break;
            case sizeof(uint64) : COMPILER_64_SET(*(uint64*) array_cnt_ptr, 0, i_vl_array_cnt[i]); break;
            default: break;
            }
        }
    }

    /*cint_logger_tests_arg_values(ctxt, output_values, o_vl_array_cnt);*/
    CINT_MEMCPY(output_values, input_values, sizeof(output_values));
    CINT_MEMCPY(o_vl_array_cnt, i_vl_array_cnt, sizeof(o_vl_array_cnt));

    /*
     * Mostly same as the input_values, except the return value
     */

    *(int*)output_values[0] = rv;


    cint = cint_logger_tests_gen_cint(ctxt,
                                      input_values,
                                      i_vl_array_cnt,
                                      output_values,
                                      o_vl_array_cnt,
                                      inline_test_templ,
                                      inline_test_templ_tok_offsets);

    for (i = 0; params[i].name && (i < nargs); i++) {

        if (input_values[i]) {

            LOG_DEBUG(BSL_LS_APPL_CINTAPILOGGER,
                      (BSL_META("%s: freeing ptr 0x%p\n"), params[i].name, input_values[i]));
            CINT_FREE(input_values[i]);
        }

        /*
        if (output_values[i]) {

            CINT_FREE(output_values[i]);
        }
        */
    }

    if (!cint) {

        LOG_ERROR(BSL_LS_APPL_CINTAPILOGGER,
                  (BSL_META("%s Test-case could not be run. Failed to generate CINT\n"), ctxt->fn));
        return 0;
    }
    /*
     * EXECUTE THE CINT!!! Reset CINT state before and after
     */
    cint_reset();

    trv = cint_interpreter_parse_and_eval_string(cint);

    cint_reset();

    CINT_FREE(cint);

    LOG_INFO(BSL_LS_APPL_CINTAPILOGGER, (BSL_META("%s Test-case "), ctxt->fn));

    if (trv != 0) {

        LOG_INFO(BSL_LS_APPL_CINTAPILOGGER, (BSL_META("FAILED: %d\n"), trv));
        result = CMD_FAIL;
    } else {

        LOG_INFO(BSL_LS_APPL_CINTAPILOGGER, (BSL_META("PASSED.\n")));
        result = CMD_OK;
    }

    return result;
}

cmd_result_t
cint_logger_test_api_using_cint (const char *api, int unit, int rv)
{
    cint_logger_api_params_t _ctxt, *ctxt;

    ctxt = &_ctxt;

    cint_logger_api_params_t_init(ctxt);

    ctxt->fn = api;

    cint_logger_init_api_params(ctxt);

    if (!ctxt->params) {
        /* API not found */
        return CMD_FAIL;
    }

    cint_logger_tests_tokens_init();

    return cint_logger_inline_tests_no_arg_changes(ctxt, unit, rv);

}

/* } */
#else
/* { */
/* Make ISO compilers happy. */
typedef int cint_logger_tests_c_not_empty;
/* } */
#endif
