/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_eval_asts.c
 * Purpose:     CINT AST evaluators
 */

#include "cint_eval_asts.h"
#include "cint_porting.h"
#include "cint_variables.h"
#include "cint_internal.h"
#include "cint_error.h"
#include "cint_debug.h"
#include "cint_eval_ast_cint.h"
#include "cint_eval_ast_print.h"
#include <stdarg.h>


static int __breakable = 0; 
static int __returnable = 0; 

#define CINT_BREAK()          CINT_ERRNO_IS(BREAK)
#define CINT_CONTINUE()       CINT_ERRNO_IS(CONTINUE)
#define CINT_RETURN()         CINT_ERRNO_IS(RETURN)
#define CINT_EXIT()           CINT_ERRNO_IS(EXIT)

typedef cint_variable_t* (*cint_vararg_f)(cint_ast_t* ast);

static cint_variable_t* 
__cint_eval_ast_Empty(cint_ast_t* ast)
{
    return NULL; 
}

static cint_variable_t* 
__cint_eval_ast_Integer(cint_ast_t* ast)
{
    return cint_auto_integer(ast->utype.integer.i); 
}

#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
static cint_variable_t* 
__cint_eval_ast_LongLong(cint_ast_t* ast)
{
    return cint_auto_longlong(ast->utype._longlong.i); 
}
#endif

static cint_variable_t* 
__cint_eval_ast_String(cint_ast_t* ast)
{
    cint_variable_t* rv; 
    rv = cint_auto_string(ast->utype.string.s); 
    return rv; 
}

#if CINT_CONFIG_INCLUDE_DOUBLES == 1
static cint_variable_t*
__cint_eval_ast_Double(cint_ast_t* ast)
{
    return cint_auto_double(ast->utype._double.d); 
}
#endif

static cint_variable_t* 
__cint_eval_ast_Type(cint_ast_t* ast)
{
    return NULL; 
}

static cint_variable_t*
__cint_eval_ast_Initializer(cint_ast_t* ast)
{
    return NULL; 
}

static cint_variable_t* 
__cint_eval_ast_EnumDef(cint_ast_t* ast)
{
    cint_enum_type_t* cet; 
    cint_enum_map_t* map; 
    cint_ast_t* e; 
    int ecount; 
    int evalue = 0; 
    int map_size;

    /*
     * Allocate a new cint_enum_type_t structure
     */
    cet = CINT_MALLOC(sizeof(*cet)); 
    if(cet == NULL) {
        cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure"); 
        return NULL; 
    }
    CINT_MEMSET(cet, 0, sizeof(*cet));

    cet->name = CINT_STRDUP(ast->utype.enumdef.identifier->utype.identifier.s); 
    ecount = cint_ast_count(ast->utype.enumdef.enumerators); 

    /* Allocate the map array based on the enum count */
    map_size = sizeof(*map)*(ecount+1);
    map = CINT_MALLOC(map_size); 

    if(map == NULL) {
        cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure"); 
        cint_datatype_clear_enumeration(cet); 
        return NULL; 
    }   
    CINT_MEMSET(map, 0, map_size);

    /* Initialize map array base on the enumerators */
    cet->enum_map = map; 
    for(e = ast->utype.enumdef.enumerators; e; e = e->next, map++) {
        map->name = CINT_STRDUP(e->utype.enumerator.identifier->utype.identifier.s); 
        if(e->utype.enumerator.value) {
            cint_variable_t* rv;

            rv = cint_eval_ast(e->utype.enumerator.value);
            if (rv) {
                map->value = cint_integer_value(rv);
                evalue = map->value;
            } else {
                cint_ast_error(ast, CINT_E_BAD_AST, "illegal enum value"); 
                cint_datatype_clear_enumeration(cet); 
                return NULL; 
            }
        }
        else {
            map->value = evalue; 
        }       
        evalue++; 
    }
    map->name = NULL; 

    cint_datatype_add_enumeration(cet); 

    return NULL; 
}

static cint_variable_t*
__cint_eval_ast_Enumerator(cint_ast_t* ast)
{
    return NULL; 
}


static cint_variable_t* 
__cint_eval_ast_Identifier(cint_ast_t* ast)
{
    cint_variable_t* rv = NULL; 
    int c; 
    cint_datatype_t dt; 
    const char* id = ast->utype.identifier.s; 

    /* Special Command Identifiers */
    if(!CINT_STRCMP(id, "quit") ||  
       !CINT_STRCMP(id, "exit")) {
        cint_errno = CINT_E_EXIT; 
        return NULL; 
    }

    /* Variables */
    if((rv = cint_variable_find(id, 0))) {
        return rv; 
    }

    /* Enumerations */
    if(cint_datatype_enum_find(id, &dt, &c) == CINT_E_NONE) {
        if(cint_variable_create(&rv, NULL, &dt.desc, CINT_VARIABLE_F_AUTO | CINT_VARIABLE_F_CONST, NULL) != CINT_E_NONE) {
            cint_internal_error(__FILE__, __LINE__, "could not create enum autovar of type '%s'", dt.desc.basetype); 
            return NULL; 
        }
        else {
            *((int*)rv->data) = c; 
        }
        return rv; 
    }

    
    /* Enumerations and constants */
    if(cint_constant_find(id, &c) == CINT_E_NONE) {
        return cint_auto_integer(c); 
    }   
    
    /* Functions */
    if(cint_datatype_find(id, &dt) == CINT_E_NONE) {
        if(dt.flags & (CINT_DATATYPE_F_FUNC | CINT_DATATYPE_F_FUNC_DYNAMIC)) {
            cint_parameter_desc_t pdt = { NULL, 0, 0 }; 
            
            pdt.basetype = id; 
            cint_variable_create(&rv, 
                                 NULL, 
                                 &pdt, 
                                 CINT_VARIABLE_F_AUTO | CINT_VARIABLE_F_CONST, 
                                 NULL); 
            return rv; 
        }
    }   

    if(rv == NULL) {
        cint_ast_error(ast, CINT_E_BAD_VARIABLE, "identifier '%s' undeclared", ast->utype.identifier.s); 
    }
    return rv; 
}

static int
_cint_eval_ast_declaration_parameter(cint_ast_t* ast,
                                     char *basetype,
                                     int basetype_len,
                                     unsigned* basetype_flags,
                                     int *pcount_p,
                                     int *array_dimension_count,
                                     int *array_dimensions)
{
    int rc; 
    cint_variable_t* rv; 
    int pcount;
    int dim_index;
    int dimensions[CINT_CONFIG_ARRAY_DIMENSION_LIMIT];

    if (ast->ntype != cintAstDeclaration) {
        return cint_ast_error(ast,
            CINT_E_BAD_AST,
            "expected declaration"); 
    }

    pcount = ast->utype.declaration.pcount; 

    rc = cint_eval_type_list(ast->utype.declaration.type,
                             basetype, basetype_len,
                             basetype_flags, 1);

    if (rc != CINT_E_NONE) {
        return rc;
    }

    if(ast->utype.declaration.num_dimension_initializers) {
        for(dim_index = 0;
            dim_index < ast->utype.declaration.num_dimension_initializers;
            dim_index++) {
            rv = cint_eval_ast(
                ast->utype.declaration.dimension_initializers[dim_index]); 
            if(rv == NULL || (cint_is_integer(rv) == 0)) {
                return cint_ast_error(ast,
                    CINT_E_BAD_AST,
                    "illegal array definition"); 
            } 
            dimensions[dim_index] = cint_integer_value(rv); 
            if(dim_index > 0 && dimensions[dim_index] <= 0) {
                return cint_ast_error(ast,
                    CINT_E_BAD_AST,
                    "illegal array definition"); 
            }
        }

        if(dimensions[0] <= 0) {
            if(dimensions[0] == -1 &&
               ast->utype.declaration.num_dimension_initializers == 1 &&
               ast->utype.declaration.init &&
               ast->utype.declaration.init->ntype == cintAstInitializer) {
                /* 
                 * Determine the proper size for this initialized array. 
                 * based on the length of the initializer chain 
                 */
                dimensions[0] =
                    cint_ast_count(ast->utype.declaration.init->
                                   utype.initializer.initializers); 
            }
            else {
                return cint_ast_error(ast,
                                      CINT_E_BAD_AST,
                                      "illegal array definition"); 
            }
        }
    }

    if (rc == CINT_E_NONE) {
        *pcount_p = pcount;
        *array_dimension_count = 
            ast->utype.declaration.num_dimension_initializers;
        for(dim_index = 0; 
            dim_index < ast->utype.declaration.num_dimension_initializers;
            dim_index++) {
            array_dimensions[dim_index] = dimensions[dim_index];
        }
    }
    return rc; 
}

static cint_variable_t* 
__cint_eval_ast_Declaration(cint_ast_t* ast)
{    
    int rc; 
    cint_variable_t* rv; 
    cint_parameter_desc_t desc; 
    unsigned basetype_flags;
    char basetype[64];
    
    rc = _cint_eval_ast_declaration_parameter(ast,
                                              basetype,
                                              sizeof(basetype),
                                              &basetype_flags,
                                              &desc.pcount,
                                              &desc.num_dimensions,
                                              desc.dimensions);

    if (rc != CINT_E_NONE) {
        return NULL;
    }

    desc.name = ast->utype.declaration.identifier->utype.string.s;
    desc.basetype = basetype;

    if (CINT_STRLEN(desc.basetype) == 0) {
        cint_ast_error(ast, CINT_E_BAD_VARIABLE,
                       "identifier '%s' has unknown type", desc.name);
        return NULL; 
    }

    if((rv=cint_variable_find(desc.name, 1)) != NULL) {
        /* Variable already exists in current scope */
        cint_ast_error(ast, CINT_E_BAD_VARIABLE,
                       "identifier '%s' redeclared", desc.name); 
        return NULL; 
    }

    rc = cint_variable_create(&rv,      
                              desc.name,
                              &desc, 
                              0, 
                              NULL);

    if(rc < 0) {
        if(rc == CINT_E_BAD_TYPE) {
            cint_ast_error(ast, rc, "cannot declare variable of type '%s'",
                           desc.basetype); 
        }
        else {
            cint_ast_error(ast, rc, "variable create error: %s\n",
                           cint_error_name(rc)); 
        }
        return NULL; 
    }
    else {

        if(ast->utype.declaration.init) {
            /* New variable has an initialization expression */
            cint_ast_t *id; 
            cint_ast_t *assign; 

            /*
             * Identifier AST
             */
            id = cint_ast_identifier(rv->name);

            /*
             * Assignment AST
             */
            assign = cint_ast_operator(cintOpAssign,
                                       id,
                                       ast->utype.declaration.init);
            cint_eval_ast(assign);

            /* sdk-67627: free the two ast allocated in this scope */ 
            cint_ast_free_single(assign);
            cint_ast_free_single(id);
        }    

        if(basetype_flags & CINT_AST_TYPE_F_CONST || 
            (desc.num_dimensions != 0 && desc.pcount == 0))  {
            rv->flags |= CINT_VARIABLE_F_CONST; 
        }
        if(basetype_flags & CINT_AST_TYPE_F_VOLATILE) {
            rv->flags |= CINT_VARIABLE_F_VOLATILE; 
        }
               
    }       
    return rv; 
}

static cint_variable_t* 
__cint_eval_ast_Operator(cint_ast_t* ast)
{
    return cint_eval_operator(ast); 
}


static int 
__cint_eval_ast_dimensions_equal(int length0,
                                 int length1,
                                 int *dimensions0,
                                 int *dimensions1)
{
    int i;
    if(length0 != length1) {
        return 0;
    }

    for(i = 0; i < length0; i++) {
        if(dimensions0[i] != dimensions1[i]) {
            return 0;
        }
    }

    return 1;
}


static cint_variable_t* 
__cint_eval_ast_Function(cint_ast_t* ast)
{
    cint_error_t rc;    
    cint_datatype_t* dtp; 
    cint_variable_t* rv = NULL; 
    cint_fparams_t fparams; 
    cint_parameter_desc_t* pd; 
    cint_ast_t* p; 
    int count; 
    int fcount; 
    cint_function_t* fp; 
    char _rstr[256] = {0};

    /*
     * Find the function 
     */
    if((dtp = ast->utype.function.dtp) == NULL) {
        
        dtp = CINT_MALLOC(sizeof(*dtp)); 
        if(dtp == NULL) {
            cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure"); 
            return NULL; 
        }

        
        if( ((rc = cint_datatype_find(ast->utype.function.name, dtp)) < 0) ||
            ((dtp->flags & CINT_DATATYPE_FLAGS_FUNC) == 0) ) {
            cint_ast_error(ast, CINT_E_NOT_FOUND, "identifier '%s' is not a function", ast->utype.function.name); 

            CINT_FREE(dtp); 
            return NULL; 
        }
        
        ast->utype.function.dtp = dtp; 
    }       
    
    fp = dtp->basetype.fp; 
    
    if(dtp->flags & CINT_DATATYPE_F_FUNC_VARARG) {
        cint_vararg_f func = (cint_vararg_f)fp->addr;
        return func(ast); 
    }   

    /* 
     * Return value?
     */
    if(!cint_parameter_void(fp->params)) {
        /* Allocate variable to hold return type */
        cint_variable_create(&rv, 
                             NULL, 
                             fp->params,
                             0, 0); 
        if(rv) {
            fparams.rv = rv->data; 
        }
        else {
            fparams.rv = NULL; 
        }
    }   

    /*
     * Check parameter count
     */
    count = cint_ast_count(ast->utype.function.parameters); 
    fcount = fp->params ? cint_parameter_count(fp->params) - 1   /* Return value doesn't count */ : 0; 
    
    if(fcount == 1 && cint_parameter_void(fp->params+1)) { 
        fcount = 0; 
    }
    
    if(count != fcount) {
        cint_ast_error(ast, CINT_E_PARAM, "wrong number of parameters to function '%s' (expected %d but received %d)", 
                       fp->name, fcount, count); 
        return NULL; 
    }   

    
    /*
     * Parameters
     */
    for(count = 0, p = ast->utype.function.parameters, pd = fp->params+1; 
        p; 
        count ++, p = p->next, pd++) {

        /* Given parameter value */
        cint_variable_t* v = cint_eval_ast(p); 
        cint_datatype_t pdt; 

        if(v == NULL) {
            cint_ast_error(ast, CINT_E_PARAM, "error evaluating argument %d to function '%s'", 
                           count+1, fp->name); 
            return NULL; 
        }

        CINT_MEMSET(&pdt, 0, sizeof(pdt));
        /* Parameter type */
        if(cint_datatype_find(pd->basetype, &pdt) != CINT_E_NONE) {
            return NULL; 
        }

        if(cint_array_combine_dimensions(&pdt.desc, pd) != CINT_E_NONE) {
            return NULL;
        }
        pdt.desc.pcount += pd->pcount;

        if(cint_type_check(&v->dt, &pdt) == 0) {
            
            int rc; 
            cint_variable_t* nv; 

            /*
             * Not the same type. Special pointer conversions, where
             * an extra level of indirection is added.
             */

            /*
             * There may be other type conversions (string to
             * bcm_mac_t, string to bcm_ip_t) that should be
             * allowable.
             */
            
            if((!CINT_STRCMP(v->dt.desc.basetype, pdt.desc.basetype) ||
                cint_type_compatible(&v->dt, &pdt) ) && 
               v->dt.desc.pcount == (pdt.desc.pcount-1) &&
               v->dt.desc.num_dimensions == (pdt.desc.num_dimensions -1) &&
               __cint_eval_ast_dimensions_equal(
                   pdt.desc.num_dimensions - 1,
                   v->dt.desc.num_dimensions == 0 ? 
                        v->cached_num_dimensions : 
                        v->dt.desc.num_dimensions, 
                   pdt.desc.dimensions + 1,
                   v->dt.desc.dimensions)) {
                
                /*
                 * The function expects a pointer to the given argument type. 
                 * Enable conversion. 
                 */
                v = cint_variable_address(v); 
                v->flags &= ~CINT_VARIABLE_F_CONST; 
            }
            else {
                
                /*
                 * Can we convert the argument through assignment? 
                 */
                if((rc = cint_variable_create(&nv, NULL, pd, CINT_VARIABLE_F_AUTO, NULL)) != CINT_E_NONE) {
                    cint_ast_error(ast, rc, "error creating temporary variables"); 
                    return NULL; 
                }
            
                if(cint_eval_operator_internal(ast, cintOpAssign, nv, v) == nv) {
                    /* Conversion successful */
                    v = nv; 
                }   
                else {
                    char* f1 = cint_datatype_format(&v->dt, _rstr, 1); 
                    cint_ast_error(ast, CINT_E_PARAM, "arg %d to function '%s' was wrong type: expected %s, received %s", 
                                   count+1, fp->name, cint_datatype_format(&pdt, _rstr, 0), f1); 
                    CINT_FREE(f1); 
                    return NULL; 
                }       
            }   
        }       
        
        /* Parameter is good to go */
        if(dtp->flags & CINT_DATATYPE_F_FUNC) {
            /* Static function call */
            fparams.args[count] = v->data; 
        }
        else {
            /* Stored for later -- see dynamic function call below */
            fparams.args[count] = v; 
        }
    }   


    /*
     * Call function
     */
    if(dtp->flags & CINT_DATATYPE_F_FUNC) {
        cint_ftrace(fp->name, 1); 
        rc = fp->body.wrapper(&fparams); 
        cint_ftrace(fp->name, 0); 
    }   
    else {

        /*
         * Clone all function parameters into a new scope using the expected parameter names
         */
        int i; 
        cint_variable_scope_push(fp->name); 

        for(i = 0, pd = fp->params+1; 
            i < count; 
            i++, pd++) {        

            cint_variable_t* pv = cint_variable_clone((cint_variable_t*)fparams.args[i]); 
            cint_variable_rename(pv, pd->name);
            cint_variable_auto_save(pv); 
        }       
        
        /* 
         * Evaluate function body
         */
        cint_ftrace(fp->name, 1); 
        __returnable++; 
        rv = cint_eval_asts(fp->body.statements); 
        if(rv) rv = cint_variable_clone(rv); 
        if(cint_errno == CINT_E_RETURN) {
            cint_errno = CINT_E_NONE; 
        }       
        __returnable--; 
        cint_ftrace(fp->name, 0); 

        /* Do not destroy return value when scope is popped */
        if(rv) rv->flags |= CINT_VARIABLE_F_NODESTROY; 

        /* Pop function scope */
        cint_variable_scope_pop(fp->name); 

        /* Add return value back into the local scope list */
        if(rv) {
            rv->flags &= ~CINT_VARIABLE_F_NODESTROY; 
            rv->flags &= ~CINT_VARIABLE_F_AUTO; 
            cint_variable_add(rv); 
        }       
    }
    
    return rv; 
}

static cint_variable_t* 
__cint_eval_ast_Elist(cint_ast_t* ast)
{
    cint_ast_t* p; 
    cint_variable_t* rv = NULL; 

    for(p = ast->utype.elist.list; p; p = p->next) {
        rv = cint_eval_ast(p); 
    }
    return rv; 
}

static cint_variable_t* 
__cint_eval_ast_While(cint_ast_t* ast)
{
    cint_variable_t* rv; 

    __breakable++; 


    if(ast->utype._while.order) {       

        rv = cint_eval_asts(ast->utype._while.statements);    

        if(CINT_BREAK()) {
            cint_errno = CINT_E_NONE; 
            return rv;
        } else if(CINT_CONTINUE()) {
            cint_errno = CINT_E_NONE; 
        } else if(CINT_RETURN()) {
            cint_errno = CINT_E_RETURN;
            return rv;
        }       
    }   
    
    while(cint_logical_value(cint_eval_ast(ast->utype._while.condition))) {

        rv = cint_eval_asts(ast->utype._while.statements);

        if(CINT_BREAK()) {
            cint_errno = CINT_E_NONE; 
            break; 
        } else if(CINT_CONTINUE()) {
            cint_errno = CINT_E_NONE; 
            continue; 
        } else if(CINT_RETURN()) {
            cint_errno = CINT_E_RETURN;
            return rv;
        } else if (cint_errno != CINT_E_NONE) {
            break;
        }
    }

    __breakable--; 
    return NULL; 
}

static int
_for_cond(cint_ast_t* ast)
{
    int cond = 1;

    if (ast && ast->ntype != cintAstEmpty) {
        cond = cint_logical_value(cint_eval_ast(ast));
    }

    return cond;
}

static cint_variable_t* 
__cint_eval_ast_For(cint_ast_t* ast)
{
    cint_variable_t* rv = NULL; 

    __breakable++; 
    
    for(cint_eval_ast(ast->utype._for.pre); 
        _for_cond(ast->utype._for.condition); 
        cint_eval_ast(ast->utype._for.post)) {
        
        rv = cint_eval_asts(ast->utype._for.statements); 

        if(CINT_BREAK()) {
            cint_errno = CINT_E_NONE; 
            break; 
        }

        if(CINT_CONTINUE()) {
            cint_errno = CINT_E_NONE; 
            continue; 
        }

        if(CINT_RETURN()) {
            break;
        }       

        if(CINT_EXIT()) {
            break;
        }
    }

    __breakable--; 

    return rv; 
}

static cint_variable_t* 
__cint_eval_ast_If(cint_ast_t* ast)
{
    cint_variable_t* rv = NULL; 

    if(cint_logical_value(cint_eval_ast(ast->utype._if.condition))) {
        rv = cint_eval_asts(ast->utype._if.statements); 
    }
    else if(ast->utype._if._else) {
        rv = cint_eval_asts(ast->utype._if._else); 
    }   
    return rv; 
}

static cint_variable_t*
__cint_eval_ast_Return(cint_ast_t* ast)
{
    cint_variable_t* rv = NULL; 
    
    if(!__returnable) {
        cint_ast_error(ast, CINT_E_BAD_AST, "return statement not within function"); 
        return NULL; 
    }   

    if(ast->utype._return.expression) {
        rv = cint_eval_ast(ast->utype._return.expression); 
    }

    cint_errno = CINT_E_RETURN; 
    return rv;
}

static cint_variable_t*
__cint_eval_ast_Continue(cint_ast_t* ast)
{
    if(!__breakable) {
        cint_ast_error(ast, CINT_E_BAD_AST, "continue statement not within loop"); 
    }
    else {
        cint_errno = CINT_E_CONTINUE; 
    }   
    return NULL; 
}

static cint_variable_t*
__cint_eval_ast_Break(cint_ast_t* ast)
{
    if(!__breakable) {
        cint_ast_error(ast, CINT_E_BAD_AST, "break statement not within loop"); 
    }
    else {
        cint_errno = CINT_E_BREAK; 
    }   
    return NULL; 
}
    
static cint_variable_t*
__cint_eval_ast_Switch(cint_ast_t* ast)
{
    cint_ast_t* s; 
    cint_ast_t* _default = NULL; 
    cint_ast_t* eq = NULL; 
    cint_variable_t* rv = NULL;
    
    /*
     * If this is a compound statement the brace operators might not get paired properly. 
     * The first open brace will never get executed, since it is not part of a case label. 
     * The final open brace may get executed as we fall through the labels (including the default). 
     *
     * The trailing closing brace is disabled for this reason. 
     */
    s = cint_ast_last(ast->utype._switch.statements); 
    if(cint_operator_type(s) == cintOpCloseBrace) {
        s->noexec = 1; 
    }

    /* Find and validate default case */
    for(s = ast->utype._switch.statements; s; s = s->next) {
        if( (s->ntype == cintAstCase) &&
            s->utype._case.expression == NULL) {
            
            if(_default) {
                cint_ast_error(ast, CINT_E_BAD_AST, "more than one default label in switch statement"); 
                return NULL; 
            }
            else {
                _default = s; 
            }
        }
    }

    eq = cint_ast_operator(cintOpEqual, ast->utype._switch.expression, NULL); 
    for(s = ast->utype._switch.statements; s; s = s->next) {
        if( (s->ntype == cintAstCase) &&
            (s->utype._case.expression) ) {
            
            cint_variable_t* v; 

            /* Check the switch expression against the case expression */
            eq->utype.operator.right = s->utype._case.expression; 
            v = cint_eval_ast(eq); 
            if(v == NULL) {
                rv = NULL;
                break;
            }
            
            if(cint_logical_value(v) == 1) {
                /* Begin executing all statements starting here */
                __breakable++; 
                rv = cint_eval_asts(s);
                __breakable--; 
                if(CINT_BREAK()) {
                    cint_errno = CINT_E_NONE; 
                }                
                if(CINT_RETURN()) {
                    cint_errno = CINT_E_RETURN;
                }
                else {
                    rv = NULL;
                }
                break;
            }   
        }
    }

    /* Free up the AST as the switch evaluation is complete. */
    cint_ast_free_single(eq);
    if (s) {
        return rv;
    }

    if(_default) {
        /* Execute default case */
        __breakable++; 
        rv = cint_eval_asts(_default);
        __breakable--; 
        if(CINT_BREAK()) {
            cint_errno = CINT_E_NONE; 
        }
        if(CINT_RETURN()) {
            cint_errno = CINT_E_RETURN;
            return rv;
        }                
    }
    return NULL; 
}

static cint_variable_t*
__cint_eval_ast_Case(cint_ast_t* ast)
{
    /* Evaluate all statements in case label */
    return cint_eval_asts(ast->utype._case.statements); 
}



static cint_variable_t*
__cint_eval_ast_Print(cint_ast_t* ast)
{
    /* cint_eval_ast_print.c */
    return cint_eval_ast_Print(ast); 
}

static cint_variable_t*
__cint_eval_ast_Cint(cint_ast_t* ast)
{
    /* cint_eval_ast_debug.c */
    return cint_eval_ast_Cint(ast); 
}

static cint_variable_t*
__cint_eval_ast_StructureDef(cint_ast_t* ast)
{
    cint_struct_type_t* cst = CINT_MALLOC(sizeof(*cst)); 
    cint_ast_t* m; 
    cint_parameter_desc_t* p0; 
    cint_parameter_desc_t* p; 
    cint_datatype_t dt; 
    int mcount; 
    int ssize = 0;
    char basetype[64];
    int rv;
    int sm_size;
    int dim_index;

    if(cst == NULL) {
        cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure"); 
        return NULL; 
    }
    CINT_MEMSET((void *)cst, 0, sizeof(*cst));     

    CINT_MEMSET(&dt, 0, sizeof(dt));

    if (ast->utype.structuredef.name) {
        cst->name =
            CINT_STRDUP(ast->utype.structuredef.name->utype.identifier.s); 
        if (cst->name == NULL) {
            cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure");
            cint_datatype_clear_structure(cst);
            return NULL; 
        }
    } else {
        cint_ast_error(ast, CINT_E_BAD_AST, "structure name not found"); 
        cint_datatype_clear_structure(cst);
        return NULL; 
    }
    
    m = ast->utype.structuredef.members; 
    mcount = cint_ast_count(m); 

    sm_size = sizeof(*p)*(mcount+1);
    cst->struct_members = CINT_MALLOC(sm_size);         
    if (cst->struct_members == NULL) {
        cint_datatype_clear_structure(cst);
        cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure"); 
        return NULL; 
    }
    CINT_MEMSET((void *)cst->struct_members, 0, sm_size);     
    
    p = (cint_parameter_desc_t *) cst->struct_members; 

    p0 = NULL;

    for(; m; m = m->next, p++) {
        if (m->ntype != cintAstDeclaration) {
            cint_ast_error(ast, CINT_E_BAD_AST,
                           "invalid structure definition");
            cint_datatype_clear_structure(cst);
            return NULL; 
        }

        /* point of this exercise is to set p->{basetype,name,pcount}
           and to determine member size.
        */
        p->name =
            CINT_STRDUP(m->utype.declaration.identifier->utype.string.s); 

        rv = _cint_eval_ast_declaration_parameter(m,
                                                  basetype,
                                                  sizeof(basetype),
                                                  NULL,
                                                  &p->pcount,
                                                  &p->num_dimensions,
                                                  p->dimensions);

        if (rv != CINT_E_NONE) {
            cint_datatype_clear_structure(cst);
            return NULL;
        }
        
        if (CINT_STRLEN(basetype) > 0) {
            /* count and array are already set, so now save the basetype */
            p->basetype = CINT_STRDUP(basetype);

            /* Save this structure member in case future structure
               members do not have a base type. Every structure member
               has a right to a base type. If it does not have a base
               type, this function will appoint one.
            */
            p0 = p;
        } else {
            /* This structure member doesn't have a basetype, so use
               the basetype from the most recent structure member that
               *did* have one. At least one structure member needs to
               have a basetype. */
            if (p0 == NULL) {
                /* It appears that no structure member had a basetype */
                cint_ast_error(ast, CINT_E_BAD_AST,
                               "no default member type definition found"); 
                cint_datatype_clear_structure(cst);
                return NULL; 
            }
            p->basetype = CINT_STRDUP(p0->basetype);
            p->pcount   = p0->pcount;
            p->num_dimensions = p0->num_dimensions;
            for(dim_index = 0;
                dim_index < p0->num_dimensions; 
                dim_index++) {
                p->dimensions[dim_index] = 
                    p0->dimensions[dim_index];
            }
        }

        if(cint_datatype_find(p->basetype, &dt) != CINT_E_NONE) {
            cint_ast_error(ast, CINT_E_BAD_AST,
                           "unknown type '%s' in structure definition",
                           p->basetype); 
            cint_datatype_clear_structure(cst);
            return NULL; 
        }

        for(dim_index = 0;
            dim_index < p->num_dimensions; 
            dim_index++) {
            dt.desc.dimensions[dim_index + dt.desc.num_dimensions] = 
                p->dimensions[dim_index];
        }
        dt.desc.num_dimensions += p->num_dimensions;
        dt.desc.pcount += p->pcount; 

        ssize += cint_datatype_size(&dt); 

        /* All members start on a word aligned address */
        while(ssize % 4) ssize++; 
        
    }    
    
    cst->size = ssize; 
    cst->maddr = cint_maddr_dynamic_struct_t; 

    cint_datatype_add_structure(cst); 

    return NULL; 
}

/* function parameter and return types can be a composite type, like
   unsigned int, but only one of those AST nodes that comprise the
   composite is the 'Type' AST node. */
static cint_ast_t *
__cint_ast_basetype(cint_ast_t* ast)
{
    int rc = CINT_E_NONE;
    char basetype[64];
    if(ast) {
        rc = cint_eval_type_list(ast,
                                 basetype, sizeof(basetype),
                                 NULL, 0);
        if (rc != CINT_E_NONE) {
            return NULL;
        }
        if(CINT_AST(ast,Type) && ast->utype.type.s){
            CINT_FREE((char*) ast->utype.type.s);
        }
        ast->ntype = cintAstType;
        ast->utype.type.s = CINT_STRDUP(basetype);
    }

    return ast;
}

static cint_variable_t*
__cint_eval_ast_FunctionDef(cint_ast_t* ast)
{
    cint_function_t* f;

    cint_ast_t* decl = ast->utype.functiondef.declaration; 
    cint_ast_t* params = ast->utype.functiondef.parameters;
    cint_ast_t* ret;
    cint_ast_t* name;

    cint_parameter_desc_t* p; 
    int count; 
    int desc_size;
    cint_datatype_t dt;

    if (decl == NULL) {
        cint_ast_error(ast, CINT_E_BAD_AST, "cannot determine declaration"); 
        return NULL; 
    }
    if (decl->ntype == cintAstType
        || decl->ntype == cintAstInteger
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
        || decl->ntype == cintAstLongLong
#endif
#if CINT_CONFIG_INCLUDE_FLOATS == 1
        || decl->ntype == cintAstFloat
#endif
        ) {
        /* This is a declaration, not a definition, so there's really
           nothing to do, yet. */
        return NULL;
    } else if (decl->ntype != cintAstDeclaration) {
        cint_internal_error(__FILE__, __LINE__,
                            "Node type %d unexpected", decl->ntype);
        return NULL;
    }

    /* Function name */
    name = decl->utype.declaration.identifier;
    if (!CINT_AST(name, Identifier)) {
        cint_internal_error(__FILE__, __LINE__,
                            "Node type %d unexpected", name->ntype);
        return NULL; 
    }

    f = CINT_MALLOC(sizeof(*f));
    if (f == NULL) {
        cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure"); 
        return NULL; 
    }
    CINT_MEMSET(f, 0, sizeof(*f)); 

    f->name = CINT_STRDUP(name->utype.identifier.s);    
        
    /* Return Type */
    ret = __cint_ast_basetype(decl->utype.declaration.type);

    if (ret == NULL) {
        cint_internal_error(__FILE__, __LINE__, "unknown return type");
        cint_datatype_clear_function(f);
        return NULL; 
    }

    if (ret->utype.type.s == NULL) {
        cint_internal_error(__FILE__, __LINE__, "unknown return basetype");
        cint_datatype_clear_function(f);
        return NULL; 
    }
    
    /* Number of parameters */
    count = cint_ast_count(params); 

    /* Allocate parameter description array to hold parameters and return value */
    desc_size = sizeof(*p)*(count+2);
    p = CINT_MALLOC(desc_size);         
    if (p == NULL) {
        cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure"); 
        cint_datatype_clear_function(f);
        return NULL; 
    }
    CINT_MEMSET(p, 0, desc_size); 
    f->params = p; 

    p->basetype = CINT_STRDUP(ret->utype.type.s);
    p->name = CINT_STRDUP("r"); 
    p->pcount = decl->utype.declaration.pcount; 
    p->num_dimensions = 0;
    
    for(count = 1, params = ast->utype.functiondef.parameters; 
        params; 
        params = params->next, count++) {
        if (CINT_AST(params, Declaration)) {
            cint_ast_t *ty;
            cint_ast_t *ident = params->utype.declaration.identifier;

            ty = __cint_ast_basetype(params->utype.declaration.type);

            if (ty == NULL) {
                cint_ast_error(ast, CINT_E_BAD_AST,
                               "param type not found"); 
                cint_datatype_clear_function(f);
                return NULL; 
            }
            
            if (ident && !CINT_AST(ident, Identifier)) {
                cint_ast_error(ast, CINT_E_BAD_AST,
                               "unexpected param ident AST %d", ident->ntype);
                cint_datatype_clear_function(f);
                return NULL;
            }

            /* A NULL basetype is used as a sentinel for the parameter
               array, so it is not allowed to be NULL here */
            if (ty->utype.type.s == NULL) {
                cint_ast_error(ast, CINT_E_BAD_AST, "base type not found");
                cint_datatype_clear_function(f);
                return NULL;
            }
            
            

            p[count].basetype = CINT_STRDUP(ty->utype.type.s);

            p[count].name = ident ?
                CINT_STRDUP(ident->utype.identifier.s) : NULL; 
            p[count].pcount = params->utype.declaration.pcount; 
            p[count].num_dimensions = 0;
            /* for CINT, parameters are either IN or INOUT */
            p[count].flags = CINT_PARAM_IN;
            if (p[count].pcount > 0) {
                cint_ast_t *modifier = params->utype.declaration.type;

                /* const pointer is in, else in/out */
                if (!(CINT_AST(modifier, Integer) &&
                      modifier->utype.integer.i == CINT_AST_TYPE_F_CONST)) {
                    p[count].flags |= CINT_PARAM_OUT;
                }
            }
        } else {
            /* If it is not a declaration, treat it as a dummy parameter.
               There still needs to be a basetype, though. */
            p[count].basetype = CINT_STRDUP("void");
        }
    }
    p[count].basetype = NULL; 
    
    /* Statements */
    f->body.statements = ast->utype.functiondef.statements; 

    /* if function is already defined, remove previous definition */
    CINT_MEMSET(&dt, 0, sizeof(dt));
    if(CINT_E_NONE == cint_datatype_find(f->name, &dt)) {
        cint_datatype_delete_function(f->name);
    }
    
    /*
     * Register with interpreter
     */
    cint_interpreter_add_function(f);
    return NULL; 
}


int
cint_interpreter_callback(cint_function_pointer_t* cb, int nargs, int nreturn, ...)
{
    int i; 
    cint_datatype_t dt; 
    cint_parameter_desc_t* params; 
    cint_variable_t* v; 
    const char* fname; 
    int returns; 
    int fcount;
    int errno_save;
    va_list args; 

    va_start(args,nreturn); 

    if(cb == NULL) {
        cint_internal_error(__FILE__, __LINE__, "callback pointer is NULL"); 
        va_end(args); 
        return 0; 
    }
    
    fname = (char*)cb->data; 

    if(fname == NULL) {
        cint_internal_error(__FILE__, __LINE__, "callback name is NULL"); 
        va_end(args); 
        return 0; 
    }

    CINT_MEMSET(&dt, 0, sizeof(dt));
    if(cint_datatype_find(fname, &dt) != 0) {
        cint_internal_error(__FILE__, __LINE__, "callback type '%s' was not found", fname); 
        va_end(args); 
        return 0; 
    }   

    if (CINT_STRLEN(fname) == 0) {
        cint_internal_error(__FILE__, __LINE__, "callback name is NULL String");
        va_end(args); 
        return 0;
    }

    /*
     * Call dynamic function. 
     *
     * Sanity check the signature to detect mismatched type errors. 
     * This could be augmented to pass full type information back with the parameters. 
     */    
    returns = !cint_parameter_void(dt.basetype.fp->params); 
    fcount = cint_parameter_count(dt.basetype.fp->params) - 1; 
    if(cint_parameter_void(dt.basetype.fp->params+1)) fcount--; 

    if(fcount != nargs) {
        cint_internal_error(__FILE__, __LINE__, "callback type '%s' was not passed the correct number of arguments (expected %d, received %d)", 
                            fname, fcount, nargs); 
        va_end(args); 
        return 0; 
    }
    if((returns) && (nreturn == 0)) {
        cint_internal_error(__FILE__, __LINE__, "callback type '%s' returns a value but calling function did not specify one", fname); 
        va_end(args); 
        return 0; 
    }
    if((returns == 0) && (nreturn)) {
        cint_internal_error(__FILE__, __LINE__, "callback type '%s' does not return a value but calling function expects one", fname); 
        va_end(args); 
        return 0; 
    }   

    cint_variable_scope_push(fname); 
    for(i = 0, params = dt.basetype.fp->params+1; params && params->basetype; params++, i++) {
        void* addr;
        if(cint_parameter_void(params)) {
            continue;
        }
        /* Get the address of this variable */
        addr = va_arg(args, void*); 
        
        /* Create a variable of the appropriate type with this address */
        cint_variable_create(&v, 
                             params->name, 
                             params, 
                             CINT_VARIABLE_F_SDATA, 
                             addr); 
    }
    /* Evaluate function body */
    cint_ftrace(dt.basetype.fp->name, 1); 
    __returnable++;
    errno_save = cint_errno;
    v = cint_eval_asts(dt.basetype.fp->body.statements); 
    cint_errno = errno_save;
    __returnable--; 
    cint_ftrace(dt.basetype.fp->name, 0); 
    
    /* Return value? */
    if(v && returns) {
        void* addr = va_arg(args, void*); 
        if(addr) {
            cint_datatype_t rt; 
            CINT_MEMSET(&rt, 0, sizeof(rt));
            if (cint_datatype_find(dt.basetype.fp->params->basetype, &rt) != 0) {
                cint_internal_error(__FILE__, __LINE__,
                                    "callback return type '%s' was not found",
                                    dt.basetype.fp->params->basetype); 
                va_end(args); 
                return 0; 
            }

            if (cint_type_check(&rt, &v->dt)) {
                CINT_MEMCPY(addr, v->data, v->size);
            } else {
                /* At least clear out the return value to avoid
                   valgrind complaints. */
                int size;

                size = cint_datatype_size(&rt);
                if (size > 0) {
                    CINT_MEMSET(addr, 0, size);
                }
                cint_ast_error(dt.basetype.fp->body.statements,
                               CINT_E_BAD_AST,
                               "return type mismatch"); 
            }
        }
    }

    cint_variable_scope_pop(fname); 

    va_end(args); 

    return 0; 
}





struct {
    const char* name; 
    cint_variable_t* (*handler)(cint_ast_t* ast); 
} __ast_handler_table[] = {

#define CINT_AST_LIST_ENTRY(_entry) { #_entry, __cint_eval_ast_##_entry },
#include "cint_ast_entry.h"

    { NULL }
};

cint_variable_t* 
cint_eval_ast(cint_ast_t* ast)
{
    cint_variable_t* rv = NULL; 
    cint_ast_type_t type; 

    cint_atrace(ast); 

    if(ast == NULL) {
        return NULL; 
    }

    type = ast->ntype; 

    if(type < 0 || type >= cintAstLast) {
        return NULL; 
    }
    
    CINT_DTRACE(("AST: %s", __ast_handler_table[type].name)); 
    if(ast->noexec) {
        CINT_DTRACE((" marked as noexec\n")); 
    }
    else {
        rv = __ast_handler_table[type].handler(ast); 
    }
    return rv;
}
    
cint_variable_t*
cint_eval_asts(cint_ast_t* ast)
{
    cint_variable_t* rv = NULL; 
    cint_ast_t* p; 
    
    cint_errno = CINT_E_NONE; 

    for(p = ast; p && (cint_errno == CINT_E_NONE); p = p->next) {
        rv = cint_eval_ast(p); 
    }
    cint_variable_auto_save(rv); 
    cint_variable_auto_clear();         
    return rv; 
}          
