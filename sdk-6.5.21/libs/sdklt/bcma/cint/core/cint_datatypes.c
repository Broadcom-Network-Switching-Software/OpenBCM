/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_datatypes.c
 * Purpose:     CINT datatype manipulation
 */

#include "cint_config.h"
#include "cint_porting.h"
#include "cint_datatypes.h"
#include "cint_internal.h"
#include "cint_interpreter.h"


/*******************************************************************************
 *
 * Global Static Data Variables
 */

/*
 * Atomic Data Types
 */
typedef struct __atomic_list_s {
    struct __atomic_list_s* next;
    cint_atomic_type_t* types;
} __atomic_list_t;

static __atomic_list_t* __atomics_list = NULL;


/*
 * Static Data Types
 */
typedef struct __data_list_s {
    struct __data_list_s* next;
    cint_data_t* data;
    void *dlhandle;
} __data_list_t;

static __data_list_t* __data_list = NULL;

/*
 * Dynamic Data Types
 */
typedef struct __function_list_s {
    struct __function_list_s* next;
    cint_function_t* function;
} __function_list_t;

typedef struct __structure_list_s {
    struct __structure_list_s* next;
    cint_struct_type_t* structure;
} __structure_list_t;

typedef struct __enum_list_s {
    struct __enum_list_s* next;
    cint_enum_type_t* enumeration;
} __enum_list_t;

static __function_list_t* __function_list = NULL;
static __structure_list_t* __structure_list = NULL;
static __enum_list_t* __enum_list = NULL;

#define CINT_CACHE_SIZE 10
typedef struct cint_type_cache_s {
    char basetype[CINT_CONFIG_MAX_VARIABLE_NAME];
    cint_datatype_t dt;
    unsigned int ref_cnt;
} cint_type_cache_t;
static cint_type_cache_t type_cache[CINT_CACHE_SIZE];

void
cint_datatype_clear_structure(cint_struct_type_t* structure)
{
    if (structure) {
        if (structure->name) {
            CINT_FREE((void *)structure->name);
        }
        if (structure->struct_members) {
            const cint_parameter_desc_t *p = structure->struct_members;
            while (p->name) {
                CINT_FREE((void *)p->name);
                if (p->basetype) {
                    CINT_FREE((void *)p->basetype);
                }
                p++;
            }
            CINT_FREE((void *)structure->struct_members);
        }
        CINT_FREE(structure);
    }
}

void
cint_datatype_clear_function(cint_function_t* function)
{
    if (function) {
        cint_ast_touch(function->body.statements, -1);
        if (function->name) {
            CINT_FREE((void *)function->name);
        }
        if (function->params) {
            cint_parameter_desc_t* p;
            for (p=function->params; p->basetype; p++) {
                CINT_FREE((void *)p->basetype);
                if (p->name) {
                    CINT_FREE((void *)p->name);
                }
            }
            CINT_FREE(function->params);
        }
        CINT_FREE(function);
    }
}

void
cint_datatype_clear_enumeration(cint_enum_type_t* enumeration)
{
    if (enumeration) {
        if (enumeration->enum_map) {
            cint_enum_map_t* map;

            for (map = enumeration->enum_map; map->name; map++ ) {
                CINT_FREE((void *)map->name);
            }
            CINT_FREE(enumeration->enum_map);
        }
        if (enumeration->name) {
            CINT_FREE((void *)enumeration->name);
        }
        CINT_FREE(enumeration);
    }
}

void
cint_datatype_clear(void)
{
    /*
     * Free all allocated data structures
     */
    __atomic_list_t* ap;
    __data_list_t* dp;
    __function_list_t* fp;
    __structure_list_t* sp;
    __enum_list_t* ep;

    ap = __atomics_list;
    while(ap) {
        __atomic_list_t* n = ap->next;
        CINT_FREE(ap);
        ap = n;
    }
    __atomics_list = NULL;


    dp = __data_list;
    while(dp) {
        __data_list_t* n = dp->next;
#if CINT_CONFIG_INCLUDE_CINT_LOAD == 1
        cint_ast_cint_cmd_unload(dp->dlhandle);
#endif
        CINT_FREE(dp);
        dp = n;
    }
    __data_list = NULL;

    fp = __function_list;
    while(fp) {
        __function_list_t* n = fp->next;
        cint_datatype_clear_function(fp->function);
        CINT_FREE(fp);
        fp = n;
    }
    __function_list = NULL;

    sp = __structure_list;
    while(sp) {
        __structure_list_t* n = sp->next;
        cint_datatype_clear_structure(sp->structure);
        CINT_FREE(sp);
        sp = n;
    }
    __structure_list = NULL;

    ep = __enum_list;
    while(ep) {
        __enum_list_t* n = ep->next;
        cint_datatype_clear_enumeration(ep->enumeration);
        CINT_FREE(ep);
        ep = n;
    }
    __enum_list = NULL;

    CINT_MEMSET(type_cache, 0, sizeof(cint_type_cache_t)*CINT_CACHE_SIZE);
    return;
}

/*
 * Format a basetype, pointer count, and array specification
 * NOTE: returns static storage for the result unless you specify alloc=1
 */

char*
cint_datatype_format_pd(const cint_parameter_desc_t* pd, int alloc)
{
    static char _rstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER*2] = {0};
    char _pstr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    char _astr[CINT_CONFIG_MAX_STACK_PRINT_BUFFER] = {0};
    int i;
    int position;
    char* p;


    /*
     * Generate the pointer part of the string
     */
    _pstr[0] = 0;
    _astr[0] = 0;
    for(p = _pstr, i = 0; i < pd->pcount; i++, p++) {
        *p = '*';
    }
    *p = 0;

    /*
     * Generate the array part of the string
     */
    if(pd->num_dimensions) {
        for(i = 0, position = 0;
            i < pd->num_dimensions &&
            pd->dimensions[i] != CINT_CONFIG_POINTER_INFINITE_DIMENSION;
            i++) {
            position += CINT_SPRINTF(
                &_astr[position],
                "[%d]",
                pd->dimensions[i]);
        }
    }

    /*
     * Combine and return
     */
    CINT_SPRINTF(_rstr, "%s%s%s", pd->basetype, _pstr, _astr);

    return (alloc) ? CINT_STRDUP(_rstr) : _rstr;
}

char*
cint_datatype_format(const cint_datatype_t* dt, int alloc)
{
    return cint_datatype_format_pd(&dt->desc, alloc);
}


int
cint_datatype_traverse(int flags, cint_datatype_traverse_t handler, void* cookie)
{
    __atomic_list_t* alist;
    __data_list_t* dlist;
    __function_list_t* flist;
    __structure_list_t* slist;
    __enum_list_t* elist;

    cint_datatype_t dt;
    CINT_MEMSET(&dt, 0, sizeof(dt));

    if(flags == 0) {
        /* Match all */
        flags = -1;
    }

    if(handler == NULL) {
        return CINT_E_PARAM;
    }

    if(flags & CINT_DATATYPE_F_ATOMIC) {

        /* Traverse all atomic types */
        for(alist = __atomics_list; alist; alist = alist->next) {
            cint_atomic_type_t* ap;

            for(ap = alist->types; ap->name; ap++) {

                CINT_MEMSET(&dt, 0, sizeof(dt));
                dt.flags = CINT_DATATYPE_F_ATOMIC;
                dt.desc.basetype = ap->name;
                dt.basetype.p = ap;

                if(handler(cookie, &dt) < 0) {
                    return -1;
                }
            }
        }

    }

    /*
     * Travserse all static datatypes
     */

    for(dlist = __data_list; dlist; dlist = dlist->next) {

        cint_struct_type_t* sp;
        cint_enum_type_t* ep;
        cint_constants_t* cp;
        cint_function_t* fp;
        cint_parameter_desc_t* tp;
        cint_function_pointer_t* fcbp;
        cint_custom_iterator_t* ip;
        cint_custom_macro_t* mp;

        if(dlist->data) {

            if(flags & CINT_DATATYPE_F_STRUCT) {

                for(sp = dlist->data->structures; sp && sp->name; sp++) {

                    CINT_MEMSET(&dt, 0, sizeof(dt));
                    dt.flags = CINT_DATATYPE_F_STRUCT;
                    dt.desc.basetype = sp->name;
                    dt.basetype.p = sp;

                    if(handler(cookie, &dt) < 0) {
                        return -1;
                    }
                }
            }

            if(flags & CINT_DATATYPE_F_ENUM) {

                for(ep = dlist->data->enums; ep && ep->name; ep++) {

                    CINT_MEMSET(&dt, 0, sizeof(dt));
                    dt.flags = CINT_DATATYPE_F_ENUM;
                    dt.desc.basetype = ep->name;
                    dt.basetype.p = ep;

                    if(handler(cookie, &dt) < 0) {
                        return -1;
                    }
                }
            }

            if(flags & CINT_DATATYPE_F_CONSTANT) {

                for(cp = dlist->data->constants; cp && cp->name; cp++) {

                    CINT_MEMSET(&dt, 0, sizeof(dt));
                    dt.flags = CINT_DATATYPE_F_CONSTANT;
                    dt.desc.basetype = "int";
                    dt.basetype.p = cp;

                    if(handler(cookie, &dt) < 0) {
                        return -1;
                    }
                }
            }

            if(flags & CINT_DATATYPE_F_FUNC) {

                for(fp = dlist->data->functions; fp && fp->name; fp++) {

                    CINT_MEMSET(&dt, 0, sizeof(dt));
                    dt.flags = CINT_DATATYPE_F_FUNC;
                    dt.desc.basetype = fp->name;
                    dt.basetype.p = fp;

                    if(handler(cookie, &dt) < 0) {
                        return -1;
                    }
                }
            }

            if(flags & CINT_DATATYPE_F_MACRO) {

                for(mp = dlist->data->macros; mp && mp->name; mp++) {

                    CINT_MEMSET(&dt, 0, sizeof(dt));
                    dt.flags = CINT_DATATYPE_F_MACRO;
                    dt.desc.basetype = mp->name;
                    dt.basetype.p = mp;

                    if(handler(cookie, &dt) < 0) {
                        return -1;
                    }
                }
            }

            if(flags & CINT_DATATYPE_F_ITERATOR) {

                for(ip = dlist->data->iterators; ip && ip->name; ip++) {

                    CINT_MEMSET(&dt, 0, sizeof(dt));
                    dt.flags = CINT_DATATYPE_F_ITERATOR;
                    dt.desc.basetype = ip->name;
                    dt.basetype.p = ip;

                    if(handler(cookie, &dt) < 0) {
                        return -1;
                    }
                }
            }

            for(tp = dlist->data->typedefs; tp && tp->name; tp++) {
                int rc;
                const char* basetype = tp->name;
                if((rc = cint_datatype_find(tp->basetype, &dt)) == 0) {
                    /*
                     * Eventually resolved to a basetype.
                     * Restore the original name of the type.
                     */
                    if(flags & dt.flags) {
                        dt.desc = *tp;
                        dt.flags |= CINT_DATATYPE_F_TYPEDEF;
                        cint_strlcpy(dt.type, basetype, sizeof(dt.type));

                        if(handler(cookie, &dt) < 0) {
                            return -1;
                        }
                    }
                }
            }

            if(flags & CINT_DATATYPE_F_FUNC_POINTER) {

                for(fcbp = dlist->data->fpointers; fcbp && fcbp->name; fcbp++) {

                    CINT_MEMSET(&dt, 0, sizeof(dt));
                    dt.flags = CINT_DATATYPE_F_FUNC_POINTER;
                    dt.desc.basetype = fcbp->name;
                    dt.basetype.p = fcbp;

                    if(handler(cookie, &dt) < 0) {
                        return -1;
                    }
                }
            }
        }
    }


    /*
     * Travserse all dynamic datatypes
     */

    if(flags & CINT_DATATYPE_F_FUNC_DYNAMIC) {

        for(flist = __function_list; flist; flist = flist->next) {

            CINT_MEMSET(&dt, 0, sizeof(dt));
            dt.flags = CINT_DATATYPE_F_FUNC_DYNAMIC;
            dt.desc.basetype = flist->function->name;
            dt.basetype.p = flist->function;

            if(handler(cookie, &dt) < 0) {
                return -1;
            }
        }
    }

    if(flags & CINT_DATATYPE_F_STRUCT) {

        for(slist = __structure_list; slist; slist = slist->next) {

            CINT_MEMSET(&dt, 0, sizeof(dt));
            dt.flags = CINT_DATATYPE_F_STRUCT;
            dt.desc.basetype = slist->structure->name;
            dt.basetype.p = slist->structure;

            if(handler(cookie, &dt) < 0) {
                return -1;
            }
        }
    }

    if(flags & CINT_DATATYPE_F_ENUM) {

        for(elist = __enum_list; elist; elist = elist->next) {

            CINT_MEMSET(&dt, 0, sizeof(dt));
            dt.flags = CINT_DATATYPE_F_ENUM;
            dt.desc.basetype = elist->enumeration->name;
            dt.basetype.p = elist->enumeration;

            if(handler(cookie, &dt) < 0) {
                return -1;
            }
        }
    }

    return 0;
}

static int
_cint_datatype_find(const char* basetype, cint_datatype_t* dt)
{
    __atomic_list_t* alist;
    __data_list_t* dlist;
    __function_list_t* flist;
    __structure_list_t* slist;
    __enum_list_t* elist;

    if(dt) {
        cint_atomic_type_t* ap = dt->cap;
        CINT_MEMSET(dt, 0, sizeof(*dt));
        cint_strlcpy(dt->type, basetype, sizeof(dt->type));
        dt->cap = ap;
    }

    /*
     * Search all atomic types
     */
    for(alist = __atomics_list; alist; alist = alist->next) {

        cint_atomic_type_t* ap;

        for(ap = alist->types; ap->name; ap++) {
            if(!CINT_STRCMP(basetype, ap->name)) {
                /*
                 * Found atomic type
                 */
                if(ap->flags & CINT_ATOMIC_TYPE_F_CAP_ONLY) {
                    /*
                     * This defines custom assign and print vectors only for the datatype.
                     * Do not treat this as the definition (which should be found later in the data).
                     *
                     * These vectors can be retrieved separately when an assignment or print
                     * is requested.
                     */
                    if(dt) {
                        dt->cap = ap;
                    }
                    /* Continue searching */
                }
                else {
                    if(dt) {
                        dt->flags = CINT_DATATYPE_F_ATOMIC;
                        dt->desc.basetype = ap->name;
                        dt->basetype.p = ap;
                    }

                    return CINT_E_NONE;
                }
            }
        }
    }

    /*
     * Search all static data types
     */
    for(dlist = __data_list; dlist; dlist = dlist->next) {

        cint_struct_type_t* sp;
        cint_enum_type_t* ep;
        cint_constants_t* cp;
        cint_function_t* fp;
        cint_parameter_desc_t* tp;
        cint_function_pointer_t* fcbp;

        if(dlist->data) {

            for(sp = dlist->data->structures; sp && sp->name; sp++) {
                if(!CINT_STRCMP(basetype, sp->name)) {
                    /*
                     * Found structure type
                     */
                    if(dt) {
                        dt->flags = CINT_DATATYPE_F_STRUCT;
                        dt->desc.basetype = sp->name;
                        dt->basetype.p = sp;
                    }
                    return CINT_E_NONE;
                }
            }

            for(ep = dlist->data->enums; ep && ep->name; ep++) {
                if(!CINT_STRCMP(basetype, ep->name)) {
                    /*
                     * Found enum type
                     */
                    if(dt) {
                        dt->flags = CINT_DATATYPE_F_ENUM;
                        dt->desc.basetype = ep->name;
                        dt->basetype.p = ep;
                    }
                    return CINT_E_NONE;
                }
            }

            for(cp = dlist->data->constants; cp && cp->name; cp++) {
                if(!CINT_STRCMP(basetype, cp->name)) {
                    /*
                     * Found constant type
                     */
                    if(dt) {
                        dt->flags = CINT_DATATYPE_F_CONSTANT;
                        dt->desc.basetype = "int";
                        dt->basetype.p = cp;
                    }
                    return CINT_E_NONE;
                }
            }

            for(fp = dlist->data->functions; fp && fp->name; fp++) {
                if(!CINT_STRCMP(basetype, fp->name)) {
                    /*
                     * Found function
                     */
                    if(dt) {
                        dt->flags = CINT_DATATYPE_F_FUNC;
                        if (fp->params == NULL) {
                            dt->flags |= CINT_DATATYPE_F_FUNC_VARARG;
                        }
                        dt->desc.basetype = fp->name;
                        dt->basetype.p = fp;
                    }
                    return CINT_E_NONE;
                }
            }

            for(tp = dlist->data->typedefs; tp && tp->name; tp++) {
                if(!CINT_STRCMP(basetype, tp->name)) {
                    /*
                     * Found a typedef. Resolve to basetype
                     */
                    int rc;
                    if((rc = _cint_datatype_find(tp->basetype, dt)) == 0) {
                        /*
                         * Eventually resolved to a basetype.
                         * Restore the original name of the type.
                         */
                        if(dt) {
                            dt->desc = *tp;
                            dt->type_num_dimensions = dt->desc.num_dimensions;
                            dt->flags |= CINT_DATATYPE_F_TYPEDEF;
                            cint_strlcpy(dt->type, basetype, sizeof(dt->type));
                        }
                    }
                    return rc;
                }
            }

            for(fcbp = dlist->data->fpointers; fcbp && fcbp->name; fcbp++) {
                if(!CINT_STRCMP(basetype, fcbp->name)) {
                    /*
                     * Found a function pointer callback type
                     */
                    if(dt) {
                        dt->flags = CINT_DATATYPE_F_FUNC_POINTER;
                        dt->desc.basetype = fcbp->name;
                        dt->basetype.p = fcbp;
                    }
                    return CINT_E_NONE;
                }
            }
        }
    }


    /*
     * Search all dynamic datatypes
     */
    for(flist = __function_list; flist; flist = flist->next) {
        if(!CINT_STRCMP(basetype, flist->function->name)) {
            /*
             * Found function
             */
            if(dt) {
                dt->flags = CINT_DATATYPE_F_FUNC_DYNAMIC;
                dt->desc.basetype = flist->function->name;
                dt->basetype.p = flist->function;
            }
            return CINT_E_NONE;
        }
    }

    for(slist = __structure_list; slist; slist = slist->next) {
        if(!CINT_STRCMP(basetype, slist->structure->name)) {
            if(dt) {
                dt->flags = CINT_DATATYPE_F_STRUCT;
                dt->desc.basetype = slist->structure->name;
                dt->basetype.p = slist->structure;
            }
            return CINT_E_NONE;
        }
    }

    for(elist = __enum_list; elist; elist = elist->next) {
        if(!CINT_STRCMP(basetype, elist->enumeration->name)) {
            if(dt) {
                dt->flags = CINT_DATATYPE_F_ENUM;
                dt->desc.basetype = elist->enumeration->name;
                dt->basetype.p = elist->enumeration;
            }
            return CINT_E_NONE;
        }
    }

    return CINT_E_TYPE_NOT_FOUND;
}

int
cint_datatype_find(const char* basetype, cint_datatype_t* dt)
{
    int rc = 0;

    int i = 0, min_index = 0;
    unsigned int min = type_cache[0].ref_cnt;
    cint_atomic_type_t* ap = NULL;

    if(dt) {
        ap = dt->cap;
        CINT_MEMSET(dt, 0, sizeof(*dt));
    }

    for(i = 0; i < CINT_CACHE_SIZE; i++) {
        if(dt && !CINT_STRCMP(basetype, type_cache[i].basetype)) {
            CINT_MEMCPY(dt, &type_cache[i].dt, sizeof(cint_datatype_t));
            type_cache[i].ref_cnt++;
            cint_strlcpy(dt->type, basetype, sizeof(dt->type));
            if(NULL == dt->cap) {
                dt->cap = ap;
            }
            return CINT_E_NONE;
        }
    }

    if((rc = _cint_datatype_find(basetype, dt)) != CINT_E_NONE) {
        /* Datatype not found */
        return rc;
    } else if (dt) {
        for(i = 0; i < CINT_CACHE_SIZE; i++) {
            if (0 == type_cache[i].ref_cnt) {
                min_index = i;
                break;
            }
            if(type_cache[i].ref_cnt <= min) {
                min_index = i;
                min = type_cache[i].ref_cnt;
            }
        }
        CINT_STRNCPY(type_cache[min_index].basetype, basetype, (CINT_CONFIG_MAX_VARIABLE_NAME - 1));
        CINT_MEMCPY(&type_cache[min_index].dt, dt, sizeof(cint_datatype_t));
        type_cache[min_index].ref_cnt = 1;
    }

    return CINT_E_NONE;
}


int
cint_datatype_size(const cint_datatype_t* dt)
{
    int size = -1;
    int dim_index;

    if(dt->desc.pcount != 0) {
        /* This is a pointer */
        size = sizeof(void*);
    }
    else {
        if(dt->flags & CINT_DATATYPE_F_ATOMIC) {
            size = dt->basetype.ap->size;
        }
        else if(dt->flags & CINT_DATATYPE_F_STRUCT) {
            size = dt->basetype.sp->size;
        }
        else if(dt->flags & CINT_DATATYPE_F_ENUM) {
            size = sizeof(int);
        }
        else if(dt->flags & (CINT_DATATYPE_F_FUNC_POINTER | CINT_DATATYPE_F_FUNC | CINT_DATATYPE_F_FUNC_DYNAMIC)) {
            size = sizeof( void (*)(void) );
        }
        else {
            cint_internal_error(__FILE__, __LINE__, "invalid datatype in cint_datatype_size()");
            return -1;
        }
    }

    for(dim_index = 0;
        dim_index < dt->desc.num_dimensions &&
            dt->desc.dimensions[dim_index] !=
                CINT_CONFIG_POINTER_INFINITE_DIMENSION;
        dim_index++) {
        size *= dt->desc.dimensions[dim_index];
    }

    return size;

}

int
cint_datatype_add_atomics(cint_atomic_type_t* types)
{
    __atomic_list_t* n = CINT_MALLOC(sizeof(*n));

    if (n == NULL) {
        return CINT_E_MEMORY;
    }

    n->types = types;
    n->next = __atomics_list;
    __atomics_list = n;

    return CINT_E_NONE;
}

static int
cint_datatype_convert_array_parameters(cint_parameter_desc_t * parameter_list)
{
    cint_parameter_desc_t* current;

    for(current = parameter_list; current && current->basetype; current++) {
        if(current->array != 0) {
            if(current->num_dimensions != 0) {
                cint_internal_error(
                    __FILE__,
                    __LINE__,
                    "type defined with both legacy and current array parameters");
                return CINT_E_INTERNAL;
            }
            current->num_dimensions = 1;
            current->dimensions[0] = current->array;
            current->array = 0;
        }
    }
    return CINT_E_NONE;
}

static int cint_datatype_convert_array_data(cint_data_t* data) {
    cint_function_t* functions;
    cint_function_pointer_t* functionPointers;
    cint_struct_type_t* structures;
    int rc;

    rc = cint_datatype_convert_array_parameters(data->typedefs);
    if(rc != CINT_E_NONE) {
        return rc;
    }

    for(functions = data->functions;
        functions && functions->name;
        functions++) {
        rc = cint_datatype_convert_array_parameters(functions->params);
        if(rc != CINT_E_NONE) {
            return rc;
        }
    }

    for(functionPointers = data->fpointers;
        functionPointers && functionPointers->name;
        functionPointers++) {
        rc = cint_datatype_convert_array_parameters(
            functionPointers->params);
        if(rc != CINT_E_NONE) {
            return rc;
        }
    }

    for(structures = data->structures;
        structures && structures->name;
        structures++) {
        rc = cint_datatype_convert_array_parameters(
            structures->struct_members);
        if(rc != CINT_E_NONE) {
            return rc;
        }
    }
    return CINT_E_NONE;
}

int
cint_datatype_add_data(cint_data_t* data, void *dlhandle)
{
    int rc;
    __data_list_t* n;

    rc = cint_datatype_convert_array_data(data);
    if(rc != CINT_E_NONE) {
        return rc;
    }

    n = CINT_MALLOC(sizeof(*n));
    if (n == NULL) {
        return CINT_E_MEMORY;
    }

    n->data = data;
    n->dlhandle = dlhandle;
    n->next = __data_list;
    __data_list = n;

    return CINT_E_NONE;
}

int
cint_datatype_add_function(cint_function_t* f)
{
    int rc;
    __function_list_t* n;

    rc = cint_datatype_convert_array_parameters(f->params);
    if(rc != CINT_E_NONE) {
        return rc;
    }

    n = CINT_MALLOC(sizeof(*n));
    if (n == NULL) {
        return CINT_E_MEMORY;
    }

    cint_ast_touch(f->body.statements, 1);
    n->function = f;
    n->next = __function_list;
    __function_list = n;

    return CINT_E_NONE;
}

void
cint_datatype_delete_type_cache(const char* basetype)
{
    int i;
    for(i = 0; i < CINT_CACHE_SIZE; i++) {
        if(!CINT_STRCMP(basetype, type_cache[i].basetype)) {
            CINT_MEMSET(&type_cache[i], 0, sizeof(cint_type_cache_t));
            return;
        }
    }
}

void
cint_datatype_delete_function(const char* fname)
{
    __function_list_t* flist = NULL;
    __function_list_t* prev  = NULL;
    __function_list_t* next  = NULL;

    for(flist = prev = __function_list; flist; flist = flist->next) {
        if(!CINT_STRCMP(fname, flist->function->name)) {
            next = flist->next;
            if( flist == __function_list ){
                __function_list = next;
            }
            cint_datatype_clear_function(flist->function);
            prev->next = next;
            CINT_FREE(flist);
            cint_datatype_delete_type_cache(fname);
            return;
        }
        prev = flist;
    }
    return ;
}

int
cint_datatype_add_structure(cint_struct_type_t* cst)
{
    int rc;
    __structure_list_t* n;

    rc = cint_datatype_convert_array_parameters(cst->struct_members);
    if(rc != CINT_E_NONE) {
        return rc;
    }

    n = CINT_MALLOC(sizeof(*n));
    if (n == NULL) {
        return CINT_E_MEMORY;
    }

    n->structure = cst;
    n->next = __structure_list;
    __structure_list = n;

    return CINT_E_NONE;
}

int
cint_datatype_add_enumeration(cint_enum_type_t* cet)
{
    __enum_list_t* n = CINT_MALLOC(sizeof(*n));

    if (n == NULL) {
        return CINT_E_MEMORY;
    }

    n->enumeration = cet;
    n->next = __enum_list;
    __enum_list = n;

    return CINT_E_NONE;
}

int
cint_datatype_enum_find(const char* name, cint_datatype_t* dt, int* value)
{
    __data_list_t* dlist;
    __enum_list_t* elist;

    if(dt) {
        CINT_MEMSET(dt, 0, sizeof(*dt));
    }

    for(dlist = __data_list; dlist; dlist = dlist->next) {

        cint_enum_type_t* ep;

        if(dlist->data) {

            for(ep = dlist->data->enums; ep && ep->name; ep++) {
                cint_enum_map_t* p;
                for(p = ep->enum_map; p->name; p++) {
                    if(!CINT_STRCMP(name, p->name)) {
                        if(dt) {
                            dt->flags = CINT_DATATYPE_F_ENUM;
                            dt->desc.basetype = ep->name;
                            dt->basetype.p = ep;
                        }
                        if(value) {
                            *value = p->value;
                        }
                        return CINT_E_NONE;
                    }
                }
            }
        }
    }

    for(elist = __enum_list; elist; elist = elist->next) {
        cint_enum_map_t* p;
        for(p = elist->enumeration->enum_map; p->name; p++) {
            if(!CINT_STRCMP(name, p->name)) {
                if(dt) {
                    dt->flags = CINT_DATATYPE_F_ENUM;
                    dt->desc.basetype = elist->enumeration->name;
                    dt->basetype.p = elist->enumeration;
                }
                if(value) {
                    *value = p->value;
                }
                return CINT_E_NONE;
            }
        }
    }

    return CINT_E_NOT_FOUND;
}


int
cint_datatype_constant_find(const char* name, int* c)
{
    /*
     * Search all enum and constant datatypes
     */
    __data_list_t* dlist;
    __enum_list_t* elist;

    for(dlist = __data_list; dlist; dlist = dlist->next) {

        cint_enum_type_t* ep;
        cint_constants_t* cp;

        if(dlist->data) {

            for(ep = dlist->data->enums; ep && ep->name; ep++) {
                cint_enum_map_t* p;
                for(p = ep->enum_map; p->name; p++) {
                    if(!CINT_STRCMP(name, p->name)) {
                        if (c) {
                            *c = p->value;
                        }
                        return CINT_E_NONE;
                    }
                }
            }

            for(cp = dlist->data->constants; cp && cp->name; cp++) {
                if(!CINT_STRCMP(name, cp->name)) {
                    if (c) {
                        *c = cp->value;
                    }
                    return CINT_E_NONE;
                }
            }
        }
    }

    for(elist = __enum_list; elist; elist = elist->next) {
        cint_enum_map_t* p;
        for(p = elist->enumeration->enum_map; p->name; p++) {
            if(!CINT_STRCMP(name, p->name)) {
                if (c) {
                    *c = p->value;
                }
                return CINT_E_NONE;
            }
        }
    }

    return CINT_E_NOT_FOUND;
}

int
cint_datatype_checkall(int print)
{

    /*
     * Validate the existance of all datatypes referenced
     * in structures and function calls.
     *
     * Usually used to find missing atomics.
     */

    __atomic_list_t* alist;
    __data_list_t* dlist;
    __function_list_t* flist;

    int rv = CINT_E_NONE;

    for(dlist = __data_list; dlist; dlist = dlist->next) {
        cint_function_t* fp;
        cint_struct_type_t* sp;
        cint_parameter_desc_t* pd;

        for(fp = dlist->data->functions; fp && fp->name; fp++) {
            /*
             * Check all function parameter types
             */
            const cint_parameter_desc_t* p;
            for(p = fp->params; p && p->basetype; p++) {
                if(cint_datatype_find(p->basetype, NULL) != 0) {
                    if(print) {
                        CINT_PRINTF("type '%s' is missing (parameter to function %s)\n", p->basetype, fp->name);
                        rv = CINT_E_NOT_FOUND;
                    }
                }
            }
        }
        for(sp = dlist->data->structures; sp && sp->name; sp++) {
            /*
             * Check all struct member types
             */
            const cint_parameter_desc_t* sm;
            for(sm = sp->struct_members; sm->name; sm++) {
                if(cint_datatype_find(sm->basetype, NULL) != 0) {
                    if(print) {
                        CINT_PRINTF("type '%s' is missing (struct member of %s)\n", sm->basetype, sp->name);
                    }
                    rv = CINT_E_NOT_FOUND;
                }
            }
        }
        for(pd = dlist->data->typedefs; pd && pd->basetype; pd++) {
            /*
             * Check all typedef'ed types.
             */
            if( (cint_datatype_find(pd->basetype, NULL) != 0) &&
                (cint_datatype_find(pd->name, NULL) != 0) ) {
                if(print) {
                    CINT_PRINTF("type '%s' is missing (typedef for '%s')\n", pd->basetype, pd->name);
                }
                rv = CINT_E_NOT_FOUND;
            }
        }
    }


    for(flist = __function_list; flist; flist = flist->next) {
        /*
         * Check all dynamic function parameter types
         */
        const cint_parameter_desc_t* p;
        for(p = flist->function->params; p && p->basetype; p++) {
            if(cint_datatype_find(p->basetype, NULL) != 0) {
                if(print) {
                    CINT_PRINTF("type '%s' is missing\n", p->basetype);
                    rv = CINT_E_NOT_FOUND;
                }
            }
        }
    }

    /*
     * Validate the handlers for all atomic types
     */
    for(alist = __atomics_list; alist; alist = alist->next) {

        const cint_atomic_type_t* ap;
        for(ap = alist->types; ap && ap->name; ap++) {

            if(ap->flags & CINT_ATOMIC_TYPE_F_ABSTRACT) {
                continue;
            }

            /*
             * Every type must have a print function
             */
            if(ap->format == NULL) {
                CINT_PRINTF("atomic type '%s' does not have a print function\n", ap->name);
                rv = CINT_E_NOT_FOUND;
            }

            /*
             * Every non-native atomic type must have an assign function
             */
            if((ap->flags == 0) && (ap->assign == 0)) {
                CINT_PRINTF("atomic type '%s' does not have an assign function\n", ap->name);
                rv = CINT_E_NOT_FOUND;
            }
        }
    }

    if(rv == CINT_E_NONE && print) {
        CINT_PRINTF("No missing types\n");
    }

    return rv;
}



