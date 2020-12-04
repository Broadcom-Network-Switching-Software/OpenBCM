/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_datatypes.h
 * Purpose:     CINT datatype interfaces
 */

#ifndef __CINT_DATATYPES_H__
#define __CINT_DATATYPES_H__

#include "cint_ast.h"
#include "cint_types.h"
#include "cint_error.h"

/*
 * Generic datatype description structure
 */

#define CINT_DATATYPE_F_ATOMIC          0x1
#define CINT_DATATYPE_F_STRUCT          0x2
#define CINT_DATATYPE_F_ENUM            0x4
#define CINT_DATATYPE_F_FUNC            0x8
#define CINT_DATATYPE_F_CONSTANT        0x10
#define CINT_DATATYPE_F_FUNC_DYNAMIC    0x20
#define CINT_DATATYPE_F_FUNC_POINTER    0x40
#define CINT_DATATYPE_F_TYPEDEF         0x80
#define CINT_DATATYPE_F_ITERATOR        0x100
#define CINT_DATATYPE_F_MACRO           0x200
#define CINT_DATATYPE_F_FUNC_VARARG     0x400
#define CINT_DATATYPE_FLAGS_FUNC        (CINT_DATATYPE_F_FUNC | CINT_DATATYPE_F_FUNC_DYNAMIC)
#define CINT_DATATYPE_FLAGS_TYPE        (CINT_DATATYPE_F_ATOMIC | CINT_DATATYPE_F_STRUCT | CINT_DATATYPE_F_ENUM | CINT_DATATYPE_F_FUNC_POINTER | CINT_DATATYPE_F_TYPEDEF)

typedef struct cint_datatype_s {
    
    /* Flags for this datatype */
    unsigned int flags; 

    /* 
     * The description of this datatype
     */ 
    cint_parameter_desc_t desc; 
    
    /*
     * Pointer to the description of the basetype
     */
    union {
        cint_atomic_type_t* ap; 
        cint_struct_type_t* sp; 
        cint_enum_type_t* ep; 
        cint_constants_t* cp; 
        cint_function_t* fp; 
        cint_function_pointer_t* fpp; 
        cint_custom_iterator_t* ip; 
        cint_custom_macro_t* mp; 
        void* p; 
    } basetype;  

    /*
     * Original type name. May be different from basetype name based on aliases or typedefs. 
     * Also used for temporary storage.
     */
    char type[CINT_CONFIG_MAX_VARIABLE_NAME]; 

    /*
     * Custom print and assignment vectors which can be used with this datatype. 
     * These are different from the atomic type definition as it this type
     * may still be an aggregate and may be treated or referenced as such in 
     * addition to the custom assignment and print options. 
     */
    cint_atomic_type_t* cap; 

    /* 
     * Used when identifying when custom operations should be used on a type 
     * defined as an array of another type. 
     *
     * For example, consider an atomic type defined to be a char[6] with custom
     * input and output functions.  For a variable of this type we could always
     * use the custom functions.  However, if we represented an array of this
     * type we would have to store the custom functions and only use them after
     * we dereferenced the entity sufficiently.  Storing the dimension count 
     * (one in this case) of the base type allows us to do this. 
 */
    int type_num_dimensions;
} cint_datatype_t; 


/*
 * Use for structures created by hand
 */
#define CINT_STRUCT_TYPE_DEFINE(_struct) \
 { \
     #_struct, \
     sizeof(_struct), \
     __cint_struct_members__##_struct, \
     __cint_maddr__##_struct \
 }


extern int
cint_datatype_size(const cint_datatype_t* dt); 

extern int
cint_datatype_find(const char* basetype, cint_datatype_t* dt); 

extern int
cint_datatype_enum_find(const char* enumid, cint_datatype_t* dt, int* value); 

typedef int (*cint_datatype_traverse_t)(void* cookie, const cint_datatype_t* dt); 

extern int
cint_datatype_traverse(int flags, cint_datatype_traverse_t cb, void* cookie); 

extern char*
cint_datatype_format(const cint_datatype_t* dt, char *_rstr, int alloc); 

extern char*
cint_datatype_format_pd(const cint_parameter_desc_t* pd, char *_rstr, int alloc); 

extern void cint_datatype_clear(void); 
extern int cint_datatype_add_atomics(cint_atomic_type_t* types); 
extern int cint_datatype_add_data(cint_data_t* data, void *dlhandle);
extern int cint_datatype_add_function(cint_function_t* f); 
extern int cint_datatype_add_structure(cint_struct_type_t* s); 
extern int cint_datatype_add_enumeration(cint_enum_type_t* cet); 

extern int cint_datatype_constant_find(const char* name, int* c); 
extern int cint_datatype_checkall(int print); 
extern void cint_datatype_clear_structure(cint_struct_type_t* structure);
extern void cint_datatype_clear_function(cint_function_t* function);
extern void cint_datatype_clear_enumeration(cint_enum_type_t* enumeration);
extern void cint_datatype_delete_function(const char* fname);
extern void cint_datatype_delete_type_cache(const char* fname);

#endif /* __CINT_DATATYPES_H__ */
