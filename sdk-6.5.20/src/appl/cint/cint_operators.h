/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_operators.h
 * Purpose:     CINT operator interfaces
 */

#ifndef __CINT_OPERATORS_H__
#define __CINT_OPERATORS_H__

#include "cint_config.h"
#include "cint_types.h"

/*
 * Operator Flags
 */
#define CINT_OPERATOR_F_ACCEPT_INTEGRAL         0x1
#define CINT_OPERATOR_F_ACCEPT_DOUBLE           0x2
#define CINT_OPERATOR_F_ACCEPT_POINTER          0x4
#define CINT_OPERATOR_F_ACCEPT_ALL              0x8

#define CINT_OPERATOR_F_LEFT                    0x100
#define CINT_OPERATOR_F_RIGHT                   0x200
#define CINT_OPERATOR_F_TYPE_CHECK              0x400
#define CINT_OPERATOR_F_OPTIONAL                0x800
#define CINT_OPERATOR_F_LOGICAL                 0x1000


#define CINT_OPERATOR_FLAGS_UNARY CINT_OPERATOR_F_RIGHT 
#define CINT_OPERATOR_FLAGS_IUNARY CINT_OPERATOR_FLAGS_UNARY | CINT_OPERATOR_F_ACCEPT_INTEGRAL
#define CINT_OPERATOR_FLAGS_AUNARY CINT_OPERATOR_FLAGS_IUNARY | CINT_OPERATOR_F_ACCEPT_DOUBLE
#define CINT_OPERATOR_FLAGS_PUNARY CINT_OPERATOR_FLAGS_AUNARY | CINT_OPERATOR_F_ACCEPT_POINTER

#define CINT_OPERATOR_FLAGS_BINARY CINT_OPERATOR_F_LEFT | CINT_OPERATOR_F_RIGHT | CINT_OPERATOR_F_TYPE_CHECK
#define CINT_OPERATOR_FLAGS_IBINARY CINT_OPERATOR_FLAGS_BINARY | CINT_OPERATOR_F_ACCEPT_INTEGRAL
#define CINT_OPERATOR_FLAGS_ABINARY CINT_OPERATOR_FLAGS_IBINARY | CINT_OPERATOR_F_ACCEPT_DOUBLE
#define CINT_OPERATOR_FLAGS_PBINARY CINT_OPERATOR_FLAGS_ABINARY | CINT_OPERATOR_F_ACCEPT_POINTER
     

typedef enum cint_operator_e {
    
#define CINT_OPERATOR_LIST_ENTRY(name, _entry, f) cintOp##_entry ,

#include "cint_op_entry.h"

    cintOpLast

} cint_operator_t; 


/*
 * Operator operand types
 */
typedef enum cint_operand_type_e {
    
    cintOperandInt,
    cintOperandUInt,
    cintOperandLong,
    cintOperandULong,
    cintOperandLongLong,
    cintOperandULongLong,
    cintOperandDouble,
    cintOperandPointer

} cint_operand_type_t; 
    
/*
 * Evaluate an operator AST. 
 */
struct cint_ast_s; 
struct cint_variable_s; 

extern struct cint_variable_s* 
cint_eval_operator(struct cint_ast_s* opnode); 

extern cint_operator_t
cint_operator_type(struct cint_ast_s* ast); 

struct cint_variable_s*
cint_eval_operator_internal(struct cint_ast_s* ast, cint_operator_t op, 
                            struct cint_variable_s* left, 
                            struct cint_variable_s* right); 

extern const char* 
cint_operator_name(cint_operator_t op);

#endif /* __CINT_OPERATORS_H__ */
