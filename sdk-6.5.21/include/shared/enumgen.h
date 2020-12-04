/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	enumgen.h
 * Purpose: 	Defines a set of Macros for managing enums and associated
 *              string arrays.
 */
#ifndef _ENUMGEN_H
#define _ENUMGEN_H

/* Enum typedef and Enum name strings generation Macros */
#define SHR_G_ENUM_BEGIN(etype) typedef enum etype##e {
#define SHR_G_ENUM_END(etype) } etype##t
#define SHR_G_ENTRY(e)      SHR_G_MAKE_STR(e)
#define SHR_G_NAME_BEGIN(etype) {
#define SHR_G_NAME_END(etype) }
#define SHR_G_MAKE_ENUM(e) \
        SHR_G_ENUM_BEGIN(e) \
        SHR_G_ENTRIES(dont_care) \
        SHR_G_ENUM_END(e)


/* These set of macros helps to keep the defined enums in sync with their string
 * name array by requiring all modification in one place.
 * To create a enum of type enum_type_t and an associated string array
 * initializer ENUM_TYPE_NAME_INIALIZER use code described in template below
 * and modify it suitably. For example the template code below defines -
    typedef enum   enum_type_e {
                            enum_type_zero,
                            enum_type_entry1,
                            enum_type_entry2,
                            enum_type_count
                            }   enum_type_t ;                 

    #define ENUM_TYPE_NAME_INITIALIZER {    "zero"  ,           \
                                            "entry1"  ,         \
                                            "entry2"  ,         \
                                            "count"    }  ;

 */

#if 0 /* Template code begin */

#include "enumgen.h"

/* New additions are made below this line */
#define SHR_G_ENTRIES(PFX, DC)      /* DO NOT CHANGE */                     \
SHR_G_ENTRY(PFX, zero),             /* DO NOT CHANGE */                     \
SHR_G_ENTRY(PFX, entry1),           /* ===### Your entries here ###=== */   \
SHR_G_ENTRY(PFX, entry2),           /* ===### Your entries here ###=== */   \
SHR_G_ENTRY(PFX, count)             /* DO NOT CHANGE */
/* New additions are made above this line*/

/* Make the enums */
#undef  SHR_G_MAKE_STR              /* DO NOT CHANGE */
#define SHR_G_MAKE_STR(a)     a     /* DO NOT CHANGE */
SHR_G_MAKE_ENUM(enum_type_);        /* ===### Change enum_type_ ###=== */

/* Make the string array */
#undef  SHR_G_MAKE_STR              /* DO NOT CHANGE */
#define SHR_G_MAKE_STR(a)     #a    /* DO NOT CHANGE */
#define ENUM_TYPE_NAME_INITIALIZER  /* ===### Change ENUM_TYPE_ ###=== */   \
        SHR_G_NAME_BEGIN(dont_care) /* DO NOT CHANGE */                     \
        SHR_G_ENTRIES(, dont_care)  /* DO NOT CHANGE */                     \
        SHR_G_NAME_END(dont_care)   /* DO NOT CHANGE */

#endif /* Template code end */

#endif  /* !_ENUMGEN_H */
