/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        api_mode_yy.h
 * Purpose:     API mode lexer/parser interfaces
 */

#ifndef   _API_MODE_YY_H_
#define   _API_MODE_YY_H_

#include "api_mode.h"
#include "tokenizer.h"

typedef struct yyltype_s
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype, yyltype_t;

typedef struct api_mode_scanner_s {
    int                 idx;            /* token idx */
    api_mode_tokens_t   tok;            /* tokenizer output */
} api_mode_scanner_t;

typedef api_mode_scanner_t *yyscan_t;
typedef api_mode_arg_t *yystype_t;

/* Define interfaces for Bison */
#define YYSTYPE yystype_t
#define YYLTYPE yyltype_t

extern int api_mode_lex (YYSTYPE *lvalp, YYLTYPE *llocp, yyscan_t scanner);

#endif /* _API_MODE_YY_H_ */
