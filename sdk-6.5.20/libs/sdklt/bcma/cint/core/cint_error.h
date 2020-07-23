/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_error.h
 * Purpose:     CINT error handling interfaces
 */

#ifndef __CINT_ERROR_H__
#define __CINT_ERROR_H__

#include "cint_porting.h"
#include "cint_ast.h"

/*
 * Error Conditions
 */
typedef enum cint_error_e {

    CINT_E_NONE =             0,
    CINT_E_START =            -100,

#define CINT_ERROR_LIST_ENTRY(_entry) CINT_E_##_entry ,
#include "cint_error_entry.h"

    CINT_E_LAST

} cint_error_t;

extern const char* cint_error_name(int err);

/* Global Error Conditions */
extern volatile cint_error_t cint_errno;

/*
 * Internal Interpreter Errors
 */
extern void cint_internal_error(const char* f, int l, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 3, 4)));

/*
 * Execution errors and warnings
 */
extern void cint_error(const char* f, int l, cint_error_t e, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 4, 5)));
extern void cint_warn(const char* f, int l, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 3, 4)));

extern int cint_ast_error(const cint_ast_t* ast, cint_error_t e, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 3, 4)));
extern int cint_ast_warn(const cint_ast_t* ast, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 2, 3)));


#define CINT_ERRNO_IS(_type) (cint_errno == CINT_E_##_type)


#endif /* __CINT_ERROR_H__ */
