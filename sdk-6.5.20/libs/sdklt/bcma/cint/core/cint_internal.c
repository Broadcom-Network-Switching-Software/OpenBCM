/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_internal.c
 * Purpose:     CINT internal functions
 */

#include "cint_internal.h"
#include "cint_porting.h"
#include "cint_variables.h"
#include "cint_interpreter.h"



/*
 * Create a variable of the given type.
 */
static int _auto_counter = 0;

cint_variable_t*
cint_auto_atomic(const char* type);

int
cint_array_combine_dimensions(cint_parameter_desc_t * type,
                              const cint_parameter_desc_t * parameter) {
    int dim_index;
    int t_pcount = type->pcount;
    int p_pcount = parameter->pcount;
    int t_array_count = type->num_dimensions;
    int p_array_count = parameter->num_dimensions;

    /*
     * The ordering of dimensions should be
     * parameter array dimensions
     * parameter pointer dimensions
     * base type pointer dimensions
     * base type array dimensions
     */

    if((t_pcount + p_pcount + t_array_count + p_array_count) >
        CINT_CONFIG_ARRAY_DIMENSION_LIMIT) {
        cint_internal_error(__FILE__, __LINE__, "dimensions exceed limit");
        return CINT_E_INTERNAL;
    }

    /*
     * run backwards so that we do not overwrite the data we need to copy
     * forwards
     */
    for(dim_index = t_array_count - 1;
        dim_index >= 0;
        dim_index--) {
        int index1 =
            dim_index + p_array_count + p_pcount + t_pcount;
        type->dimensions[index1] = type->dimensions[dim_index];
    }

    for(dim_index = 0;
        dim_index < t_pcount + p_pcount; dim_index++) {
        type->dimensions[dim_index + p_array_count] =
            CINT_CONFIG_POINTER_INFINITE_DIMENSION;
    }

    for(dim_index = 0; dim_index < p_array_count; dim_index++) {
        type->dimensions[dim_index] =
            parameter->dimensions[dim_index];
    }

    type->num_dimensions =
        t_pcount + p_pcount + t_array_count + p_array_count;
    return CINT_E_NONE;
}


cint_error_t
cint_variable_create(cint_variable_t** rvar,
                      const char* vname,
                      const cint_parameter_desc_t* desc,
                      unsigned vflags,
                      void* addr)
{
    int rc;
    cint_variable_t* v;
    int size;

    char name[CINT_CONFIG_MAX_VARIABLE_NAME];

    if(vname == NULL) {
        /* Create an autovar */
        CINT_SPRINTF(name, "__auto$%d", _auto_counter);
        _auto_counter++;
        vflags |= CINT_VARIABLE_F_AUTO;
    }
    else {
        cint_strlcpy(name, vname, sizeof(name));
    }

    v = cint_variable_alloc();

    if(v == NULL) {
        return CINT_E_MEMORY;
    }

    if((rc = cint_datatype_find(desc->basetype, &v->dt)) != CINT_E_NONE) {
        /* Datatype not found */
        return rc;
    }

    if( (v->dt.flags & CINT_DATATYPE_F_ATOMIC) &&
        (v->dt.basetype.ap->flags & CINT_ATOMIC_TYPE_F_ABSTRACT) &&
        (v->dt.desc.pcount == 0) &&
        (desc->pcount == 0) ) {

        /* Cannot declare abstract variables */
        cint_variable_free(v);
        return CINT_E_BAD_TYPE;
    }

    v->name = CINT_STRDUP(name);

    if(v->name == NULL) {
        cint_variable_free(v);
        return CINT_E_MEMORY;
    }

    v->flags = vflags;

    if((rc = cint_array_combine_dimensions(&(v->dt.desc), desc)) !=
        CINT_E_NONE) {
        return rc;
    }

    v->dt.desc.pcount += desc->pcount;

    if(v->dt.desc.num_dimensions > 0 &&
        v->dt.desc.dimensions[0] == CINT_CONFIG_POINTER_INFINITE_DIMENSION) {
        v->cached_num_dimensions = v->dt.desc.num_dimensions;
        v->dt.desc.num_dimensions = 0;
    }

    if((size = cint_datatype_size(&v->dt)) <= 0) {
        return CINT_E_INTERNAL;
    }

    if (addr || (vflags & CINT_VARIABLE_F_SDATA)) {
        v->data = addr;
    }
    else {
        v->data = CINT_MALLOC(size);
        if(v->data == NULL) {
            cint_variable_free(v);
            return CINT_E_MEMORY;
        }
        CINT_MEMSET(v->data, 0, size);

        /* Special value assignments */
        if(v->dt.flags & CINT_DATATYPE_F_FUNC) {
            *(cint_fpointer_t*)v->data = v->dt.basetype.fp->addr;
        }
    }

    if(v->data == NULL) {
        cint_variable_free(v);
        return CINT_E_MEMORY;
    }

    v->size = size;

    if(rvar) {
        *rvar = v;
        cint_variable_add(v);
    }
    else {
        cint_variable_free(v);
    }

    return CINT_E_NONE;
}


cint_variable_t*
cint_auto_atomic(const char* type)
{
    cint_error_t rc;
    cint_parameter_desc_t dt;
    cint_variable_t* rv = NULL;

    dt.basetype = type;
    dt.pcount = 0;
    dt.num_dimensions = 0;

    rc = cint_variable_create(&rv,
                              NULL,
                              &dt,
                              CINT_VARIABLE_F_AUTO | CINT_VARIABLE_F_CONST,
                              NULL);
    return (rc == CINT_E_NONE) ? rv : NULL;
}

cint_variable_t*
cint_auto_string(const char* str)
{
    cint_error_t rc;
    cint_variable_t* rv = NULL;

    cint_parameter_desc_t dt;
    dt.basetype = "char";
    dt.pcount = 1;
    dt.num_dimensions = 0;

    rc = cint_variable_create(&rv,
                              NULL,
                              &dt,
                              CINT_VARIABLE_F_AUTO |
                              CINT_VARIABLE_F_CONST |
                              CINT_VARIABLE_F_CSTRING,
                              NULL);

    if(rv) {
        *((char**)rv->data) = cint_cstring_value(str);
    }

    return (rc == CINT_E_NONE) ? rv : NULL;
}

int
cint_is_integer(cint_variable_t* v)
{
    return v && (cint_atomic_flags(v) & CINT_ATOMIC_TYPE_FLAGS_INTEGRAL);
}

int
cint_is_enum(cint_variable_t* v)
{
    return (v &&
            (v->dt.flags & CINT_DATATYPE_F_ENUM) &&
            v->dt.desc.pcount == 0 &&
            v->dt.desc.num_dimensions == 0);
}


cint_variable_t*
cint_auto_pointer(const char* type, void* value)
{
    cint_datatype_t dt;
    cint_variable_t* v = NULL;
    cint_error_t rc;

    CINT_MEMSET(&dt, 0, sizeof(dt));
    rc = cint_datatype_find((type) ? type : "void", &dt);
    dt.desc.pcount=1;
    if((rc = cint_variable_create(&v, NULL, &dt.desc, CINT_VARIABLE_F_CONST, NULL)) == CINT_E_NONE) {
        *(void**)v->data = value;
    }

    return v;
}


int
cint_is_pointer(cint_variable_t* v)
{
    return v && (v->dt.desc.pcount > 0 && v->dt.desc.num_dimensions == 0);
}

int
cint_is_vpointer(cint_variable_t* v)
{
    return cint_is_pointer(v) && !CINT_STRCMP(v->dt.desc.basetype, "void");
}

unsigned
cint_atomic_flags(cint_variable_t* v)
{
    if(v == NULL) {
        return 0;
    }

    if(v->dt.desc.pcount || v->dt.desc.num_dimensions) {
        return 0;
    }

    if((v->dt.flags & CINT_DATATYPE_F_ATOMIC) == 0) {
        return 0;
    }

    return v->dt.basetype.ap->flags;
}


/*
 * 'long' variable operations
 */
long
cint_long_value(cint_variable_t* v)
{
    long rc = 0;

    switch((cint_atomic_flags(v) & CINT_ATOMIC_TYPE_FLAGS_MASK))
        {
        case CINT_ATOMIC_TYPE_F_CHAR:
            if ((cint_atomic_flags(v) & CINT_ATOMIC_TYPE_F_UNSIGNED) != 0)
            {
                rc = *(unsigned char*)v->data;
            } else {
                rc = *(signed char*)v->data;
            }
            break;
        case CINT_ATOMIC_TYPE_F_SHORT:
            if ((cint_atomic_flags(v) & CINT_ATOMIC_TYPE_F_UNSIGNED) != 0)
            {
                rc = *(unsigned short*)v->data;
            } else {
                rc = *(short*)v->data;
            }
            break;

        case CINT_ATOMIC_TYPE_F_INT:    rc = *(int*)v->data; break;
        case CINT_ATOMIC_TYPE_F_LONG:   rc = *(long*)v->data; break;
        case CINT_ATOMIC_TYPE_F_LONGLONG: rc = *(CINT_LONGLONG*)v->data; break;
        case CINT_ATOMIC_TYPE_F_DOUBLE:   rc = *(CINT_DOUBLE*)v->data; break;
        case CINT_ATOMIC_TYPE_F_BOOL:   rc = *(_Bool*)v->data; break;
        default:
            {
                if(v->dt.flags & CINT_DATATYPE_F_ENUM) {
                    rc = *(int*)v->data;
                }
                break; /* Should never get here */
            }
        }

    return rc;
}

cint_variable_t*
cint_long_set(cint_variable_t* v, long i)
{
    switch((cint_atomic_flags(v) & CINT_ATOMIC_TYPE_FLAGS_MASK))
        {
        case CINT_ATOMIC_TYPE_F_CHAR:   *(signed char*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_SHORT:  *(short*)v->data = i;  break;
        case CINT_ATOMIC_TYPE_F_INT:    *(int*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_LONG:   *(long*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_LONGLONG: *(CINT_LONGLONG*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_DOUBLE:   *(CINT_DOUBLE*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_BOOL:   *(_Bool*)v->data = i; break;
        default:
            {
                if(v->dt.flags & CINT_DATATYPE_F_ENUM) {
                    *(int*)v->data = i;
                }
                break;
            }
        }
    return v;
}

cint_variable_t*
cint_auto_long(long i)
{
    cint_variable_t* rv = NULL;

    rv = cint_auto_atomic("long");

    if(rv) {
        *((long*)rv->data) = i;
    }

    return rv;
}


cint_variable_t*
cint_auto_ulong(unsigned long i)
{
    cint_variable_t* rv = NULL;

    rv = cint_auto_atomic("unsigned long");

    if(rv) {
        *((unsigned long*)rv->data) = i;
    }

    return rv;
}

/*
 * 'int' variable operations
 */
int
cint_integer_value(cint_variable_t* v)
{
    return cint_long_value(v);
}

cint_variable_t*
cint_integer_set(cint_variable_t* v, int i)
{
    return cint_long_set(v, (long)i);
}

cint_variable_t*
cint_auto_integer(int i)
{
    cint_variable_t* rv = NULL;

    rv = cint_auto_atomic("int");

    if(rv) {
        *((int*)rv->data) = i;
    }

    return rv;
}

cint_variable_t*
cint_auto_uinteger(unsigned int i)
{
    cint_variable_t* rv = NULL;

    rv = cint_auto_atomic("unsigned int");

    if(rv) {
        *((unsigned int*)rv->data) = i;
    }

    return rv;
}

/*
 * 'bool' variable operations
 */
_Bool
cint_bool_value(cint_variable_t* v)
{
    return cint_long_value(v);
}

cint_variable_t*
cint_bool_set(cint_variable_t* v, _Bool i)
{
    return cint_long_set(v, (long)i);
}

cint_variable_t*
cint_auto_bool(_Bool i)
{
    cint_variable_t* rv = NULL;

    rv = cint_auto_atomic("_Bool");

    if(rv) {
        *((_Bool*)rv->data) = i;
    }

    return rv;
}

/*
 * 'char' variable operations
 */
signed char
cint_char_value(cint_variable_t* v)
{
    return cint_long_value(v);
}

cint_variable_t*
cint_char_set(cint_variable_t* v, char c)
{
    return cint_long_set(v, (long)c);
}

cint_variable_t*
cint_auto_char(signed char c)
{
    cint_variable_t* rv = NULL;
    rv = cint_auto_atomic("char");
    if(rv) {
        *((signed char*)rv->data) = c;
    }
    return rv;
}

/*
 * 'short' variable operations
 */
short
cint_short_value(cint_variable_t* v)
{
    return cint_long_value(v);
}

cint_variable_t*
cint_short_set(cint_variable_t* v, short s)
{
    return cint_long_set(v, (long)s);
}

cint_variable_t*
cint_auto_short(short c)
{
    cint_variable_t* rv = NULL;
    rv = cint_auto_atomic("short");
    if(rv) {
        *((short*)rv->data) = c;
    }
    return rv;
}

/*
 * 'long long' variable operations
 */
CINT_LONGLONG
cint_longlong_value(cint_variable_t* v)
{
#if CINT_CONFIG_INCLUDE_LONGLONGS == 0

    /* Should never even get here */
    cint_internal_error(__FILE__, __LINE__, "in 'longlong' function but long longs not supported");
    return 0;

#else

    long long rc = 0;

    switch((cint_atomic_flags(v) & CINT_ATOMIC_TYPE_FLAGS_MASK))
        {
        case CINT_ATOMIC_TYPE_F_CHAR:
            if(cint_atomic_flags(v) & CINT_ATOMIC_TYPE_F_UNSIGNED)
            {
                rc = *(unsigned char*)v->data;
            }
            else
            {
                rc = *(signed char*)v->data;
            }
            break;
        case CINT_ATOMIC_TYPE_F_SHORT:
            if(cint_atomic_flags(v) & CINT_ATOMIC_TYPE_F_UNSIGNED)
            {
                rc = *(unsigned short*)v->data;
            }
            else
            {
                rc = *(short*)v->data;
            }
            break;
        case CINT_ATOMIC_TYPE_F_INT:
            if(cint_atomic_flags(v) & CINT_ATOMIC_TYPE_F_UNSIGNED)
            {
                rc = *(unsigned int*)v->data;
            }
            else
            {
                rc = *(int*)v->data;
            }
            break;
        case CINT_ATOMIC_TYPE_F_LONG:
            if(cint_atomic_flags(v) & CINT_ATOMIC_TYPE_F_UNSIGNED)
            {
                rc = *(unsigned long*)v->data;
            }
            else
            {
                rc = *(long*)v->data;
            }
            break;
        case CINT_ATOMIC_TYPE_F_LONGLONG:
            if(cint_atomic_flags(v) & CINT_ATOMIC_TYPE_F_UNSIGNED)
            {
                rc = *(unsigned CINT_LONGLONG*)v->data;
            }
            else
            {
                rc = *(CINT_LONGLONG*)v->data;
            }
            break;
        case CINT_ATOMIC_TYPE_F_DOUBLE:   rc = *(CINT_DOUBLE*)v->data; break;
        default:
            {
                if(v->dt.flags & CINT_DATATYPE_F_ENUM) {
                    rc = *(int*)v->data;
                }
                break;
            }
        }

    return rc;

#endif
}

cint_variable_t*
cint_longlong_set(cint_variable_t* v, CINT_LONGLONG i)
{
#if CINT_CONFIG_INCLUDE_LONGLONGS == 0

    /* Should never even get here */
    cint_internal_error(__FILE__, __LINE__, "in 'longlong' function but long longs not supported");
    return NULL;

#else

    switch((cint_atomic_flags(v) & CINT_ATOMIC_TYPE_FLAGS_MASK))
        {
        case CINT_ATOMIC_TYPE_F_CHAR:   *(signed char*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_SHORT:  *(short*)v->data = i;  break;
        case CINT_ATOMIC_TYPE_F_INT:    *(int*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_LONG:   *(long*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_LONGLONG: *(CINT_LONGLONG*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_DOUBLE:   *(CINT_DOUBLE*)v->data = i; break;
        default:
            {
                if(v->dt.flags & CINT_DATATYPE_F_ENUM) {
                    *(int*)v->data = i;
                }
                break;
            }
        }
    return v;

#endif
}

cint_variable_t*
cint_auto_longlong(CINT_LONGLONG i)
{

#if CINT_CONFIG_INCLUDE_LONGLONGS == 0

    /* Should never even get here */
    cint_internal_error(__FILE__, __LINE__, "in 'longlong' function but long longs not supported");
    return NULL;

#else

    cint_variable_t* rv = NULL;

    rv = cint_auto_atomic("long long");

    if(rv) {
        *((long long*)rv->data) = i;
    }

    return rv;
#endif
}

cint_variable_t*
cint_auto_ulonglong(unsigned CINT_LONGLONG i)
{

#if CINT_CONFIG_INCLUDE_LONGLONGS == 0

    /* Should never even get here */
    cint_internal_error(__FILE__, __LINE__, "in 'longlong' function but long longs not supported");
    return NULL;

#else

    cint_variable_t* rv = NULL;

    rv = cint_auto_atomic("unsigned long long");

    if(rv) {
        *((unsigned long long*)rv->data) = i;
    }

    return rv;
#endif
}



/*
 * 'double' variable operations
 */
CINT_DOUBLE
cint_double_value(cint_variable_t* v)
{
#if CINT_CONFIG_INCLUDE_DOUBLES == 0

    /* Should never even get here */
    cint_internal_error(__FILE__, __LINE__, "in 'double' function but doubles not supported");
    return 0;

#else

    double rc = 0;

    switch((cint_atomic_flags(v) & CINT_ATOMIC_TYPE_FLAGS_MASK))
        {
        case CINT_ATOMIC_TYPE_F_CHAR:   rc = *(signed char*)v->data; break;
        case CINT_ATOMIC_TYPE_F_SHORT:  rc = *(short*)v->data; break;
        case CINT_ATOMIC_TYPE_F_INT:    rc = *(int*)v->data; break;
        case CINT_ATOMIC_TYPE_F_LONG:   rc = *(long*)v->data; break;
        case CINT_ATOMIC_TYPE_F_LONGLONG: rc = *(CINT_LONGLONG*)v->data; break;
        case CINT_ATOMIC_TYPE_F_DOUBLE:   rc = *(CINT_DOUBLE*)v->data; break;
        default:
            {
                if(v->dt.flags & CINT_DATATYPE_F_ENUM) {
                    rc = *(int*)v->data;
                }
                break;
            }
        }

    return rc;

#endif
}

cint_variable_t*
cint_double_set(cint_variable_t* v, CINT_DOUBLE i)
{
#if CINT_CONFIG_INCLUDE_DOUBLES == 0

    /* Should never even get here */
    cint_internal_error(__FILE__, __LINE__, "in 'double' function but doubles not supported");
    return NULL;

#else

    switch((cint_atomic_flags(v) & CINT_ATOMIC_TYPE_FLAGS_MASK))
        {
        case CINT_ATOMIC_TYPE_F_CHAR:   *(signed char*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_SHORT:  *(short*)v->data = i;  break;
        case CINT_ATOMIC_TYPE_F_INT:    *(int*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_LONG:   *(long*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_LONGLONG: *(CINT_LONGLONG*)v->data = i; break;
        case CINT_ATOMIC_TYPE_F_DOUBLE:   *(CINT_DOUBLE*)v->data = i; break;
        default:
            {
                if(v->dt.flags & CINT_DATATYPE_F_ENUM) {
                    *(int*)v->data = i;
                }
                break;
            }
        }
    return v;

#endif
}

cint_variable_t*
cint_auto_double(CINT_DOUBLE i)
{

#if CINT_CONFIG_INCLUDE_DOUBLES == 0

    /* Should never even get here */
    cint_internal_error(__FILE__, __LINE__, "in 'double' function but doubles not supported");
    return NULL;

#else

    cint_variable_t* rv = NULL;

    rv = cint_auto_atomic("double");

    if(rv) {
        *((double*)rv->data) = i;
    }

    return rv;
#endif
}

/* copy cstring data from src to dst */
static void
cint_variable_cstring_copy(cint_variable_t* dst, cint_variable_t* src)
{
    char **from = (char **)src->data;
    char **to   = (char **)dst->data;

    /* For C string data, allocate a new string buffer so this copied
       variable data will remain valid after the source variable is
       deallocated. If the destination pointer is not NULL, free it. */
    if (*to) {
        CINT_FREE(*to);
    }
    *to = CINT_STRDUP(*from);
    dst->flags |= CINT_VARIABLE_F_CSTRING;
}

/* copy data from src to dst */
void
cint_variable_data_copy(cint_variable_t* dst, cint_variable_t* src)
{
    if (src->flags & CINT_VARIABLE_F_CSTRING) {
        cint_variable_cstring_copy(dst, src);
    } else {
        CINT_MEMCPY(dst->data, src->data, dst->size);
    }
}

cint_variable_t*
cint_variable_clone(cint_variable_t* v)
{
    cint_error_t rc;
    cint_variable_t* rv;

    rc = cint_variable_create(&rv,
                              NULL,
                              &v->dt.desc,
                              CINT_VARIABLE_F_AUTO,
                              NULL);

    if(rv) {
        cint_variable_data_copy(rv, v);
    }
    return (rc == CINT_E_NONE) ? rv : NULL;
}

cint_variable_t*
cint_variable_address(cint_variable_t* v)
{
    cint_error_t rc;
    cint_variable_t* rv = NULL;
    cint_parameter_desc_t desc;
    int dim_index;

    desc = v->dt.desc;
    desc.pcount++;

    if(((v->cached_num_dimensions + 1) > CINT_CONFIG_ARRAY_DIMENSION_LIMIT) ||
        ((desc.num_dimensions + 1) > CINT_CONFIG_ARRAY_DIMENSION_LIMIT)) {
        return NULL;
    }

    for(dim_index = CINT_CONFIG_ARRAY_DIMENSION_LIMIT - 1;
        dim_index > 0;
        dim_index--) {
        desc.dimensions[dim_index] =
            desc.dimensions[dim_index - 1];
    }
    desc.dimensions[0] = CINT_CONFIG_POINTER_INFINITE_DIMENSION;

    if(v->cached_num_dimensions != 0) {
        desc.num_dimensions = v->cached_num_dimensions;
    }
    desc.num_dimensions++;

    rc = cint_variable_create(&rv,
                              NULL,
                              &desc,
                              CINT_VARIABLE_F_AUTO | CINT_VARIABLE_F_CONST,
                              NULL);

    if(rv) {
        rv->dt.type_num_dimensions = v->dt.type_num_dimensions;
        rv->dt.cap = v->dt.cap;
        * ((void**) rv->data) = v->data;
    }
    return (rc == CINT_E_NONE) ? rv : NULL;
}


int
cint_ctoi(const char *s, long* dst)
{
    unsigned long	n;
    unsigned int	neg, base = 10;

    s += (neg = (*s == '-'));

    if (*s == '0') {
    s++;

    if (*s == 'x' || *s == 'X') {
        base = 16;
        s++;
    } else if (*s == 'b' || *s == 'B') {
        base = 2;
        s++;
    } else {
        base = 8;
    }
    }

    for (n = 0; ((*s >= 'a' && *s <= 'f' && base > 10) ||
         (*s >= 'A' && *s <= 'F' && base > 10) ||
         (*s >= '0' && *s <= '9')); s++) {
    n = n * base +
        (*s >= 'a' ? *s - 'a' + 10 :
         *s >= 'A' ? *s - 'A' + 10 :
         *s - '0');
    }

    *dst = (neg ? -n : n);


    if(*s && !(*s == 'l' || *s == 'L' || *s == 'u' || *s == 'U')) {
        /* Parse error */
        return -1;
    }
    else {
        /* Success */
        return 0;
    }
}

#if CINT_CONFIG_INCLUDE_LONGLONGS == 1

/*
  Not all systems that support 'long long' properly support the '%ll'
  print format specification, so CINT does not use '%ll' directly, but
  uses this instead.
 */

char *
cint_lltoa(char *buf,           /* Result buffer        */
           int len,             /* length of buffer     */
           unsigned long long num,       /* Number to convert    */
           int sign,            /* True if signed conversion */
           int base,            /* Conversion base      */
           int prec)            /* Minimum digits       */
{
    char *s;
    int digit, c;

    if (sign) {
        if ((signed long long)num < 0) {
            num = -num;
        } else {
            sign = 0;
        }
    }

    /* start at end of buffer */
    s = buf+len;
    if (s > buf) {
        *--s = 0;
    }

    /* convert digits */
    do {
        if (s <= buf) {
            break;
        }
        digit = num % base;
        c = (digit < 10 ? '0' : '7') + digit;
        *--s = c;
        num /= base;
        prec--;
    } while (num);

    /* zero fill if needed */
    while (prec-- > 0 && (s > buf)) {
        *--s = '0';
    }

    /* add sign if requested */
    if (sign && (s > buf)) {
        *--s = '-';
    }

    return s;
}

int
cint_ctolli(const char *s, long long* dst)
{
    unsigned long long	n;
    unsigned int	neg, base = 10;

    s += (neg = (*s == '-'));

    if (*s == '0') {
    s++;

    if (*s == 'x' || *s == 'X') {
        base = 16;
        s++;
    } else if (*s == 'b' || *s == 'B') {
        base = 2;
        s++;
    } else {
        base = 8;
    }
    }

    for (n = 0; ((*s >= 'a' && *s <= 'f' && base > 10) ||
         (*s >= 'A' && *s <= 'F' && base > 10) ||
         (*s >= '0' && *s <= '9')); s++) {
    n = n * base +
        (*s >= 'a' ? *s - 'a' + 10 :
         *s >= 'A' ? *s - 'A' + 10 :
         *s - '0');
    }

    *dst = (neg ? -n : n);


    if(*s && !(*s == 'l' || *s == 'L' || *s == 'u' || *s == 'U')) {
        /* Parse error */
        return -1;
    }
    else {
        /* Success */
        return 0;
    }
}
#endif

int
cint_chartoi(const char* str, int* dst)
{
    int rc;
    const char* s = str;

    if(s == NULL) {
        return -1;
    }

    if(*s++ != '\'') {
        /* Not a character constant */
        return -1;
    }

    /* First character after quote */
    switch(*s)
        {
        case 0:
            {
                /* incorrectly formed constant */
                return -1;
            }

        case '\\':
            {
                /* Escaped character */
                s++;
                switch(*s) /* First character after backslash */
                    {
                    case '0': rc = 0; break;
                    case '\\': rc = '\\'; break;
                    case 'a': rc = '\a'; break;
                    case 'b': rc = '\b'; break;
                    case 'f': rc = '\f'; break;
                    case 'n': rc = '\n'; break;
                    case 'r': rc = '\r'; break;
                    case 't': rc = '\t'; break;
                    case 'v': rc = '\v'; break;
                    case '\'': rc = '\''; break;
                    case '\"': rc = '\"'; break;
                    case '\?': rc = '\?'; break;
                    default:
                        {
                            /* Warn and assume character value */
                            cint_warn(NULL, 0, "invalid escape sequence (%s)", str);
                            rc = *s;
                            break;
                        }
                    }
                s++; /* Advance to next character */
                break;
            }
        default:
            {
                rc = *s++; /* Assign character value and advance to next character */
                break;
            }
        }


    /* Validate and warn on input */
    switch(s[0])
        {
        case '\'':
            {
                switch(s[1])
                    {
                    case 0: /* Good */ break;
                    default:
                        {
                            /* Bad */
                            cint_warn(NULL, 0, "invalid character sequence (%s)", str);
                            break;
                        }
                    }
                break;
            }
        default:
            {
                if(s[0] == 0) {
                    cint_warn(NULL, 0, "invalid character sequence (%s)", str);
                }
                else {
                    cint_warn(NULL, 0, "multi-byte characters not supported (%s)", str);
                }
                break;
            }
        }

    /* In all cases we return the first valid character value */
    *dst = rc;
    return 0;
}

#if CINT_CONFIG_INCLUDE_DOUBLES == 1

int
cint_ctod(const char *s, double* dst)
{
    double n = 0;
    double f = 0;
    int neg;

    s += (neg = (*s == '-'));

    /* Whole digits */
    for (n = 0; (*s >= '0' && *s <= '9'); s++) {
        n = n * 10 + (*s - '0');
    }

    /* Fractional digits */
    if(*s == '.') {
        double b = .1;
        s++;
        for(f = 0; (*s >= '0' && *s <= '9'); s++) {
            f += (*s - '0') * b;
            b *= .1;
        }
    }
    n+=f;

    /* Exponent */
    if(*s == 'E' || *s == 'e') {
        long exp;
        if(cint_ctoi(s+1, &exp) == 0) {
            if(exp >= 0) {
                while(exp--) {
                    n = n*10;
                }
            }
            else {
                exp = -exp;
                while(exp--) {
                    n = n / 10;
                }
            }
            /* String already validated by cint_ctoi() - skip to end */
            while(*s) s++;
        }
        else {
            /* Bad exponent */
            return -1;
        }
    }

    *dst = (neg ? -n : n);


    if(*s && !(*s == 'l' || *s == 'L' || *s == 'f' || *s == 'F')) {
        /* Parse error */
        return -1;
    }
    else {
        /* Success */
        return 0;
    }
}
#endif

void*
cint_pointer_value(cint_variable_t* v)
{
    unsigned flags;

    flags = cint_atomic_flags(v);

    if(v->dt.desc.pcount) {
        return *(void**)v->data;
    }
    else if(flags & CINT_ATOMIC_TYPE_FLAGS_CSIL) {
        return (void*) cint_long_value(v);
    }
    else if(flags & CINT_ATOMIC_TYPE_F_LONGLONG) {
        /* Should warn here */
        return (void*) cint_long_value(v);
    }
    return NULL;
}

int
cint_logical_value(cint_variable_t* v)
{
    unsigned flags;

    if(v == NULL) {
        return 0;
    }

    flags = cint_atomic_flags(v);

    if(v->dt.desc.pcount) {
        return (*(void**)v->data) != NULL;
    }
    else if(flags & CINT_ATOMIC_TYPE_FLAGS_CSIL) {
        return (cint_long_value(v) != 0);
    }
    else if(flags & CINT_ATOMIC_TYPE_F_LONGLONG) {
        return (cint_longlong_value(v) != 0);
    }
    else if(flags & CINT_ATOMIC_TYPE_F_DOUBLE) {
        return (cint_double_value(v) != 0);
    }
    else if(v->dt.flags & CINT_DATATYPE_F_ENUM) {
        return (cint_integer_value(v) != 0);
    }
    cint_internal_error(__FILE__, __LINE__, "unknown variable type in cint_logical_value()");
    return 0;
}



char*
cint_cstring_value(const char* s)
{
    char* rv;
    const char* src;
    char* dst;

    if(s == NULL) {
        return NULL;
    }

    rv = CINT_STRDUP(s);

    for(src = s, dst = rv; *src; src++, dst++) {

        if(src == s && *src == '"') {
            /* Skip leading quote */
            dst--;
            continue;
        }

        if(*src == '\\') {
            switch(*(src+1))
                {
                case '\\':
                    *dst = '\\'; break;
                case 'n':
                    *dst = '\n'; break;
                case 't':
                    *dst = '\t'; break;
                case 'r':
                    *dst  = '\r'; break;
                case '"':
                    *dst = '"'; break;
                }
            src++;
        }
        else {
            *dst = *src;
        }
    }

    /* Remove trailing quote */
    dst--;
    *dst = 0;
    return rv;
}

static int
cint_type_enum_int(cint_datatype_t* dte, cint_datatype_t* dti)
{
    return ((dte->flags & CINT_DATATYPE_F_ENUM) &&
            (dti->flags & CINT_DATATYPE_F_ATOMIC) &&
            (dti->basetype.ap->flags & CINT_ATOMIC_TYPE_F_INT));
}

int
cint_type_compatible(cint_datatype_t* dt1, cint_datatype_t* dt2)
{
    return cint_type_enum_int(dt1, dt2) || cint_type_enum_int(dt2, dt1);
}

int
cint_type_check(cint_datatype_t* dt1, cint_datatype_t* dt2)
{
    int dim_index;
    if(dt1->desc.pcount != dt2->desc.pcount) {
        /* Different pointer counts */
        return 0;
    }
    if(dt1->desc.num_dimensions != dt2->desc.num_dimensions) {
        /* Different array dimensions */
        return 0;
    }
    else {
        for(dim_index = 0;
            dim_index < dt1->desc.num_dimensions;
            dim_index++) {
            if(dt1->desc.dimensions[dim_index] !=
                dt2->desc.dimensions[dim_index]) {
                /* Different array sizes */
                return 0;
            }
        }
    }

    if(dt1->basetype.p != dt2->basetype.p) {
        return cint_type_compatible(dt1, dt2);
    }

    return 1;
}


int
cint_eval_type_list(cint_ast_t* ast, char* basetype_out, int len,
                    unsigned* flags,
                    int       is_variable_declaration)
{
    unsigned f = 0;
    cint_ast_t* p;
    int rc = CINT_E_NONE;
    const char* modifier = NULL;
    char type[64];
    const char *basetype = NULL;

    if (flags) {
        *flags = 0;
    }

    for(p = ast; p && rc == CINT_E_NONE; p = p->next) {

        if(p->ntype == cintAstInteger) {
            switch (p->utype.integer.i)
                {
                case CINT_AST_TYPE_F_SIGNED:
                    {
                        if(f & CINT_AST_TYPE_F_SIGNED) {
                            rc = cint_ast_error(ast, CINT_E_BAD_TYPE, "'signed' specified multiple times");
                        }
                        else if(f & CINT_AST_TYPE_F_UNSIGNED) {
                            rc = cint_ast_error(ast, CINT_E_BAD_TYPE, "both 'signed' and 'unsigned' specified");
                        }
                        else {
                            modifier = "signed";
                        }
                        break;
                    }
                case CINT_AST_TYPE_F_UNSIGNED:
                    {
                        if(f & CINT_AST_TYPE_F_UNSIGNED) {
                            rc = cint_ast_error(ast, CINT_E_BAD_TYPE, "'unsigned' specified multiple times");
                        }
                        else if(f & CINT_AST_TYPE_F_SIGNED) {
                            rc = cint_ast_error(ast, CINT_E_BAD_TYPE, "both 'signed' and 'unsigned' specified");
                        }
                        else {
                            modifier = "unsigned";
                        }
                        break;
                    }
                case CINT_AST_TYPE_F_EXTERN:
                    {
                        if(is_variable_declaration){
                            rc = cint_ast_error(ast, CINT_E_BAD_TYPE, "'extern' variables are not supported");
                        }
                        break;
                    }
                case CINT_AST_TYPE_F_CONST:
                    {
                        if(f & CINT_AST_TYPE_F_CONST) {
                            rc = cint_ast_error(ast, CINT_E_BAD_TYPE, "'const' specified multiple times");
                        }
                        break;
                    }
                case CINT_AST_TYPE_F_TYPEDEF:
                    {
                        if(f & CINT_AST_TYPE_F_TYPEDEF) {
                            rc = cint_ast_error(ast, CINT_E_BAD_TYPE, "'typedef' specified multiple times");
                        }
                        break;
                    }
                case CINT_AST_TYPE_F_VOLATILE:
                    {
                        if(f & CINT_AST_TYPE_F_VOLATILE) {
                            rc = cint_ast_error(ast, CINT_E_BAD_TYPE, "'volatile' specified multiple times");
                        }
                        break;
                    }
                case CINT_AST_TYPE_F_STATIC:
                    {
                        if(f & CINT_AST_TYPE_F_STATIC) {
                            rc = cint_ast_error(ast, CINT_E_BAD_TYPE, "'static' specified multiple times");
                        }
                        break;
                    }
                default:
                    {
                        rc = cint_ast_error(ast, CINT_E_BAD_AST, "unknown ast type flag 0x%lx", p->utype.integer.i);
                        break;
                    }
                }

            if(rc == CINT_E_NONE) {
                f |= p->utype.integer.i;
            }
        }
        else if(p->ntype == cintAstType) {
            if(basetype) {

                /*
                 * Already a a type defined. Check for optional 'int' type
                 */
                if(CINT_STRCMP(basetype, "int") && !CINT_STRCMP(p->utype.type.s, "int")) {
                    /*
                     * This is an optional int after a sized type -- like 'short int x';
                     * Just ignore it.
                     */
                }
                else if(!CINT_STRCMP(basetype, "long") && !CINT_STRCMP(p->utype.type.s, "long")) {
                    /* This is a 'long long' type */
                    basetype = "long long";
                }
                else {
                    rc = cint_ast_error(ast, CINT_E_BAD_AST, "two or more data types in declaration");
                }
            }
            else {
                basetype = p->utype.type.s;
            }
        }
        else {
            rc = cint_ast_error(ast, CINT_E_BAD_AST, "Bad ast node in typelist (%d)", p->ntype);
        }
    }

    /* Special cases and modifiers */
    if((basetype == NULL) && (f & (CINT_AST_TYPE_F_SIGNED|CINT_AST_TYPE_F_UNSIGNED))) {
        /* Type was specified as just "signed" or "unsigned" */
        basetype = "int";
    }

    if(modifier) {
        CINT_SPRINTF(type, "%s %s", modifier, basetype);
        basetype = type;
    }

    if (flags) {
        *flags = f;
    }

    if (rc == CINT_E_NONE) {
        if (basetype) {
            cint_strlcpy(basetype_out, basetype, len);
        } else {
            *basetype_out = 0;
        }
    }

    return rc;
}


extern char*
cint_strlcpy(char* dst, const char* src, int size)
{
    CINT_STRNCPY(dst, src, size-1);
    dst[size-1] = 0;
    return dst;
}

void*
cint_maddr_dynamic_struct_t(void* p, int mnum, cint_struct_type_t* parent)
{
    int dim_index;
    int i;
    int offset = 0;
    const cint_parameter_desc_t* pd;
    cint_datatype_t dt;
    CINT_MEMSET(&dt, 0, sizeof(dt));

    for(i = 0, pd = parent->struct_members;
        i < mnum;
        i++, pd++) {


        if(cint_datatype_find(pd->basetype, &dt) != CINT_E_NONE) {
            cint_internal_error(__FILE__, __LINE__, "structure type unknown");
            return p;
        }

        for(dim_index = 0;
            dim_index < pd->num_dimensions;
            dim_index++) {
            dt.desc.dimensions[dim_index + dt.desc.num_dimensions] =
                pd->dimensions[dim_index];
        }
        dt.desc.num_dimensions += pd->num_dimensions;
        dt.desc.pcount += pd->pcount;
        offset += cint_datatype_size(&dt);

        while(offset % 4) offset++;
    }

    return ( ((char*)p) + offset );
}

int
cint_parameter_count(cint_parameter_desc_t* start)
{
    cint_parameter_desc_t* pd;
    for(pd = start; pd && pd->basetype; pd++);
    return pd - start;
}

int
cint_parameter_void(cint_parameter_desc_t* pd)
{
    if(pd && pd->basetype) {
        return !CINT_STRCMP(pd->basetype, "void") && pd->pcount == 0;
    }
    else {
        return 1;
    }
}

void
cint_fparams_print(cint_parameter_desc_t* pd)
{
    if(cint_parameter_void(pd)) {
        CINT_PRINTF("(void)");
    }
    else {
        CINT_PRINTF("(");
        while((pd+1)->basetype) {
            CINT_PRINTF("%s %s, ", cint_datatype_format_pd(pd, 0), pd->name);
            pd++;
        }
        CINT_PRINTF("%s %s)", cint_datatype_format_pd(pd, 0), pd->name);
    }
}

int
cint_snprintf_ex(char** dst, int* size, const char* fmt, ...)
{
    va_list args;
    int c = 0;

    va_start(args, fmt);

    /* Space left? */
    if(*size >= 0) {
        c = CINT_VSNPRINTF(*dst, *size, fmt, args);
        *dst += c;
        *size -= c;
    }

    va_end(args);

    return c;
}
