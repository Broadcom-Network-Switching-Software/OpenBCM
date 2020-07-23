/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_operators.c
 * Purpose:     CINT operator functions
 */

#include "cint_operators.h"
#include "cint_porting.h"
#include "cint_variables.h"
#include "cint_eval_asts.h"
#include "cint_internal.h"
#include "cint_debug.h"
#include "cint_error.h"
#include "cint_datatypes.h"



/*******************************************************************************
 * 
 * These macros are used to implement most of the arithmetic and logical 
 * operators
 */

/* Perform a binary operation based on the given operand type */
#define _CINT_BINARY_RESULT(_op,_type) ( (cint_##_type##_value(left)) _op (cint_##_type##_value(right)))
/* Perform a binary operation based on the given operand type and create an autovar for the result */
#define _CINT_BINARY_CREATE(_op,_type) ( (cint_auto_##_type(_CINT_BINARY_RESULT(_op,_type))) )

/* Perform a unary operation based on the given operand type */
#define _CINT_UNARY_RESULT(_op,_type) ( (_op cint_##_type##_value(right)) )
/* Perform a unary operation based on the given operand type and create an autovar for the result */
#define _CINT_UNARY_CREATE(_op,_type) ( cint_auto_##_type(_CINT_UNARY_RESULT(_op,_type)) )

/* Perform a complete unary/binary arithmetical assign/create operation */
#define _CINT_ARITH_OPERATION(_op,_ub,_ac) \
    switch(optype) { \
      case cintOperandInt:           return _CINT##_ub##_ac(_op, integer); \
      case cintOperandUInt:          return _CINT##_ub##_ac(_op, uinteger); \
      case cintOperandLong:          return _CINT##_ub##_ac(_op, long); \
      case cintOperandULong:         return _CINT##_ub##_ac(_op, ulong); \
      case cintOperandLongLong:      return _CINT##_ub##_ac(_op, longlong); \
      case cintOperandULongLong:     return _CINT##_ub##_ac(_op, ulonglong); \
      case cintOperandDouble:        return _CINT##_ub##_ac(_op, double); \
      case cintOperandPointer:       return _CINT##_ub##_ac(_op, logical); \
      default: cint_internal_error(__FILE__, __LINE__, "_CINT_ARITH_OPERATION: unexpected operand type 0x%x", optype); return NULL; } \

/* Perform a complete unary/binary arithmetical assign/create operation */
#define _CINT_INTEGRAL_OPERATION(_op,_ub,_ac) \
    switch(optype) { \
      case cintOperandInt:           return _CINT##_ub##_ac(_op, integer); \
      case cintOperandUInt:          return _CINT##_ub##_ac(_op, uinteger); \
      case cintOperandULong:         return _CINT##_ub##_ac(_op, ulong); \
      case cintOperandLongLong:      return _CINT##_ub##_ac(_op, longlong); \
      case cintOperandULongLong:      return _CINT##_ub##_ac(_op, ulonglong); \
      default: cint_internal_error(__FILE__, __LINE__, "_CINT_INTEGRAL_OPERATION: unexpected operand type 0x%x", optype); return NULL; } \



/* Perform arithmetical and integral operations */
#define CINT_BINARY_RESULT(_op) _CINT_ARITH_OPERATION(_op,_BINARY,_RESULT)
#define CINT_BINARY_CREATE(_op) _CINT_ARITH_OPERATION(_op,_BINARY,_CREATE)
#define CINT_UNARY_RESULT(_op)  _CINT_ARITH_OPERATION(_op,_UNARY,_RESULT)
#define CINT_UNARY_CREATE(_op)  _CINT_ARITH_OPERATION(_op,_UNARY,_CREATE)

/* Perform integral-only operations */
#define CINT_IBINARY_RESULT(_op) _CINT_INTEGRAL_OPERATION(_op,_BINARY,_RESULT)
#define CINT_IBINARY_CREATE(_op) _CINT_INTEGRAL_OPERATION(_op,_BINARY,_CREATE)
#define CINT_IUNARY_RESULT(_op)  _CINT_INTEGRAL_OPERATION(_op,_UNARY,_RESULT)
#define CINT_IUNARY_CREATE(_op)  _CINT_INTEGRAL_OPERATION(_op,_UNARY,_CREATE)

/* Pointer Comparison Operations */
#define CINT_PCBINARY_CREATE(_op) if(optype == cintOperandPointer) return cint_auto_integer(cint_pointer_value(left) _op cint_pointer_value(right))
#define CINT_PCUNARY_CREATE(_op) if(optype == cintOperandPointer) return _op cint_pointer_value(right)
#define CINT_PBINARY_CREATE(_op) CINT_PCBINARY_CREATE(_op); CINT_BINARY_CREATE(_op)
#define CINT_PUNARY_CREATE(_op) CINT_PCUNARY_CREATE(_op); CINT_UNARY_CREATE(_op)

/*
 * Check the actual operand value to be zero based on the given operand type
 */
static int
__cint_operator_operand_is_zero(cint_variable_t* operand, cint_operand_type_t optype)
{
    if (operand != NULL) {
        switch(optype) {
            case cintOperandInt:           return (0 == cint_integer_value(operand));
            case cintOperandUInt:          return (0 == cint_uinteger_value(operand));
            case cintOperandLong:          return (0 == cint_long_value(operand));
            case cintOperandULong:         return (0 == cint_ulong_value(operand));
            case cintOperandLongLong:      return (0 == cint_longlong_value(operand)); 
            case cintOperandULongLong:     return (0 == cint_ulonglong_value(operand));
            case cintOperandDouble:        return ((CINT_DOUBLE)0.0 == cint_double_value(operand));
            case cintOperandPointer:       return (NULL == cint_pointer_value(operand));
            default: cint_internal_error(__FILE__, __LINE__, "__cint_operand_value_is_zero: unexpected operand type 0x%x", optype); return 1; 
        }
    }
    return(1);
}

/*
 * Calculate the size of the datatype to which a variable points
 */
static int
__cint_pointer_datatype_size(cint_variable_t* v)
{
    cint_datatype_t dt; 
    dt = v->dt; 
    dt.desc.pcount--; 
    return cint_datatype_size(&dt); 
}


/*
 * Add a constant value to a pointer variable and return a 
 * new variable with the result. 
 */
static cint_variable_t*
__cint_pointer_add_constant(cint_variable_t* v, int count)
{
    char* cp; 
    cint_variable_t* rv; 
    char* fptr = NULL;

    /* Clone the variable */
    rv = cint_variable_clone(v); 

    /* cp gets the pointer value */
    cp = (char*) *(void**)rv->data; 
    fptr = cp;
    cp += __cint_pointer_datatype_size(v) * count; 
    if(rv->flags & CINT_VARIABLE_F_CSTRING) {
        /*
         * if incremented ptr is 'char*'
         * 1. re-assing new pointer based on incremented address.
         * 2. free original ptr
         */
        *(void**)rv->data = (void*) CINT_STRDUP(cp);
        if(fptr) {
            CINT_FREE(fptr);
        }
    } else {
        *(void**)rv->data = (void*)cp;
    }
    return rv; 
}

static int
__cint_pointer_conversion(cint_variable_t** v)
{
    int dim_index;
    cint_error_t rc = CINT_E_NONE; 
    /*
     * Convert arrays to pointers to first element
     */
    if(v && *v && (*v)->dt.desc.num_dimensions != 0) {
        
        cint_variable_t* rv; 
        cint_parameter_desc_t desc = (*v)->dt.desc;

        /* Drop the array and increase the pointer count */
        desc.num_dimensions = 0; 
        desc.pcount++; 
        
        /* allocate pointer */
        rc = cint_variable_create(&rv, NULL, &desc, CINT_VARIABLE_F_AUTO, NULL); 
        if (rc == CINT_E_NONE) {
            /* Point to first element */
            *(void**)rv->data = (*v)->data; 
        
            /* Pass the original array size along for bounds checking
               and debug */
            rv->cached_num_dimensions = (*v)->dt.desc.num_dimensions;
            rv->dt.cap = (*v)->dt.cap;
            rv->dt.type_num_dimensions = (*v)->dt.type_num_dimensions;
            rv->dt.desc.num_dimensions = 0;
            for(dim_index = 0;
                dim_index < (*v)->dt.desc.num_dimensions; 
                dim_index++) {
                rv->dt.desc.dimensions[dim_index] = 
                    desc.dimensions[dim_index];
            }

            /* Pass along AUTOCASTS */
            if((*v)->flags & CINT_VARIABLE_F_AUTOCAST) {
                rv->flags |= CINT_VARIABLE_F_AUTOCAST; 
                (*v)->flags &= ~CINT_VARIABLE_F_AUTOCAST; 
            }
            *v = rv;
        }
    }   
    return rc; 
}

static void __cint_bad_operands(cint_ast_t* ast, int op); 

static int
__cint_check_operand(cint_variable_t* v, unsigned opflags)
{
    if(v) {
        unsigned vflags = cint_atomic_flags(v); 

        if(v->dt.desc.pcount > 0) {
            /* This is a pointer */
            return (opflags & CINT_OPERATOR_F_ACCEPT_POINTER); 
        }       

        if(vflags & CINT_ATOMIC_TYPE_FLAGS_INTEGRAL) {
            /* This is an integral type */
            return (opflags & CINT_OPERATOR_F_ACCEPT_INTEGRAL); 
        }

        if(vflags & CINT_ATOMIC_TYPE_F_DOUBLE) {
            /* This is a double type */
            return (opflags & CINT_OPERATOR_F_ACCEPT_DOUBLE); 
        }        

        /* All other types */
        return (opflags & CINT_OPERATOR_F_ACCEPT_ALL); 
    }
    return 1; 
}       

/*
 * All operator handlers. 
 */

static cint_variable_t* 
__cint_operator_RightShift(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_IBINARY_CREATE(>>); 
}


static cint_variable_t* 
__cint_operator_LeftShift(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_IBINARY_CREATE(<<); 
}


static cint_variable_t* 
__cint_operator_Increment(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    cint_variable_t* rv = NULL; 

    if(left) {
        /* Post Increment */
        rv = cint_variable_clone(left);  
        cint_eval_operator_internal(ast, cintOpAssign, left, 
                                   cint_eval_operator_internal(ast, cintOpAdd, left, cint_auto_integer(1))); 
    }
    else if(right) {
        /* Pre Increment */
        rv = right; 
        cint_eval_operator_internal(ast, cintOpAssign, right, 
                                    cint_eval_operator_internal(ast, cintOpAdd, right, cint_auto_integer(1))); 
    }
    return rv; 
}


static cint_variable_t* 
__cint_operator_Decrement(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    cint_variable_t* rv = NULL; 

    if(left) {
        /* Post Decrement */
        rv = cint_variable_clone(left); 
        cint_eval_operator_internal(ast, cintOpAssign, left, 
                                    cint_eval_operator_internal(ast, cintOpSubtract, left, cint_auto_integer(1))); 
    }
    else if(right) {
        /* Pre Decrement */
        rv = right; 
        cint_eval_operator_internal(ast, cintOpAssign, right, 
                                    cint_eval_operator_internal(ast, cintOpSubtract, right, cint_auto_integer(1))); 
    }
    return rv;
}


static cint_variable_t* 
__cint_operator_LogicalAnd(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    /*
     * Left and Right are not yet evaluated. 
     */

    left = cint_eval_ast(ast->utype.operator.left); 
    if(left == NULL || (__cint_check_operand(left, CINT_OPERATOR_FLAGS_PBINARY) == 0)) {
        __cint_bad_operands(ast, cintOpLogicalAnd); 
        return NULL; 
    }

    if(cint_logical_value(left) == 0) {
        /* Shortcut evaluation */
        return cint_auto_integer(0); 
    }

    right = cint_eval_ast(ast->utype.operator.right); 
    if(right == NULL || (__cint_check_operand(right, CINT_OPERATOR_FLAGS_PBINARY) == 0)) {
        __cint_bad_operands(ast, cintOpLogicalAnd); 
        return NULL; 
    }
    
    return cint_auto_integer(cint_logical_value(right)); 
}


static cint_variable_t* 
__cint_operator_LogicalOr(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    /*
     * Left and Right are not yet evaluated. 
     */

    left = cint_eval_ast(ast->utype.operator.left); 
    if(left == NULL || (__cint_check_operand(left, CINT_OPERATOR_FLAGS_PBINARY) == 0)) {
        __cint_bad_operands(ast, cintOpLogicalOr); 
        return NULL; 
    }

    if(cint_logical_value(left) == 1) {
        /* Shortcut evaluation */
        return cint_auto_integer(1); 
    }

    right = cint_eval_ast(ast->utype.operator.right); 
    if(right == NULL || (__cint_check_operand(right, CINT_OPERATOR_FLAGS_PBINARY) == 0)) {
        __cint_bad_operands(ast, cintOpLogicalOr); 
        return NULL; 
    }
    
    return cint_auto_integer(cint_logical_value(right)); 
}


static cint_variable_t* 
__cint_operator_LessThanOrEqual(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_PBINARY_CREATE(<=); 
}


static cint_variable_t* 
__cint_operator_GreaterThanOrEqual(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_PBINARY_CREATE(>=); 
}


static cint_variable_t* 
__cint_operator_Equal(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_PBINARY_CREATE(==); 
}


static cint_variable_t* 
__cint_operator_NotEqual(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_PBINARY_CREATE(!=); 
}


static cint_variable_t* 
__cint_operator_OpenBrace(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    cint_variable_lscope_push("}"); 
    return NULL; 
}


static cint_variable_t* 
__cint_operator_CloseBrace(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    cint_variable_lscope_pop("{"); 
    return NULL; 
}


static cint_variable_t* 
__cint_operator_Assign(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{    
    unsigned lflags; 
    unsigned rflags; 
    cint_error_t rc; 
    char _rstr[256] = {0};

    /*
     * Left may not be a constant
     */
    if(left->flags & CINT_VARIABLE_F_CONST) {
        cint_ast_error(ast, CINT_E_BAD_OPERANDS, "constant lvalue in assignment"); 
        return NULL; 
    }
    /*
     * Is right a bracket initializer?
     */
    if(right == NULL) {
        /* Left must be a structure or array */
        if(left->dt.desc.num_dimensions) {
            cint_ast_t* init; 
            
            

            /* Foreach entry in the array */
            int i; 
            for(i = 0, init = ast->utype.operator.right->utype.initializer.initializers; 
                init && i < left->dt.desc.dimensions[0]; 
                i++, init = init->next) {
                /* 
                 * Build an AST for this assignment 
                 */
                cint_ast_t* l = cint_ast_identifier(left->name); 
                cint_ast_t* r = cint_ast_integer(i); 
                cint_ast_t* m  = cint_ast_operator(cintOpOpenBracket, l, r); 
                cint_ast_t* nast = cint_ast_operator(cintOpAssign, m, init); 
                cint_eval_ast(nast); 

                /* Free up the ASTs as initializer evaluation is complete. */
                cint_ast_free_single(nast);
                cint_ast_free_single(m);
                cint_ast_free_single(r);
                cint_ast_free_single(l);
            }   
            if(init) {
                cint_ast_error(ast, CINT_E_BAD_OPERANDS, "too many initializers"); 
                return NULL; 
            }
            return left; 
        }
        else if(left->dt.flags & CINT_DATATYPE_F_STRUCT) {
            cint_ast_t* init; 
            
            /* Foreach parameter in the structure */
            const cint_parameter_desc_t* pd; 
            for(init = ast->utype.operator.right->utype.initializer.initializers, pd = left->dt.basetype.sp->struct_members; 
                pd->name && init; 
                pd++, init = init->next)
                {
                    /* 
                     * Build an AST for this assignment 
                     */
                    cint_ast_t* l = cint_ast_identifier(left->name); 
                    cint_ast_t* r = cint_ast_identifier(pd->name); 
                    cint_ast_t* m = cint_ast_operator(cintOpDot, l, r); 
                    cint_ast_t* nast = cint_ast_operator(cintOpAssign, m, init); 
                    cint_eval_ast(nast);

                    /* Free up the ASTs as initializer evaluation is complete. */
                    cint_ast_free_single(nast);
                    cint_ast_free_single(m);
                    cint_ast_free_single(r);
                    cint_ast_free_single(l);
                }

            if(init) {
                cint_ast_error(ast, CINT_E_BAD_OPERANDS, "too many initializers"); 
                return NULL; 
            }
        }       
        else {
            cint_ast_error(ast, CINT_E_BAD_OPERANDS, "bad initializer"); 
            return NULL; 
        }        
        return left; 
    }


    lflags = cint_atomic_flags(left); 
    rflags = cint_atomic_flags(right); 

    if(lflags && rflags) {
        
        /* Atomic types accept conversion/truncation */
        switch(lflags & CINT_ATOMIC_TYPE_FLAGS_MASK)
            {
            case CINT_ATOMIC_TYPE_F_CHAR:
                if (lflags & CINT_ATOMIC_TYPE_F_SIGNED) {
                    cint_char_set(left, cint_char_value(right)); return left;
                } else {
                    cint_char_set(left, cint_uchar_value(right)); return left;
                }
            case CINT_ATOMIC_TYPE_F_SHORT:        cint_short_set(left, cint_short_value(right)); return left; 
            case CINT_ATOMIC_TYPE_F_INT:          cint_integer_set(left, cint_integer_value(right)); return left; 
            case CINT_ATOMIC_TYPE_F_LONG:         cint_long_set(left, cint_long_value(right)); return left; 
            case CINT_ATOMIC_TYPE_F_LONGLONG:     cint_longlong_set(left, cint_longlong_value(right)); return left; 
            case CINT_ATOMIC_TYPE_F_DOUBLE:        cint_double_set(left, cint_double_value(right)); return left; 
            default: cint_internal_error(__FILE__, __LINE__, "unexpected atomic type flags 0x%x", (unsigned int)lflags); return NULL; 
            }   
    }
    
    /* Pointer conversion for right hand side is allowed */
    if(cint_type_check(&left->dt, &right->dt) == 0) {
        __cint_pointer_conversion(&right); 
    }

    if(right == NULL) {
        cint_internal_error(__FILE__, __LINE__, "unexpected error");
        return NULL;
    }

    if( (left->dt.desc.pcount == 0) &&
        ( (left->dt.flags & CINT_DATATYPE_F_ATOMIC) || left->dt.cap) && 
        (((ast->utype.operator.right) && 
          (ast->utype.operator.right->ntype == cintAstString)) ||
         (right->flags & CINT_VARIABLE_F_CSTRING))) {
        /* 
         * This is a string assignment to a custom/atomic type. Pass the string to the type's 
         * assignment handler. 
         */
        
        if(left->dt.cap && left->dt.cap->assign) {
            if((rc = left->dt.cap->assign(left->data, *(char**)right->data)) != CINT_E_NONE) {
                cint_ast_error(ast, rc, "datatype '%s' assignment error", left->dt.cap->name); 
                return NULL; 
            }   
        } else if(left->dt.basetype.ap->assign) {
            cint_error_t rc = left->dt.basetype.ap->assign(left->data, *(char**)right->data); 
            if(rc != CINT_E_NONE) {
                cint_ast_error(ast, rc, "datatype '%s' assignment error", cint_datatype_format(&left->dt,_rstr,0)); 
                return NULL; 
            }
        }
        else {
            cint_ast_error(ast, CINT_E_UNSUPPORTED, "datatype '%s' from '%s' has no assignment function.", 
                           cint_datatype_format(&left->dt, _rstr, 0), left->dt.type); 
            return NULL; 
        }

        return left; 
    }

    /*
     * Function Pointer Assignments
     */
    if( (left->dt.flags & CINT_DATATYPE_F_FUNC_POINTER) ) {

        if(right->dt.flags & CINT_DATATYPE_F_FUNC) {
            /* function pointer = compiled function reference */
            CINT_MEMCPY(left->data, right->data, left->size); 
        }
        else if(right->dt.flags & CINT_DATATYPE_F_FUNC_DYNAMIC) {
            /* 
             * function pointer = dynamic function reference. 
             * 
             * This can only be supported if there is a matching function pointer
             * type for the dynamic function
             */

            /* Copy compiled callback handler type into address */
            CINT_MEMCPY(left->data, &left->dt.basetype.fpp->cb, sizeof(cint_fpointer_t)); 
            /* Set the data as the name of the dynamic function */
            left->dt.basetype.fpp->data = (void*)right->dt.basetype.fp->name;             
        }
        return left; 
    }   

    /*
     * Otherwise left and right must be of the same type with the following exceptions:
     *  void* = any pointer
     *  any pointer = void*
     *  variables with the AUTOCAST flag set. 
     */    
    if( (cint_type_check(&left->dt, &right->dt) == 0) && /* different types */
        ((cint_is_vpointer(left) && cint_is_pointer(right)) == 0) &&  /* not void* = pointer assignement */
        ((cint_is_pointer(left) && cint_is_vpointer(right)) == 0) &&  /* not a pointer = void* assignment */
        ((right->flags & CINT_VARIABLE_F_AUTOCAST) == 0) )            /* right side is not an autocast */
        {     
            char* f1 = cint_datatype_format(&left->dt, _rstr, 1); 
            cint_ast_error(ast, CINT_E_BAD_OPERANDS, "incompatible types in assignment (%s = %s)", 
                           f1, cint_datatype_format(&right->dt, _rstr, 0)); 
            CINT_FREE(f1); 
            return NULL; 
        }

    if(left->size != right->size &&
       right->dt.desc.pcount == 0) {
        /* Not allowed, even with autocasting */
        char* f1 = cint_datatype_format(&left->dt, _rstr, 1); 
        cint_ast_error(ast, CINT_E_BAD_OPERANDS, "incompatible sizes in assignment (%s = %s)", 
                       f1, cint_datatype_format(&right->dt, _rstr, 0)); 
        CINT_FREE(f1); 
        return NULL;         
    }

    cint_variable_data_copy(left, right);

    /* Clear any autocast flags set for this assignment */
    if(CINT_VARIABLE_F_AUTOCAST) {
        right->flags &= ~CINT_VARIABLE_F_AUTOCAST; 
    }

    return left; 
}


static cint_variable_t* 
__cint_operator_OpenBracket(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    int index = 0; 

    /*
     * Left must resolve to a pointer
     */
    if(left->dt.desc.pcount == 0) {
        cint_ast_error(ast, CINT_E_BAD_TYPE, "subscripted value is not a pointer"); 
        return NULL; 
    }
    
    /*
     * Left must not be void
     */
    if(cint_is_vpointer(left)) {
        cint_ast_error(ast, CINT_E_BAD_TYPE, "cannot dereference void pointer"); 
        return NULL; 
    }

    /*
     * Left must not be NULL
     */
    if( (*(char**)left->data) == NULL) {
        cint_ast_error(ast, CINT_E_BAD_VARIABLE, "subscripted value is NULL"); 
        return NULL; 
    }

    /*
     * Right must resolve to integral value
     */ 
    if((cint_atomic_flags(right) & CINT_ATOMIC_TYPE_FLAGS_INTEGRAL) == 0) {
        cint_ast_error(ast, CINT_E_BAD_TYPE, "array subscript is not an integral"); 
        return NULL; 
    }
    else {
        index = cint_integer_value(right); 
    }

    /* If the original variable was a pointer-converted array, check the bounds */
    if(left->cached_num_dimensions) {
        if(index < 0 || index >= left->dt.desc.dimensions[0]) {
            cint_ast_error(ast, CINT_E_BAD_TYPE, "array subscript out of range"); 
            return NULL;
        }       
    }   
    {   
        cint_error_t rc; 
        cint_parameter_desc_t desc; 
        cint_variable_t* rv; 
        char* p; 
        int dim_index;
        int dataTypeSize;

        desc = left->dt.desc; 

        if(left->cached_num_dimensions <= 1) {
            /* dereference to base element type */
            desc.pcount--; 
            rc = cint_variable_create(
                &rv, 
                NULL, 
                &desc, 
                CINT_VARIABLE_F_AUTO|CINT_VARIABLE_F_SDATA, 
                (void*)0x1); 
            dataTypeSize = rv->size;
        }
        else {
            /* remove one dimension */
            desc.pcount = 1;
            desc.num_dimensions = 0;
            rc = cint_variable_create(
                &rv, 
                NULL, 
                &desc,
                CINT_VARIABLE_F_AUTO|CINT_VARIABLE_F_SDATA,
                (void*)0x1); 

            rv->dt.desc.num_dimensions = left->cached_num_dimensions - 1;
            for(dim_index = 0; 
                dim_index < rv->dt.desc.num_dimensions;
                dim_index++) {
                rv->dt.desc.dimensions[dim_index] = 
                    desc.dimensions[dim_index + 1];
            }

            rv->dt.desc.pcount = left->dt.desc.pcount - 1;

            dataTypeSize = cint_datatype_size(&rv->dt);
        }

        if(rv->dt.desc.num_dimensions > 0 && 
            rv->dt.desc.dimensions[0] == 
                CINT_CONFIG_POINTER_INFINITE_DIMENSION) {
            rv->cached_num_dimensions = rv->dt.desc.num_dimensions;
            rv->dt.desc.num_dimensions = 0;
        }

        if(rv->cached_num_dimensions >= left->dt.type_num_dimensions ||
            rv->dt.desc.num_dimensions >= left->dt.type_num_dimensions) {
            rv->dt.cap = left->dt.cap;
            rv->dt.type_num_dimensions = left->dt.type_num_dimensions;
        }
        
        if (rc == CINT_E_NONE) {
            /* Replace the address */
            p = *(char**)left->data; 
            p += (dataTypeSize * cint_integer_value(right));
            rv->data = p;
        }
        return rv;
    }
    return NULL; 
}

static cint_variable_t* 
__cint_operator_BitwiseAnd(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_IBINARY_CREATE(&); 
}


static cint_variable_t* 
__cint_operator_Not(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_UNARY_CREATE(!); 
}


static cint_variable_t* 
__cint_operator_Tilde(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_IUNARY_CREATE(~); 
}


static cint_variable_t* 
__cint_operator_Subtract(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    if(left->dt.desc.pcount) {
        /*
         * We can subtract constant integral values and other pointers of the same type
         */
        if((cint_atomic_flags(right) & CINT_ATOMIC_TYPE_FLAGS_INTEGRAL) == 0) {
            
            if(cint_type_check(&left->dt, &right->dt)) {
                /* Same pointer type */
                char* leftp = (char*) *(void**)left->data; 
                char* rightp = (char*) *(void**)right->data; 

                /* Return the difference between the pointers */
                return cint_auto_integer( (leftp-rightp) / __cint_pointer_datatype_size(left));                 
            }
            else {
                cint_ast_error(ast, CINT_E_BAD_OPERANDS, "invalid pointer subtraction"); 
                return NULL; 
            }
        }
        else {
            return __cint_pointer_add_constant(left, -1*cint_integer_value(right)); 
        }
    }
    CINT_BINARY_CREATE(-); 
}


static cint_variable_t* 
__cint_operator_Add(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{    
    if(left->dt.desc.pcount) {
        /* 
         * We can only add constant integral values to pointers
         */     
        if((cint_atomic_flags(right) & CINT_ATOMIC_TYPE_FLAGS_INTEGRAL) == 0) {
            cint_ast_error(ast, CINT_E_BAD_OPERANDS, "invalid pointer addition"); 
            return NULL; 
        }
        return __cint_pointer_add_constant(left, cint_integer_value(right)); 
    }
    CINT_BINARY_CREATE(+); 
}


static cint_variable_t* 
__cint_operator_Multiply(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_BINARY_CREATE(*); 
}


static cint_variable_t* 
__cint_operator_Divide(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    if ((0 == cint_logical_value(right)) || (0 != __cint_operator_operand_is_zero(right, optype))) {
        cint_ast_error(ast, CINT_E_BAD_OPERANDS, "attempt to divide by zero");
        return NULL;
    }

    CINT_BINARY_CREATE(/); 
}


static cint_variable_t* 
__cint_operator_Mod(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    if ((0 == cint_logical_value(right)) || (0 != __cint_operator_operand_is_zero(right, optype))) {
        cint_ast_error(ast, CINT_E_BAD_OPERANDS, "attempt to divide by zero");
        return NULL;
    }

    CINT_IBINARY_CREATE(%); 
}


static cint_variable_t* 
__cint_operator_LessThan(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_PBINARY_CREATE(<); 
}


static cint_variable_t* 
__cint_operator_GreaterThan(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_PBINARY_CREATE(>); 
}


static cint_variable_t* 
__cint_operator_BitwiseXor(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_IBINARY_CREATE(^); 
}


static cint_variable_t* 
__cint_operator_BitwiseOr(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_IBINARY_CREATE(|); 
}


static cint_variable_t* 
__cint_operator_Question(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    /*
     * Left contains the condition. 
     */
    if(cint_logical_value(left)) {
        /* Right contains the true expression */
        return cint_eval_ast(ast->utype.operator.right); 
    }
    else {
        return cint_eval_ast(ast->utype.operator.extra); 
    }
}


static cint_variable_t* 
__cint_operator_AddressOf(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    return cint_variable_address(right); 
}


static cint_variable_t* 
__cint_operator_Dereference(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    int dim_index;
    if(right->dt.desc.pcount == 0) {
        cint_ast_error(ast, CINT_E_BAD_OPERANDS, "cannot dereference non-pointer"); 
        return NULL; 
    }
    else if (!CINT_STRCMP(right->dt.desc.basetype, "void")) {
        cint_ast_error(ast, CINT_E_BAD_OPERANDS, "attempt to dereference void pointer"); 
        return NULL; 
    }
    else {
        cint_error_t rc; 
        cint_parameter_desc_t desc;             
        cint_variable_t* rv; 

        desc = right->dt.desc; 
        desc.pcount--; 
        desc.num_dimensions = right->cached_num_dimensions - 1;
        for(dim_index = 0; 
            dim_index < desc.num_dimensions; 
            dim_index++) {
            desc.dimensions[dim_index] = 
                desc.dimensions[dim_index + 1];
        }

        if((*(void**)right->data) == NULL) {
            cint_ast_error(ast, CINT_E_BAD_OPERANDS, "attempt to dereference NULL pointer"); 
            return NULL; 
        }

        rc = cint_variable_create(
            &rv, 
            NULL, 
            &desc, 
            CINT_VARIABLE_F_AUTO|CINT_VARIABLE_F_SDATA, 
            *(void**)right->data);
        rv->dt.desc.pcount = right->dt.desc.pcount - 1;
        rv->dt.desc.num_dimensions = right->cached_num_dimensions - 1;
        for(dim_index = 0;
            dim_index < rv->dt.desc.num_dimensions;
            dim_index++) {
            rv->dt.desc.dimensions[dim_index] = 
                right->dt.desc.dimensions[dim_index + 1];
        }

        if(rv->dt.desc.num_dimensions > 0 && 
            rv->dt.desc.dimensions[0] == 
                CINT_CONFIG_POINTER_INFINITE_DIMENSION) {
            rv->cached_num_dimensions = rv->dt.desc.num_dimensions;
            rv->dt.desc.num_dimensions = 0;
        }

        rv->dt.cap = right->dt.cap;
        rv->dt.type_num_dimensions = right->dt.type_num_dimensions;
        return (rc == CINT_E_NONE) ? rv : NULL;
    }
}


static cint_variable_t* 
__cint_operator_Positive(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_UNARY_CREATE(+);
}


static cint_variable_t* 
__cint_operator_Negative(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    CINT_UNARY_CREATE(-); 
}


static cint_variable_t* 
__cint_operator_Sizeof(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    return cint_auto_integer(right->size); 
}


static int
__find_struct_member_index(const char* member, const cint_parameter_desc_t* table)
{
    const cint_parameter_desc_t* sm; 
    
    for(sm = table; sm->name; sm++) {
        if(!CINT_STRCMP(sm->name, member)) {
            return sm-table;
        }
    }
    return CINT_E_NOT_FOUND; 
}

static cint_variable_t*
__cint_operator_Dot(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    int idx; 
    void* addr; 
    cint_variable_t* rv; 

    /*
     * Left must always be a structure
     * Right must always be an identifier
     */
    const char* member = ast->utype.operator.right->utype.identifier.s; 
    
    if( ((left->dt.flags & CINT_DATATYPE_F_STRUCT) == 0) ||
        left->dt.desc.pcount != 0 ) {
        cint_ast_error(ast, CINT_E_PARAM, "request for member '%s' in something not a structure", 
                       member); 
        return NULL; 
    }
    
    /*
     * Left points to a structure 
     */
    idx = __find_struct_member_index(member, left->dt.basetype.sp->struct_members); 
    if(idx < 0) {
        /* Member does no exist in this structure */
        cint_ast_error(ast, CINT_E_PARAM, "struct '%s' has no member '%s'", 
                       left->dt.basetype.sp->name, member); 
        return NULL; 
    }

    /*
     * Get the address of this member
     */
    addr = left->dt.basetype.sp->maddr(left->data, idx, left->dt.basetype.sp); 

    /*
     * Create a variable of this type with this address
     */
    cint_variable_create(&rv, 
                         NULL, 
                         left->dt.basetype.sp->struct_members+idx, 
                         CINT_VARIABLE_F_SDATA, 
                         addr); 
    
    return rv; 
}

static cint_variable_t*
__cint_operator_Arrow(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    int idx; 
    void* addr; 
    cint_variable_t* rv; 

    /*
     * Left must always be a structure pointer
     * Right must always be an identifier
     */
    const char* member = ast->utype.operator.right->utype.identifier.s; 
    
    if( ((left->dt.flags & CINT_DATATYPE_F_STRUCT) == 0) ||
        left->dt.desc.pcount != 1 ) {
        cint_ast_error(ast, CINT_E_PARAM, "request for member '%s' in something not a structure", 
                       member); 
        return NULL; 
    }
    
    /*
     * Left points to a structure pointer 
     */
    idx = __find_struct_member_index(member, left->dt.basetype.sp->struct_members); 
    if(idx < 0) {
        /* Member does no exist in this structure */
        cint_ast_error(ast, CINT_E_PARAM, "struct '%s' has no member '%s'", 
                       left->dt.basetype.sp->name, member); 
        return NULL; 
    }

    /*
     * Check for NULL pointer dereference 
     */
    if((*(void**)left->data) == NULL) {
        cint_ast_error(ast, CINT_E_BAD_OPERANDS, "attempt to dereference a NULL structure member pointer"); 
        return NULL; 
    }

    /*
     * Get the address of this member
     */
    addr = left->dt.basetype.sp->maddr(*((void**)left->data), idx, left->dt.basetype.sp); 
    
    
    /*
     * Create a variable of this type with this address
     */
    cint_variable_create(&rv, 
                         NULL, 
                         left->dt.basetype.sp->struct_members+idx, 
                         CINT_VARIABLE_F_SDATA, 
                         addr); 
    
    return rv; 
}


static cint_variable_t*
__cint_operator_Typecast(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right)
{
    /*
     * Right is the evaluated expression to typecast. 
     * The left Ast contains the typename. 
     *
     * Full typename parsing is not yet implemented. 
     * 
     * Only the following are currently supported:
     *
     *  Casts to 'void*' -- left == 1 (See grammar)
     *  Autocasts -- left == 2 (See grammer) allow variables to be any required type
 */
    cint_ast_t* type = ast->utype.operator.left; 
    if(type == CINT_AST_PTR_VOID) {
        /* Cast to 'void*' */
        if(cint_is_pointer(right) || cint_is_integer(right)) {
            return cint_auto_pointer("void", cint_pointer_value(right)); 
        }   
        else {
            cint_ast_error(ast, CINT_E_BAD_TYPE, "bad type in cast"); 
            return NULL; 
        }
    }
    
    if(type == CINT_AST_PTR_AUTO) {
        /* Autocast */
        right->flags |= CINT_VARIABLE_F_AUTOCAST; 
        return right; 
    }
    return NULL; 
}
    

static int
__cint_arithmetic_conversions(cint_variable_t** left, cint_variable_t** right)
{
    unsigned lflags = 0; 
    unsigned rflags = 0; 

    /* Convert enums to ints */
    if(left && cint_is_enum(*left)) {
        *left = cint_auto_integer(cint_integer_value(*left)); 
    }
    if(right && cint_is_enum(*right)) {
        *right = cint_auto_integer(cint_integer_value(*right)); 
    }
    
    if(left == NULL || *left == NULL || right == NULL || *right == NULL) {
        /* Nothing else to do */
        return CINT_E_NONE; 
    }

    if(cint_type_check(&(*left)->dt, &(*right)->dt)) {
        /* They are already the same type. Nothing to do. */
        return CINT_E_NONE; 
    }

    lflags = cint_atomic_flags(*left); 
    rflags = cint_atomic_flags(*right); 

    if( ((lflags & CINT_ATOMIC_TYPE_FLAGS_ARITH) == 0) ||
        ((rflags & CINT_ATOMIC_TYPE_FLAGS_ARITH) == 0)) {
        /* They are not both atomic arithmetic types. Nothing to do. */
        return CINT_E_NONE;
    }


    /* If either is 'double', convert both to double */
    if((lflags & CINT_ATOMIC_TYPE_F_DOUBLE) || (rflags & CINT_ATOMIC_TYPE_F_DOUBLE)) {
        if(lflags & CINT_ATOMIC_TYPE_F_DOUBLE) {
            *right = cint_auto_double(cint_double_value(*right)); 
        }
        else {
            *left = cint_auto_double(cint_double_value(*left)); 
        }
        return CINT_E_NONE; 
    }

    /* 
     * Integral promotions. All char, short, and enum => integer. 
     * It is assumed all values will fit in an integer. 
     * We don't perform conversion to unsigned integer
     */
    if(lflags & (CINT_ATOMIC_TYPE_F_CHAR | CINT_ATOMIC_TYPE_F_SHORT) ) {
        *left = cint_auto_integer(cint_integer_value(*left)); 
        lflags = cint_atomic_flags(*left); 
    }   
    if(rflags & (CINT_ATOMIC_TYPE_F_CHAR | CINT_ATOMIC_TYPE_F_SHORT)) {
        *right = cint_auto_integer(cint_integer_value(*right)); 
        rflags = cint_atomic_flags(*right); 
    }

    if(rflags == lflags) {
        /* At this point if the flags are equal they are the same type. */
        return CINT_E_NONE; 
    }   

    /*
     * Long Long and long promotion
     */
    if( (lflags & CINT_ATOMIC_TYPE_F_LONGLONG) ||
        (lflags & CINT_ATOMIC_TYPE_F_LONGLONG) ) {

        if(lflags & CINT_ATOMIC_TYPE_F_LONGLONG) {
            *right = cint_auto_longlong(cint_longlong_value(*right)); 
        }
        else {
            *left = cint_auto_longlong(cint_longlong_value(*left)); 
        }
        return CINT_E_NONE;
    }
    if( (lflags & CINT_ATOMIC_TYPE_F_LONG) ||
        (rflags & CINT_ATOMIC_TYPE_F_LONG) ) {

        if(lflags & CINT_ATOMIC_TYPE_F_LONG) {
            *right = cint_auto_long(cint_long_value(*right)); 
        }
        else {
            *left = cint_auto_long(cint_long_value(*left)); 
        }
        return CINT_E_NONE; 
    }
    
    return CINT_E_NONE; 
}


static struct {

    const char* name; 
    const char* iname; 
    cint_variable_t* (*handler)(cint_ast_t* ast, cint_operand_type_t optype, cint_variable_t* left, cint_variable_t* right); 
    unsigned flags; 

} __operator_table[] = {

#define CINT_OPERATOR_LIST_ENTRY(iname, _entry,f) { iname, #_entry, __cint_operator_##_entry, f }, 
#include "cint_op_entry.h"

    { NULL }
}; 


cint_variable_t*
cint_eval_operator_internal(cint_ast_t* ast, cint_operator_t op, cint_variable_t* left, cint_variable_t* right)
{
    unsigned flags; 
    int optype = 0; 
    cint_variable_t* rv = NULL; 

    flags = __operator_table[op].flags; 
    
    if((flags & CINT_OPERATOR_F_ACCEPT_ALL) == 0) {

        /*
         * Perform all arithmetic type conversions
         */
        __cint_arithmetic_conversions(&left, &right); 
    
        /*
         * Perform pointer conversions 
         */
        __cint_pointer_conversion(&left); 
        __cint_pointer_conversion(&right); 
        /* 
         * Not all types are accepted by the operator. 
         * Check the operands for compatibility. 
         */
        if(__cint_check_operand(left, flags) == 0) {
            __cint_bad_operands(ast, op); 
            return NULL; 
        }
        if(__cint_check_operand(right, flags) == 0) {
            __cint_bad_operands(ast, op); 
            return NULL; 
        }       
    }
            
    if( (left && left->dt.desc.pcount) ||
        (right && right->dt.desc.pcount) ) {

        /* Pointer operands */
        optype = cintOperandPointer; 
    }
    else {

        unsigned flags = cint_atomic_flags(right) | cint_atomic_flags(left); 
        
        switch(flags & CINT_ATOMIC_TYPE_FLAGS_MASK) 
            {
            case CINT_ATOMIC_TYPE_F_INT:         optype = cintOperandInt; break; 
            case CINT_ATOMIC_TYPE_F_LONG:        optype = cintOperandLong; break; 
            case CINT_ATOMIC_TYPE_F_LONGLONG:    optype = cintOperandLongLong; break; 
            case CINT_ATOMIC_TYPE_F_DOUBLE:      optype = cintOperandDouble; break; 
            }   
        /* If either operand is unsigned, treat all as unsigned */
        optype += ( (flags & CINT_ATOMIC_TYPE_F_UNSIGNED) != 0); 
    }

    rv = __operator_table[op].handler(ast, optype, left, right); 


    if(flags & CINT_OPERATOR_F_LOGICAL) {
        /*
         * The result of this operator should be a logical integer. 
         */
        rv = cint_auto_integer(cint_logical_value(rv)); 
    }

    return rv;   
}

cint_variable_t*
cint_eval_operator(cint_ast_t* ast) 
{
    cint_operator_t op = ast->utype.operator.op; 
    cint_variable_t* left = NULL; 
    cint_variable_t* right = NULL; 
    unsigned flags = 0; 

    if(op >= cintOpLast) {
        cint_errno = CINT_E_BAD_OPERATOR; 
        return NULL; 
    }   
    
    if(ast->ntype != cintAstOperator) {
        cint_internal_error(__FILE__, __LINE__, "cint_eval_operator on non operator ast"); 
        return NULL; 
    }

    CINT_DTRACE(("OP '%s' %s", __operator_table[op].name, __operator_table[op].iname));     
    
    flags = __operator_table[op].flags; 

    /*
     * Evaluate Left Value
     */
    if(flags & CINT_OPERATOR_F_LEFT) {
        left = cint_eval_ast(ast->utype.operator.left); 
        if(left == NULL &&
           !(flags & CINT_OPERATOR_F_OPTIONAL)) {
            __cint_bad_operands(ast, op); 
            return NULL; 
        }        
    }

    /* 
     * Evaluate Right Value
     */
    if(flags & CINT_OPERATOR_F_RIGHT) {
        right = cint_eval_ast(ast->utype.operator.right); 

        if(right == NULL && 
           !(flags & CINT_OPERATOR_F_OPTIONAL) &&
           ((ast->utype.operator.right == NULL) ||
           !(op == cintOpAssign &&
             ast->utype.operator.right->ntype == cintAstInitializer)) ) {
            __cint_bad_operands(ast, op); 
            return NULL; 
        }        
    }
    
    return cint_eval_operator_internal(ast, op, left, right); 
                         
}

static void __cint_bad_operands(cint_ast_t* ast, int op)
{       
    cint_ast_error(ast, CINT_E_BAD_OPERANDS, "invalid operand(s) to '%s'", __operator_table[op].name); 
}

const char*
cint_operator_name(cint_operator_t op)
{
    if(op >= cintOpLast) {
        cint_errno = CINT_E_BAD_OPERATOR; 
        return NULL; 
    }
    else {
        return __operator_table[op].name; 
    }
}

cint_operator_t
cint_operator_type(cint_ast_t* ast)
{
    cint_operator_t rc = cintOpLast; 
    
    if(ast->ntype == cintAstOperator) {
        rc = ast->utype.operator.op; 
    }

    return rc; 
}
        
