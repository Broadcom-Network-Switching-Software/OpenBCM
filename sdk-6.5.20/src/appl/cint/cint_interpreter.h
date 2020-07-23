/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_interpreter.h
 * Purpose:     CINT interpreter interface
 */

#ifndef __CINT_INTERPRETER_H__
#define __CINT_INTERPRETER_H__

#include "cint_ast.h"
#include "cint_types.h"
#include "cint_error.h"
#include "cint_datatypes.h"


typedef struct cint_interpreter_debug_state_s {
    int dtrace; 
    int atrace; 
    int ftrace; 
    int aunique; 
} cint_interpreter_debug_state_t; 

typedef struct cint_interpreter_state_s {
    
    cint_interpreter_debug_state_t debug;
    int parse_only; 
    int print_expr; 
    int extended_events;
} cint_interpreter_state_t; 

typedef enum cint_interpreter_event_s {
    cintEventNone, 
    cintEventReset, 
    cintEventDatatypeAdded, 
    cintEventGlobalVariableAdded, 
    cintEventLast
} cint_interpreter_event_t; 

typedef int (*cint_event_handler_cb)(void* cookie, cint_interpreter_event_t event); 

typedef int (*cint_interpreter_init_cb_t)(void *user_data);

extern cint_interpreter_state_t interp; 

extern cint_error_t cint_interpreter_init(void); 
extern cint_error_t cint_interpreter_event(cint_interpreter_event_t event); 
extern cint_error_t cint_interpreter_event_register(cint_event_handler_cb handler, void* cookie); 
extern cint_error_t cint_interpreter_event_unregister(cint_event_handler_cb handler); 

extern cint_error_t cint_interpreter_add_atomics(cint_atomic_type_t* atomics); 
extern cint_error_t cint_interpreter_add_data(cint_data_t* data, void *dlhandle); 
extern cint_error_t cint_interpreter_validate_types(int print); 
extern cint_error_t cint_interpreter_evaluate(cint_ast_t* root); 
extern cint_error_t cint_interpreter_show_variables(void); 
extern cint_error_t cint_interpreter_create_pointer(const char* basetype, const char* name, void* ptr); 

extern int cint_interpreter_is_type(const char* str); 

extern int cint_interpreter_is_iterator(const char* str, cint_ast_t** handle); 
extern cint_ast_t* cint_interpreter_iterator(cint_ast_t* handle, cint_ast_t* arguments, cint_ast_t* statements); 

extern int cint_interpreter_is_macro(const char* str, cint_ast_t** handle); 
extern cint_ast_t* cint_interpreter_macro(cint_ast_t* handle, cint_ast_t* arguments); 

extern int cint_interpreter_add_function(cint_function_t* f); 
extern int cint_interpreter_run(const char* fname); 
extern int cint_syntax_check(void); 

extern cint_ast_t* cint_interpreter_tree(void); 

extern int cint_interpreter_initialize_register(cint_interpreter_init_cb_t init_cb,
                                                void *user_data); 
extern int cint_interpreter_initialize_unregister(cint_interpreter_init_cb_t init_cb,
                                                  void *user_data); 


#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
extern int cint_interpreter_include_set(const char* path); 
extern int cint_interpreter_include_add(const char* path); 
extern int cint_interpreter_include_get(const char* name, char** path_rv); 
#endif

#if CINT_CONFIG_INCLUDE_PARSER == 1
extern int cint_interpreter_parse(void* handle, const char* prompt, int argc, char** argv);
extern cint_ast_t* cint_interpreter_parse_string(const char* expr); 
#endif



#endif /* __INTERPRETER_H__ */
