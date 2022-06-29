/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cint_ast.c
 * Purpose:     CINT AST node functions
 *
 */

#include "cint_ast.h"
#include "cint_config.h"
#include "cint_porting.h"
#include "cint_internal.h"

#ifdef CINT_COV
#include "cint_lcov.h"
void skip_ast_coverage(cint_ast_t* ast, cint_ast_t* covered);
#endif

char* (*cint_ast_get_file_f)(void) = NULL; 
int (*cint_ast_get_line_f)(void) = NULL; 
#ifdef BCM_SAND_SUPPORT
/* Will be used on the macro BCM_IF_ERROR_RETURN_MSG */
char cint_ast_last_declared_function_name[200] = {0,};
#endif

static cint_ast_t* __ast_list = NULL; 

static cint_ast_t* 
cint_ast_alloc(cint_ast_type_t type)
{
    cint_ast_t* ast = CINT_MALLOC(sizeof(*ast)); 

    if (ast == NULL) {
        cint_ast_error(NULL, CINT_E_MEMORY, "memory allocation failure"); 
        return NULL; 
    }
    CINT_MEMSET(ast, 0, sizeof(*ast)); 
    ast->ntype = type; 
    
    if(cint_ast_get_file_f) {
        ast->file = cint_ast_get_file_f();
#ifdef CINT_COV
        ast->file = set_file_name(cint_ast_get_file_f());
#endif
    }

    if(cint_ast_get_line_f) {
        ast->line = cint_ast_get_line_f(); 
    }

    /* All allocated ASTs are kept in a private list for easier deallocation */
    ast->inext = __ast_list; 
    __ast_list = ast; 
    return ast;
}

/* 
* Function : cint_ast_free_single
* Decsription : This function
* 1. finds ast and removes it from __ast_list
* 2. frees ast by calling cint_ast_free().
* Usage : This is used in case you want to free an AST immediately after 
* it is used. __ast_list will be freed by cint_ast_free_all() but this is 
* called only after exiting loop scope. 
* When loop count is really large, AST's are not be freed up and RSS increases
* and finally causes out of memory even though it is not used. (SDK-67627)
*/
void 
cint_ast_free_single (cint_ast_t* a)
{        
   if (__ast_list != NULL) {
     if (__ast_list == a) {             /* if "a" is at head of list */
        __ast_list = __ast_list->inext;
     } else {                           /* search list for "a" */
        cint_ast_t* current = __ast_list;
        while ((current->inext != NULL) && (current->inext != a)) {
          current = current->inext;
        }
        if (current->inext == a) {
          current->inext = current->inext->inext;
        }
     }
   }
   cint_ast_free(a);
}

static void
cint_ast_obj_free(const void *s)
{
    if (s) {
        CINT_FREE((void *)s);
    }
}

void
cint_ast_free(cint_ast_t* ast)
{
    switch (ast->ntype) {
    case cintAstString:
        cint_ast_obj_free(ast->utype.string.s);
        break;
    case cintAstFunction:
        cint_ast_obj_free(ast->utype.function.name);
        cint_ast_obj_free(ast->utype.function.dtp);
        break;
    case cintAstIdentifier:
        cint_ast_obj_free(ast->utype.identifier.s);
        break;
    case cintAstType:
        cint_ast_obj_free(ast->utype.type.s);
        break;
    default:
        break;
    }
    CINT_FREE(ast); 
}

void
cint_ast_free_all(void)
{
    /* Free all inactive AST nodes on the private list */
    cint_ast_t* a;
    cint_ast_t* active = NULL;
    for(a = __ast_list; a;) {
        cint_ast_t* next = a->inext;
        if (a->refcount > 0) {
            a->inext = active;
            active = a;
        } else {
            cint_ast_free(a);
        }
        a = next; 
    }   
    __ast_list = active; 
}       

cint_ast_t* 
cint_ast_operator(cint_operator_t operator, cint_ast_t* left, cint_ast_t* right)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstOperator); 

#ifdef CINT_COV
    /*Adding lines containing operator in the coverage if the
    operator is "=", "++" or "--"*/
    if(operator == cintOpAssign || operator == cintOpIncrement || operator == cintOpDecrement){
      if(left){
         init_coverage(&ast->file, &left->line, NULL, NULL);
      }
      else if(right){
         init_coverage(&ast->file, &right->line, NULL, NULL);
      }
    }
#endif

    ast->utype.operator.op = operator; 
    ast->utype.operator.left = left; 
    ast->utype.operator.right = right; 
    return ast;
}

cint_ast_t* 
cint_ast_integer(int i)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstInteger);
    ast->utype.integer.i = i; 
    return ast; 
}

#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
cint_ast_t* 
cint_ast_long_long(long long i)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstLongLong);
    ast->utype._longlong.i = i; 
    return ast; 
}

#endif

#if CINT_CONFIG_INCLUDE_DOUBLES == 1
cint_ast_t* 
cint_ast_double(double d)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstDouble);
    ast->utype._double.d = d; 
    return ast; 
}
#endif

cint_ast_t*
cint_ast_string(const char* s)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstString); 
    ast->utype.string.s = CINT_STRDUP(s);         
    return ast;
}

cint_ast_t*
cint_ast_identifier(const char* s)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstIdentifier); 
    ast->utype.identifier.s = CINT_STRDUP(s); 
    return ast;
}

cint_ast_t*
cint_ast_type(const char* s)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstType); 
    ast->utype.type.s = CINT_STRDUP(s); 
    return ast;
}

cint_ast_t*
cint_ast_declaration(void)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstDeclaration); 
    return ast;
}

cint_ast_t*
cint_ast_initializer(cint_ast_t* inits)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstInitializer); 
    ast->utype.initializer.initializers = inits; 
    return ast; 
}

cint_ast_t*
cint_ast_function(cint_ast_t* f, cint_ast_t* params)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstFunction); 
    if(f) {
        if(f->ntype != cintAstIdentifier) {
            /* Only identifiers can be specified for function calls */
            CINT_PRINTF("**error: Cannot parse function ast: "); 
            cint_ast_dump(f, 0); 
            CINT_PRINTF("\n\n"); 
            return NULL; 
        }
        ast->utype.function.name = CINT_STRDUP(f->utype.identifier.s); 
#ifdef CINT_COV
        /*Adding lines containing funciton calls to the coverage*/
        init_coverage(&ast->file, &f->line, NULL, NULL);
        /*Setting function line to function identifier line*/
        ast->line = f->line;
        /*Removing unwanted coverage (function params)*/
        skip_ast_coverage(params, f);
#endif      
    }   

    ast->utype.function.parameters = params; 

    return ast; 
}

cint_ast_t* 
cint_ast_function_def(void)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstFunctionDef); 
    return ast; 
}

cint_ast_t* 
cint_ast_structure_def(cint_ast_t* name, cint_ast_t* members)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstStructureDef); 
    ast->utype.structuredef.name = name; 
    ast->utype.structuredef.members = members; 
    return ast; 
}


cint_ast_t* 
cint_ast_elist(cint_ast_t* first)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstElist); 
    ast->utype.elist.list = first; 
    return ast; 
}

cint_ast_t* 
cint_ast_while(cint_ast_t* expr, cint_ast_t* statements, int order)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstWhile); 
#ifdef CINT_COV
    if(expr){
      init_coverage(&expr->file, &expr->line, NULL, NULL);
    }
#endif
    ast->utype._while.condition = expr; 
    ast->utype._while.statements = statements; 
    ast->utype._while.order = order; 
    return ast;
}

cint_ast_t* 
cint_ast_for(cint_ast_t* pre, cint_ast_t* cond, cint_ast_t* post, cint_ast_t* statements)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstFor); 
#ifdef CINT_COV
    init_coverage(&ast->file, &pre->line, NULL, NULL);
#endif
    ast->utype._for.pre = pre; 
    ast->utype._for.condition = cond; 
    ast->utype._for.post = post; 
    ast->utype._for.statements = statements; 
    return ast; 
}

cint_ast_t* 
cint_ast_if(cint_ast_t* str_if, cint_ast_t* expr, cint_ast_t* statements, cint_ast_t* _else)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstIf); 
#ifdef CINT_COV
    if(str_if){
      init_coverage(&ast->file, &str_if->line, NULL, NULL);
      ast->line = str_if->line;
    }
    /*Removing unwanted coverage(if condition)*/
    skip_ast_coverage(expr, ast);
#endif
    ast->utype._if.condition = expr; 
    ast->utype._if.statements = statements; 
    ast->utype._if._else = _else; 
    return ast; 
}

cint_ast_t* cint_ast_continue(void)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstContinue); 
#ifdef CINT_COV
    init_coverage(&ast->file, &ast->line, NULL, NULL);
#endif
    return ast; 
}

cint_ast_t* 
cint_ast_break(void)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstBreak); 
#ifdef CINT_COV
    init_coverage(&ast->file, &ast->line, NULL, NULL);
#endif
    return ast; 
}

cint_ast_t*
cint_ast_return(cint_ast_t* expr)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstReturn); 
#ifdef CINT_COV
    init_coverage(&ast->file, &ast->line, NULL, NULL);
#endif
    ast->utype._return.expression = expr; 
    return ast; 
}

cint_ast_t* 
cint_ast_print(cint_ast_t* expr)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstPrint); 
#ifdef CINT_COV
    init_coverage(&ast->file, &ast->line, NULL, NULL);
#endif
    ast->utype.print.expression = expr; 
    return ast; 
}

cint_ast_t* 
cint_ast_cint(cint_ast_t* args)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstCint); 
    ast->utype.cint.arguments = args; 
    return ast; 
}

cint_ast_t*
cint_ast_switch(cint_ast_t* str_switch, cint_ast_t* expr, cint_ast_t* statements)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstSwitch); 
#ifdef CINT_COV
    if(str_switch){
      init_coverage(&ast->file, &str_switch->line, NULL, NULL);
      ast->line = str_switch->line;
    }
#endif
    ast->utype._switch.expression = expr; 
    ast->utype._switch.statements = statements;         
    return ast; 
}

cint_ast_t*
cint_ast_case(cint_ast_t* expr, cint_ast_t* statements)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstCase); 
    ast->utype._switch.expression = expr; 
    ast->utype._switch.statements = statements;         
    return ast; 
}

cint_ast_t* 
cint_ast_enumerator(cint_ast_t* identifier, cint_ast_t* value)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstEnumerator); 
    ast->utype.enumerator.identifier = identifier; 
    ast->utype.enumerator.value = value; 
    return ast; 
}
    
cint_ast_t* 
cint_ast_enumdef(cint_ast_t* identifier, cint_ast_t* enumerators)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstEnumDef); 
    ast->utype.enumdef.identifier = identifier; 
    ast->utype.enumdef.enumerators = enumerators; 
#ifdef CINT_COV
    /*Removing unwanted coverage*/
    skip_ast_coverage(enumerators, NULL);
#endif
    return ast; 
}

cint_ast_t*
cint_ast_empty(void)
{
    cint_ast_t* ast = cint_ast_alloc(cintAstEmpty); 
    return ast; 
}

cint_ast_t*
cint_ast_statement_with_no_effect(cint_ast_t* ast)
{
    if(!(ast &&
        (ast->ntype == cintAstIdentifier) &&
        (ast->utype.identifier.s) &&
        (!CINT_STRCMP(ast->utype.identifier.s, "exit")||
         !CINT_STRCMP(ast->utype.identifier.s, "quit")))) {
        cint_warn(NULL, 0, "statement with no effect");
    }

    return ast;
}

#define L_SUFFIX 1
#define U_SUFFIX 2
#define LL_SUFFIX 4
#define BAD_SUFFIX 8

static int
_cint_ast_int_suffix(const char* s)
{
    int c,suffix;

    suffix = 0;
    while ((c=*s++) != 0) {
        switch (c) {
        case 'l':
        case 'L':
            if (suffix & (LL_SUFFIX)) {
                suffix |= BAD_SUFFIX;
            } else if (suffix & (L_SUFFIX)) {
                suffix &= ~L_SUFFIX;
                suffix |= LL_SUFFIX;
            } else {
                suffix |= L_SUFFIX;
            }
            break;
        case 'u':
        case 'U':
            if (suffix & (U_SUFFIX)) {
                suffix |= BAD_SUFFIX;
            } else {
                suffix |= U_SUFFIX;
            }
            break;
        }
    }
    return suffix;
}

cint_ast_t*
cint_ast_constant(const char* s, cint_ast_const_t ctype)
{
    switch (ctype) {
    case cintAstConstHex:
    case cintAstConstOctal:
    case cintAstConstDecimal:
        {
            int suffix = _cint_ast_int_suffix(s);

            if ((suffix & BAD_SUFFIX) == 0) {
                if (suffix & LL_SUFFIX) {
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
                    long long i = 0; 
                    if(cint_ctolli(s, &i) == 0) {
                        return cint_ast_long_long(i); 
                    }
#endif
                } else {
                    long i = 0; 
                    if(cint_ctoi(s, &i) == 0) {
                        return cint_ast_integer(i); 
                    }
                }
            }
        }
        break;
    case cintAstConstChar:
        {
            int i = 0; 

            if(cint_chartoi(s, &i) == 0) {
                return cint_ast_integer(i); 
            }
        }
        break;
    case cintAstConstFloat:
#if CINT_CONFIG_INCLUDE_DOUBLES == 1
        {      
            double d; 
            if(cint_ctod(s, &d) == 0) {
                return cint_ast_double(d); 
            }
        }
#endif
        break;
    }
    cint_ast_error(NULL, CINT_E_BAD_EXPRESSION, "unrecognized integer format"); 
    return NULL; 
}

int 
cint_ast_append(cint_ast_t* root, cint_ast_t* tail)
{
    int i = 0; 
    cint_ast_t* p;

    /* Protect against loops in the AST list. Normally this should not
       happen, but if it does, do not go into an infinite loop. */
    for(p = root; p && p->next && p != p->next; p = p->next) {
        if (tail != NULL && p == tail) {
            /* error: forms a loop */
            cint_ast_error(tail, CINT_E_BAD_AST, "loop in AST");
            return 0;
        }
        i++;
    }
    if (p) {
        p->next = tail;
        i++;
    }
    return i; 
}

int
cint_ast_count(cint_ast_t* root)
{
    if(root) {
        return cint_ast_append(root, NULL); 
    }   
    else {
        return 0; 
    }
}

cint_ast_t* 
cint_ast_last(cint_ast_t* root)
{
    cint_ast_t* p; 
    for(p = root; p && p->next; p = p->next); 
    return p; 
}

int
cint_ast_int(cint_ast_t* a)
{
    return CINT_AST(a, Integer) ? a->utype.integer.i : 0 ;
}

     
const char *
cint_ast_str(cint_ast_t* a)
{
    return CINT_AST(a, String) ? a->utype.string.s : NULL;
}

cint_ast_t* 
cint_ast_ternary(cint_ast_t* expression, cint_ast_t* t, cint_ast_t* f)
{
    cint_ast_t *ast = cint_ast_operator(cintOpQuestion, expression, t);
    if (ast) {
        ast->utype.operator.extra = f;
    }
    return ast;
}

cint_ast_t* 
cint_ast_comma(cint_ast_t* left, cint_ast_t* right)
{
    cint_ast_t *ast = NULL;

    if (left) {
        if (left->ntype == cintAstElist) {
            cint_ast_append(left->utype.elist.list, right); 
            ast = left; 
        } else {
            ast = cint_ast_elist(left);
            if (ast) {
                cint_ast_append(ast->utype.elist.list, right);
            }
        }
    }
    return ast;    
}

cint_ast_t* 
cint_ast_declaration_init(cint_ast_t* spec, cint_ast_t* init)
{
    /* Assign the type in spec to all declarations in the list */
    cint_ast_t* p; 
    for(p = init; p; p = p->next) {
        switch(p->ntype) {
        case cintAstDeclaration:
            p->utype.declaration.type = spec;
            break; 
        case cintAstFunctionDef:
            if (p->utype.functiondef.declaration && 
                p->utype.functiondef.declaration->ntype ==
                cintAstDeclaration) {
                p->utype.functiondef.declaration->utype.declaration.type =
                    spec;
            }
            break;
        default:
            break;
        }
    }
    return init;
}


cint_ast_t* 
cint_ast_declarator_init(cint_ast_t* decl, cint_ast_t* init)
{
#ifdef CINT_COV
    init_coverage(&decl->file, &decl->line, NULL, NULL);
    if(init && init->line != decl->line){
      skip_line_coverage(init->file, &init->line);
    }
#endif

    if (decl && decl->ntype == cintAstDeclaration) {
        decl->utype.declaration.init = init;
    }
    return decl;
}


cint_ast_t* 
cint_ast_struct_declaration(cint_ast_t* qual, cint_ast_t* decl)
{
    if (decl && decl->ntype == cintAstDeclaration) {
        decl->utype.declaration.type = qual;
    }
    return decl;
}

cint_ast_t* 
cint_ast_pointer_declarator(cint_ast_t* ptr, cint_ast_t* decl)
{
    if (decl) {
        switch(decl->ntype) {
        case cintAstDeclaration:
            decl->utype.declaration.pcount = cint_ast_int(ptr);
            break; 
        case cintAstFunctionDef:
            if (decl->utype.functiondef.declaration) {
                decl->utype.functiondef.declaration->utype.declaration.pcount =
                    cint_ast_int(ptr);
            }
            break; 
        default: break;
            /* Error */
        }
    }
    return decl;        
}

cint_ast_t* 
cint_ast_identifier_declarator(cint_ast_t* ident)
{
    cint_ast_t *ast;

    ast = cint_ast_declaration();
    if (ast) {
        ast->utype.declaration.identifier = ident;
    }

    return ast;
}

cint_ast_t* 
cint_ast_array_declarator(cint_ast_t* decl, cint_ast_t* len)
{
    if(decl) {
        if(decl->utype.declaration.num_dimension_initializers >=
            CINT_CONFIG_ARRAY_DIMENSION_LIMIT) {
                cint_ast_error(
                    decl, 
                    CINT_E_BAD_AST, 
                    "dimension limit exceeded");
                return NULL;
        }
        if (decl->ntype == cintAstDeclaration) {
            int index = decl->utype.declaration.num_dimension_initializers;
            decl->utype.declaration.dimension_initializers[index] = len;
            decl->utype.declaration.num_dimension_initializers++;
        }
    }
    return decl;
}


cint_ast_t* 
cint_ast_function_declarator(cint_ast_t* decl, cint_ast_t* param)
{
    cint_ast_t *ast = cint_ast_function_def();
#ifdef CINT_COV
    if(decl){
      ast->line = decl->line;
      /* Removing unwanted coverage */
      skip_ast_coverage(param, decl);
    }
#endif

    if (ast) {
        ast->utype.functiondef.declaration = decl; 
        ast->utype.functiondef.parameters = param;
#ifdef BCM_SAND_SUPPORT
        sal_strncpy(cint_ast_last_declared_function_name,
            decl->utype.declaration.identifier->utype.function.name,
            sizeof(cint_ast_last_declared_function_name) - 1);
#endif
    }
    return ast;
}

cint_ast_t* 
cint_ast_pointer_indirect(cint_ast_t* val)
{
    if (CINT_AST(val, Integer)) {
        val->utype.integer.i++;
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
    } else if (CINT_AST(val, LongLong)) {
        val->utype._longlong.i++;
#endif
    }

    return val;
}

cint_ast_t* 
cint_ast_parameter_declaration_append(cint_ast_t* spec, cint_ast_t* decl)
{
    if (decl && decl->ntype == cintAstDeclaration) {
        decl->utype.declaration.type = spec;
    }

    return decl;
}

cint_ast_t* 
cint_ast_parameter_declaration(cint_ast_t* decl)
{
    cint_ast_t *ast = cint_ast_declaration();

    if (ast) {
        ast->utype.declaration.type = decl;
    }

    return ast;
}

cint_ast_t* 
cint_ast_compound_statement(cint_ast_t* stmt)
{
    /* compound_statements get their own scope operators */              
    cint_ast_t* ss = cint_ast_operator(cintOpOpenBrace, 0, 0); 
    cint_ast_t* se = cint_ast_operator(cintOpCloseBrace, 0, 0); 

    if (ss) {
        /* Enclose all statements within new scope */
        ss->next = stmt; 
        /* Append closing scope to statement list */
        cint_ast_append(ss, se);
    }

    /* Return statement list */
    return ss;
}

cint_ast_t* 
cint_ast_function_definition(cint_ast_t* ty,
                             cint_ast_t* decl, cint_ast_t* stmt)
{
    if (CINT_AST(decl, FunctionDef)) {
        if (decl->utype.functiondef.declaration &&
            /* signed, unsigned is detected as CINT_AST(ty, Integer) */
            (CINT_AST(ty, Type) || CINT_AST(ty, Integer))) {
            decl->utype.functiondef.declaration->utype.declaration.type = ty;
        }
        decl->utype.functiondef.statements = stmt; 
#ifdef CINT_COV
        init_coverage(&decl->file, &decl->line, decl->utype.functiondef.declaration->utype.declaration.identifier->utype.identifier.s, &stmt->line);
#endif
    }
    return decl;
}

/* Modify reference count for AST and all children by 'delta' */
void
cint_ast_touch(cint_ast_t* ast, int delta)
{
    cint_ast_t* p;
    int dimension;

    for (p=ast; p; p=p->next) {
        if ((p == CINT_AST_PTR_VOID) || (p == CINT_AST_PTR_AUTO)) {
            /* special node types are terminal */
            break;
        }
        p->refcount += delta;
        switch(p->ntype) {
        case cintAstDeclaration:
            cint_ast_touch(p->utype.declaration.type, delta);
            for (dimension = 0; 
                dimension < p->utype.declaration.num_dimension_initializers; 
                ++dimension) {
                cint_ast_touch(
                    p->utype.declaration.dimension_initializers[dimension],
                    delta);
            }
            cint_ast_touch(p->utype.declaration.identifier, delta);
            cint_ast_touch(p->utype.declaration.init, delta);
            break;
        case cintAstInitializer:
            cint_ast_touch(p->utype.initializer.initializers, delta);
            break;
        case cintAstOperator:
            cint_ast_touch(p->utype.operator.left, delta);
            cint_ast_touch(p->utype.operator.right, delta);
            cint_ast_touch(p->utype.operator.extra, delta);
            break;
        case cintAstFunction:
            cint_ast_touch(p->utype.function.parameters, delta);
            break;
        case cintAstFunctionDef:
            cint_ast_touch(p->utype.functiondef.declaration, delta);
            cint_ast_touch(p->utype.functiondef.parameters, delta);
            cint_ast_touch(p->utype.functiondef.statements, delta);
            break;
        case cintAstStructureDef:
            cint_ast_touch(p->utype.structuredef.name, delta);
            cint_ast_touch(p->utype.structuredef.members, delta);
            break;
        case cintAstElist:
            cint_ast_touch(p->utype.elist.list, delta);
            break;
        case cintAstWhile:
            cint_ast_touch(p->utype._while.condition, delta);
            cint_ast_touch(p->utype._while.statements, delta);
            break;
        case cintAstFor:
            cint_ast_touch(p->utype._for.pre, delta);
            cint_ast_touch(p->utype._for.condition, delta);
            cint_ast_touch(p->utype._for.post, delta);
            cint_ast_touch(p->utype._for.statements, delta);
            break;
        case cintAstIf:
            cint_ast_touch(p->utype._if.condition, delta);
            cint_ast_touch(p->utype._if.statements, delta);
            cint_ast_touch(p->utype._if._else, delta);
            break;
        case cintAstReturn:
            cint_ast_touch(p->utype._return.expression, delta);
            break;
        case cintAstSwitch:
            cint_ast_touch(p->utype._switch.expression, delta);
            cint_ast_touch(p->utype._switch.statements, delta);
            break;
        case cintAstCase:
            cint_ast_touch(p->utype._case.expression, delta);
            cint_ast_touch(p->utype._case.statements, delta);
            break;
        case cintAstEnumerator:
            cint_ast_touch(p->utype.enumerator.identifier, delta);
            cint_ast_touch(p->utype.enumerator.value, delta);
            break;
        case cintAstEnumDef:
            cint_ast_touch(p->utype.enumdef.identifier, delta);
            cint_ast_touch(p->utype.enumdef.enumerators, delta);
            break;
        case cintAstPrint:
            cint_ast_touch(p->utype.print.expression, delta);
            break;
        case cintAstCint:
            cint_ast_touch(p->utype.cint.arguments, delta);
            break;
        default:
            break;
        }
    }
        
}

#ifdef CINT_COV
/*
 * Function:  skip_ast_coverage 
 * --------------------
 * Used to skip between ast nodes or from covered ast to last node of ast 
 *
 *  ast: abstract syntax tree nodes to be excluded from the coverage
 *  covered: ast line that needs to be counted
 */
void skip_ast_coverage(cint_ast_t* ast, cint_ast_t* covered){
   cint_ast_t* current = ast;
   int last_node_line = 0, i = 0;
   while(current){
      last_node_line = current->line;
      current = current->next;
   }

   /*Removing lines from covered ast to the last ast node from the coverage or from first to last node in the list*/
   for(i = covered ? covered->line : ast->line; last_node_line && i<=last_node_line; i++){
      /*Not skipping if on line that needs to be counted*/
      if(!covered || covered->line != i){
         skip_line_coverage(ast->file, &i);
      }
   }

}
#endif
