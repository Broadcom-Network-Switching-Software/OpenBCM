/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_builtins.c
 * Purpose:     CINT building function handlers
 */

/***********************************************************************
 *
 * Native C Atomic Datatypes and Builtin Interpreter Data Types
 */

#include "cint_config.h"
#include "cint_porting.h"
#include "cint_interpreter.h"
#include "cint_variables.h"
#include "cint_ast.h"
#include "cint_internal.h"
#include "cint_eval_asts.h"

static int 
__cint_format_signed_char(void* p, char* dst, int size, cint_atomic_format_t format)
{
    int i = *((signed char*)p); 

    switch(format) 
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%d (0x%.2x) '%c'", i, i&0xff, (i < 127 && i > 31) ? i : '.'); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%c", i); 
                break; 
            }
        }       
                
    return 0; 
}       

static int 
__cint_format_unsigned_char(void* p, char* dst, int size, cint_atomic_format_t format)
{
    unsigned int i = *((unsigned char*)p); 
    
    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%u (0x%.2x)", i, i); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "0x%.2x", i); 
                break; 
            }
        }

    return 0; 
}       

static int 
__cint_format_signed_short(void* p, char* dst, int size, cint_atomic_format_t format)
{
    signed short* u = (signed short*)p; 
    
    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%hd (0x%.4hx)", *u, *u); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%hd", *u); 
                break;
            }
        }

    return 0; 
}       

static int 
__cint_format_unsigned_short(void* p, char* dst, int size, cint_atomic_format_t format)
{
    unsigned short* u = (unsigned short*)p; 

    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {   
                cint_snprintf_ex(&dst, &size, "%hu (0x%.4hx)", *u, *u); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%hu", *u); 
                break; 
            }
        }
            
    return 0; 
}       

static int 
__cint_format_signed_int(void* p, char* dst, int size, cint_atomic_format_t format)
{
    signed int* i = (signed int*) p; 

    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%d (0x%x)", *i, *i); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%d", *i); 
                break; 
            }
        }       

    return 0; 
}       


static int 
__cint_format_unsigned_int(void* p, char* dst, int size, cint_atomic_format_t format)
{
    unsigned int* i = (unsigned int*) p; 

    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%u (0x%x)", *i, *i); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%u", *i); 
                break; 
            }
        }       

    return 0; 
}       

static int 
__cint_format_signed_long(void* p, char* dst, int size, cint_atomic_format_t format)
{
    signed long* u = (signed long*)p;     
    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%ld (0x%.8lx)", *u, *u); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%ld", *u); 
                break; 
            }
        }       

    return 0; 
}       


static int 
__cint_format_unsigned_long(void* p, char* dst, int size, cint_atomic_format_t format)
{
    unsigned long* u = (unsigned long*)p;     

    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%lu (0x%.8lx)", *u, *u);                 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%lu", *u); 
                break; 
            }
        }       

    return 0; 
}       

#if CINT_CONFIG_INCLUDE_DOUBLES == 1

static int
__cint_format_double(void* p, char* dst, int size, cint_atomic_format_t format)
{
    double* d = (double*)p; 

    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%g", *d); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%g", *d); 
                break; 
            }
        }       

    return 0; 
}

#endif /* CINT_CONFIG_INCLUDE_DOUBLES */

#if CINT_CONFIG_INCLUDE_LONGLONGS == 1

static int
__cint_format_signed_longlong(void* p, char* dst, int size, cint_atomic_format_t format)
{
    signed long long* u = (signed long long*)p;
    char buf1[50], *s1;
    char buf2[50], *s2;

    s1 = cint_lltoa(buf1, sizeof(buf1), *u, 0, 16, 16);
    s2 = cint_lltoa(buf2, sizeof(buf2), *u, 1, 10, 0);
    
    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%s (0x%s)", s2, s1); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%s", s2); 
                break; 
            }
        }       

    return 0; 
}

static int
__cint_format_unsigned_longlong(void* p, char* dst, int size, cint_atomic_format_t format)
{
    unsigned long long* u = (unsigned long long*)p; 
    char buf1[50], *s1;
    char buf2[50], *s2;

    s1 = cint_lltoa(buf1, sizeof(buf1), *u, 0, 16, 16);
    s2 = cint_lltoa(buf2, sizeof(buf2), *u, 0, 10, 0);
    
    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
            {
                cint_snprintf_ex(&dst, &size, "%s (0x%s)", s2, s1); 
                break; 
            }
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "%s", s2); 
                break; 
            }
        }      

    return 0; 
}


#endif /* CINT_CONFIG_INCLUDE_LONGLONGS */

static cint_atomic_type_t __cint_builtin_atomics[] = 
    {
        
        /* Character types */
        {
            "char", 
            sizeof(signed char), 
            CINT_ATOMIC_TYPE_F_CHAR | CINT_ATOMIC_TYPE_F_SIGNED, 
            __cint_format_signed_char,
            NULL
        }, 
        {
            "signed char", 
            sizeof(char), 
            CINT_ATOMIC_TYPE_F_CHAR | CINT_ATOMIC_TYPE_F_SIGNED,
            __cint_format_signed_char,
            NULL
        }, 
        {
            "unsigned char", 
            sizeof(char), 
            CINT_ATOMIC_TYPE_F_CHAR | CINT_ATOMIC_TYPE_F_UNSIGNED,
            __cint_format_unsigned_char,
            NULL, 
        }, 

        /* Short types */
        {
            "short", 
            sizeof(short), 
            CINT_ATOMIC_TYPE_F_SHORT | CINT_ATOMIC_TYPE_F_SIGNED, 
            __cint_format_signed_short,
            NULL
        }, 
        {
            "signed short", 
            sizeof(short), 
            CINT_ATOMIC_TYPE_F_SHORT | CINT_ATOMIC_TYPE_F_SIGNED,
            __cint_format_signed_short,
            NULL
        }, 

        {
            "unsigned short", 
            sizeof(short), 
            CINT_ATOMIC_TYPE_F_SHORT | CINT_ATOMIC_TYPE_F_UNSIGNED, 
            __cint_format_unsigned_short,
            NULL
        }, 


        /* Int types */
        {
            "int", 
            sizeof(int), 
            CINT_ATOMIC_TYPE_F_INT | CINT_ATOMIC_TYPE_F_SIGNED, 
            __cint_format_signed_int,
            NULL
        }, 
        {
            "signed int", 
            sizeof(int), 
            CINT_ATOMIC_TYPE_F_INT | CINT_ATOMIC_TYPE_F_SIGNED, 
            __cint_format_signed_int, 
            NULL
        }, 
        {
            "unsigned int", 
            sizeof(int), 
            CINT_ATOMIC_TYPE_F_INT | CINT_ATOMIC_TYPE_F_UNSIGNED,
            __cint_format_unsigned_int, 
            NULL,
        }, 

        /* Long types */
        {
            "long", 
            sizeof(long), 
            CINT_ATOMIC_TYPE_F_LONG | CINT_ATOMIC_TYPE_F_SIGNED,
            __cint_format_signed_long,
            NULL
        }, 
        {
            "signed long", 
            sizeof(long), 
            CINT_ATOMIC_TYPE_F_LONG | CINT_ATOMIC_TYPE_F_SIGNED, 
            __cint_format_signed_long, 
            NULL
        }, 
        {
            "unsigned long", 
            sizeof(long), 
            CINT_ATOMIC_TYPE_F_LONG | CINT_ATOMIC_TYPE_F_UNSIGNED, 
            __cint_format_unsigned_long, 
            NULL
        }, 

#if CINT_CONFIG_INCLUDE_LONGLONGS == 1
        {
            "long long", 
            sizeof(long long), 
            CINT_ATOMIC_TYPE_F_LONGLONG | CINT_ATOMIC_TYPE_F_SIGNED, 
            __cint_format_signed_longlong,
            NULL
        },
        {
            "signed long long", 
            sizeof(long long), 
            CINT_ATOMIC_TYPE_F_LONGLONG | CINT_ATOMIC_TYPE_F_SIGNED, 
            __cint_format_signed_longlong, 
            NULL
        },
        {
            "unsigned long long", 
            sizeof(long long), 
            CINT_ATOMIC_TYPE_F_LONGLONG | CINT_ATOMIC_TYPE_F_UNSIGNED, 
            __cint_format_unsigned_longlong,
            NULL
        },
#endif /* CINT_CONFIG_INCLUDE_LONGLONGS */


#if CINT_CONFIG_INCLUDE_DOUBLES == 1
        {
            "double", 
            sizeof(double), 
            CINT_ATOMIC_TYPE_F_DOUBLE,
            __cint_format_double, 
            NULL
        },
        {
            "float", 
            sizeof(double),
            CINT_ATOMIC_TYPE_F_DOUBLE, 
            __cint_format_double, 
            NULL
        },
#endif
        {
            "void", 
            sizeof(void*),
            CINT_ATOMIC_TYPE_F_ABSTRACT,
            NULL, 
            NULL, 
        },
            
        { NULL }, 
    }; 


/*
 * Global Interpreter State
 */
cint_interpreter_state_t interp; 


static cint_parameter_desc_t __cint_struct_members__cint_interpreter_debug_state_t[] = 
    {
        {
            "int", 
            "dtrace", 
            0,
            0
        },
        {
            "int", 
            "atrace", 
            0, 
            0,
        },
        {
            "int", 
            "ftrace", 
        },
        {       
            "int", 
            "aunique", 
        },
        { NULL }
    };

static void*
__cint_maddr__cint_interpreter_debug_state_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cint_interpreter_debug_state_t* s = (cint_interpreter_debug_state_t*) p;

    switch(mnum)
        {
        case 0: rv = &(s->dtrace); break;
        case 1: rv = &(s->atrace); break;
        case 2: rv = &(s->ftrace); break;
        case 3: rv = &(s->aunique); break;
        default: rv = NULL; break;
        }

    return rv;
}

static void*
__cint_maddr__cint_interpreter_state_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cint_interpreter_state_t* s = (cint_interpreter_state_t*) p;

    switch(mnum)
        {
        case 0: rv = &(s->debug); break;
        case 1: rv = &(s->parse_only); break; 
        case 2: rv = &(s->print_expr); break; 
        default: rv = NULL; break;
    }

    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cint_interpreter_state_t[] =     
    {   
        {
            "cint_interpreter_debug_state_t", 
            "debug", 
            0,0
        },
        {
            "int",
            "parse_only", 
            0, 0
        }, 
        {
            "int",
            "print_expr", 
            0, 0
        }, 
        { NULL }, 
    }; 

static cint_struct_type_t __cint_builtin_structures[] = 
    {
        CINT_STRUCT_TYPE_DEFINE(cint_interpreter_state_t), 
        CINT_STRUCT_TYPE_DEFINE(cint_interpreter_debug_state_t),
        { NULL }
    };
  
static cint_constants_t __cint_builtin_constants[] = 
    {
        /* Basic Constants */


        /*
         * Config option constants. 
         * Allows scripts to alter their behavior based on 
         * the config options for the interpreter
         */
#define __CONFIG_OPTION(_x) { #_x, _x }
#define CONFIG_OPTION(_x) __CONFIG_OPTION(CINT_CONFIG_##_x)

        CONFIG_OPTION(INCLUDE_DOUBLES), 
        CONFIG_OPTION(INCLUDE_LONGLONGS), 
        CONFIG_OPTION(INCLUDE_MAIN), 
        CONFIG_OPTION(MAX_VARIABLE_NAME),
        CONFIG_OPTION(INCLUDE_CINT_LOAD),
        CONFIG_OPTION(INCLUDE_CINT_SOURCE), 
        CONFIG_OPTION(INCLUDE_DTRACE), 
        CONFIG_OPTION(INCLUDE_DEBUG), 
        CONFIG_OPTION(INCLUDE_STDLIB), 
        CONFIG_OPTION(INCLUDE_SDK_SAL), 
        CONFIG_OPTION(INCLUDE_TEST_DATA),

        { NULL }
    }; 

CINT_FWRAPPER_CREATE2_VP0(cint_variable_dump); 



void
cint_reset(void)
{
    cint_interpreter_init(); 
    cint_interpreter_event(cintEventReset); 
}

int
cint_defined(const char* id)
{
    /* 
     * Returns 1 if the given name identifier exists. 
     * Exported to scripts to control their execution based
     * on identifier availability. 
     */
    int rv = 0; 

    if(id == NULL) {
        rv = 0; 
    }
    else if(cint_variable_find(id, 0)) {
        rv = 1; 
    }
    else if(cint_datatype_enum_find(id, NULL, NULL) == CINT_E_NONE) {
        rv = 1; 
    }   
    else if(cint_constant_find(id, NULL) == CINT_E_NONE) {
        rv = 1; 
    }
    else if(cint_datatype_find(id, NULL) == CINT_E_NONE) {
        rv = 1; 
    }
    else if(cint_interpreter_is_macro(id, NULL)) {
        rv = 1; 
    }
    else if(cint_interpreter_is_iterator(id, NULL)) {
        rv = 1; 
    }
    return rv; 
}
           

CINT_FWRAPPER_CREATE2_VP0(cint_reset); 
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0, 
                         cint_defined, 
                         char*, char, identifier, 1, 0); 


/*
 * Posix Timer routines. Mostly for profiling. 
 */

#if CINT_CONFIG_INCLUDE_POSIX_TIMER == 1

#include <time.h>
#if (!defined(CLOCK_REALTIME) && !defined(CINT_TIMER_GET)) || \
    defined(CINT_USE_GETTIMEOFDAY)
#include <sys/time.h>
#endif /* !CLOCK_REALTIME && !CINT_TIMER_GET */

unsigned long
cint_timer_get(void)
{
#if defined(CINT_TIMER_GET)
    return (unsigned long)CINT_TIMER_GET;
#elif defined(CLOCK_REALTIME) && !defined(CINT_USE_GETTIMEOFDAY)
    struct timespec    ltv;
    clock_gettime(CLOCK_REALTIME, &ltv);
    return (ltv.tv_sec * 1000000 + ltv.tv_nsec / 1000);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec  * 1000000 + tv.tv_usec);
#endif /* CLOCK_REALTIME */
}

long
cint_timer_start(long t0)
{
    return t0 - cint_timer_get(); 
}

long
cint_timer_stop(long t0)
{
    return t0 + cint_timer_get(); 
}
    
CINT_FWRAPPER_CREATE2_RP0(unsigned long, unsigned long, 0, 0, 
                         cint_timer_get); 

CINT_FWRAPPER_CREATE2_RP1(long, long, 0, 0, 
                         cint_timer_start,
                          long, long, t0, 0, 0, CINT_PARAM_IN); 

CINT_FWRAPPER_CREATE2_RP1(long, long, 0, 0, 
                         cint_timer_stop,
                         long, long, t0, 0, 0, CINT_PARAM_IN); 

#endif /* CINT_CONFIG_INCLUDE_POSIX_TIMER */

static cint_variable_t* cint_builtin_printf(cint_ast_t* ast); 
static cint_variable_t* cint_builtin_sprintf(cint_ast_t* ast); 
static cint_variable_t* cint_builtin_snprintf(cint_ast_t* ast); 

static cint_function_t __cint_builtin_functions[] = 
        {
                CINT_FWRAPPER_ENTRY(cint_variable_dump), 
#if CINT_CONFIG_INCLUDE_POSIX_TIMER == 1
                CINT_FWRAPPER_ENTRY(cint_timer_start), 
                CINT_FWRAPPER_ENTRY(cint_timer_stop), 
                CINT_FWRAPPER_ENTRY(cint_timer_get), 
#endif 
                CINT_FWRAPPER_ENTRY(cint_reset),
                CINT_FWRAPPER_VARARG_ENTRY("printf", cint_builtin_printf),
                CINT_FWRAPPER_VARARG_ENTRY("sprintf", cint_builtin_sprintf),
                CINT_FWRAPPER_VARARG_ENTRY("snprintf", cint_builtin_snprintf),
                CINT_FWRAPPER_ENTRY(cint_defined), 
                CINT_ENTRY_LAST
        }; 


cint_data_t cint_builtin_data = 
        {   
        __cint_builtin_atomics,         /* Atomics */
        __cint_builtin_functions,       /* Functions */
        __cint_builtin_structures,      /* Structures */
        NULL,                           /* Enums */
        NULL,                           /* Typedefs */
        __cint_builtin_constants,       /* Constants */
        NULL,                           /* Cb Functions */
    }; 

typedef enum {
    cvt_integer_v,
    cvt_long_v,
    cvt_longlong_v,
    cvt_double_v,
    cvt_ptr_string,
    cvt_string,
    cvt_ptr_void,
    cvt_fmt,
    cvt_char
} cint_print_driver_cvt_t;

typedef struct {
    char *stg;  /* staging buffer for intermediate prints */
    char *out;  /* output buffer for final print */
    int len;    /* length of both fmt and out buffer */
    int cur;    /* Current length of output buffer */
} cint_print_dest_t;

typedef int (*cint_print_driver_f)(cint_print_dest_t *dst,
                                    const char *fmt,
                                    const void *src,
                                    cint_print_driver_cvt_t cvt);

static int
cint_builtin_printf_driver(cint_print_dest_t *dst,
                           const char *fmt,
                           const void *src,
                           cint_print_driver_cvt_t cvt)
{
    int len = 0;
    switch (cvt) {
    case cvt_integer_v:
        len = CINT_PRINTF(fmt, cint_integer_value((cint_variable_t *)src)); 
        break;
    case cvt_long_v:
        len = CINT_PRINTF(fmt, cint_long_value((cint_variable_t *)src)); 
        break;
    case cvt_longlong_v:
        len = CINT_PRINTF(fmt, cint_longlong_value((cint_variable_t *)src)); 
        break;
    case cvt_double_v:
        len = CINT_PRINTF(fmt, cint_double_value((cint_variable_t *)src)); 
        break;
    case cvt_ptr_string:
        len = CINT_PRINTF(fmt, *(char**)src); 
        break;
    case cvt_string:
        len = CINT_PRINTF(fmt, (char*)src); 
        break;
    case cvt_ptr_void:
        len = CINT_PRINTF(fmt, *(void**)src); 
        break;
    case cvt_fmt:
        len = CINT_PRINTF(fmt, NULL); 
        break;
    case cvt_char:
        len = CINT_PRINTF(fmt, *(char *)src); 
        break;
    }
    return len;
}

static int
cint_builtin_sprintf_driver(cint_print_dest_t *dst,
                            const char *fmt,
                            const void *src,
                            cint_print_driver_cvt_t cvt)
{
    int n = 0;

    switch (cvt) {
    case cvt_integer_v:
        n = CINT_SNPRINTF(dst->stg, dst->len, fmt,
                          cint_integer_value((cint_variable_t *)src)); 
        break;
    case cvt_long_v:
        n = CINT_SNPRINTF(dst->stg, dst->len, fmt,
                          cint_long_value((cint_variable_t *)src)); 
        break;
    case cvt_longlong_v:
        n = CINT_SNPRINTF(dst->stg, dst->len, fmt,
                          cint_longlong_value((cint_variable_t *)src)); 
        break;
    case cvt_double_v:
        n = CINT_SNPRINTF(dst->stg, dst->len, fmt,
                          cint_double_value((cint_variable_t *)src)); 
        break;
    case cvt_ptr_string:
        n = CINT_SNPRINTF(dst->stg, dst->len, fmt, *(char**)src); 
        break;
    case cvt_string:
        n = CINT_SNPRINTF(dst->stg, dst->len, fmt, (char*)src); 
        break;
    case cvt_ptr_void:
        n = CINT_SNPRINTF(dst->stg, dst->len, fmt, *(void**)src); 
        break;
    case cvt_fmt:
        n = CINT_SNPRINTF(dst->stg, dst->len, fmt, NULL); 
        break;
    case cvt_char:
        n = CINT_SNPRINTF(dst->stg, dst->len, fmt, *(char *)src); 
        break;
    }

    if (n > 0) {
        /* overflow? */
        dst->cur += n;
        if (dst->cur <= dst->len) {
            /* good - cat staging to output. */
            CINT_STRNCAT(dst->out, dst->stg, dst->len);
        }
    }
    return n;
}

static cint_variable_t* 
cint_builtin_print_kernel(char *name,
                          cint_print_driver_f print,
                          cint_ast_t* ast,
                          cint_print_dest_t *dst,
                          cint_ast_t* args)
{
    char* pfmt; 
    const char* s; 
    char buffer[CINT_CONFIG_MAX_VARIABLE_NAME+1]; 
    char _rstr[256] = {0};
    cint_variable_t* ret = NULL;
    int len = 0;
    /*
     * Only constant strings are currently supported
     */
    if(args && args->ntype == cintAstString) {
        pfmt = cint_cstring_value(args->utype.string.s); 
    }
    else {
        cint_ast_error(ast, CINT_E_BAD_AST, "format argument to '%s' must be a string literal", name); 
        return NULL; 
    }   

    /* Foreach character in the format string */
    for(s = pfmt; s && *s;) {


        /* Embedded variable/perl style */
        
        if(*s == '$') {
            
            cint_variable_t* v; 

            s++; 
            if(*s == '{') {
                int i = 0; 
                s++; 
                while(*s && *s != '}') {
                    buffer[i++] = *s++; 
                    buffer[i] = 0; 
                }
                if(*s != '}') {
                    cint_ast_error(ast, CINT_E_BAD_AST, "bad variable specifier in format string"); 
                    CINT_FREE(pfmt); 
                    return NULL; 
                }
                s++; 
            }
            else {
                int i = 0; 
                /* non-identifer characters */
                while(*s && (*s != ' ' && *s != ':' && *s != '\n' && *s != '\t' && *s != '\r' && 
                             *s != '(' && *s != ')' && *s != '\'' && *s != '.')) {
                    buffer[i++] = *s++; 
                    buffer[i] = 0; 
                }                
            }
            v = cint_variable_find(buffer, 0);             
            if(v == NULL) {
                cint_ast_error(ast, CINT_E_BAD_VARIABLE, "variable '%s' is undefined\n", buffer); 
            }
            else {
                cint_variable_printf(v); 
            }   
        }       
        /* Printf-style */
        else if(*s == '%') {
            /* Start of format specifier */
            int i = 0;            
            int larg = 0; 
            int _break = 0; 
            cint_variable_t* v; 

            args = args->next;         
            v = cint_eval_ast(args); 

            if(v == NULL && s[1] != '%') {
                cint_ast_error(ast, CINT_E_BAD_AST, "error evaluating required argument to printf"); 
                CINT_FREE(pfmt); 
                return NULL; 
            }

            buffer[i++] = *s++; 
            buffer[i] = 0; 

            while(*s && !_break) {
                buffer[i++] = *s; 
                buffer[i] = 0; 

                switch(*s)
                    {
                    case 'l': larg++; break; 
                    case 'd': 
                    case 'i':
                    case 'x':
                    case 'X':
                    case 'u':
                    case 'c':
                    case 'o':
                        {
                            /* Expects an integer type argument */
                            unsigned flags = cint_atomic_flags(v); 
                            if(flags & (CINT_ATOMIC_TYPE_FLAGS_INTEGRAL)) {
                                    
                                if(larg == 0) {
                                    len += print(dst, buffer, v, cvt_integer_v); 
                                }
                                else if(larg == 1) {
                                    len += print(dst, buffer, v, cvt_long_v); 
                                }
                                else if(larg > 1){
                                    len += print(dst, buffer, v, cvt_longlong_v); 
                                }
                                _break = 1; 
                            }
                            else {
                                cint_ast_error(ast, CINT_E_BAD_TYPE, "printf expected integer type but received %s\n", 
                                               (v) ? cint_datatype_format(&v->dt, _rstr, 0) : "none"); 
                                CINT_FREE(pfmt); 
                                return NULL; 
                            }  
                            break; 
                        }
                    case 'f':
                    case 'F':
                    case 'g':
                    case 'G':
                    case 'e':
                    case 'E':
                        {
                            /* Expects a double type argument */
                            unsigned flags = cint_atomic_flags(v); 
                            if(flags & CINT_ATOMIC_TYPE_FLAGS_ARITH) {
                                len += print(dst, buffer, v, cvt_double_v); 
                                _break = 1; 
                            }
                            else {
                                cint_ast_error(ast, CINT_E_BAD_TYPE, "printf expected double type but received %s\n", 
                                               (v) ? cint_datatype_format(&v->dt, _rstr, 0) : "none"); 
                                CINT_FREE(pfmt); 
                                return NULL; 
                            }       
                            break;
                        }
                    case 's':
                        {
                            /* Expects a string or char* arguments */
                            if(v && 
                                !CINT_STRCMP(v->dt.desc.basetype, "char")) {
                                if(v->dt.desc.pcount == 1 &&
                                    v->dt.desc.num_dimensions == 0) {
                                    len += print(
                                        dst, 
                                        buffer,
                                        v->data,
                                        cvt_ptr_string); 
                                }       
                                else if(v->dt.desc.pcount == 0 && 
                                    v->dt.desc.num_dimensions != 0) {
                                    len += print(dst, buffer, v->data, cvt_string); 
                                }       
                                _break = 1; 
                                break; 
                            }
                            cint_ast_error(ast, CINT_E_BAD_TYPE, "printf expected string type but received %s\n", 
                                           (v) ? cint_datatype_format(&v->dt, _rstr, 0) : "none"); 
                            CINT_FREE(pfmt); 
                            return NULL; 
                        }
                    case 'p':
                        {
                            if(!v || v->dt.desc.pcount == 0) {
                                cint_ast_error(ast, CINT_E_BAD_TYPE, "printf expected pointer type but received %s\n", 
                                               (v) ? cint_datatype_format(&v->dt, _rstr, 0) : "none"); 
                                CINT_FREE(pfmt); 
                                return NULL; 
                            }   
                            else {
                                len += print(dst, buffer, v->data, cvt_ptr_void); 
                                _break = 1; 
                            }
                            break; 
                        }
                    case '%':
                        {
                            len += print(dst, buffer, NULL, cvt_fmt); 
                            _break = 1; 
                            break; 
                        }                        
                    }
                s++; 
            }
        }
        else {
            len += print(dst, "%c", s++, cvt_char); 
        }
    }
    
    CINT_FREE(pfmt); 
    ret = cint_auto_integer(len);
    return ret;
}


static cint_variable_t* 
cint_builtin_printf(cint_ast_t* ast)
{
    return cint_builtin_print_kernel("printf",
                                     cint_builtin_printf_driver,
                                     ast,
                                     NULL,
                                     ast->utype.function.parameters);
}

static cint_variable_t *
_cint_builtin_print_buffer(cint_ast_t* ast)
{
    cint_variable_t *str;

    /* Evaluate the first argument and expect a character buffer */
    str = cint_eval_ast(ast);
    if (str == NULL) {
        cint_ast_error(ast, CINT_E_BAD_VARIABLE,
                       "could not evaluate the first argument (string)");
        return NULL;
    }

    if (!((CINT_STRCMP(str->dt.desc.basetype, "char")==0) &&
          (((str->dt.desc.pcount == 1) && 
            (str->dt.desc.num_dimensions == 0)) ||
           ((str->dt.desc.pcount == 0) && 
            (str->dt.desc.num_dimensions > 0))))) {
        cint_ast_error(ast, CINT_E_BAD_VARIABLE, 
                       "the first argument (str) must be a char* or char[]");
        return NULL;
    }

    return str;
}

static cint_variable_t* 
cint_builtin_sprintf(cint_ast_t* ast)
{
    cint_variable_t *str;
    cint_variable_t* v;
    cint_ast_t* arg;
    cint_print_dest_t dst;

    arg = ast->utype.function.parameters;

    /* Evaluate the first argument and expect a character buffer */
    str = _cint_builtin_print_buffer(arg);
    if (str == NULL) {
        return str;
    }

    /* 
     * The function needs to deal with two ways a string can be
     * passed, that are quite different in CINT.
     *
     *  1) It can be a real char*. In that case, str->data points to
     *  the place, where the pointer is stored, so additional
     *  dereference is required. Also in that case the actual buffer
     *  length is unknown, so we simply allocate a big buffer for the
     *  staging area and pray that the result fits. If not, the
     *  program will crash, just like any other C program that uses
     *  sprintf() unsafely.
     *
     *  2) It can also be a char[]. In that case, str->data points to
     *  the first character in the array. Moreover, we have the
     *  information about the length of the array and we'll use it for
     *  the staging buffer allocation
 */
    if (str->dt.desc.pcount == 1) { 
        dst.out = *(char **)str->data;   /* char * was passed */
        dst.len = CINT_CONFIG_SPRINTF_BUFFER_LENGTH;
    } else {
        dst.out = (char *)str->data;     /* char[] was passed */
        dst.len = str->dt.desc.dimensions[0];
    }

    dst.stg = CINT_MALLOC(dst.len);
    if (!dst.stg) {
        cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure");
        return NULL;
    }

    dst.out[0] = 0; /* start output buffer with a zero length string */
    dst.cur = 1; /* account for terminating NUL */
    /* go print */
    v = cint_builtin_print_kernel("sprintf",
                                  cint_builtin_sprintf_driver,
                                  ast, &dst, arg->next);
    CINT_FREE(dst.stg);
    if (dst.cur > dst.len) {
        cint_ast_error(ast, CINT_E_BAD_AST,
                       "buffer length too small, "
                       "allocated %d chars, needs %d chars",
                       dst.len, dst.cur);
    }

    return v;
}
static cint_variable_t* 
cint_builtin_snprintf(cint_ast_t* ast)
{
    cint_variable_t* str;
    cint_variable_t* size;
    cint_variable_t* v;
    cint_ast_t* arg;
    cint_print_dest_t dst;

    arg = ast->utype.function.parameters;

    /* Evaluate the first argument and expect a character buffer */
    str = _cint_builtin_print_buffer(arg);
    if (str == NULL) {
        return str;
    }

    /* Evaluate the second argument and get an integer */
    size = cint_eval_ast(arg->next);
    if (size == NULL) {
        cint_ast_error(ast, CINT_E_BAD_VARIABLE, 
                       "could not evaluate the second argument (size)");
        return NULL;
    }
        
    if (!cint_is_integer(size)) {
        cint_ast_error(ast, CINT_E_BAD_VARIABLE, 
                       "the second argument (size) must be an integral type");
        return NULL;
    }

    /* Setup destination buffers. The staging buffer is the same size as the
       output buffer for the worst case. */
    dst.len = cint_integer_value(size);

    /* 
     * The function needs to deal with two ways a string can be
     * passed, that are quite different in CINT.
     *
     *  1) It can be a real char*. In that case, str->data points to
     *  the place, where the pointer is stored, so additional
     *  dereference is required
     *
     *  2) It can also be a char[]. In that case, str->data points to
     *  the first character in the array. Moreover, we have the
     *  information about the length of the array and thus we can
     *  perform an additional check
     */

    if (str->dt.desc.pcount == 1) { 
        dst.out = *(char **)str->data;   /* char * was passed */
    } else {
        dst.out = (char *)str->data;     /* char[] was passed */
        if (dst.len > str->dt.desc.dimensions[0]) { 
            /* Most probably we'll be out of bounds */
            cint_ast_error(ast, CINT_E_BAD_VARIABLE,
                           "the first parameter is an array of length %d "
                           "and the second parameter exceeds that length",
                           str->dt.desc.dimensions[0]);
            return NULL;
        }  
    }

    dst.stg = CINT_MALLOC(dst.len);
    if (!dst.stg) {
        cint_ast_error(ast, CINT_E_MEMORY, "memory allocation failure");
        return NULL;
    }

    dst.out[0] = 0; /* start output buffer with a zero length string */
    dst.cur = 1; /* account for terminating NUL */
    /* go print */
    v = cint_builtin_print_kernel("snprintf",
                                  cint_builtin_sprintf_driver,
                                  ast, &dst, arg->next->next);
    CINT_FREE(dst.stg);
    if (dst.cur > dst.len) {
        cint_ast_error(ast, CINT_E_BAD_AST,
                       "buffer length too small, "
                       "allocated %d chars, needs %d chars",
                       dst.len, dst.cur);
    }

    return v;
}
