/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_ast.h
 * Purpose:     CINT AST node interfaces
 */

#ifndef __CINT_AST_H__
#define __CINT_AST_H__

#include "cint_config.h"
#include "cint_operators.h"

typedef enum cint_ast_type_e {

    cintAstNone = -1,

#define CINT_AST_LIST_ENTRY(_entry) cintAst##_entry ,
#include "cint_ast_entry.h"

    cintAstLast

} cint_ast_type_t;

#define CINT_AST_TYPE_VALID(_e) (_e > cintAstNone && _e < cintAstLast)

typedef struct cint_ast_s {
    /* Internal Use Only */
    struct cint_ast_s* inext;


    struct cint_ast_s* next;

    const char* file;
    int line;

    int refcount;
    int noexec;
    cint_ast_type_t ntype;

    /* Node Types */
    union {

        /* Constant Integer */
        struct {
            long i;
        } integer;
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
        /* Constant Long Long */
        struct {
            long long i;
        } _longlong;
#endif

#if CINT_CONFIG_INCLUDE_DOUBLES == 1
        /* Constant Double */
        struct {
            double d;
        } _double;
#endif


        /* Constant String */
        struct {
            const char* s;
        } string;


        /* Type */

#define CINT_AST_TYPE_F_SIGNED          0x1
#define CINT_AST_TYPE_F_UNSIGNED        0x2
#define CINT_AST_TYPE_F_STATIC          0x4
#define CINT_AST_TYPE_F_EXTERN          0x8
#define CINT_AST_TYPE_F_CONST           0x10
#define CINT_AST_TYPE_F_TYPEDEF         0x20
#define CINT_AST_TYPE_F_VOLATILE        0x40

        struct {
            const char* s;
        } type;


        /* Identifier */
        struct {
            const char* s;
        } identifier;


        /* Declaration */
        struct {
            struct cint_ast_s* type;
            int pcount;
            int num_dimension_initializers;
            struct cint_ast_s*
                dimension_initializers[CINT_CONFIG_ARRAY_DIMENSION_LIMIT];
            struct cint_ast_s* identifier;
            struct cint_ast_s* init;
        } declaration;

        struct {
            struct cint_ast_s* initializers;
        } initializer;

        /* Operator */
        struct {
            cint_operator_t op;
            struct cint_ast_s* left;
            struct cint_ast_s* right;
            struct cint_ast_s* extra;
        } operator;


        /* Function Call */
        struct {
            const char* name;
            struct cint_ast_s* parameters;
            struct cint_datatype_s* dtp;
        } function;

        /* Function Definition */
        struct {
            struct cint_ast_s* declaration;
            struct cint_ast_s* parameters;
            struct cint_ast_s* statements;
        } functiondef;

        /* Structure Definition */
        struct {
            struct cint_ast_s* name;
            struct cint_ast_s* members;
        } structuredef;

        /* Expression List */
        struct {
            struct cint_ast_s* list;
        } elist;

        /* While */
        struct {
            struct cint_ast_s* condition;
            struct cint_ast_s* statements;
            int order;
        } _while;


        /* For */
        struct {
            struct cint_ast_s* pre;
            struct cint_ast_s* condition;
            struct cint_ast_s* post;
            struct cint_ast_s* statements;
        } _for;


        /* If */
        struct {
            struct cint_ast_s* condition;
            struct cint_ast_s* statements;
            struct cint_ast_s* _else;
        } _if;

        /* Return */
        struct {
            struct cint_ast_s* expression;
        } _return;

        /* Switch */
        struct {
            struct cint_ast_s* expression;
            struct cint_ast_s* statements;
        } _switch;

        /* Case */
        struct {
            struct cint_ast_s* expression;
            struct cint_ast_s* statements;
        } _case;

        /* Enumerator */
        struct {
            struct cint_ast_s* identifier;
            struct cint_ast_s* value;
        } enumerator;

        /* EnumDef */
        struct {
            struct cint_ast_s* identifier;
            struct cint_ast_s* enumerators;
        } enumdef;

        /* Print */
        struct {
            struct cint_ast_s* expression;
        } print;

        /* Interpreter Commands */
        struct {
            struct cint_ast_s* arguments;
        } cint;

    } utype;


} cint_ast_t;

typedef enum cint_ast_const_e {
    cintAstConstHex,
    cintAstConstOctal,
    cintAstConstDecimal,
    cintAstConstChar,
    cintAstConstFloat
} cint_ast_const_t;

/* Return TRUE if the AST is of the indicated type */
#define CINT_AST(ast, ty) (((ast) != NULL) && ((ast)->ntype == cintAst##ty))

#define CINT_AST_PTR_VOID ((cint_ast_t*)0x1)
#define CINT_AST_PTR_AUTO ((cint_ast_t*)0x2)

extern cint_ast_t* cint_ast_integer(int i);
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
extern cint_ast_t* cint_ast_long_long(long long i);
#endif
#if CINT_CONFIG_INCLUDE_DOUBLES == 1
extern cint_ast_t* cint_ast_double(double d);
#endif
extern cint_ast_t* cint_ast_constant(const char* s, cint_ast_const_t ctype);
extern cint_ast_t* cint_ast_string(const char* s);
extern cint_ast_t* cint_ast_var(const char* s);
extern cint_ast_t* cint_ast_operator(cint_operator_t operator, cint_ast_t* left, cint_ast_t* right);
extern cint_ast_t* cint_ast_statement(cint_ast_t* s);
extern cint_ast_t* cint_ast_identifier(const char* s);
extern cint_ast_t* cint_ast_initializer(cint_ast_t* inits);
extern cint_ast_t* cint_ast_type(const char* s);
extern cint_ast_t* cint_ast_declaration(void);
extern cint_ast_t* cint_ast_function(cint_ast_t* expr, cint_ast_t* args);
extern cint_ast_t* cint_ast_function_def(void);
extern cint_ast_t* cint_ast_structure_def(cint_ast_t* name, cint_ast_t* members);
extern cint_ast_t* cint_ast_elist(cint_ast_t* first);
extern cint_ast_t* cint_ast_while(cint_ast_t* expr, cint_ast_t* statements, int order);
extern cint_ast_t* cint_ast_for(cint_ast_t* pre, cint_ast_t* cond, cint_ast_t* post, cint_ast_t* statements);
extern cint_ast_t* cint_ast_if(cint_ast_t* expr, cint_ast_t* statements, cint_ast_t* _else);
extern cint_ast_t* cint_ast_case(cint_ast_t* expr, cint_ast_t* statements);
extern cint_ast_t* cint_ast_switch(cint_ast_t* expr, cint_ast_t* statements);
extern cint_ast_t* cint_ast_print(cint_ast_t* expr);
extern cint_ast_t* cint_ast_cint(cint_ast_t* args);
extern cint_ast_t* cint_ast_continue(void);
extern cint_ast_t* cint_ast_break(void);
extern cint_ast_t* cint_ast_return(cint_ast_t* expr);
extern cint_ast_t* cint_ast_enumerator(cint_ast_t* identifier, cint_ast_t* value);
extern cint_ast_t* cint_ast_enumdef(cint_ast_t* identifier, cint_ast_t* enumerators);
extern cint_ast_t* cint_ast_empty(void);
extern cint_ast_t* cint_ast_statement_with_no_effect(cint_ast_t* args);

extern const char* (*cint_ast_get_file_f)(void);
extern int (*cint_ast_get_line_f)(void);

extern cint_ast_t* cint_ast_last(cint_ast_t* root);
extern int cint_ast_append(cint_ast_t* root, cint_ast_t* tail);
extern int cint_ast_count(cint_ast_t* root);
extern void cint_ast_dump(cint_ast_t* root, int indent);
extern void cint_ast_dump_single(cint_ast_t* root, int indent);
extern void cint_ast_free_all(void);
extern void cint_ast_free(cint_ast_t* ast);
extern void cint_ast_free_single (cint_ast_t* a);

extern int cint_ast_int(cint_ast_t* a);
extern const char* cint_ast_str(cint_ast_t* a);
extern cint_ast_t* cint_ast_ternary(cint_ast_t* expression,
                                    cint_ast_t* t, cint_ast_t* f);
extern cint_ast_t* cint_ast_comma(cint_ast_t* left, cint_ast_t* right);
extern cint_ast_t* cint_ast_declaration_init(cint_ast_t* spec,
                                             cint_ast_t* init);
extern cint_ast_t* cint_ast_declarator_init(cint_ast_t* decl,
                                            cint_ast_t* init);
extern cint_ast_t* cint_ast_struct_declaration(cint_ast_t* qual,
                                               cint_ast_t* decl);
extern cint_ast_t* cint_ast_pointer_declarator(cint_ast_t* ptr,
                                               cint_ast_t* decl);
extern cint_ast_t* cint_ast_identifier_declarator(cint_ast_t* ident);
extern cint_ast_t* cint_ast_array_declarator(cint_ast_t* decl,
                                             cint_ast_t* len);
extern cint_ast_t* cint_ast_function_declarator(cint_ast_t* decl,
                                                cint_ast_t* param);
extern cint_ast_t* cint_ast_pointer_indirect(cint_ast_t* val);
extern cint_ast_t* cint_ast_parameter_declaration_append(cint_ast_t* spec,
                                                         cint_ast_t* decl);
extern cint_ast_t* cint_ast_parameter_declaration(cint_ast_t* decl);
extern cint_ast_t* cint_ast_compound_statement(cint_ast_t* stmt);
extern cint_ast_t* cint_ast_function_definition(cint_ast_t* ty,
                                                cint_ast_t* decl,
                                                cint_ast_t* stmt);
extern void cint_ast_touch(cint_ast_t* ast, int delta);

#endif /* __CINT_AST_H__ */
