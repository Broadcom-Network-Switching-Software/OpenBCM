%{
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        api_grammar.y
 * Purpose:     API mode parser
 */
%}

/* API mode grammar */

%{
#include "api_mode_yy.h"
extern void api_mode_error(yyltype *loc, yyscan_t yyscanner,
                           void *parser, char const *s);
%}

%right '='

%define api.pure
%define api.push_pull "both"
%name-prefix="api_mode_"
%locations
%debug
%verbose
%error-verbose

%lex-param   { yyscan_t yyscanner }
%parse-param { yyscan_t yyscanner }
%parse-param { void *parser }

%token IDENT
%token KEY
%token CONSTANT
%token AGGREGATE
%token PROMPT
%token EMPTY
%token KEY_VALUE
%token RANGE
%token ITEM
%token ASSIGN
%token VALUE
%token PRINT
%token VAR
%token CREATE

%%

statements
    : statement
      { api_mode_execute(parser, $1); YYACCEPT; }
    | statements ';' statement
      { $$ = api_mode_append($1, api_mode_append($2,$3)); }
    ;

statement
    : executable
    | '!'                                       /* set info mode */
    | '!' executable
        { $$ = api_mode_append($1,$2); }        /* info */
    | identifiers '?'                           /* help */
        { $$ = api_mode_append($1,$2); }
    | PRINT selector_list                       /* print */
        { $$ = api_mode_append($1,$2); }
    | CREATE identifiers keys                   /* type constructor */
        { $$ =  api_mode_append($1,api_mode_append($2,$3)); }
    | VAR selector_list '=' argument           /* variable assignment */
        { $$ = api_mode_append($1,api_mode_append($2,$4)); }
    | VAR selectors PROMPT                      /* prompt-mode assignment */
        { $$ = api_mode_append($1,api_mode_append($2,$3)); }
    ;

executable
    : identifiers
    | identifiers PROMPT                        /* prompt */
        { $$ = api_mode_append($1,$2); }
    | identifiers positional_arguments          /* execute */
        { $$ = api_mode_append($1,$2); }
    | identifiers keyword_arguments             /* execute */
        { $$ = api_mode_append($1,$2); }
    ; 

identifiers
    : IDENT
        { $$ = api_mode_mark($1, IS_FIRST); }
    | identifiers IDENT
        { $$ = api_mode_append($1,$2); }
    ;

selectors
    : IDENT
        { $$ = api_mode_mark($1, IS_FIRST); }
    | selectors '.' IDENT
        { $$ = api_mode_sub_append($1,$3); }
    ;

selector_list
    : selectors
        { $$ = api_mode_mark($1, IS_FIRST); }
    | selector_list selectors
        { $$ = api_mode_append($1,$2); }
    ;

keys
    : KEY
        { $$ = api_mode_mark($1, IS_FIRST); }
    | keys KEY
        { $$ = api_mode_append($1,$2); }
    ;

positional_arguments
    : argument
        { $$ = api_mode_mark($1, IS_FIRST); }
    | positional_arguments argument
        { $$ = api_mode_append($1,$2); }
    ;

positional_item
    : ITEM
    | ITEM '-' ITEM
        { $$ = api_mode_range($1, $3, NULL, NULL); }
    | ITEM '*' CONSTANT
        { $$ = api_mode_range($1, NULL, NULL, $3); }
    | ITEM '-' ITEM '/' CONSTANT
        { $$ = api_mode_range($1, $3, NULL, $5); }
    | ITEM '*' ITEM '/' CONSTANT
        { $$ = api_mode_range($1, NULL, $3, $5); }
    ;

positional_list
    : positional_item
        { $$ = api_mode_mark($1, IS_FIRST); }
    | positional_list ',' positional_item
        { $$ = api_mode_append($1,$3); }
    ;

keyword_arguments
    : keyword_argument
        { $$ = api_mode_mark($1, IS_FIRST); }
    | keyword_arguments keyword_argument
        { $$ = api_mode_append($1,$2); }
    ;

keyword_argument
    : KEY '=' argument
        { $$ = api_mode_key_value($1,$3); }
    ;

argument
    : CONSTANT
    | VALUE
    | aggregate
    ;

aggregate
    : '{' aggregate_arguments '}'
        { $$ = api_mode_sub(api_mode_node(parser,"", AGGREGATE), $2); }
    | positional_list
        { $$ = api_mode_sub(api_mode_node(parser,"", AGGREGATE), $1); }
    ;

aggregate_arguments
    : keyword_arguments
    | positional_arguments
    | '='
        { $$ = api_mode_node(parser,"{=}", PROMPT); }
    ;

