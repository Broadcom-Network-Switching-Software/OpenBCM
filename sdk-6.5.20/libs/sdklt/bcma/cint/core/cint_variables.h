/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_variables.h
 * Purpose:     CINT variable interfaces
 */

#ifndef __CINT_VARIABLES_H__
#define __CINT_VARIABLES_H__

/*
 * Generic variable instance structure
 */

#include "cint_types.h"
#include "cint_error.h"
#include "cint_interpreter.h"

#define CINT_VARIABLE_F_AUTO            0x1
#define CINT_VARIABLE_F_SDATA           0x2
#define CINT_VARIABLE_F_SNAME           0x4
#define CINT_VARIABLE_F_CONST           0x8
#define CINT_VARIABLE_F_VOLATILE        0x10
#define CINT_VARIABLE_F_NODESTROY       0x20
#define CINT_VARIABLE_F_AUTOCAST        0x40
#define CINT_VARIABLE_F_CSTRING         0x80
#define CINT_VARIABLE_F_ENUM            0x100

typedef struct cint_variable_s {

    /* Private */
    struct cint_variable_s* next;
    struct cint_variable_s* prev;


    /* Variable Flags */
    unsigned int flags;

    /* The name of the variable */
    char* name;

    cint_datatype_t dt;

    void* data;
    int size;

    /*
     * When arrays are converted to pointers their dimension data is saved here
     * and the source is set to zero.
     */
    int cached_num_dimensions;

} cint_variable_t;


/*
 * Initialize variable management
 */
extern int cint_variable_init(void);

/*
 * Start a new local variable scope.
 * All variables from previous local scopes are also available.
 */
extern int cint_variable_lscope_push(const char* msg);

/*
 * End the existing local variable scope
 */
extern int cint_variable_lscope_pop(const char* msg);

/*
 * Start a new variable scope.
 */
extern int cint_variable_scope_push(const char* msg);

/*
 * End an existing variable scope.
 */
extern int cint_variable_scope_pop(const char* msg);

/*
 * Variable List for given scope
 */
extern cint_variable_t* cint_variable_scope(int);

/*
 * Allocate a variable
 */
extern cint_variable_t* cint_variable_alloc(void);

/*
 * Free a variable
 */
extern int cint_variable_free(cint_variable_t* v);

/*
 * Find a variable. All scopes or current scope only.
 */
extern cint_variable_t* cint_variable_find(const char* name, int cscope);

/*
 * Add a variable to the current scope or global scope.
 */
extern int cint_variable_add(cint_variable_t* v);
extern int cint_variable_list_remove(cint_variable_t* v);
extern int cint_variable_rename(cint_variable_t* v, const char *name);

/*
 * Delete a variable from the current scope.
 */
extern int cint_variable_delete(const char* name);

/*
 * Clear and delete all variables with the AUTO flag in the current scope
 */
extern int cint_variable_auto_save(cint_variable_t* v);
extern int cint_variable_auto_clear(void);

extern int cint_variable_clear(void);
extern int cint_variable_dump(void);

#endif /* __CINT_VARIABLES_H__ */
