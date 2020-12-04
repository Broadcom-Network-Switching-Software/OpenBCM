%{
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_grammar.y
 * Purpose:     CINT C Parser
 */
%}
/*
 * Parser Generator Options
 */

/* Pure, rentrant */
%define api.pure
%define api.push_pull "both"
%name-prefix="cint_c_"

/* Include location information */
%locations

/* The scanner object is the first argument to for the parser */
%parse-param { yyscan_t yyscanner }
/* The cint_c_parser_t control structure is the second parameter to the parse function */
%parse-param { cint_cparser_t* cparser }

/* The scanner object must be passed to yylex() */
%lex-param { yyscan_t yyscanner }

/*
 * TOKENS
 */

%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE T_VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

/* ADDED */
%token PRINT CINT

/* Programmable Language Extensions */
%token ITERATOR MACRO

%token XEOF 0

%{

#ifndef LONGEST_SOURCE_LINE
#define LONGEST_SOURCE_LINE 256
#endif

 typedef struct cint_c_parser_s {
     int x;
 } cint_c_parser_t;

typedef void* yyscan_t;

#define YY_TYPEDEF_YY_SCANNER_T
#define YYERROR_VERBOSE 1

#include "cint_config.h"
#include "cint_parser.h"

#include "cint_yy.h"
#include "cint_c.tab.h"


void cint_c_error(YYLTYPE * locp, yyscan_t yyscanner, cint_cparser_t * cp,
                  const char *msg);
extern int cint_c_lex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param,
                      yyscan_t yyscanner);
char *cint_current_line(yyscan_t yyscanner, char *const lineBuffer, const int lineLen,
                        int *column, int *tokLen, char **curFile, int *curLine);


#if CINT_CONFIG_INCLUDE_PARSER == 1


#include "cint_interpreter.h"


%}



%start translation_unit

%%

primary_expression
        : IDENTIFIER { $$ = $1; }
        | CONSTANT { $$ = $1; }
        | STRING_LITERAL { $$ = $1; }
        | '(' expression ')' { $$ = $2; }
	;

postfix_expression
        : primary_expression { $$ = $1; }
        | postfix_expression '[' expression ']' { $$ = cint_ast_operator(cintOpOpenBracket, $1, $3); }
	| postfix_expression '(' ')'
          {
              $$ = cint_ast_function($1, 0);
          }
	| postfix_expression '(' argument_expression_list ')'
          {
              $$ = cint_ast_function($1, $3);
          }
        | postfix_expression '.' IDENTIFIER
          {
              $$ = cint_ast_operator(cintOpDot, $1, $3);
          }
	| postfix_expression PTR_OP IDENTIFIER
          {
              $$ = cint_ast_operator(cintOpArrow, $1, $3);
          }
        | postfix_expression INC_OP { $$ = cint_ast_operator(cintOpIncrement, $1, 0); }
        | postfix_expression DEC_OP { $$ = cint_ast_operator(cintOpDecrement, $1, 0); }
	;

argument_expression_list
	: assignment_expression
        | argument_expression_list ',' assignment_expression
          {
              cint_ast_append($1, $3);
              $$ = $1;
          }
	;

unary_expression
        : postfix_expression { $$ = $1; }
        | INC_OP unary_expression        { $$ = cint_ast_operator(cintOpIncrement, 0, $2); }
	| DEC_OP unary_expression        { $$ = cint_ast_operator(cintOpDecrement, 0, $2); }
        | unary_operator cast_expression { $$ = cint_ast_operator(cint_ast_int($1), 0, $2); }
        | SIZEOF unary_expression        { $$ = cint_ast_operator(cintOpSizeof, 0, $2); }
	| SIZEOF '(' type_name ')'       { $$ = cint_ast_operator(cintOpSizeof, 0, $3); }
	;

unary_operator
        : '&' { $$ = cint_ast_integer(cintOpAddressOf); }
	| '*' { $$ = cint_ast_integer(cintOpDereference); }
	| '+' { $$ = cint_ast_integer(cintOpPositive); }
	| '-' { $$ = cint_ast_integer(cintOpNegative); }
	| '~' { $$ = cint_ast_integer(cintOpTilde); }
	| '!' { $$ = cint_ast_integer(cintOpNot); }
	;

cast_expression
	: unary_expression
/*      | '(' type_name ')' cast_expression { $$ = cint_ast_operator(cintOpTypecast, $2, $4); } */

        | '(' T_VOID '*' ')' cast_expression { $$ = cint_ast_operator(cintOpTypecast, CINT_AST_PTR_VOID, $5); }

        | '(' AUTO ')' cast_expression { $$ = cint_ast_operator(cintOpTypecast, CINT_AST_PTR_AUTO, $4); }
	;

multiplicative_expression
	: cast_expression
        | multiplicative_expression '*' cast_expression { $$ = cint_ast_operator(cintOpMultiply, $1, $3); }
        | multiplicative_expression '/' cast_expression { $$ = cint_ast_operator(cintOpDivide, $1, $3); }
        | multiplicative_expression '%' cast_expression { $$ = cint_ast_operator(cintOpMod, $1, $3); }
	;

additive_expression
	: multiplicative_expression
        | additive_expression '+' multiplicative_expression { $$ = cint_ast_operator(cintOpAdd, $1, $3); }
	| additive_expression '-' multiplicative_expression { $$ = cint_ast_operator(cintOpSubtract, $1, $3); }
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression { $$ = cint_ast_operator(cintOpLeftShift, $1, $3); }
	| shift_expression RIGHT_OP additive_expression { $$ = cint_ast_operator(cintOpRightShift, $1, $3); }
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression { $$ = cint_ast_operator(cintOpLessThan, $1, $3); }
	| relational_expression '>' shift_expression { $$ = cint_ast_operator(cintOpGreaterThan, $1, $3); }
	| relational_expression LE_OP shift_expression { $$ = cint_ast_operator(cintOpLessThanOrEqual, $1, $3); }
	| relational_expression GE_OP shift_expression { $$ = cint_ast_operator(cintOpGreaterThanOrEqual, $1, $3); }
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression { $$ = cint_ast_operator(cintOpEqual, $1, $3); }
	| equality_expression NE_OP relational_expression { $$ = cint_ast_operator(cintOpNotEqual, $1, $3); }
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression { $$ = cint_ast_operator(cintOpBitwiseAnd, $1, $3); }
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression { $$ = cint_ast_operator(cintOpBitwiseXor, $1, $3); }
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression { $$ = cint_ast_operator(cintOpBitwiseOr, $1, $3); }
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression { $$ = cint_ast_operator(cintOpLogicalAnd, $1, $3); }
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression { $$ = cint_ast_operator(cintOpLogicalOr, $1, $3); }
	;

conditional_expression
	: logical_or_expression
        | logical_or_expression '?' expression ':' conditional_expression
          { $$ = cint_ast_ternary($1, $3, $5); }
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
          {
              /*
               * Arithmetic assignment operators are converted to distinct operator/= expressions
               * for the benefit of simplifying the interpreter operator logic.
               *
               * Convert any expressions of the form "x <op>= y" to "x = x <op> y";
 */
              $$ = cint_ast_operator(cint_ast_int($2), $1, $3);
              if(cint_ast_int($2) != cintOpAssign) {
                  /* Perform the operator on the left and right and assign it to the left */
                  $$ = cint_ast_operator(cintOpAssign, $1, $$);
              }
          }
	;

assignment_operator
        : '='                   { $$ = cint_ast_integer(cintOpAssign); }
        | MUL_ASSIGN            { $$ = cint_ast_integer(cintOpMultiply); }
	| DIV_ASSIGN            { $$ = cint_ast_integer(cintOpDivide); }
	| MOD_ASSIGN            { $$ = cint_ast_integer(cintOpMod); }
	| ADD_ASSIGN            { $$ = cint_ast_integer(cintOpAdd); }
	| SUB_ASSIGN            { $$ = cint_ast_integer(cintOpSubtract); }
	| LEFT_ASSIGN           { $$ = cint_ast_integer(cintOpLeftShift); }
	| RIGHT_ASSIGN          { $$ = cint_ast_integer(cintOpRightShift); }
	| AND_ASSIGN            { $$ = cint_ast_integer(cintOpBitwiseAnd); }
	| XOR_ASSIGN            { $$ = cint_ast_integer(cintOpBitwiseXor); }
	| OR_ASSIGN             { $$ = cint_ast_integer(cintOpBitwiseOr); }
	;

expression
        : assignment_expression { $$ = $1; }
	| expression ',' assignment_expression
          {
              $$ = cint_ast_comma($1, $3);
          }
	;

constant_expression
	: conditional_expression
	;

declaration
        : declaration_specifiers ';' { $$ = $1; }
        | declaration_specifiers init_declarator_list ';'
          {
              $$ = cint_ast_declaration_init($1, $2);
          }
	;

declaration_specifiers
        : storage_class_specifier {$$ = $1; }
        | storage_class_specifier declaration_specifiers { cint_ast_append($1, $2); $$ = $1; }
        | type_specifier { $$ = $1; }
        | type_specifier declaration_specifiers { cint_ast_append($1, $2); $$ = $1; }
        | type_qualifier
        | type_qualifier declaration_specifiers { cint_ast_append($1, $2); $$ = $1; }
	;

init_declarator_list
	: init_declarator
        | init_declarator_list ',' init_declarator { cint_ast_append($1, $3); $$ = $1; }
	;

init_declarator
        : declarator { $$ = $1; }
        | declarator '=' initializer
          { $$ = cint_ast_declarator_init($1, $3); }
	;

storage_class_specifier
        : EXTERN        { $$ = cint_ast_integer(CINT_AST_TYPE_F_EXTERN); }
        | TYPEDEF       { $$ = cint_ast_integer(CINT_AST_TYPE_F_TYPEDEF); }
        | STATIC        { $$ = cint_ast_integer(CINT_AST_TYPE_F_STATIC); }
        | AUTO          /* IGNORED */
        | REGISTER      /* IGNORED */
	;

type_specifier
	: T_VOID      { $$ = cint_ast_type("void"); }
	| CHAR      { $$ = cint_ast_type("char"); }
	| SHORT     { $$ = cint_ast_type("short"); }
	| INT       { $$ = cint_ast_type("int"); }
        | LONG      { $$ = cint_ast_type("long"); }
	| FLOAT     { $$ = cint_ast_type("float"); }
	| DOUBLE    { $$ = cint_ast_type("double"); }
	| SIGNED    { $$ = cint_ast_integer(CINT_AST_TYPE_F_SIGNED); }
	| UNSIGNED  { $$ = cint_ast_integer(CINT_AST_TYPE_F_UNSIGNED); }
	| struct_or_union_specifier
	| enum_specifier
        | TYPE_NAME { $$ = $1; }
	;

struct_or_union_specifier
        : struct_or_union IDENTIFIER '{' struct_declaration_list '}' { $$ = cint_ast_structure_def($2, $4); }
        | struct_or_union '{' struct_declaration_list '}' { $$ = cint_ast_structure_def(0, $3); }
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
        | struct_declaration_list struct_declaration { cint_ast_append($1, $2); $$ = $1; }
	;

struct_declaration
    : specifier_qualifier_list struct_declarator_list ';'
      {
          $$ = cint_ast_struct_declaration($1, $2);
      }

	;

struct_declarator_list
	: struct_declarator
        | struct_declarator_list ',' struct_declarator { cint_ast_append($1, $3); $$ = $1; }
	;

struct_declarator
	: declarator
/*	| ':' constant_expression                       NOT SUPPORTED */
/*	| declarator ':' constant_expression            NOT SUPPORTED */
	;

enum_specifier
        : ENUM '{' enumerator_list '}'
        | ENUM IDENTIFIER '{' enumerator_list '}' { $$ = cint_ast_enumdef($2, $4); }
	| ENUM IDENTIFIER
	;

enumerator_list
        : enumerator { $$ = $1; }
        | enumerator_list ',' enumerator { cint_ast_append($1, $3); $$ = $1; }
	;

enumerator
        : IDENTIFIER { $$ = cint_ast_enumerator($1, 0); }
        | IDENTIFIER '=' constant_expression { $$ = cint_ast_enumerator($1, $3); }
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list { cint_ast_append($1, $2); $$ = $1; }
        | type_specifier { $$ = $1; }
        | type_qualifier specifier_qualifier_list { cint_ast_append($1, $2); $$ = $1; }
        | type_qualifier { $$ = $1; }
	;

type_qualifier
        : CONST         { $$ = cint_ast_integer(CINT_AST_TYPE_F_CONST); }
        | VOLATILE      { $$ = cint_ast_integer(CINT_AST_TYPE_F_VOLATILE); }
	;

declarator
        : pointer direct_declarator
        {
            $$ = cint_ast_pointer_declarator($1, $2);
        }
        | direct_declarator { $$ = $1; }
	;

direct_declarator
        : IDENTIFIER { $$ = cint_ast_identifier_declarator($1); }
	| '(' declarator ')'
	| direct_declarator '[' constant_expression ']'
          { $$ = cint_ast_array_declarator($1, $3); }
	| direct_declarator '[' ']'
          { $$ = cint_ast_array_declarator($1, cint_ast_integer(-1)); }
	| direct_declarator '(' parameter_type_list ')'
          {
              $$ = cint_ast_function_declarator($1, $3);
          }
/*	| direct_declarator '(' identifier_list ')'       K&R NOT SUPPORTED */
	| direct_declarator '(' ')'
          {
              $$ = cint_ast_function_declarator($1, 0);
          }
	;

pointer
        : '*' { $$ = cint_ast_integer(1); }
	| '*' type_qualifier_list { $$ = cint_ast_integer(1); }                /* QUALIFIER NOT SUPPORTED */
	| '*' pointer { $$ = cint_ast_pointer_indirect($2); }
        | '*' type_qualifier_list pointer               /* NOT SUPPORTED */
	;

type_qualifier_list
        : type_qualifier
        | type_qualifier_list type_qualifier { cint_ast_append($1, $2); $$ = $1; }
	;


parameter_type_list
	: parameter_list
/*	| parameter_list ',' ELLIPSIS           REMOVED */
	;

parameter_list
        : parameter_declaration {$$ = $1;}
	| parameter_list ',' parameter_declaration
          {
              cint_ast_append($1, $3);
              $$ = $1;
          }
	;

parameter_declaration
        : declaration_specifiers declarator
          {
              $$ = cint_ast_parameter_declaration_append($1, $2);
          }
	| declaration_specifiers abstract_declarator
        | declaration_specifiers
          {
              $$ = cint_ast_parameter_declaration($1);
          }
	;

/*
 * K&R not supported
identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;
 */

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
        : assignment_expression         { $$ = $1; }
        | '{' initializer_list '}'      { $$ = cint_ast_initializer($2); }
        | '{' initializer_list ',' '}'  { $$ = cint_ast_initializer($2); }
	;

initializer_list
	: initializer
        | initializer_list ',' initializer { cint_ast_append($1, $3); $$ = $1; }
	;

statement
	: labeled_statement                     /* NOT SUPPORTED */
        | declaration_statement                 /*  ADDED -- declarations are now statements */
	| compound_statement
          {
              $$ = cint_ast_compound_statement($1);
          }

	| expression_statement
	| selection_statement
	| iteration_statement {$$=$1;}
	| jump_statement
        | print_statement     /* ADDED */
        | cint_statement     /* ADDED */
        | IDENTIFIER ';' { $$ = cint_ast_statement_with_no_effect($1); }
        | CONSTANT ';' { $$ = cint_ast_statement_with_no_effect($1); }
        | STRING_LITERAL ';' { $$ = cint_ast_statement_with_no_effect($1); }
	;


print_statement
        : PRINT expression_statement { $$ = cint_ast_print($2); }
        | PRINT TYPE_NAME ';' { $$ = cint_ast_print($2); }


keyword_arg
: SIZEOF        { $$ = cint_ast_string("sizeof"); }
| TYPEDEF       { $$ = cint_ast_string("typedef"); }
| EXTERN        { $$ = cint_ast_string("extern"); }
| STATIC        { $$ = cint_ast_string("static"); }
| AUTO          { $$ = cint_ast_string("auto"); }
| REGISTER      { $$ = cint_ast_string("register"); }
| CHAR          { $$ = cint_ast_string("char"); }
| SHORT         { $$ = cint_ast_string("short"); }
| INT           { $$ = cint_ast_string("int"); }
| LONG          { $$ = cint_ast_string("long"); }
| SIGNED        { $$ = cint_ast_string("signed"); }
| UNSIGNED      { $$ = cint_ast_string("unsigned"); }
| FLOAT         { $$ = cint_ast_string("float"); }
| DOUBLE        { $$ = cint_ast_string("double"); }
| CONST         { $$ = cint_ast_string("const"); }
| VOLATILE      { $$ = cint_ast_string("volatile"); }
| T_VOID        { $$ = cint_ast_string("void"); }
| STRUCT        { $$ = cint_ast_string("struct"); }
| UNION         { $$ = cint_ast_string("union"); }
| ENUM          { $$ = cint_ast_string("enum"); }
| CASE          { $$ = cint_ast_string("case"); }
| DEFAULT       { $$ = cint_ast_string("default"); }
| IF            { $$ = cint_ast_string("if"); }
| ELSE          { $$ = cint_ast_string("else"); }
| SWITCH        { $$ = cint_ast_string("switch"); }
| WHILE         { $$ = cint_ast_string("while"); }
| DO            { $$ = cint_ast_string("do"); }
| FOR           { $$ = cint_ast_string("for"); }
| GOTO          { $$ = cint_ast_string("goto"); }
| CONTINUE      { $$ = cint_ast_string("continue"); }
| BREAK         { $$ = cint_ast_string("break"); }
| RETURN        { $$ = cint_ast_string("return"); }
| CINT          { $$ = cint_ast_string("cint"); }
| PRINT         { $$ = cint_ast_string("print"); }




cint_argument
        : IDENTIFIER
        | TYPE_NAME
        | STRING_LITERAL
        | CONSTANT
        | keyword_arg


cint_argument_list
        : cint_argument
        | cint_argument_list cint_argument
          {
              cint_ast_append($1, $2);
              $$ = $1;
          }


cint_statement
        : CINT cint_argument_list ';' { $$ = cint_ast_cint($2); }

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement       { $$ = cint_ast_case($2, $4); }
        | DEFAULT ':' statement                        { $$ = cint_ast_case(0, $3); }
	;

declaration_statement
        : declaration

compound_statement
        : '{' '}' { $$ = cint_ast_empty(); }
        | '{' statement_list '}' { $$ = $2; }
/*        | '{' declaration_list '}'                    REMOVED -- declarations are now statements */
/*        | '{' declaration_list statement_list '}'     REMOVED -- declarations are now statements */
	;

declaration_list
        : declaration { $$ = $1; }
        | declaration_list declaration { $$ = $1; cint_ast_append($1, $2); }
	;

statement_list
        : statement { $$ = $1; }
        | statement_list statement
          {
              cint_ast_append($1, $2);
              $$ = $1;
          }
	;

expression_statement
        : ';' { $$ = cint_ast_empty(); }
	| expression ';'
	;

selection_statement
        : IF '(' expression ')' statement
          {
              $$ = cint_ast_if($3, $5, 0);
          }
	| IF '(' expression ')' statement ELSE statement
          {
              $$ = cint_ast_if($3, $5, $7);
          }
        | SWITCH '(' expression ')' statement { $$ = cint_ast_switch($3, $5); }
        | MACRO '(' argument_expression_list ')' ';' { $$ = cint_interpreter_macro($1, $3); }
	;

iteration_statement
	: WHILE '(' expression ')' statement
          {
              $$ = cint_ast_while($3, $5, 0);
          }
	| DO statement WHILE '(' expression ')' ';'
          {
              $$ = cint_ast_while($5, $2, 1);
          }
	| FOR '(' expression_statement expression_statement ')' statement
          {
              $$ = cint_ast_for($3, $4, 0, $6);
          }
	| FOR '(' expression_statement expression_statement expression ')' statement
          {
              $$ = cint_ast_for($3, $4, $5, $7);
          }
        | ITERATOR '(' argument_expression_list ')' statement
          {
              $$ = cint_interpreter_iterator($1, $3, $5);
          }
        | ITERATOR '(' ')' statement
          {
              $$ = cint_interpreter_iterator($1, 0, $3);
          }
	;

jump_statement
	: GOTO IDENTIFIER ';'                           /* NOT SUPPORTED */
        | CONTINUE ';'          { $$ = cint_ast_continue(); }
        | BREAK ';'             { $$ = cint_ast_break(); }
        | RETURN ';'            { $$ = cint_ast_return(0); }
        | RETURN expression ';' { $$ = cint_ast_return($2); }
	;


translation_unit
        : external_declaration
        {
            /* Accept and append this translation unit */
            if(cparser->result) {
                cint_ast_append(cparser->result, $1);
            }
            else {
                cparser->result = $1;
            }

            if(yychar == YYEMPTY) {
                /*
                 * This unit is complete and we have no dangling lookahead.
                 * Return to the application.
                 */
                YYACCEPT;
            }
            else {
                /*
                 * A lookahead has been consumed. If we were to return from parsing
                 * now we would lose the lookahead on the next invokation.
                 *
                 * We will continue to parse units until it is safe to return.
                 */
            }
        }
	| translation_unit external_declaration
        {
            /* Accept and append this translation unit */
            if(cparser->result) {
                cint_ast_append(cparser->result, $2);
            }
            else {
                cparser->result = $1;
            }

            if(yychar == YYEMPTY) {
                /*
                 * This unit is complete and we have no dangling lookahead.
                 * Return to the application.
                 */
                YYACCEPT;
            }
            else {
                /*
                 * A lookahead has been consumed. If we were to return from parsing
                 * now we would lose the lookahead on the next invokation.
                 *
                 * We will continue to parse units until it is safe to return.
                 */
            }
        }
        | XEOF /* Allow Empty Files/Translation Units */
        {
            cparser->result = 0;
            YYACCEPT;
        }
	;


external_declaration
        : function_definition
/*	| declaration              REMOVED -- Statements are also declarations */
        | statement;            /* ADDED -- allow statements to be interpreted at top level */
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement /* K&R NOT SUPPORTED */
	| declaration_specifiers declarator compound_statement
          {
              $$ = cint_ast_function_definition($1, $2, $3);
          }
/*      | declarator declaration_list compound_statement     REMOVED */
/*	| declarator compound_statement                        REMOVED */
	;

%%

#include "cint_porting.h"

void cint_c_error(YYLTYPE* locp, yyscan_t yyscanner, cint_cparser_t* cp, const char* msg)
{
    const int sourceLineLen = LONGEST_SOURCE_LINE;      /* Truncate source lines longer than 256 characters. */
    char sourceLine[LONGEST_SOURCE_LINE];
    char errLine[LONGEST_SOURCE_LINE];
    char *errPtr = errLine;
    int errCol;
    int tokLen;
    int i;
    char *currentFileName;
    int currentLineNum;

    (void) cint_current_line(yyscanner, sourceLine, sourceLineLen, &errCol,
                             &tokLen, &currentFileName, &currentLineNum);
    if (sourceLine[0] && !cint_cparser_interactive()) {
        /* Print current source line (if there is one) and not interactive */
        CINT_PRINTF("%s\n", sourceLine);
    }
    if (tokLen) {
        /* Create a "marker" line pointing to offending token. */
        for (i = 0; i < errCol; i++) {
            *errPtr++ = ' ';
        }
        for (i = 0; i < tokLen; i++) {
            *errPtr++ = '^';
        }
        *errPtr = 0;
	errPtr = errLine;
    } else {
        /* If tokLen is zero, either the line was longer than max allowed or current line is empty. */
        errPtr = "[No current line]";
    }

    /* print marker and parser message */
    if (currentFileName) {
        /* If file name is NULL, we're at the top */
        CINT_PRINTF("%s %s [%s:%d]\n", errPtr, msg, currentFileName, currentLineNum);
    } else if (currentLineNum) {
        CINT_PRINTF("%s %s [%d]\n", errPtr, msg, currentLineNum);
    } else {
        CINT_PRINTF("%s %s\n", errPtr, msg);
    }
}


#else /* CINT_CONFIG_INCLUDE_PARSER */
int cint_grammar_c_not_empty;
#endif
