/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_test_data.c
 * Purpose:     CINT test data
 */

/***********************************************************************
 *
 * CINT Test Datatypes
 */

#include "cint_config.h"
#include "cint_porting.h"
#include "cint_interpreter.h"
#include "cint_internal.h"

/*
 * Definitions for the test data
 */
#if CINT_CONFIG_INCLUDE_LONGLONGS == 1

#define CT_LONGLONG long long
#define CT_ULONGLONG unsigned long long

#else

#define CT_LONGLONG int
#define CT_ULONGLONG unsigned int

#endif

#if CINT_CONFIG_INCLUDE_DOUBLES == 1
#define CT_DOUBLE double
#else
#define CT_DOUBLE int
#endif

#define _CT_STRINGIFY(t) #t
#define CT_STRINGIFY(t) _CT_STRINGIFY(t)

/*************************************************************
 *
 * Atomic Datatypes
 */

/*
 * Implements a simple bitmap interface.
 */

typedef unsigned int ct_bitmap_t;

int cint_test_data_init(void);
int cint_test_data_deinit(void);
cint_ast_t*
__CT_BITMAP_ITER_HANDLER(const char* name, cint_ast_t* arguments, cint_ast_t* statements);

static int
__ct_print_ct_bitmap_t(void* p, char* dst, int size, cint_atomic_format_t format)
{
    unsigned int m = *((unsigned int*)p);
    unsigned int i;

    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
        case cintAtomicFormatCintPrintf:
        default:
            {
                cint_snprintf_ex(&dst, &size, "{ ");
                for(i = (1 << (sizeof(m)*8 - 1)); i != 1; i>>=1) {
                    cint_snprintf_ex(&dst, &size, "%d,", (m&i) ? 1 : 0);
                }
                cint_snprintf_ex(&dst, &size, "%d }", m&1);
            }
        }
    return 0;
}

static int
__ct_assign_ct_bitmap_t(void* dst, const char* expr)
{
    const char* s;
    unsigned int bmap = 0;

    for(s = expr; s && *s; s++) {
        if(*s == '1' || *s == '0') {
            bmap <<= 1;
            bmap |= (*s) - '0';
        }
        else if(*s != ' ') {
            return CINT_E_BAD_EXPRESSION;
        }
    }
    *((unsigned int*)dst) = bmap;
    return CINT_E_NONE;
}


static int
__ct_print_ct_array_t(void* src, char* dst, int size, cint_atomic_format_t format)
{
    unsigned int* a = (unsigned int*)src;
    int i;

    switch(format)
        {
        case cintAtomicFormatCintPrintVariable:
        case cintAtomicFormatCintPrintf:
        default:
            {
                for(i = 0; i < 15; i++) {
                    cint_snprintf_ex(&dst, &size, "%d:", a[i]);
                }
                cint_snprintf_ex(&dst, &size, "%d", a[i]);
            }
        }
    return 0;
}

static int
__ct_assign_ct_array_t(void* dst, const char* expr)
{
    unsigned int* p = (unsigned int*)dst;
    /* Don't care about expression, just for testing purposes */
    int i;
    for(i = 0; i < 16; i++) {
        p[i] = i;
    }
    return 0;
}



/*
 * Atomic datatype description table
 */
static cint_atomic_type_t __ct_atomics[] =
    {
        {
            "ct_bitmap_t",
            sizeof(unsigned int),
            0,
            __ct_print_ct_bitmap_t,
            __ct_assign_ct_bitmap_t,
        },
        {
            "ct_array_t",
            0,
            CINT_ATOMIC_TYPE_F_CAP_ONLY,
            __ct_print_ct_array_t,
            __ct_assign_ct_array_t,
        },
        { NULL },
    };


/*************************************************************
 *
 * Enumerations
 */

typedef enum ct_enum_e {
    ctEnum0 = 0,
    ctEnum1,
    ctEnum2,
    ctEnum3
} ct_enum_t;

/*
 * Enum map
 */
static cint_enum_map_t __ct_enum_map__ct_enum_t[] =
{
    { "ctEnum0", ctEnum0 },
    { "ctEnum1", ctEnum1 },
    { "ctEnum2", ctEnum2 },
    { "ctEnum3", ctEnum3 },
    { NULL }
};

/*
 * Enumeration datatype description table
 */
static cint_enum_type_t __ct_enums[] =
{
    { "ct_enum_t", __ct_enum_map__ct_enum_t },
    { NULL }
};



/*************************************************************
 *
 * Structures
 */

typedef struct ct_struct_s {
    char c;
    unsigned char uc;
    short s;
    unsigned short us;
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    CT_LONGLONG ll;
    CT_ULONGLONG ull;
    CT_DOUBLE d;
    int* ip;
    void* vp;

    char cbuffer[16];
    int** pbuffer[8];

    ct_bitmap_t bitmap;

} ct_struct_t;

typedef struct ct_struct_simple_t {
    int x;
    ct_struct_t ct_struct;
    ct_bitmap_t bitmap;
    ct_struct_t ct_structs[2];
} ct_struct_simple_t;

/*
 * Description of structure parameters
 */
static cint_parameter_desc_t
__ct_struct_members__ct_struct_t[] =
    {
        { "char", "c", 0, 0 },
        { "unsigned char", "uc", 0, 0 },
        { "short", "s", 0, 0 },
        { "unsigned short", "us", 0, 0 },
        { "int", "i", 0, 0 },
        { "unsigned int", "ui", 0, 0 },
        { "long", "l", 0, 0 },
        { "unsigned long", "ul", 0, 0 },
        { CT_STRINGIFY(CT_LONGLONG), "ll", 0, 0 },
        { CT_STRINGIFY(CT_ULONGLONG), "ull", 0, 0 },
        { CT_STRINGIFY(CT_DOUBLE), "d", 0, 0 },
        { "int", "ip", 1, 0 },
        { "void", "vp", 1, 0 },
        { "char", "cbuffer", 0, 16 },
        { "int", "pbuffer", 2, 8 },
        { "ct_bitmap_t", "bitmap", 0, 0 },

        { NULL }
    };

static cint_parameter_desc_t
__ct_struct_members__ct_struct_simple_t[] =
    {
        { "int", "x", 0, 0 },
        { "ct_struct_t", "ct_struct", 0, 0 },
        { "ct_bitmap_t", "bitmap", 0, 0 },
        { "ct_struct_t", "ct_structs", 0, 2 },

        { NULL }
    };

/*
 * Member address handlers
 */
static void*
__ct_maddr__ct_struct_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    ct_struct_t* s = (ct_struct_t*) p;

    switch(mnum)
        {
        case 0: rv = &(s->c); break;
        case 1: rv = &(s->uc); break;
        case 2: rv = &(s->s); break;
        case 3: rv = &(s->us); break;
        case 4: rv = &(s->i); break;
        case 5: rv = &(s->ui); break;
        case 6: rv = &(s->l); break;
        case 7: rv = &(s->ul); break;
        case 8: rv = &(s->ll); break;
        case 9: rv = &(s->ull); break;
        case 10: rv = &(s->d); break;
        case 11: rv = &(s->ip); break;
        case 12: rv = &(s->vp); break;
        case 13: rv = s->cbuffer; break;
        case 14: rv = s->pbuffer; break;
        case 15: rv = &(s->bitmap); break;
        default: rv = NULL; break;
        }

    return rv;
}

static void*
__ct_maddr__ct_struct_simple_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    ct_struct_simple_t* s = (ct_struct_simple_t*) p;

    switch(mnum)
        {
        case 0: rv = &(s->x); break;
        case 1: rv = &(s->ct_struct); break;
        case 2: rv = &(s->bitmap); break;
        case 3: rv = s->ct_structs; break;
        default: rv = NULL; break;
        }
    return rv;
}


/*
 * Structure datatype description table
 */
static cint_struct_type_t __ct_structures[] =
{
    {
        "ct_struct_t",
        sizeof(ct_struct_t),
        __ct_struct_members__ct_struct_t,
        __ct_maddr__ct_struct_t
    },
    {
        "ct_struct_simple_t",
        sizeof(ct_struct_simple_t),
        __ct_struct_members__ct_struct_simple_t,
        __ct_maddr__ct_struct_simple_t
    },

    { NULL }
};


/*************************************************************
 *
 * Functions
 */

static void
ct_void_void_function(void)
{
    CINT_PRINTF("__ct_void_void_function\n");
}

static void
ct_void_int_function(int x)
{
    CINT_PRINTF("__ct_void_int_function: %d\n", x);
}

static int
ct_int_int_function(int x)
{
    CINT_PRINTF("__ct_int_int_function: %d\n", x);
    return 100;
}

static ct_enum_t
ct_kitchen_sink(char c,
                  short s,
                  int i,
                  const char* str,
                  ct_enum_t e,
                  ct_bitmap_t b,
                  ct_struct_t* sp,
                  _Bool bl)

{
    char fbuffer[256];

    CINT_PRINTF("__ct_kitchen_sink ENTER\n");
    CINT_PRINTF("c=%d 0x%x\n", c, c);
    CINT_PRINTF("s=%hd 0x%hx\n", s, s);
    CINT_PRINTF("i=%d 0x%x\n", i, i);
    CINT_PRINTF("str=%s\n", str);
    CINT_PRINTF("e=%d\n", e);
    CINT_PRINTF("b = ");

    __ct_print_ct_bitmap_t(&b, fbuffer, sizeof(fbuffer), 0);
    CINT_PRINTF("%s\n", fbuffer);

    if(sp) {
        CINT_PRINTF("sp->bitmap = ");
        __ct_print_ct_bitmap_t(&sp->bitmap, fbuffer, sizeof(fbuffer), 0);
        CINT_PRINTF("%s\n", fbuffer);
    }
    CINT_PRINTF("bl=%d 0x%x\n", bl, bl);
    CINT_PRINTF("__ct_kitchen_sink EXIT\n");
    return ctEnum1;
}

typedef void (*ct_callback0_f)(void);
typedef ct_enum_t (*ct_kitchen_sink_callback_f)(char, short, int, const char*, ct_enum_t, ct_bitmap_t, ct_struct_t*, _Bool);

static void
ct_function_callbacker0(int x, ct_callback0_f cb)
{
    CINT_PRINTF("__ct_function_callbacker ENTER\n");
    CINT_PRINTF("x=%d\n", x);
    if(cb) {
        cb();
    }
    else {
        CINT_PRINTF("cb is NULL\n");
    }
    CINT_PRINTF("__ct_function_callbacker EXIT\n");
}

static ct_enum_t
ct_kitchen_sink_callbacker(int x, ct_kitchen_sink_callback_f cb)
{
    ct_enum_t rc = 0;
    ct_struct_t sp;
    CINT_MEMSET(&sp, 0, sizeof(sp));
    sp.bitmap = 0xA5;

    CINT_PRINTF("__ct_kitchen_sink_callbacker ENTER\n");
    CINT_PRINTF("x = %d\n", x);
    if(cb == NULL) {
        CINT_PRINTF("cb = NULL\n");
    }
    else {
        rc = cb(1, 2, 3, "4", 0, 0xA5, &sp, 1);
        CINT_PRINTF("rc = %d\n", rc);
    }
    CINT_PRINTF("__ct_kitchen_sink_callbacker EXIT\n");
    return rc;
}

static void
ct_compiled_callback0(void)
{
    CINT_PRINTF("__ct_compiled_callback0 ENTER\n");
    CINT_PRINTF("__ct_compiled_callback0 EXIT\n");
}

static int __ct_event_handler1(void* cookie, cint_interpreter_event_t event)
{
    CINT_PRINTF("event_handler1: event=%d cookie=%p\n", event, cookie);
    return 0;
}
static int __ct_event_handler2(void* cookie, cint_interpreter_event_t event)
{
    CINT_PRINTF("event_handler2: event=%d cookie=%p\n", event, cookie);
    return 0;
}

static int __ct_init_handler1(void* cookie)
{
    return 0;
}

static int __ct_init_handler2(void* cookie)
{
    return 0;
}

static int __ct_init_handler3(void* cookie)
{
    return 0;
}

static void
ct_test_handlers(void)
{
    CINT_PRINTF("START\n");
    /* event */
    cint_interpreter_event_register(__ct_event_handler1, (void*)1);
    cint_interpreter_event(cintEventNone);
    cint_interpreter_event_register(__ct_event_handler2, (void*)2);
    cint_interpreter_event(cintEventNone);
    cint_interpreter_event_unregister(__ct_event_handler1);
    cint_interpreter_event(cintEventNone);
    cint_interpreter_event_unregister(__ct_event_handler1);
    cint_interpreter_event(cintEventNone);
    cint_interpreter_event_unregister(__ct_event_handler2);
    CINT_PRINTF("NO MORE EVENT HANDLERS\n");
    cint_interpreter_event(cintEventNone);

    /* init */
    cint_interpreter_initialize_register(__ct_init_handler1, (void*)1);
    cint_interpreter_initialize_register(__ct_init_handler2, (void*)2);
    cint_interpreter_initialize_register(__ct_init_handler3, NULL);
    cint_interpreter_initialize_unregister(__ct_init_handler2, NULL);
    cint_interpreter_initialize_unregister(__ct_init_handler2, (void*)2);
    cint_interpreter_initialize_unregister(__ct_init_handler3, NULL);
    cint_interpreter_initialize_unregister(__ct_init_handler1, (void*)1);
    cint_interpreter_initialize_unregister(__ct_init_handler1, (void*)1);
    cint_interpreter_initialize_unregister(NULL, NULL);
    CINT_PRINTF("NO MORE INIT HANDLERS\n");

    CINT_PRINTF("DONE\n");
}

static int
__ct_reset_handler(void* cookie, cint_interpreter_event_t event)
{
    CINT_PRINTF("RESET HANDLER: cookie=%p, event=%d\n", cookie, event);
    return 0;
}


static void
ct_install_reset_handler(void)
{
    cint_interpreter_event_register(__ct_reset_handler, (void*)1);
}


/*
 * Function Wrappers
 */
CINT_FWRAPPER_CREATE_VP0(ct_install_reset_handler);

CINT_FWRAPPER_CREATE_VP0(ct_test_handlers);

CINT_FWRAPPER_CREATE_VP0(ct_void_void_function);

CINT_FWRAPPER_CREATE_VP1(ct_void_int_function,
                         int,int,x,0,0);

CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         ct_int_int_function,
                         int,int,x,0,0);

CINT_FWRAPPER_CREATE_RP8(ct_enum_t,ct_enum_t,0,0,
                         ct_kitchen_sink,
                         char,char,c,0,0,
                         short,short,s,0,0,
                         int,int,i,0,0,
                         char*,char,str,1,0,
                         ct_enum_t,ct_enum_t,e,0,0,
                         ct_bitmap_t,ct_bitmap_t,b,0,0,
                         ct_struct_t*,ct_struct_t,sp,1,0,
                         _Bool,_Bool,bl,0,0);

CINT_FWRAPPER_CREATE_VP2(ct_function_callbacker0,
                         int,int,x,0,0,
                         ct_callback0_f,ct_callback0_f,cb,0,0);

CINT_FWRAPPER_CREATE_VP0(ct_compiled_callback0);

CINT_FWRAPPER_CREATE_RP2(ct_enum_t, ct_enum_t, 0, 0,
                         ct_kitchen_sink_callbacker,
                         int,int,x,0,0,
                         ct_kitchen_sink_callback_f,ct_kitchen_sink_callback_f,cb,0,0);

#define CT_BITMAP_MEMBER(bp,pos) ((bp) & (1<<pos))

static int __CT_BITMAP_MEMBER(ct_bitmap_t* bp, int* pos)
{
    return CT_BITMAP_MEMBER(*bp,*pos);
}

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         __CT_BITMAP_MEMBER,
                         ct_bitmap_t*,ct_bitmap_t,bp,1,0,
                         int*,int,pos,1,0);

typedef int (*ct_void_parameter_callback_f)(void);

static int
__ct_void_callbacker(ct_void_parameter_callback_f test) {
    return test();
}

CINT_FWRAPPER_CREATE2_RP1(int, int, 0, 0,
                          __ct_void_callbacker,
                          ct_void_parameter_callback_f,
                          ct_void_parameter_callback_f,
                          test, 0, 0, CINT_PARAM_INOUT);

/*
 * Function datatype description table
 */
static cint_function_t __ct_functions[] =
    {
        CINT_FWRAPPER_ENTRY(ct_install_reset_handler),
        CINT_FWRAPPER_ENTRY(ct_test_handlers),
        CINT_FWRAPPER_ENTRY(ct_void_void_function),
        CINT_FWRAPPER_ENTRY(ct_void_int_function),
        CINT_FWRAPPER_ENTRY(ct_int_int_function),
        CINT_FWRAPPER_ENTRY(ct_kitchen_sink),
        CINT_FWRAPPER_ENTRY(ct_function_callbacker0),
        CINT_FWRAPPER_ENTRY(ct_compiled_callback0),
        CINT_FWRAPPER_ENTRY(ct_kitchen_sink_callbacker),
        CINT_FWRAPPER_NENTRY("CT_BITMAP_MEMBER", __CT_BITMAP_MEMBER),
        CINT_FWRAPPER_NENTRY("ct_void_callbacker", __ct_void_callbacker),
        CINT_ENTRY_LAST
    };



/*************************************************************
 *
 * Function Pointers
 */
static cint_function_pointer_t __ct_function_pointers[4];

static void
__ct_callback_handler0(void)
{
    cint_interpreter_callback(__ct_function_pointers+0, 0, 0);
}

static ct_enum_t
__ct_kitchen_sink_callback(char c,
                           short s,
                           int i,
                           const char* str,
                           ct_enum_t e,
                           ct_bitmap_t b,
                           ct_struct_t* sp,
                           _Bool bl)

{
    ct_enum_t rc;
    cint_interpreter_callback(__ct_function_pointers+1,
                              8, 1,
                              &c,
                              &s,
                              &i,
                              &str,
                              &e,
                              &b,
                              &sp,
                              &bl,
                              &rc);
    return rc;
}

/*
 * Function callback descriptor table
 */
CINT_FWRAPPER_PARAMS_VP0(__ct_callback_handler0);

static cint_parameter_desc_t __cint_parameters__ct_void_handler[] =
{
    { "int", "r", 0, 0 },
    { NULL }
};

static int
__ct_void_parameter_callback(void) {
    int ret;
    cint_interpreter_callback(&__ct_function_pointers[2], 0, 1, &ret);
    return ret;
}

static cint_function_pointer_t __ct_function_pointers[4] =
    {
        {
            "ct_callback0_f",
            (cint_fpointer_t) __ct_callback_handler0,
            __cint_parameters____ct_callback_handler0,
        },
        {
            "ct_kitchen_sink_callback_f",
            (cint_fpointer_t) __ct_kitchen_sink_callback,
            __cint_parameters__ct_kitchen_sink,
        },
        {
           "ct_void_parameter_callback_f",
            (cint_fpointer_t) __ct_void_parameter_callback,
           __cint_parameters__ct_void_handler
        },
        { NULL }
    };


/*************************************************************
 *
 * Iterators
 */
cint_ast_t*
__CT_BITMAP_ITER_HANDLER(const char* name, cint_ast_t* arguments, cint_ast_t* statements)
{
    cint_ast_t* _for;
    cint_ast_t* _if;
    cint_ast_t* _condition;
    cint_ast_t* arg1;

    /*
     * We only take two arguments
     */
    if(cint_ast_count(arguments) != 2) {
        cint_ast_error(arguments, CINT_E_BAD_AST, "wrong number of arguments to %s() -- expected 2, received %d",
                       name, cint_ast_count(arguments));
        return NULL;
    }
    arg1 = arguments->next;

    /*
     * This returns the following code tree:
     *
     * for(arg1 = 0; arg1 < 32; arg1++) {
     *    if(CT_BITMAP_MEMBER(arg0, arg1)
     *       [statements]
     *
     * to simulator BCM_PBMP_ITER()
 */


    /* Function call "CT_BITMAP_MEMBER(arg0, arg1)" */
    _condition = cint_ast_function(cint_ast_identifier("CT_BITMAP_MEMBER"), arguments);

    /* if(CT_BITMAP_MEMBER(arg0,arg1)) { statements } */
    _if = cint_ast_if(_condition, statements, 0);

    /* for(arg1 = 0; arg1 < 32; arg1++) (condition) */
    _for = cint_ast_for( /* arg1 = 0 */
                        cint_ast_operator(cintOpAssign,
                                          arg1,
                                          cint_ast_integer(0)),
                        /* arg1 < 32 */
                        cint_ast_operator(cintOpLessThan,
                                          arg1,
                                          cint_ast_integer(32)),

                        /* arg1++ */
                        cint_ast_operator(cintOpAssign,
                                          arg1,
                                          cint_ast_operator(cintOpAdd,
                                                            arg1,
                                                            cint_ast_integer(1))),

                        /* statements */
                        _if);

    return _for;
}

static cint_custom_iterator_t  __ct_iterators[] =
    {
        { "CT_BITMAP_ITER", __CT_BITMAP_ITER_HANDLER },
        { NULL }
    };

/*************************************************************
 *
 * Macros
 */
static cint_ast_t*
__CT_IF_ERROR_RETURN_HANDLER(const char* name, cint_ast_t* arguments)
{
    /* Inserts the following code:

    do { int __rv__; if ((__rv__ = (arguments)) < 0) return(__rv__); } while(0)

    */
    cint_ast_t* _while;
    cint_ast_t* __rv__ = cint_ast_identifier("__rv__");
    cint_ast_t* _decl_rv = cint_ast_declaration();
    cint_ast_t* _if;

    /* Declare __rv__ = (arguments) */
    _decl_rv->utype.declaration.type = cint_ast_type("int");
    _decl_rv->utype.declaration.pcount = 0;
    _decl_rv->utype.declaration.num_dimension_initializers = 0;
    _decl_rv->utype.declaration.identifier = __rv__;
    _decl_rv->utype.declaration.init = arguments;

    _if = cint_ast_if(cint_ast_operator(cintOpLessThan,
                                        __rv__,
                                        cint_ast_integer(0)),
                      cint_ast_return(__rv__),
                      NULL);

    /* Make the if statement come after the declaration */
    cint_ast_append(_decl_rv, _if);

    _while = cint_ast_while(/* while(0) */
                            cint_ast_integer(0),

                            /* statements */
                            _decl_rv,

                            /* order */
                            1);
    return _while;
}



static cint_custom_macro_t __ct_macros[] =
    {
        { "CT_IF_ERROR_RETURN", __CT_IF_ERROR_RETURN_HANDLER },

        { NULL }
    };

/*************************************************************
 *
 * Typedefs
 */
static cint_parameter_desc_t __ct_typedefs[] =
    {
        { "void", "ct_voidp_t", 1, 0 },
        { "int", "ct_int_t", 0, 0 },
        { "ct_bitmap_t", "ct_bitmapp_t", 1, 0 },
        { "unsigned int", "ct_array_t", 0, 16 },
        { "unsigned int", "xct_array_t", 0, 16 },

        { NULL },
    };


/*************************************************************
 *
 * Constants
 */
static cint_constants_t __ct_constants[] =
    {
        { "ct_constant_db", 0xdeadbeef },
        { "ct_constant_null", 0 },
        { "ct_constant_cafe", 0xcafe },
        { "ct_true", 0x1 },
        { "ct_false", 0x0 },

        { NULL }
    };




/*************************************************************
 *
 * Datatype definition and export
 */

cint_data_t cint_test_data =
    {
        __ct_atomics,
        __ct_functions,
        __ct_structures,
        __ct_enums,
        __ct_typedefs,
        __ct_constants,
        __ct_function_pointers,
        __ct_iterators,
        __ct_macros
    };


CINT_LOAD_DATA(cint_test_data);

static int
cint_test_data_init_handler(void *data)
{
    cint_interpreter_add_atomics(cint_test_data.atomics);
    cint_interpreter_add_data(&cint_test_data, NULL);

    return CINT_E_NONE;
}

int
cint_test_data_init(void)
{
    return
        cint_interpreter_initialize_register(cint_test_data_init_handler, NULL);
}

int
cint_test_data_deinit(void)
{
    return
        cint_interpreter_initialize_unregister(cint_test_data_init_handler, NULL);
}
