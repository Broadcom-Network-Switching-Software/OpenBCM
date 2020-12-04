/*
 * $Id: cint_logger_tests_tokens.h
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger_tests_tokens.h
 * Purpose:     CINT logger tests tokens repeatedly included file
 */

#ifndef __CINT_LOGGER_TESTS_TOKENS_H__
/* { */
#define __CINT_LOGGER_TESTS_TOKENS_H__

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
/* { */

#ifdef CINT_LOGGER_TESTS_TOKEN
#undef CINT_LOGGER_TESTS_TOKEN
#endif

#define TOKEN_TYPE(tok) CINT_LOGGER_TESTS_TOKEN_ ## tok

#define CINT_LOGGER_TESTS_TOKEN(tok) TOKEN_TYPE(tok),

typedef enum {
#include "cint_logger_tests_tokens.i"
} cint_logger_tests_token_type_e;
#undef CINT_LOGGER_TESTS_TOKEN

typedef struct {
    int line_offset;
    int line_length;
    int offset;
    cint_logger_tests_token_type_e type;
} cint_logger_tests_token_t;

typedef void (*cint_logger_tests_token_handler_t)(
                cint_logger_api_params_t *ctxt,
                uint8 *input_values[CINT_CONFIG_MAX_FPARAMS+1],
                int i_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],
                uint8 *output_values[CINT_CONFIG_MAX_FPARAMS+1],
                int o_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],
                const char templ[],
                cint_logger_tests_token_t *token,
                char *obuf,
                int *osz);

extern void cint_logger_tests_tokens_init(void);

extern char* cint_logger_tests_gen_cint(cint_logger_api_params_t *ctxt,
                                  uint8 *input_values[CINT_CONFIG_MAX_FPARAMS+1],
                                  int i_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],
                                  uint8 *output_values[CINT_CONFIG_MAX_FPARAMS+1],
                                  int o_vl_array_cnt[CINT_CONFIG_MAX_FPARAMS+1],
                                  const char templ[],
                                  cint_logger_tests_token_t tok_offsets[]);




/* } */
#endif
/* } */
#endif /* __CINT_LOGGER_TESTS_TOKENS_H__ */
