/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tokenizer.h
 * Purpose:     API mode tokenizer
 */

#ifndef   _TOKENIZER_H_
#define   _TOKENIZER_H_

#include "cint_interpreter.h"

typedef enum api_mode_token_type_e {
    API_MODE_TOKEN_TYPE_NONE,
    API_MODE_TOKEN_TYPE_VALUE,
    API_MODE_TOKEN_TYPE_QUOTE,
    API_MODE_TOKEN_TYPE_SEPARATOR,
    API_MODE_TOKEN_TYPE_COMMENT,
    API_MODE_TOKEN_TYPE_ERROR
} api_mode_token_type_t;

typedef enum api_mode_token_error_e {
    API_MODE_TOKEN_ERROR_NONE,
    API_MODE_TOKEN_ERROR_INIT,
    API_MODE_TOKEN_ERROR_ESCAPE,
    API_MODE_TOKEN_ERROR_QUOTE,
    API_MODE_TOKEN_ERROR_CHAR,
    API_MODE_TOKEN_ERROR_UNEXPECTED,
    API_MODE_TOKEN_ERROR_STATE,
    API_MODE_TOKEN_ERROR_LAST
} api_mode_token_error_t;

typedef struct api_mode_token_s {
    const char *str;                    /* NUL terminated token string */
    int first_line;                     /* source buffer start line */
    int first_column;                   /* source buffer start column */
    int last_line;                      /* source buffer end line */
    int last_column;                    /* source buffer end column */
    int ident;                          /* indentifier? */
    api_mode_token_type_t token_type;   /* tokenizer type */
} api_mode_token_t;

typedef struct api_mode_tokens_s {
    int len;                            /* number of valid tokens */
    api_mode_token_error_t error;       /* error state */
    char *buf;                          /* token data memory buffer */
    api_mode_token_t *token;            /* array of 'len' tokens */
} api_mode_tokens_t;


extern int api_mode_tokenizer(const char *input, api_mode_tokens_t *tokens);
extern int api_mode_tokenizer_free(api_mode_tokens_t *tokens);
extern const char *api_mode_tokenizer_error_str(api_mode_token_error_t error);

#endif /* _TOKENIZER_H_ */
